/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define UI32_DELAY_TIME 0x1FFFFF

#define BUTTON_1_PIN    GPIO_Pin_13
#define BUTTON_1_PORT   GPIOC
#define BUTTON_1_CLK    RCC_APB2Periph_GPIOC

#define BUTTON_2_PIN    GPIO_Pin_0
#define BUTTON_2_PORT   GPIOA
#define BUTTON_2_CLK    RCC_APB2Periph_GPIOA

#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01


#define LED1_PIN        GPIO_Pin_6
#define LED1_GPIO_PORT  GPIOC
#define LED1_GPIO_CLK   RCC_APB2Periph_GPIOC

#define LED2_PIN        GPIO_Pin_7
#define LED2_GPIO_PORT  GPIOC
#define LED2_GPIO_CLK   RCC_APB2Periph_GPIOC



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure, GPIO_InitStructure_usart;

USART_InitTypeDef USART_InitStructure;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
uint16_t PrescalerValue = 0;

uint16_t CanId = 0x11;

// Can zeug
__IO uint32_t ret = 0; /* for return of the interrupt handling */
volatile TestStatus TestRx;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

void NVIC_Configuration(void);

void TIMER6_Configuration(void);

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


/* Private functions ---------------------------------------------------------*/

int main(void)
{
  uint32_t ui32_delay_time = 0x1FFFFF;
       
  /* Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, DISABLE);  


  /* BEGIN OF USARTx */

  /* USART2 configured as follow:
          - BaudRate = 9260 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure_usart.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure_usart.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure_usart.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure_usart);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure_usart.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure_usart.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure_usart);

  /* USART configuration */
  USART_Init(USART2, &USART_InitStructure);
    
  /* Enable USART */
  USART_Cmd(USART2, ENABLE); 
  
  /* USART3 configured as follow:
          - BaudRate = 19200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 19200;
  
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure_usart.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure_usart.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure_usart.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure_usart);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure_usart.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure_usart.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOD, &GPIO_InitStructure_usart);
  
  /* USART configuration */
  USART_Init(USART3, &USART_InitStructure);
    
  /* Enable USART */
  USART_Cmd(USART3, ENABLE);


  /* END OF USARTx */


  /* Button Init */
  Button_Init();

  /* LED Init */
  LED_Init();
  LED_On(1);
  LED_On(2);

  /* CAN1 Init */
  CAN_Config();

  /* NVIC Configuration */
  NVIC_Configuration();

  Delay(ui32_delay_time);
  while(Button_GetState(1) == KEY_PRESSED)
    CanId = 0x12;
  LED_Off(1);
  LED_Off(2);


  // CAN zeugs
  CanRxMsg RxMessage;

  while (1)
  {
    if(Button_GetState(1) == KEY_PRESSED)
    {
      while(Button_GetState(1) == KEY_PRESSED);

      // CAN zeugs
      CanTxMsg TxMessage;
      // transmit
      TxMessage.StdId=CanId;
      TxMessage.RTR=CAN_RTR_DATA;
      TxMessage.IDE=CAN_ID_STD;
      TxMessage.DLC=2;
      TxMessage.Data[0]=0xaa;
      TxMessage.Data[1]=0xe1;
      CAN_Transmit(CAN1, &TxMessage);
    }
    else if(Button_GetState(2) == KEY_PRESSED)
    {
      while(Button_GetState(2) == KEY_PRESSED);

      // CAN zeugs
      CanTxMsg TxMessage;
      // transmit
      TxMessage.StdId=CanId;
      TxMessage.RTR=CAN_RTR_DATA;
      TxMessage.IDE=CAN_ID_STD;
      TxMessage.DLC=2;
      TxMessage.Data[0]=0xaa;
      TxMessage.Data[1]=0xc2;
      CAN_Transmit(CAN1, &TxMessage);
      
    }

    // receive
    RxMessage.StdId=0x00;
    RxMessage.IDE=CAN_ID_STD;
    RxMessage.DLC=0;
    RxMessage.Data[0]=0x00;
    RxMessage.Data[1]=0x00;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    
    if( RxMessage.StdId != CanId && RxMessage.Data[0] == 0xaa )
    {
      if(RxMessage.Data[1] == 0xe1)
      {
        LED_On(1);
        LED_Off(2);
      }
      Delay(ui32_delay_time/2);
      if(RxMessage.Data[1] == 0xc2)
      {
        LED_On(2);
        LED_Off(1);
      }
    }
    Delay(ui32_delay_time/2);
  }
  while(1)
  {
      /* Insert delay */
    Delay(ui32_delay_time);
  }
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}




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


/**
  * @brief  Configures the CAN

  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
void Initilaize_Canbus(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  uint32_t i = 0;

  /* CAN register init */
  CAN_DeInit(CAN1);

  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM=DISABLE;
  CAN_InitStructure.CAN_ABOM=DISABLE;
  CAN_InitStructure.CAN_AWUM=DISABLE;
  CAN_InitStructure.CAN_NART=DISABLE;
  CAN_InitStructure.CAN_RFLM=DISABLE;
  CAN_InitStructure.CAN_TXFP=DISABLE;
  CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;
  
  /* Baudrate = 125kbps*/
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;
  CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler=48;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber=0;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;  
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;


//  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
}


/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  
  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  /* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);
  
  /* CANx Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 1MBps*/
//  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
//  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
//  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
//  CAN_InitStructure.CAN_Prescaler = 4;
//  CAN_Init(CAN1, &CAN_InitStructure);

  /* Baudrate = 125kbps*/
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;
  CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler=48;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
}



/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{

  /* Enable the TIM6 global Interrupt */
  NVIC_InitTypeDef NVIC_InitStructure_TIM6;
  NVIC_InitStructure_TIM6.NVIC_IRQChannel =  TIM6_IRQn;
  NVIC_InitStructure_TIM6.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure_TIM6.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure_TIM6.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure_TIM6);
  
  /* Enable the CAN RX Interrupt */
  NVIC_InitTypeDef NVIC_InitStructure_CAN;
  NVIC_InitStructure_CAN.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure_CAN.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure_CAN.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure_CAN.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure_CAN);
}

/* OLD ~ Not USED */

void TimerConfigAusMain(void)
{


   /* ---------------------------------------------------------------------------
    TIM9 Configuration: Output Compare Toggle Mode:
    TIM9CLK = SystemCoreClock (72MHz),
    The objective is to get TIM9 counter clock at 24 MHz:
     - Prescaler = (TIM9CLK / TIM9 counter clock) - 1
    CC1 update rate = TIM9 counter clock / CCR1Val = 732.4 Hz
    CC2 update rate = TIM9 counter clock / CCR2Val = 1464.8 Hz
  ----------------------------------------------------------------------------*/
  
  /* TIM6 clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  /* Compute the prescaler value */
  //PrescalerValue = (uint16_t) (SystemCoreClock / 1000) - 1;

  /* Time base configuration */
  //TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  //TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  //TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  //TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM enable counter */
  //TIM_Cmd(TIM6, ENABLE);

  /* TIM IT enable */
  //TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);


}



void TIMER6_Configuration(void)
{
    /* Fills each TIM_TimeBaseInitStruct member with its default value */ 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    /* Set the default configuration */
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0x017C;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

    /* Enable TIM6 */
    TIM_Cmd(TIM6,ENABLE);
    
    /* Enable TIEMR6 interrupt */
    TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
    
    /*Update Interupt (URS bit) Enable */
    TIM_UpdateRequestConfig(TIM6, TIM_UpdateSource_Regular);
}
