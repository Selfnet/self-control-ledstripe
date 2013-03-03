
#include "tcp_app.h"
#include "io-helper.h"
#include "uip.h"

#include <string.h>

#include "usbd_cdc_vcp.h"

#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a


/*---------------------------------------------------------------------------*/

static void handle_input(struct tcp_test_app_state *s)
{
    char * tmp = (char *)uip_appdata;
    int    len = uip_len;
    //s->inputbuf = uip_appdata;

    VCP_DataTx(tmp, len );

    //BSP:
    //Idx: 0123456789012
    //Str: SEND LED 1 ON
    if(strncmp(tmp, "SEND", 4) == 0 && len > 10)
    {
        CanTxMsg TxMessage;
        TxMessage.StdId=0x11;
        TxMessage.RTR=CAN_RTR_DATA;
        TxMessage.IDE=CAN_ID_STD;
        TxMessage.DLC=2; //0 - 8
        TxMessage.Data[0]=0x00;
        TxMessage.Data[1]=0x00;
        if(strncmp(tmp+5, "LED", 3) == 0)
        {
            if(strncmp(tmp+9, "1", 1) == 0)
                TxMessage.Data[0]=1;
            else
                TxMessage.Data[0]=2;
            
            if(strncmp(tmp+11, "ON", 2) == 0)
                TxMessage.Data[1]=1;
            else if(strncmp(tmp+11, "OFF", 3) == 0)
                TxMessage.Data[1]=2;
            else
                TxMessage.Data[1]=3;
        }
        LED_Toggle(2);
        CAN_Transmit(CAN1, &TxMessage);
        
        uint8_t send_string[11];
        memcpy(send_string+0 , "Can: ", 5);
        memcpy(send_string+4 , &TxMessage.StdId, 1);
        memcpy(send_string+5 , ",", 1);
        memcpy(send_string+6 , &TxMessage.Data[0], 1);
        memcpy(send_string+7 , ",", 1);
        memcpy(send_string+8 , &TxMessage.Data[1], 1);
        memcpy(send_string+9 , "\n",1);
        send_string[10] = 0x0;
        VCP_DataTx(send_string, 10);        
    }
    else
    {
        LED_Toggle(2);
    }
}

static void
handle_connection(struct tcp_test_app_state *s)
{
  if( uip_newdata() )
    handle_input(s);
  if( strlen(s->outputbuf) > 0 )
  {
    uip_send(s->outputbuf , strlen(s->outputbuf) );
    memset(s->outputbuf, 0, 50); //leeren
  }
    
}

void tcp_test_appcall(void)
{
  struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);

  if(uip_closed() || uip_aborted() || uip_timedout()) {
  } else if(uip_connected()) {
    //PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
    //PSOCK_INIT(&s->sout, s->outputbuf, sizeof(s->outputbuf) - 1);
    s->timer = 0;
    strcpy(s->outputbuf , "hallo - bitte can-bus anschliesen");
    
    handle_connection(s); //TODO
  } else if(s != 0) {
    if(uip_poll()) {
      ++s->timer;
      strcpy(s->outputbuf+strlen(s->outputbuf) , "Timer");
      if(s->timer >= 20) {
	        uip_abort(); //TimeOut
      }
    } else {
      s->timer = 0;
    }
    handle_connection(s); //TODO
  } else {
    uip_abort();
  }
}

