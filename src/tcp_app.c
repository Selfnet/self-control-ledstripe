
#include "tcp_app.h"
#include "io-helper.h"
#include "uip.h"
#include "can.h"

#include <string.h>

#include "usbd_cdc_vcp.h"

#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a


/*---------------------------------------------------------------------------*/

void handle_input(struct tcp_test_app_state *s)
{
    char * tmp = ((char *)uip_appdata);
    int    len = uip_len;
    ////s->inputbuf = uip_appdata;

    //VCP_DataTx(tmp, len );
    ////memcpy(s->outputbuf , tmp , len);
    ////s->outputbuf[len+1] = 0;


    // CAN functions

    // *** erklärung zu can vars ***
    //Sender        = RxMessage.ExtId & 0b111111111110000000    (11Bit)
    //Type          = RxMessage.ExtId & 0b000000000001111111    (7Bit)
    //Empfaenger    = RxMessage.StdId                           (11Bit)
    //ID-Type       = RxMessage.IDE (CAN_Id_Standard or CAN_Id_Extended) DEFAULT=1 (immer extended)
    //get_set?      = RxMessage.RTR: (1-> SendData (seter) | 0-> Request Data (geter))

//                                              9876 54321098 7654321
// 1111000011110000111100001111000011110000111100001 11100001 1110000

    // ethernet bytes:
    // SENDER0 | SENDER1 | EMPFAENGER0 | EMPFAENGER1 | TYPE | SEND-REQUEST | DATA0 - DATA7
    // EMPFAENGER0 | EMPFAENGER1 | TYPE | SEND-REQUEST | DATA0 - DATA7

    // 0x15 | extID0 | extID1 | extID2 | extID3 | len | DATA0 - DATA7
    // 0x15 | extID0 | Proto  |  Empf  | Sender | len | DATA0 - DATA7

    //ping vom server
    if(len >= 2 && tmp[0] == 1 )
    {
        s->outputbuf[ s->outpt++ ] = tmp[0];
        s->outputbuf[ s->outpt++ ] = tmp[1];
    }
    // can gateway nachricht
    else if(tmp[0] == 0x15 && len >= 5 && len <= 14)
    {
        uint32_t extID = *(uint32_t *)(uip_appdata+1);

        CanTxMsg TxMessage;
        TxMessage.IDE = CAN_ID_EXT;                                 //immer extended can frames
        TxMessage.ExtId = extID;
        TxMessage.RTR = 0;
        TxMessage.DLC = tmp[5]; //0 bis 8
        if(TxMessage.DLC > 8)
            TxMessage.DLC = 8;
        int i;
        for(i = 0 ; i <= TxMessage.DLC ; i++)
        {
            TxMessage.Data[i] = tmp[i+6];
        }
        CAN_Transmit(CAN1, &TxMessage);

        // *** print TxMessage struct ***
        //CanMsg per Ethernet verschicken
        if(s->outpt + 24 + TxMessage.DLC < 128)
        {
            strcpy(s->outputbuf+s->outpt , "can msg send\n");
            s->outpt += strlen("can msg send")+1;

            s->outputbuf[ s->outpt++ ] = 'T';
            s->outputbuf[ s->outpt++ ] = getRecipient(TxMessage.ExtId)>>8; //sender0
            s->outputbuf[ s->outpt++ ] = getRecipient(TxMessage.ExtId);    //sender1
            
            s->outputbuf[ s->outpt++ ] = getSender(TxMessage.ExtId)>>8;    //empfaenger0
            s->outputbuf[ s->outpt++ ] = getSender(TxMessage.ExtId);       //empfaenger1
            
            s->outputbuf[ s->outpt++ ] = getTyp(TxMessage.ExtId);          //type

            s->outputbuf[ s->outpt++ ] = ((TxMessage.RTR == CAN_RTR_Remote)? 1 : 0);  //send-request (RTR)

            //data
            for(i=0 ; i < TxMessage.DLC ; i++)
            {
                s->outputbuf[ s->outpt++ ] = TxMessage.Data[i];
            }
            s->outputbuf[ s->outpt++ ] = '\n'; //nullbyte anhängen
            s->outputbuf[ s->outpt++ ] = 0; //nullbyte anhängen
        }

        LED_Toggle(1);
    }
/*
    else if(tmp[0] == 202) //light
    {
        if(tmp[1] == 1) //light 1
        {
            if(tmp[2] == 2) // on 
                GPIOB->BSRR = GPIO_Pin_7;
            else if(tmp[2] == 1) // off
                GPIOB->BRR = GPIO_Pin_7;
        }
    }*/
}

static void
handle_connection(struct tcp_test_app_state *s)
{
    if( uip_newdata() )
        handle_input(s);
    if( s->outpt > 0 )
    {
        if(s->outpt >= 128)
            s->outpt = 127;
        uip_send(s->outputbuf , s->outpt );
        memset(s->outputbuf, 0, s->outpt); //leeren
        s->outpt = 0;
    }
}

void tcp_test_appcall(void)
{
    struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);

    if(uip_closed() || uip_aborted() || uip_timedout())
    {
    }
    else if(uip_connected())
    {
        s->timer = 0;
        s->outpt = 0;
        strcpy(s->outputbuf+s->outpt , "hallo - bitte can-bus anschliesen");
        s->outpt += strlen("hallo - bitte can-bus anschliesen");

        handle_connection(s); //TODO
    }
    else if(s != 0)
    {
        if(uip_poll())
        {
            ++s->timer;
            //strcpy(s->outputbuf+strlen(s->outputbuf) , "Timer");
            if(s->timer >= 20)
                uip_abort(); //TimeOut
        }
        else
        {
            s->timer = 0;
        }
        handle_connection(s); //TODO
    }
    else
    {
        uip_abort();
    }
}

