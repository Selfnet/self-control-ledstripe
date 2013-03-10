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

#include "usb_core.h"
#include "usbd_usr.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"

#include "led_pwm.h"

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


int Ethernet_Test()
{
    ethernet_init();

    /* uIP stack main loop */
    uIPMain(); //ethernet.c

    //wenn uIPMain zu ende --> aufräumen
    ethernet_deinit();

    return 0;
}

void ButtonInit(void)
{

    // *** Button2 ***

    //EXTI structure to init EXT
    EXTI_InitTypeDef EXTI_InitStructure;
    //Connect EXTI Line to Button Pin
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //PortA.0 --> Btn2 | PortD.0 --> CanRX pin (kein Can Interrupt)
    //Configure Button EXTI line
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    //select interrupt mode
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //generate interrupt on rising edge
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    //enable EXTI line
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //send values to registers
    EXTI_Init(&EXTI_InitStructure);


    //configure NVIC
    //NVIC structure to set up NVIC controller
    NVIC_InitTypeDef NVIC_InitStructure;
    //select NVIC channel to configure
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    //set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    //enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStructure);

    // *** Button1 ***
/*
    // Disable Tamper pin
    BKP_TamperPinCmd(DISABLE);
    // Disable Tamper interrupt
    BKP_ITConfig(DISABLE);
    // Tamper pin active on low level
    BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low);
    // Clear Tamper pin Event(TE) pending flag
    BKP_ClearFlag();
    // Enable Tamper interrupt
    BKP_ITConfig(ENABLE);
    // Enable Tamper pin
    BKP_TamperPinCmd(ENABLE);

*/
    //configure NVIC
    //select NVIC channel to configure
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//TAMPER_IRQn;
    //set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    //set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    //enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStructure);




/// TEST


  //if (__EXTI_USED & (1 << 13)) {                            // EXTI13 used

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
//    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function

#define __AFIO_EXTICR4 0x00000020
    AFIO->EXTICR[3] &= 0xFF0F;                              // clear used pin
    AFIO->EXTICR[3] |= (0x00F0 & __AFIO_EXTICR4);           // set pin to use

#define __EXTI_IMR                0x00002001              //  3
#define __EXTI_EMR                0x00000000              //  4
#define __EXTI_RTSR               0x00000000              //  5
#define __EXTI_FTSR               0x00002001              //  6

    EXTI->IMR       |= ((1 << 13) & __EXTI_IMR);            // unmask interrupt
    EXTI->EMR       |= ((1 << 13) & __EXTI_EMR);            // unmask event
    EXTI->RTSR      |= ((1 << 13) & __EXTI_RTSR);           // set rising edge
    EXTI->FTSR      |= ((1 << 13) & __EXTI_FTSR);           // set falling edge

    //if (__EXTI_INTERRUPTS & (1 << 13)) {                    // interrupt used
      //NVIC->ISER[1] |= (1 << (EXTI15_10_IRQChannel & 0x1F));// enable interrupt EXTI 10..15
    //}
  //} // end EXTI13 used

  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);


}


/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
    /* Button Init */
    Button_Init();
    ButtonInit(); //interrupt init 

    /* LED Init */
    LED_Init();
    LED_On(1);

    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();
    
    enable_PWM();

    CAN_Config();

    //uint8_t send_string[] = {RS232_PRE,0,1,2,RS232_POST};
    uint8_t send_string[50];
    USBD_Init(&USB_OTG_dev,
        USB_OTG_FS_CORE_ID,
        &USR_desc,
        &USBD_CDC_cb,
        &USR_cb);

    VCP_DataTx("Hallo!\n", 7);

    /* Add your application code here */

    //Btn1 -> SendCan Led2 Toogle
    //Btn2 -> 
    //Ethernet msg format: 'SEND LED X (ON|OFF|xxx)'
    //Alle can nachrichten werden auf USB ausgegeben
    Ethernet_Test();

}

/**
* @brief  Retargets the C library printf function to the USART.
* @param  None
* @retval None
*/

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
