
#include "tcp_app.h"
#include "io-helper.h"
#include "uip.h"

#include <string.h>

#include "usbd_cdc_vcp.h"
#include "led_pwm.h"

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
    //memcpy(s->outputbuf , tmp , len);
    //s->outputbuf[len+1] = 0;


    if(strncmp(tmp, "GET", 3) == 0)
    {
        if(strncmp(tmp+3, " /", 2) == 0)
        {
            //strcpy(s->outputbuf, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
//            strcpy(s->outputbuf, "HTTP/1.0 200 OK\r\nServer: C\r\nContent-Length: 46\r\nCache-Control: no-cache\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<a href=\"/on\">on</a> <a href=\"/off\">off</a>\r\n\r\n");
            strcpy(s->outputbuf, "HTTP/1.0 200 OK\r\nServer: C\r\nCache-Control: no-cache\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<a href=\"/on\">on</a> <a href=\"/off\">off</a> <a href=\"/fade\">fade</a> <a href=\"/light_tgl\">light_tgl</a> \r\n\r\n");


            if(strncmp(tmp+5, "on", 2) == 0)
            {
                led.target_r = 0;
                led.target_g = 0;
                led.target_b = 0;
                led.mode = 0;
                led.time = 1000;
                start_fade(&led);
            }
            else if(strncmp(tmp+5, "off", 3) == 0)
            {
                led.target_r = 2048;
                led.target_g = 2048;
                led.target_b = 2048;
                led.mode = 0;
                led.time = 1000;
                start_fade(&led);
            }
            else if(strncmp(tmp+5, "fade", 4) == 0)
            {
                led.mode = 2;
            }
            else if(strncmp(tmp+5, "light_tgl", 9) == 0)
            {
                GPIOB->ODR ^= GPIO_Pin_7;
            }
            else if(strncmp(tmp+5, "rgb/", 4) == 0)
            { // rgb/<r>/<g>/<b>/
                led.mode = 0;
                //sscanf(tmp+9, "%d/%d/%d/" , led.r, led.g, led.b);
                set_RGB(&led);
            }
        }
    }
    //BSP:
    //Idx: 0123456789012
    //Str: SEND LED 1 ON
    else if(strncmp(tmp, "SEND", 4) == 0 && len > 10)
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
        LED_Toggle(1);
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
    else if(strncmp(tmp, "LEDOFF", 6) == 0)
    {
        led.target_r = 2048;
        led.target_g = 2048;
        led.target_b = 2048;
        led.mode = 0;
        led.time = 1000;
        start_fade(&led);
    }
    else if(strncmp(tmp, "LEDON", 5) == 0)
    {
        led.target_r = 0;
        led.target_g = 0;
        led.target_b = 0;
        led.mode = 0;
        led.time = 1000;
        start_fade(&led);
    }
    else if(strncmp(tmp, "LEDRND", 6) == 0)
    {
        led.mode = 2;
        strcpy(s->outputbuf, "LEDRND");
    }
    else if(strncmp(tmp, "MODE", 4) == 0)
    {
        led.mode = tmp[5]-'0';
    }
    else if(tmp[0] == 100)
    {
        led.mode = 0;
        led.r = convert_color(tmp[1]);
        led.g = convert_color(tmp[2]);
        led.b = convert_color(tmp[3]);
        set_RGB(&led);
        uint8_t send_string[11];
        memcpy(send_string+0 , "RGB: ", 5);
        /*memcpy(send_string+4 , led.r, 1);
        memcpy(send_string+5 , ",", 1);
        memcpy(send_string+6 , led.g, 1);
        memcpy(send_string+7 , ",", 1);
        memcpy(send_string+8 , led.b, 1);*/
        memcpy(send_string+4 , "\n",1);
        send_string[10] = 0x0;
        VCP_DataTx(send_string, 10);
        LED_Toggle(1);
    }
    else if(tmp[0] == 101)
    {
        led.mode = 0;
        led.target_r = convert_color(tmp[1]);
        led.target_g = convert_color(tmp[2]);
        led.target_b = convert_color(tmp[3]);
        led.time = (int)tmp[4]*100;
        start_fade(&led);
        uint8_t send_string[11];
        memcpy(send_string+0 , "FRGB:", 5);
        /*memcpy(send_string+4 , led.r, 1);
        memcpy(send_string+5 , ",", 1);
        memcpy(send_string+6 , led.g, 1);
        memcpy(send_string+7 , ",", 1);
        memcpy(send_string+9 , led.b, 1);*/
        memcpy(send_string+4 , "\n",1);
        send_string[10] = 0x0;
        VCP_DataTx(send_string, 10);
        LED_Toggle(1);
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
      //strcpy(s->outputbuf+strlen(s->outputbuf) , "Timer");
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

