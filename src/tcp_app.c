
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


    if(len >= 3 && tmp[0] == 1 && tmp[1] == 1 && tmp[2] == 1)
    {
        //ping vom server - einfach ignorieren
    }
    else if(len >= 4)
    {
        uint32_t recId = (((uint32_t)tmp[0])<<8)+tmp[1];
        CanTxMsg TxMessage;
        TxMessage.IDE = CAN_ID_EXT;                                 //immer extended can frames
        TxMessage.ExtId = 0;
        setRecipient(&TxMessage.ExtId , recId );                    //empfaenger
        setSender(&TxMessage.ExtId, NODE_CAN_ID);                   //sender (hoeherwaertige bits)
        setTyp(&TxMessage.ExtId , tmp[2]);                          //type   (niederwertige bits)
        TxMessage.RTR = tmp[3]==1 ? CAN_RTR_Remote : CAN_RTR_Data;  //senden order abfragen
        TxMessage.DLC = len - 4; //0 bis 8
        int i;
        for(i = 0 ; i <= len-4 ; i++)
        {
            TxMessage.Data[i] = tmp[i+4];
        }
        CAN_Transmit(CAN1, &TxMessage);

        strcpy(s->outputbuf+s->outpt , "can msg send\n");
        s->outpt += strlen("can msg send")+1;


        // *** print TxMessage struct ***
            //CanMsg per Ethernet verschicken
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

        LED_Toggle(1);
    }
/*
    else if(100 <= tmp[0] && tmp[0] <= 200 && len >= 2)
    {
        CanTxMsg TxMessage;
        TxMessage.StdId=0x10;
        TxMessage.RTR=CAN_RTR_DATA;
        TxMessage.IDE=CAN_ID_STD;
        TxMessage.DLC=len; //0 bis 8
        TxMessage.Data[0]=tmp[1]; //led nr
        TxMessage.Data[1]=tmp[0]-100; //mode
        int i;
        for(i = 2; i < len && i <= 7; i++)
        {
            TxMessage.Data[i]=tmp[i]; //data i
        }
        CAN_Transmit(CAN1, &TxMessage);
        LED_Toggle(1);
    }
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
    else
    {
        //LED_Toggle(2);
    }
}

static void
handle_connection(struct tcp_test_app_state *s)
{
    if( uip_newdata() )
        handle_input(s);
    if( s->outpt > 0 )
    {
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

