
#include "includes.h"

/**
 * Enable DMA interrupts.
 */

void ethernet_enable_interrupt(void)
{
    // Enable DMA interrupts
    //#if INT_ENABLE_NIS
    ETH->DMAIER |= ETH_DMAIER_NISE;         // Normal interrupt summary (master flag ob ueberhaput interrupts ausgeloest werden sollen
    //#endif
    #if INT_ENABLE_AIS
    ETH->DMAIER |= ETH_DMAIER_AISE;         // Abnormal interrupt summary
    #endif


    #if INT_ENABLE_ERI
    ETH->DMAIER |= ETH_DMAIER_ERIE;         // Early receive
    #endif
    #if INT_ENABLE_FBEI
    ETH->DMAIER |= ETH_DMAIER_FBEIE;        // Fatal bus error
    #endif
    #if INT_ENABLE_ETI
    ETH->DMAIER |= ETH_DMAIER_ETIE;         // Early transmit interrupt
    #endif
    #if INT_ENABLE_RWTI
    ETH->DMAIER |= ETH_DMAIER_RWTIE;        // Receive watchdog timeout
    #endif
    #if INT_ENABLE_RPSI
    ETH->DMAIER |= ETH_DMAIER_RPSIE;        // Receive process stopped
    #endif
    #if INT_ENABLE_RBUI
    ETH->DMAIER |= ETH_DMAIER_RBUIE;        // Receive buffer unavailable
    #endif
    //#if INT_ENABLE_RI
    ETH->DMAIER |= ETH_DMAIER_RIE;          // Received
    //#endif
    #if INT_ENABLE_TUI
    ETH->DMAIER |= ETH_DMAIER_TUIE;         // Transmit underflow
    #endif
    #if INT_ENABLE_ROI
    ETH->DMAIER |= ETH_DMAIER_ROIE;         // Receive overflow
    #endif
    #if INT_ENABLE_TJTI
    ETH->DMAIER |= ETH_DMAIER_TJTIE;        // Transmit jabber timeout
    #endif
    #if INT_ENABLE_TBUI
    ETH->DMAIER |= ETH_DMAIER_TBUIE;        // Transmit buffer unavailable
    #endif
    #if INT_ENABLE_TPSI
    ETH->DMAIER |= ETH_DMAIER_TPSIE;        // Transmit process stopped
    #endif
    #if INT_ENABLE_TI
    ETH->DMAIER |= ETH_DMAIER_TIE;          // Transmit
    #endif
    
    
    //configure NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    //select NVIC channel to configure
    NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;//TAMPER_IRQn;
    //set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    //set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    //enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStructure);
}



int ethernet_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    ETH_InitTypeDef ETH_InitStructure;

    //printf("connect LAN cable and press Enter\n\r");
    //while('\r' != getchar());


    /* Enable ETHERNET clock  */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);


    /* Enable GPIOs clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |    RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                            RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE| RCC_APB2Periph_AFIO, ENABLE);

    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);


    /* Get HSE clock = 25MHz on PA8 pin(MCO) */
    /* set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
    RCC_PLL3Config(RCC_PLL3Mul_10);
    /* Enable PLL3 */
    RCC_PLL3Cmd(ENABLE);
    
    /* Wait till PLL3 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
    {;}

    /* Get clock PLL3 clock on PA8 pin */
    RCC_MCOConfig(RCC_MCO_PLL3CLK);

    /* Configure PA2 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //ETH_RMII_MDIO

    /* Configure PC1, PC2 and PC3 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //ETH_RMII_MDC

    /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_11 | //ETH_RMII_TX_EN
                                    GPIO_Pin_12 | //ETH_RMII_TXD0
                                    GPIO_Pin_13;  //ETH_RMII_TXD1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /**************************************************************/
    /*               For Remapped Ethernet pins                   */
    /*************************************************************/

    /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
    GPIO_PinRemapConfig(GPIO_Remap_ETH, DISABLE);

    /* Configure PA0, PA1 and PA3 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 ; //WKUP | ETH_RMII_REF_CLK | ETH_RMII_CRS_DV
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PB10 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // ETH_MII_RX_ER (PB10)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure PC3 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;// ETH_MII_TX_CLK (PC3)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //ETH_RMII_RXD0 | ETH_RMII_RXD1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure); /**/

    /* MCO pin configuration------------------------------------------------- */
    /* Configure MCO (PA8) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // --R17--(R33)--verbunden--> ETH_RMII_REF_CLK
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();


    /* Software reset */
    ETH_SoftwareReset();


    /* Wait for software reset */
    while(ETH_GetSoftwareResetStatus()==SET)
    {;}


    /* ETHERNET Configuration ------------------------------------------------------*/
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
    //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
    //ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_Speed = ETH_Speed_100M;

    unsigned int PhyAddr;
    for(PhyAddr = 1; 32 >= PhyAddr; PhyAddr++) //bin mir immernoch nicht sicher was das hier macht ^^
    {
        if((0x0022 == ETH_ReadPHYRegister(PhyAddr,2))
            && (0x1619 == (ETH_ReadPHYRegister(PhyAddr,3)))) break;
    }

    if(32 < PhyAddr)
    {
        //printf("Ethernet Phy Not Found\n\r");
        return 1;
    }
    
    /* Configure Ethernet */
    if(0 == ETH_Init(&ETH_InitStructure, PhyAddr))
    {
        //printf("Ethernet Initialization Failed\n\r");
        return 1;
    }

    ETH_MACITConfig(ETH_MAC_IT_PMT,ENABLE);
    
    //ethernet_enable_interrupt();
}


void ethernet_deinit(void)
{
    TIM_DeInit(TIM2);

    /* Enable the TIM2 Interrupt */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIM2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}
