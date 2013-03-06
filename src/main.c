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

#if MAIN_CONTROLLER

int Ethernet_Test()
{
    Ethernet_Init();

    /* uIP stack main loop */
    uIPMain(); //ethernet.c

    //wenn uIPMain zu ende --> aufräumen

    TIM_DeInit(TIM2);

    /* Enable the TIM2 Interrupt */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    return 0;
}

#endif //MAIN_CONTROLLER

void ButtonInit(void)
{
    //EXTI structure to init EXT
    EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC structure to set up NVIC controller
    NVIC_InitTypeDef NVIC_InitStructure;
    //Connect EXTI Line to Button Pin
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0); //PortA.0 --> Btn2 | PortD.0 --> CanRX pin (kein Can Interrupt)
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

/*  //Test weise nen haufen scheis anmachen
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_Init(&NVIC_InitStructure);*/
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
    //ButtonInit(); //interrupt init 

    /* LED Init */
    LED_Init();
    LED_On(1);

    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

    //uint8_t send_string[] = {RS232_PRE,0,1,2,RS232_POST};
    uint8_t send_string[50];
    USBD_Init(&USB_OTG_dev,
        USB_OTG_FS_CORE_ID,
        &USR_desc,
        &USBD_CDC_cb,
        &USR_cb);

    CAN_Config();

    VCP_DataTx("Hallo!", 6);

    /* Add your application code here */

    //Btn1 -> SendCan Led2 Toogle
    //Btn2 -> 
    //Ethernet msg format: 'SEND LED X (ON|OFF|xxx)'
    //Alle can nachrichten werden auf USB ausgegeben
    Ethernet_Test();
    //STM_EVAL_GPIOReset();

/*	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS | RCC_AHBPeriph_ETH_MAC |
	RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx , DISABLE);
	RCC_APB2PeriphClockCmd(~0xFFFF0002,DISABLE);
	RCC_APB1PeriphClockCmd(~(0xC10137C0 | RCC_APB1Periph_USART3),DISABLE);

	while (1)
	{
		int ch;
		if(0 < ( ch = getchar()))
		{
			putchar(ch);
		}
	}*/
	LED_On(2);
	
    uint32_t nCount;
    while(1){
        LED_Toggle(1);
        for(nCount = UI32_DELAY_TIME; nCount != 0; nCount--);
    }
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
