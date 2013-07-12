//led zeug

#include "stm32f10x.h"
#include "rgb_led.h"

uint8_t SPI_MASTER_Buffer_Tx[BufferSize];

uint32_t get_rgb_led(uint8_t *mem_addr, uint16_t lednr, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = 0;
    *g = 0;
    *b = 0;

    uint32_t data_r = 0b100100100100100100100100;
    uint32_t data_g = 0b100100100100100100100100;
    uint32_t data_b = 0b100100100100100100100100;


    *(( (uint8_t *)&data_b)+2) = *(mem_addr + lednr * 9+0);
    *(( (uint8_t *)&data_b)+1) = *(mem_addr + lednr * 9+1);
    *(( (uint8_t *)&data_b)+0) = *(mem_addr + lednr * 9+2);


    *(( (uint8_t *)&data_r)+2) = *(mem_addr + lednr * 9+3);
    *(( (uint8_t *)&data_r)+1) = *(mem_addr + lednr * 9+4);
    *(( (uint8_t *)&data_r)+0) = *(mem_addr + lednr * 9+5);

    *(( (uint8_t *)&data_g)+2) = *(mem_addr + lednr * 9+6);
    *(( (uint8_t *)&data_g)+1) = *(mem_addr + lednr * 9+7);
    *(( (uint8_t *)&data_g)+0) = *(mem_addr + lednr * 9+8);

    for(int i = 0; i < 3 ; i++)
    {
        *r |= ( data_r >> (i*3+1-i) ) & (1<<i);
        *g |= ( data_g >> (i*3+1-i) ) & (1<<i);
        *b |= ( data_b >> (i*3+1-i) ) & (1<<i);
    }

    return 0;
}

uint32_t set_rgb_led(uint8_t *mem_addr, uint16_t lednr, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t data_r = 0b100100100100100100100100;
    uint32_t data_g = 0b100100100100100100100100;
    uint32_t data_b = 0b100100100100100100100100;
    for(int i = 0; i < 8 ; i++)
    {
        data_r |= (r & (1<<i)) << (i*3+1-i);
        data_g |= (g & (1<<i)) << (i*3+1-i);
        data_b |= (b & (1<<i)) << (i*3+1-i);
    }
    *(mem_addr + lednr * 9+0) = *(( (uint8_t *)&data_b)+2);
    *(mem_addr + lednr * 9+1) = *(( (uint8_t *)&data_b)+1);
    *(mem_addr + lednr * 9+2) = *(( (uint8_t *)&data_b)+0);

    *(mem_addr + lednr * 9+3) = *(( (uint8_t *)&data_r)+2);
    *(mem_addr + lednr * 9+4) = *(( (uint8_t *)&data_r)+1);
    *(mem_addr + lednr * 9+5) = *(( (uint8_t *)&data_r)+0);

    *(mem_addr + lednr * 9+6) = *(( (uint8_t *)&data_g)+2);
    *(mem_addr + lednr * 9+7) = *(( (uint8_t *)&data_g)+1);
    *(mem_addr + lednr * 9+8) = *(( (uint8_t *)&data_g)+0);
    return 0;
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    set_rgb_led(SPI_MASTER_Buffer_Tx, 0, r, g, b);
    for(int i = 1 ; i < NUMBER_OF_LEDS ; i++)
    {
        memcpy(SPI_MASTER_Buffer_Tx + i*9, SPI_MASTER_Buffer_Tx, 9);
    }
}

void fill_rgb_led_buffer(void)
{
    for(int i = 0 ; i < BufferSize ; i+=3)
    {
        SPI_MASTER_Buffer_Tx[i]   = 0b10010010;
        SPI_MASTER_Buffer_Tx[i+1] = 0b01001001;
        SPI_MASTER_Buffer_Tx[i+2] = 0b00100100;
    }

/*    for(int i = 0 ; i < NUMBER_OF_LEDS ; i++)
    {
        set_rgb_led(&SPI_MASTER_Buffer_Tx[0], i , (i%3==0)?255:0,(i%3==1)?255:0,(i%3==2)?255:0);
//        set_rgb_led(&SPI_MASTER_Buffer_Tx[0], i , 255,255,255);
    }*/

    for(int i = 0 ; i < 17 ; i++)
        SPI_MASTER_Buffer_Tx[NUMBER_OF_LEDS*3*3+i] = 0;
}

void test_led_spi(void)
{
    fill_rgb_led_buffer();

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
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_MASTER_Buffer_Tx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
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

  /* Wait for DMA1 channel4 transfer complete */
  while (!DMA_GetFlagStatus(SPI_MASTER_Tx_DMA_FLAG));
  
  LED_Toggle(2);
  LED_Toggle(1);
}
