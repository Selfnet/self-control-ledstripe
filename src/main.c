/**
******************************************************************************
* @file    Project/Template/main.c
* @author  MCD Application Team
* @version V3.1.0
* @date    06/19/2009
* @brief   Main program body
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
*/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "io-helper.h" //dirks button+led func

#include "ethernet_init.h"

#include "usb_core.h"
#include "usbd_usr.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"

#include "led_pwm.h"
#include "can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define UI32_DELAY_TIME 0x1FFFFF


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t CriticalSecCntr;
USART_InitTypeDef USART_InitStructure;

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void SysTickStart(uint32_t Tick)
{
    RCC_ClocksTypeDef Clocks;
    volatile uint32_t dummy;

    RCC_GetClocksFreq(&Clocks);

    dummy = SysTick->CTRL;
    SysTick->LOAD = (Clocks.HCLK_Frequency/8)/Tick;

    SysTick->CTRL = 1;
}


void SysTickStop(void)
{
    SysTick->CTRL = 0;
}


void test_rgb_timer_init(void)
{
    // Output clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // ** Output (PWM) Ports definieren **
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel =  TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Compute the prescaler value 1x pro sek*/
//    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 100000);
    uint16_t PrescalerValue = (uint16_t) 72000;

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 4;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM1, ENABLE);

    /* TIM IT enable */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);




}

void test_rgb_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    //Timer clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // Output clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    // ** Output (PWM) Ports definieren **
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);



    // ** PWM Timer **
    // Time base layout settings: TIM3 works because f = 72KHz 72M divided by 72000000, we subtract 1 and prescaler (0)
    // 72000000 on share for a period of 1 (999 +1) and get some 72000
    TIM_TimeBaseStructure.TIM_Period = 2;
    TIM_TimeBaseStructure.TIM_Prescaler = 42;//72000000;  //fclk = 72M/72M - 1 = 0 //35
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    //0 = do not share the clock
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up mode

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


    TIM_OCInitTypeDef TIM_OCInitStructure;

    // ** PWM Timer Channel**
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // low state

    //TIM3 CN2
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //PB5
    

    // ** Reload TIM ARR Config **
    TIM_ARRPreloadConfig(TIM4,ENABLE);

    //  Enable the timer
    TIM_Cmd(TIM4, ENABLE);
    
    TIM4->CCR1 = 2;

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel =  TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* TIM IT enable */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

}

