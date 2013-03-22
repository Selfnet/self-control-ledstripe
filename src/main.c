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


/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
    /* Button Init */
    button_init();

    /* LED Init */
    LED_init();
    LED_On(1);

    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

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
