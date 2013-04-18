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
extern void USB_OTGFS1_GlobalHandler(void);

#include "io-helper.h"
#include "uip.h"
#include "led_pwm.h"


#include <string.h>


//usb
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
//send
#include "usbd_cdc_vcp.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//usb
extern USB_OTG_CORE_HANDLE                  USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler        (USB_OTG_CORE_HANDLE *pdev);
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler  (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

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

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_WKUP_IRQHandler(void) //fuer usb
{
    if(USB_OTG_dev.cfg.low_power)
    {
        *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
        SystemInit();
        USB_OTG_UngateClock(&USB_OTG_dev);
    }
    EXTI_ClearITPendingBit(EXTI_Line18);
}

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void) //fuer usb
{
    USBD_OTG_ISR_Handler (&USB_OTG_dev);
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void) //fuer ethernet
{
extern  void Tim2Handler (void);
    Tim2Handler();
}


/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None

* Return         : None
*******************************************************************************/
#include <stdlib.h>

void TIM6_IRQHandler(void) 
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
    if(led.mode == 2) //random target rgb
    {
        led.target_r = rand()%2*255;
        led.target_g = rand()%2*255;
        led.target_b = rand()%2*255;
        led.time = led.std_time;
        start_fade(&led);
        led.mode = 3; //random - fading
    }
    else if(led.mode == 5) //random change
    {
        fade_rnd_RGB(&led);
    }
    else if(led.mode == 4) //stress
    {
        led.r = rand()%255;
        led.g = rand()%255;
        led.b = rand()%255;
        set_RGB(&led);
    }
    else if(led.mode == 9) //debug
        _update_PWM( &led );//set_RGB(&led);
    else
    {
        if(led.time > 0)
        {
            fade_RGB(&led);
            if(led.time == 0 && led.mode == 0)
            {
                // TODO send msg when finished
                struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);
                //strcpy(s->outputbuf , "fading finished");
                send_ascii("fading finished", strlen("fading finished") );
            }
        }
        else if(led.mode == 3)
            led.mode = 2;
    }
}


/******************************************************************************/

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
        send_sync(1);
        LED_On(2);
    }
    
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI15_10_IRQHandler(void) //Button1
{
    if (EXTI->PR & (1<<13)) {                       // EXTI0 interrupt pending?
        EXTI->PR |= (1<<13);                          // clear pending interrupt
    }
    //Check if EXTI_Line0 is asserted
    //if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        //LED_On(1);
        //GPIOB->BSRR = GPIO_Pin_7;
        led.mode = 9;
        led.cur_b += 100;
        led.cur_r = 0;
        led.cur_g = 0;
        if(led.cur_r >= 0 && led.cur_r <= 2047 && led.cur_g >= 0 && led.cur_g <= 2047 && led.cur_b >= 0 && led.cur_b <= 2047)
            ;//_update_PWM( &led );
        else
        {
            LED_Toggle(2);
            led.cur_b = 2048;
        }
        _update_PWM( &led );
    }
    //we need to clear line pending bit manually
    //EXTI_ClearITPendingBit(EXTI_Line15);
}

// TODO
// *** ETH Interrupt ***
void ETH_IRQHandler(void)
{
    #if defined (_DEBUG) && DEBUG_DMA_INT
    debug_printf("DMA Int:");
    #endif

    // Normal interrupt summary
    if ((ETH->DMASR & ETH_DMASR_NIS) == ETH_DMASR_NIS)
    {
        //#if INT_ENABLE_RI
        if ((ETH->DMASR & ETH_DMASR_RS) == ETH_DMASR_RS)
        {
            // Ethernet frame received
            LED_Toggle(1);

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_RS;

            // Process receive interrupt
            /*if (receiveIntHandler)
            {
                receiveIntHandler();
            }*/
        }
        //#endif
        #if INT_ENABLE_TI
        if ((ETH->DMASR & ETH_DMASR_TS) == ETH_DMASR_TS)
        {
            // Ethernet frame sent
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" TS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_TS;
        }
        #endif
        #if INT_ENABLE_TBUI
        if ((ETH->DMASR & ETH_DMASR_TBUS) == ETH_DMASR_TBUS)
        {
            // Transmit buffer unavailable
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" TBUS");
            #endif

            // Clear interrupt flag, transmition is resumed after descriptors have been prepared
            ETH->DMASR = ETH_DMASR_TBUS;
        }
        #endif
        #if INT_ENABLE_ERI
        if ((ETH->DMASR & ETH_DMASR_ERS) == ETH_DMASR_ERS)
        {
            // Early receive
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" ERS");
            #endif

            // Clear interrupt flag. Also cleared automatically by RI
            ETH->DMASR = ETH_DMASR_ERS;
        }
        #endif

        // Clear normal interrupt flag
        ETH->DMASR = ETH_DMASR_NIS;
    }

    // Abnormal interrupt summary
    if ((ETH->DMASR & ETH_DMASR_AIS) == ETH_DMASR_AIS)
    {
        LED_Toggle(2);
    
        #if INT_ENABLE_FBEI
        if ((ETH->DMASR & ETH_DMASR_FBES) == ETH_DMASR_FBES)
        {
            // Fatal bus error
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" FBES");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_FBES;
        }
        #endif
        #if INT_ENABLE_TPSI
        if ((ETH->DMASR & ETH_DMASR_TPSS) == ETH_DMASR_TPSS)
        {
            // Transmit process stopped
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" TPSS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_TPSS;
        }
        #endif
        #if INT_ENABLE_TJTI
        if ((ETH->DMASR & ETH_DMASR_TJTS) == ETH_DMASR_TJTS)
        {
            // Transmit jabber timeout
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" TJTS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_TJTS;
        }
        #endif
        #if INT_ENABLE_ROI
        if ((ETH->DMASR & ETH_DMASR_ROS) == ETH_DMASR_ROS)
        {
            // Receive overflow
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" ROS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_ROS;
        }
        #endif
        #if INT_ENABLE_TUI
        if ((ETH->DMASR & ETH_DMASR_TUS) == ETH_DMASR_TUS)
        {
            // Transmit underflow
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" TUS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_TUS;
        }
        #endif
        #if INT_ENABLE_RBUI
        if ((ETH->DMASR & ETH_DMASR_RBUS) == ETH_DMASR_RBUS)
        {
            // Receive buffer unavailable
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" RBUS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_RBUS;
        }
        #endif
        #if INT_ENABLE_RPSI
        if ((ETH->DMASR & ETH_DMASR_RPSS) == ETH_DMASR_RPSS)
        {
            // Receive process stopped
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" RPSS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_RPSS;
        }
        #endif
        #if INT_ENABLE_RWTI
        if ((ETH->DMASR & ETH_DMASR_RWTS) == ETH_DMASR_RWTS)
        {
            // Receive watchdog timeout
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" RWTS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_RWTS;
        }
        #endif
        #if INT_ENABLE_ETI
        if ((ETH->DMASR & ETH_DMASR_ETS) == ETH_DMASR_ETS)
        {
            // Early transmit interrupt
            #if defined (_DEBUG) && DEBUG_DMA_INT
            debug_printf(" ETS");
            #endif

            // Clear interrupt flag
            ETH->DMASR = ETH_DMASR_ETS;
        }
        #endif

        // Clear abnormal interrupt flag
        ETH->DMASR = ETH_DMASR_AIS;
    }
}



// *** CAN Interrupt ***
void CAN1_RX0_IRQHandler(void)
{
    prozess_can_it();
    //CAN_ClearFlag(CAN1, CAN_FLAG_FMP0);
    //CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