void test_led_spi(void)
{


#define BufferSize       (6*3*3+17)

/*  #define SPI_MASTER                   SPI2
  #define SPI_MASTER_CLK               RCC_APB1Periph_SPI2
  #define SPI_MASTER_GPIO              GPIOB
  #define SPI_MASTER_GPIO_CLK          RCC_APB2Periph_GPIOB
//  #define SPI_MASTER_PIN_NSS           GPIO_Pin_4
//  #define SPI_MASTER_PIN_SCK           GPIO_Pin_3
//  #define SPI_MASTER_PIN_MISO          GPIO_Pin_6
  #define SPI_MASTER_PIN_MOSI          GPIO_Pin_15
*/

  #define SPI_MASTER                   SPI3   /* SPI pins are remapped by software */
  #define SPI_MASTER_CLK               RCC_APB1Periph_SPI3
  #define SPI_MASTER_GPIO              GPIOC
  #define SPI_MASTER_GPIO_NSS          GPIOA
  #define SPI_MASTER_GPIO_CLK          (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA)  
  #define SPI_MASTER_PIN_NSS           GPIO_Pin_4
  #define SPI_MASTER_PIN_SCK           GPIO_Pin_10
  #define SPI_MASTER_PIN_MISO          GPIO_Pin_11
  #define SPI_MASTER_PIN_MOSI          GPIO_Pin_12
  
  #define SPI_MASTER_DMA               DMA2
  #define SPI_MASTER_DMA_CLK           RCC_AHBPeriph_DMA2
  #define SPI_MASTER_Tx_DMA_Channel    DMA2_Channel2
  #define SPI_MASTER_Tx_DMA_FLAG       DMA2_FLAG_TC2
  #define SPI_MASTER_DR_Base           0x40013C0C
  //                                SPI3_BASE + 0x0C//0x4000380C 
                                   //  0x4000380C

uint8_t SPI_MASTER_Buffer_Tx[BufferSize] = {
                                             219, 109, 182,  //on
                                             219, 109, 182,  //on
                                             219, 109, 182,  //on
                                             
                                             219, 109, 182,  //on
                                             146, 73 , 36,  //off
                                             146, 73 , 36,  //off
                                             
                                             146, 73 , 36,  //off
                                             219, 109, 182,  //on
                                             146, 73 , 36,  //off
                                             
                                             146, 73 , 36,  //off
                                             146, 73 , 36,  //off
                                             219, 109, 182,  //on
                                             
                                             219, 109, 182,  //on
                                             219, 109, 182,  //on
                                             219, 109, 182,  //on
                                             
                                             219, 109, 182,  //on
                                             146, 73 , 36,  //off
                                             219, 109, 182,  //on
                                             
                                             0,0,0,0,0,
                                             0,0,0,0,0,
                                             0,0,0,0,0,
                                             0,0
                                            };


  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  RCC_AHBPeriphClockCmd(SPI_MASTER_DMA_CLK, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIO clock for SPI_MASTER */
  RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable SPI_MASTER Periph clock */
  RCC_APB1PeriphClockCmd(SPI_MASTER_CLK, ENABLE);

  /* Enable SPI3 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

  /* Configure SPI_MASTER pins: SCK and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);

  /* Configure SPI_MASTER NSS pin */
  GPIO_InitStructure.GPIO_Pin = SPI_MASTER_PIN_NSS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_MASTER_GPIO_NSS, &GPIO_InitStructure);
  
  
  // dma 2 - channel 2 = spi3 tx
  
  DMA_InitTypeDef  DMA_InitStructure;
  /* SPI_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
  DMA_DeInit(SPI_MASTER_Tx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;//SPI3_BASE; //(uint32_t)SPI_SLAVE_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_MASTER_Buffer_Tx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(SPI_MASTER_Tx_DMA_Channel, &DMA_InitStructure);
  
  
  SPI_InitTypeDef  SPI_InitStructure;
  /* SPI_MASTER configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_MASTER, &SPI_InitStructure);

  /* Enable SPI_MASTER NSS output for master mode */
  SPI_SSOutputCmd(SPI_MASTER, ENABLE);


  /* Enable SPI_MASTER */
  SPI_Cmd(SPI_MASTER, ENABLE);

  /* Enable SPI_SLAVE Rx request */
  SPI_I2S_DMACmd(SPI_MASTER, SPI_I2S_DMAReq_Tx, ENABLE);

  /* Enable DMA1 Channel4 */
  DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, ENABLE);

while(1)
{
  /* Wait for DMA1 channel4 transfer complete */
  while (!DMA_GetFlagStatus(SPI_MASTER_Tx_DMA_FLAG));
  
  LED_Toggle(2);
  LED_Toggle(1);
}


}

/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
    SysTickStart(0xFFFF);

    /* Button Init */
    //button_init();


    /* LED Init */
    LED_init();
    LED_On(1);

    LED_On(2);
    LED_Off(1);
    //test_rgb_timer_init();
    //test_rgb_pwm_init();
    test_led_spi();
    LED_On(2);
    LED_On(1);
    while(1);


    // initalizes the pwm timer and start autofading mode for RGB-Leds
    //enable_PWM();

    // initialize CAN-Bus and enable CAN Interrupts
    CAN_config();

    // initialize USB VirtualComPort
    /*uint8_t send_string[50];
    USBD_Init(&USB_OTG_dev,
        USB_OTG_FS_CORE_ID,
        &USR_desc,
        &USBD_CDC_cb,
        &USR_cb);
    // send greetings to the COM-Port ;)
    VCP_DataTx("Hallo!\n", 7);*/

    // initialize Ethernet (Pins, Speed, Interrupt, etc...)
    ethernet_init();





    /* uIP stack main loop */
    uIPMain(); //lokated in ethernet.c

    //wenn uIPMain zu ende --> aufräumen (sollte eigl. nie passieren aber ist ordentlicher)
    ethernet_deinit();
}


#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
* @}
*/


/******************* (C) COPYRIGHT 2009 STMicroelectronics ***
    **END OF FILE****/
