#ifndef __RGB_LED_H__
#define __RGB_LED_H__

#include "timer.h"

#define SPI_MASTER                   SPI3   /* SPI pins are remapped by software */
#define SPI_MASTER_CLK               RCC_APB1Periph_SPI3
#define SPI_MASTER_GPIO              GPIOC
#define SPI_MASTER_GPIO_NSS          GPIOA
#define SPI_MASTER_GPIO_CLK          (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA)  
#define SPI_MASTER_PIN_NSS           GPIO_Pin_4
#define SPI_MASTER_PIN_SCK           GPIO_Pin_10
#define SPI_MASTER_PIN_MISO          GPIO_Pin_11
#define SPI_MASTER_PIN_MOSI          GPIO_Pin_12

#define SPI_MASTER_DMA               DMA2
#define SPI_MASTER_DMA_CLK           RCC_AHBPeriph_DMA2
#define SPI_MASTER_Tx_DMA_Channel    DMA2_Channel2
#define SPI_MASTER_Tx_DMA_FLAG       DMA2_FLAG_TC2


struct ledstripe_t 
{
    uint8_t mode;
    uint32_t pos;
    uint8_t data[9*5];
    struct timer led_timer;
} ledstripe;


//#define NUMBER_OF_LEDS 42
#define NUMBER_OF_LEDS (5*30)
#define NUMBER_OF_LEDS (150*2+81*2)
#define BufferSize     (NUMBER_OF_LEDS*9+17)
extern uint8_t SPI_MASTER_Buffer_Tx[BufferSize];


uint32_t set_rgb_led(uint8_t *mem_addr, uint16_t lednr, uint8_t r, uint8_t g, uint8_t b);
void fill_rgb_led_buffer(void);
void test_led_spi(void);

#endif
