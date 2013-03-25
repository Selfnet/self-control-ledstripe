/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    $Revision: #1 $
 **************************************************************************/
#include "ethernet.h"

#include <stdio.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#include "led_pwm.h"

#define TRUE 1
#define FALSE 0

typedef union _TranDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  DB            : 1;
    uint32_t  UF            : 1;
    uint32_t  ED            : 1;
    uint32_t  CC            : 4;
    uint32_t  VF            : 1;
    uint32_t  EC            : 1;
    uint32_t  LC            : 1;
    uint32_t  NC            : 1;
    uint32_t  LSC           : 1;
    uint32_t  IPE           : 1;
    uint32_t  FF            : 1;
    uint32_t  JT            : 1;
    uint32_t  ES            : 1;
    uint32_t  IHE           : 1;
    uint32_t                : 3;
    uint32_t  TCH           : 1;
    uint32_t  TER           : 1;
    uint32_t  CIC           : 2;
    uint32_t                : 2;
    uint32_t  DP            : 1;
    uint32_t  DC            : 1;
    uint32_t  FS            : 1;
    uint32_t  LSEG          : 1;
    uint32_t  IC            : 1;
    uint32_t  OWN           : 1;
  };
} TranDesc0_t, * pTranDesc0_t;

typedef union _TranDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  TBS1          :13;
    uint32_t                : 3;
    uint32_t  TBS2          :12;
    uint32_t                : 3;
  };
} TranDesc1_t, * pTranDesc1_t;

typedef union _RecDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  RMAM_PCE      : 1;
    uint32_t  CE            : 1;
    uint32_t  DE            : 1;
    uint32_t  RE            : 1;
    uint32_t  RWT           : 1;
    uint32_t  FT            : 1;
    uint32_t  LC            : 1;
    uint32_t  IPHCE         : 1;
    uint32_t  LS            : 1;
    uint32_t  FS            : 1;
    uint32_t  VLAN          : 1;
    uint32_t  OE            : 1;
    uint32_t  LE            : 1;
    uint32_t  SAF           : 1;
    uint32_t  DERR          : 1;
    uint32_t  ES            : 1;
    uint32_t  FL            :14;
    uint32_t  AFM           : 1;
    uint32_t  OWN           : 1;
  };
} RecDesc0_t, * pRecDesc0_t;

typedef union _recDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  RBS1          :13;
    uint32_t                : 1;
    uint32_t  RCH           : 1;
    uint32_t  RER           : 1;
    uint32_t  RBS2          :14;
    uint32_t  DIC           : 1;
  };
} RecDesc1_t, * pRecDesc1_t;

