
#include "stm32f10x.h"
#include "can.h"
#include "uip.h"

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_config(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    CAN_InitTypeDef         CAN_InitStructure;
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;

    // GPIO clock enable 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // Configure CAN pin: RX 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Configure CAN pin: TX 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);

    // CANx Periph clock enable 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    // CAN register init 
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    // CAN cell init 
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;

    /* CAN Baudrate = 1MBps*/
    //  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    //  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    //  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    //  CAN_InitStructure.CAN_Prescaler = 4;
    //  CAN_Init(CAN1, &CAN_InitStructure);

    // Baudrate = 125kbps
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;
    CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler=48;
    CAN_Init(CAN1, &CAN_InitStructure);

    // CAN filter init 
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);


    // Enable the CAN RX Interrupt
    NVIC_InitTypeDef NVIC_InitStructure_CAN;
    NVIC_InitStructure_CAN.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure_CAN.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure_CAN.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure_CAN.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure_CAN);

    //  Enable CAN Interrupt
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}


/*
Field name	                        Length (bits)	Purpose
Start-of-frame	                    1	Denotes the start of frame transmission
Identifier A	                    11	First part of the (unique) identifier for the data which also represents the message priority
Substitute remote request (SRR)	    1	Must be recessive (1). Optional
Identifier extension bit (IDE)	    1	Must be recessive (1). Optional
Identifier B	                    18	Second part of the (unique) identifier for the data which also represents the message priority
Remote transmission request (RTR)	1	Must be dominant (0)
Reserved bits (r0, r1)	            2	Reserved bits (it must be set dominant (0), but accepted as either dominant or recessive)
Data length code (DLC)*	            4	Number of bytes of data (0–8 bytes)
Data field	                        0–64 (0-8 bytes)	Data to be transmitted (length dictated by DLC field)
CRC	                                15	Cyclic redundancy check
CRC delimiter	                    1	Must be recessive (1)
ACK slot	                        1	Transmitter sends recessive (1) and any receiver can assert a dominant (0)
ACK delimiter	                    1	Must be recessive (1)
End-of-frame (EOF)	                7	Must be recessive (1)
*/

// *** CAN Id Functions
int getSender(uint32_t ExtId)
{
    return (ExtId>>(11+7)) & 0x7ff;
}

int getRecipient(uint32_t ExtId)
{
    return (ExtId>>(7)) & 0x7ff;
}

int getTyp(uint32_t ExtId)
{
    return (ExtId & 0x7f);
}

void setSender(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7ff)<<(11+7);
}

void setRecipient(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7ff)<<7;
}

void setTyp(uint32_t *ExtId , int recipient)
{
    *ExtId |= (((uint32_t)recipient) & 0x7f);
}


// *** erklärung zu can vars ***
//Sender        = RxMessage.ExtId & 0b11111111111000000000000000000 (11Bit)
//Empfaenger    = RxMessage.ExtId & 0b00000000000111111111110000000 (11Bit)
//Type          = RxMessage.ExtId & 0b00000000000000000000001111111 (7Bit)
//ID-Type       = RxMessage.IDE (CAN_Id_Standard or CAN_Id_Extended) DEFAULT=1 (immer extended)
//get_set?      = RxMessage.RTR: (1-> SendData (seter) | 0-> Request Data (geter))

// ethernet bytes:
// SENDER0 | SENDER1 | EMPFAENGER0 | EMPFAENGER1 | TYPE | SEND-REQUEST | DATA0 - DATA7



void prozess_can_it(void)
{
    LED_Toggle(1);

    CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

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

    if(RxMessage.IDE == CAN_Id_Standard)
    {
        //iggen 
    }
    else
    {
        LED_Toggle(2);
        // wenn es an mich ist ORDER wenn es nicht von mir kommt
        if( getRecipient(RxMessage.ExtId) == NODE_CAN_ID || getSender(RxMessage.ExtId) != NODE_CAN_ID )
        {
            LED_Toggle(1);
            
            // TODO send msg when finished
            struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);

            //CanMsg per Ethernet verschicken
            s->outputbuf[ s->outpt++ ] = 'R';
            s->outputbuf[ s->outpt++ ] = getRecipient(RxMessage.ExtId)>>8; //sender0
            s->outputbuf[ s->outpt++ ] = getRecipient(RxMessage.ExtId);    //sender1
            
            s->outputbuf[ s->outpt++ ] = getSender(RxMessage.ExtId)>>8;    //empfaenger0
            s->outputbuf[ s->outpt++ ] = getSender(RxMessage.ExtId);       //empfaenger1
            
            s->outputbuf[ s->outpt++ ] = getTyp(RxMessage.ExtId);          //type

            s->outputbuf[ s->outpt++ ] = ((RxMessage.RTR == CAN_RTR_REMOTE)? 1 : 0);  //send-request (RTR)

            //data
            int i;
            for(i=0 ; i < RxMessage.DLC ; i++)
            {
                s->outputbuf[ s->outpt++ ] = RxMessage.Data[i];
            }
            s->outputbuf[ s->outpt++ ] = '\n'; //nullbyte anhängen
            s->outputbuf[ s->outpt++ ] = 0; //nullbyte anhängen
        }
    }
}

