
#include "io-helper.h"


/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @retval None
  */
void Button_Init(void)
{
  //Button1
  GPIO_InitTypeDef GPIO_InitStructure_Button1;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(BUTTON_1_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure_Button1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure_Button1.GPIO_Pin = BUTTON_1_PIN;
  GPIO_Init( BUTTON_1_PORT , &GPIO_InitStructure_Button1);

  //Button2
  GPIO_InitTypeDef GPIO_InitStructure_Button2;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(BUTTON_2_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure_Button2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure_Button2.GPIO_Pin = BUTTON_2_PIN;
  GPIO_Init( BUTTON_2_PORT , &GPIO_InitStructure_Button2);
}


/**
  * @brief  Returns the selected Button state.
  * @retval The Button GPIO pin value.
  */
uint32_t Button_GetState(uint32_t btnnr)
{
  if(btnnr == 1)
    return GPIO_ReadInputDataBit(BUTTON_1_PORT, BUTTON_1_PIN); //pullup
  else
    return !GPIO_ReadInputDataBit(BUTTON_2_PORT, BUTTON_2_PIN); //pulldown
}



/**
  * @brief  Configures LED GPIO.
  * @retval None
  */
void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure_Led1;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(LED1_GPIO_CLK, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure_Led1.GPIO_Pin = LED1_PIN;
  GPIO_InitStructure_Led1.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure_Led1.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure_Led1);
  
  GPIO_InitTypeDef  GPIO_InitStructure_Led2;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(LED2_GPIO_CLK, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure_Led2.GPIO_Pin = LED2_PIN;
  GPIO_InitStructure_Led2.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure_Led2.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure_Led2);
}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_On(uint32_t lednr)
{
  if(lednr == 1)
    LED1_GPIO_PORT->BSRR = LED1_PIN;
  else if(lednr == 2)
    LED2_GPIO_PORT->BSRR = LED2_PIN;
}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_Off(uint32_t lednr)
{
  if(lednr == 1)
    LED1_GPIO_PORT->BRR = LED1_PIN;
  else if(lednr == 2)
    LED2_GPIO_PORT->BRR = LED2_PIN;
}


/**
  * @brief  Turns LEDx on.
  * @retval None
  */
void LED_Toggle(uint32_t lednr)
{
  if(lednr == 1)
    LED1_GPIO_PORT->ODR ^= LED1_PIN;
  else if(lednr == 2)
    LED2_GPIO_PORT->ODR ^= LED2_PIN;
}