typedef union _EnetDmaDesc_t
{
  uint32_t Data[4];
  // Rx DMA descriptor
  struct
  {
    RecDesc0_t  RxDesc0;
    RecDesc1_t  RxDesc1;
    uint32_t *  pBuffer;
    union
    {
      uint32_t *              pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Rx;
  // Tx DMA descriptor
  struct
  {
    TranDesc0_t               TxDesc0;
    TranDesc1_t               TxDesc1;
    uint32_t *                pBuffer1;
    union
    {
      uint32_t *              pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Tx;
} EnetDmaDesc_t, * pEnetDmaDesc_t;

#pragma data_alignment=4
uint8_t RxBuff[EMAC_MAX_PACKET_SIZE];
#pragma data_alignment=4
uint8_t TxBuff[EMAC_MAX_PACKET_SIZE];

#pragma data_alignment=128
EnetDmaDesc_t EnetDmaRx;

#pragma data_alignment=128
EnetDmaDesc_t EnetDmaTx;


uint32_t uIPMain(void)
{
    uint32_t i;
    uip_ipaddr_t ipaddr;
    struct timer periodic_timer, arp_timer;

    LED_On(2);

    // Sys timer init 1/100 sec tick
    clock_init(2);

    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);

    // Initialize the ethernet device driver
    // Init MAC
    // Phy network negotiation
    tapdev_init(); // ENET_TxDscrInit (ethernet.c) & ENET_RxDscrInit (ethernet.c) &  ETH_Start (stm32_eth.c)

    // uIP web server
    // Initialize the uIP TCP/IP stack.
    uip_init(); // (uip.c)

    // Init WEB server
    uip_ipaddr(ipaddr, 10,43,100,112);
    uip_sethostaddr(ipaddr); //ip
    uip_ipaddr(ipaddr, 10,43,0,254);
    uip_setdraddr(ipaddr);  //gw
    uip_ipaddr(ipaddr, 255,255,0,0);
    uip_setnetmask(ipaddr); //nm

    // Initialize the TELNET server.
    uip_listen(HTONS(23));
    //uip_listen(HTONS(80));

    VCP_DataTx("Listen...\n", 11);

    LED_Off(1);

    CanRxMsg RxMessage;
    int can_last_msg_id = 0;
    uint32_t nCount;
  
    while(1)
    {
        uip_len = tapdev_read(uip_buf);
        if(uip_len > 0) //read input
        {
            if(BUF->type == htons(UIP_ETHTYPE_IP)) //Layer3?
            {
                uip_arp_ipin(); 
                uip_input();
                /* If the above function invocation resulted in data that
                 should be sent out on the network, the global variable
                 uip_len is set to a value > 0. */
                if(uip_len > 0)
                {
                    uip_arp_out(); //get ARP of destination - or default gw (uip_arp.c)
                    tapdev_send(uip_buf,uip_len);
                }
            }
            else if(BUF->type == htons(UIP_ETHTYPE_ARP)) //Layer2?
            {
                uip_arp_arpin();
                /* If the above function invocation resulted in data that
                should be sent out on the network, the global variable
                uip_len is set to a value > 0. */
                if(uip_len > 0)
                {
                    tapdev_send(uip_buf,uip_len);
                }
            }
        }
        else if(timer_expired(&periodic_timer)) //check if there is data in the send queue of each connection and send it
        {
            timer_reset(&periodic_timer);
            for(i = 0; i < UIP_CONNS; i++)
            {
                uip_periodic(i); //sets uip_conn to the current connections (macro uip.h)
                /* If the above function invocation resulted in data that
                should be sent out on the network, the global variable
                uip_len is set to a value > 0. */
                if(uip_len > 0)
                {
                    uip_arp_out(); //get ARP of destination - or default gw (uip_arp.c)
                    tapdev_send(uip_buf,uip_len);
                }
            }
            #if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++)
            {
                uip_udp_periodic(i);
                /* If the above function invocation resulted in data that
                should be sent out on the network, the global variable
                uip_len is set to a value > 0. */
                if(uip_len > 0)
                {
                    uip_arp_out(); //get ARP of destination - or default gw (uip_arp.c)
                    tapdev_send();
                }
            }
            #endif /* UIP_UDP */

            /* Call the ARP timer function every 10 seconds. */
            /* It updates the arp-table (removes old entries) */
            if(timer_expired(&arp_timer))
            {
                timer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
    }
    return(TRUE);
}


/*******************************************************************************
* Function Name  : ENET_RxDscrInit
* Description    : Initializes the Rx ENET descriptor chain. Single Descriptor
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENET_RxDscrInit(void)
{
    /* Initialization */
    /* Assign temp Rx array to the ENET buffer */
    EnetDmaRx.Rx.pBuffer = (uint32_t *)RxBuff;

    /* Initialize RX ENET Status and control */
    EnetDmaRx.Rx.RxDesc0.Data = 0;

    /* Initialize the next descriptor- In our case its single descriptor */
    EnetDmaRx.Rx.pEnetDmaNextDesc = &EnetDmaRx;

    EnetDmaRx.Rx.RxDesc1.Data = 0;
    EnetDmaRx.Rx.RxDesc1.RER  = 0; // end of ring
    EnetDmaRx.Rx.RxDesc1.RCH  = 1; // end of ring

    /* Set the max packet size  */
    EnetDmaRx.Rx.RxDesc1.RBS1 = EMAC_MAX_PACKET_SIZE;

    /* Setting the VALID bit */
    EnetDmaRx.Rx.RxDesc0.OWN = 1;
    /* Setting the RX NEXT Descriptor Register inside the ENET */
    ETH->DMARDLAR = (uint32_t)&EnetDmaRx;
    /* Setting the RX NEXT Descriptor Register inside the ENET */
    //ETH_DMARDLAR = (uint32_t)&EnetDmaRx;
}

/*******************************************************************************
* Function Name  : ENET_TxDscrInit
* Description    : Initializes the Tx ENET descriptor chain with single descriptor
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void ENET_TxDscrInit(void)
{
    /* ENET Start Address */
    EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

    /* Next Descriptor Address */
    EnetDmaTx.Tx.pEnetDmaNextDesc = &EnetDmaTx;

    /* Initialize ENET status and control */
    EnetDmaTx.Tx.TxDesc0.TCH  = 1;
    EnetDmaTx.Tx.TxDesc0.Data = 0;
    EnetDmaTx.Tx.TxDesc1.Data = 0;
    /* Tx next set to Tx descriptor base */
    ETH->DMATDLAR = (uint32_t)&EnetDmaTx;

}

void tapdev_init(void)
{
    ENET_TxDscrInit();
    ENET_RxDscrInit();

    ETH_Start();
}

/*************************************************************************
 * Function Name: tapdev_read
 * Parameters:
 * Return:
 *
 * Description: Read data for MAC/DMA Controller
 *
 *************************************************************************/
uint32_t tapdev_read(void * pPacket)
{
    uint32_t size;
    /*check for validity*/
    if(0 == EnetDmaRx.Rx.RxDesc0.OWN)
    {
        /*Get the size of the packet*/
        size = EnetDmaRx.Rx.RxDesc0.FL; // CRC
        //MEMCOPY_L2S_BY4((u8*)ppkt, RxBuff, size); /*optimized memcopy function*/
        memcpy(pPacket, RxBuff, size);   //string.h library*/
    }
    else
    {
        return(ENET_NOK);
    }
    /* Give the buffer back to ENET */
    EnetDmaRx.Rx.RxDesc0.OWN = 1;
    /* Start the receive operation */
    ETH->DMARPDR = 1;
    /* Return no error */
    return size;
}

/*************************************************************************
 * Function Name: tapdev_send
 * Parameters:
 * Return:
 *
 * Description: Send data to MAC/DMA Controller
 *
 *************************************************************************/
void tapdev_send(void *pPacket, uint32_t size)
{
    while(EnetDmaTx.Tx.TxDesc0.OWN);

    /* Copy the  application buffer to the driver buffer
     Using this MEMCOPY_L2L_BY4 makes the copy routine faster
     than memcpy */
    //MEMCOPY_L2S_BY4((u8*)TxBuff, (u8*)ppkt, size);
    memcpy(TxBuff, pPacket, size);

    /* Assign ENET address to Temp Tx Array */
    EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

    /* Setting the Frame Length*/
    EnetDmaTx.Tx.TxDesc0.Data = 0;
    EnetDmaTx.Tx.TxDesc0.TCH  = 1;
    EnetDmaTx.Tx.TxDesc0.LSEG = 1;
    EnetDmaTx.Tx.TxDesc0.FS   = 1;
    EnetDmaTx.Tx.TxDesc0.DC   = 0;
    EnetDmaTx.Tx.TxDesc0.DP   = 0;

    EnetDmaTx.Tx.TxDesc1.Data = 0;
    EnetDmaTx.Tx.TxDesc1.TBS1 = (size&0xFFF);

    /* Start the ENET by setting the VALID bit in dmaPackStatus of current descr*/
    EnetDmaTx.Tx.TxDesc0.OWN = 1;

    /* Start the transmit operation */
    ETH->DMATPDR = 1;
}


