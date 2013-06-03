/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "stm32f10x_it.h"

#include "io-helper.h"
#include "can.h"


#include <string.h>

/*
//usb
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
//send
#include "usbd_cdc_vcp.h"*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//usb
/*
extern USB_OTG_CORE_HANDLE                  USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler        (USB_OTG_CORE_HANDLE *pdev);
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler  (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif
*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/******************************************************************************/

#include "clock.h"
void TIM1_UP_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update );
    clock_tick();
}



/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None

* Return         : None
*******************************************************************************/
#include <stdlib.h>

//void TIM6_IRQHandler(void)
//{
//    TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
//}


/******************************************************************************/

#include "rgb_led.h"

/**
  * @brief  This function handles ExternalInterrupt 0 (Port[A-D] Pin0) Handler.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void) //Button2
{
    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
/*        ledstripe.data[0] = rand()%255;
        ledstripe.data[1] = rand()%255;
        ledstripe.data[2] = rand()%255;
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , 0 , ledstripe.data[0],ledstripe.data[1],ledstripe.data[2]);
        set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 0  , rand() % 255,rand() % 255,rand() % 255);*/
    }
    
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI15_10_IRQHandler(void) //Button1
{
    if (EXTI->PR & (1<<13)) {                       // EXTI0 interrupt pending?
        EXTI->PR |= (1<<13);                          // clear pending interrupt
    }
    //ledstripe.mode++;
    //Check if EXTI_Line0 is asserted
    //if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
//        set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 0  , 0,0,64);
    }
    //we need to clear line pending bit manually
    //EXTI_ClearITPendingBit(EXTI_Line13);
}


// *** CAN Interrupt ***
void CAN1_RX0_IRQHandler(void)
{
    prozess_can_it();
    //CAN_ClearFlag(CAN1, CAN_FLAG_FMP0);
    //CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
