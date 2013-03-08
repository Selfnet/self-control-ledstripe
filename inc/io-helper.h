
#ifndef __IO_HELPER_H__
#define __IO_HELPER_H__

#include "stm32f10x.h"


#define UI32_DELAY_TIME 0x1FFFFF

#define BUTTON_1_PIN    GPIO_Pin_13
#define BUTTON_1_PORT   GPIOC
#define BUTTON_1_CLK    RCC_APB2Periph_GPIOC

#define BUTTON_2_PIN    GPIO_Pin_0
#define BUTTON_2_PORT   GPIOA
#define BUTTON_2_CLK    RCC_APB2Periph_GPIOA

#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01

//gruen
#define LED1_PIN        GPIO_Pin_6  
#define LED1_GPIO_PORT  GPIOC
#define LED1_GPIO_CLK   RCC_APB2Periph_GPIOC



//orange
#define LED2_PIN        GPIO_Pin_7
#define LED2_GPIO_PORT  GPIOC
#define LED2_GPIO_CLK   RCC_APB2Periph_GPIOC


//Buttons
void Button_Init(void);
uint32_t Button_GetState(uint32_t);

// Can functions
void CAN_Config(void);

// Led
void LED_Init(void);
void LED_On(uint32_t lednr);
void LED_Off(uint32_t lednr);
void LED_Toggle(uint32_t lednr);

#endif
