/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : clock-arch.c
 *    Description : Implementation of architecture-specific clock functionality
 *
 *    History :
 *    1. Date        : October 4, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
**************************************************************************/
#include "clock.h"

volatile clock_time_t Ticks;

/*************************************************************************
 * Function Name: clock_tick
 * Parameters: none
 *
 * Return: none
 *
 * Description: TIM2 interrupt handler
 *
 *************************************************************************/
void clock_tick(void)
{
  ++Ticks;
}

/*************************************************************************
 * Function Name: clock_time
 * Parameters: none
 *
 * Return: none
 *
 * Description: The current clock time, measured in system ticks
 *
 *************************************************************************/
clock_time_t clock_time(void)
{
  return(Ticks);
}


void init_clock(void)
{
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
//    uint16_t PrescalerValue = (uint16_t) (8);
    uint16_t PrescalerValue = (uint16_t) (64);

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 100;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM1, ENABLE);

    /* TIM IT enable */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}
