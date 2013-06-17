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
#include "stm32f10x.h"

#include "includes.h"
#include "io-helper.h" //dirks button+led func

#include "stm32f10x.h"



//#include "led_pwm.h"
#include "can.h"

#include "rgb_led.h"
#include "timer.h"
#include "clock.h"

#include <string.h>
#include <stdlib.h>

#include "game_dotcatching.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define UI32_DELAY_TIME 0x1FFFFF

#define RAND_MAX 0xFFFF

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//uint32_t CriticalSecCntr;
//USART_InitTypeDef USART_InitStructure;

//__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

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


    uint8_t ps2data[10];
    uint8_t ps2data_ptr = 0;

/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
    SysTickStart(0xFFFF);

    // Button Init
    button_init();

    // LED Init
    LED_init();
    LED_On(1);

    LED_On(2);
    LED_Off(1);
    //test_rgb_timer_init();
    //test_rgb_pwm_init();
    test_led_spi();
    //LED_On(2);
    LED_On(1);

    flash_function_1();

    // initialize CAN-Bus and enable CAN Interrupts
    CAN_config();

    init_clock();

    struct timer sec_timer,fast_timer;
    timer_set(&fast_timer, 100); //1x pro sec wird gesynced
    timer_set(&sec_timer, 1000); //1x pro sec wird gesynced

    LED_Off(1);
    
    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 0  , 64,0,0);
    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 1  , 255,0,0);
    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 2  , 255,64,0);
    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 3  , 255,255,0);
    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], 4  , 255,255,64);

//    set_rgb_led(&SPI_MASTER_Buffer_Tx[0], NUMBER_OF_LEDS  , 255,255,64);

/*    set_rgb_led(&ledstripe.data[0], 0  , 0,0,64);*/
/*    set_rgb_led(&ledstripe.data[0], 1  , 0,0,64);*/
/*    set_rgb_led(&ledstripe.data[0], 2  , 0,0,64);*/
/*    set_rgb_led(&ledstripe.data[0], 3  , 0,0,64);*/
/*    set_rgb_led(&ledstripe.data[0], 4  , 0,0,64);*/

    set_rgb_led(&ledstripe.data[0], 4  , 64,0,0);
    set_rgb_led(&ledstripe.data[0], 3  , 255,0,0);
    set_rgb_led(&ledstripe.data[0], 2  , 255,64,0);
    set_rgb_led(&ledstripe.data[0], 1  , 255,255,0);
    set_rgb_led(&ledstripe.data[0], 0  , 255,255,64);

    ledstripe.mode = 0x10;
    ledstripe.pos  = 0;
    ledstripe.data[0] = 0;

    LED_Off(1);
    LED_Off(2);


    init_game(&game);
    while(1)
    {
        if(timer_expired(&sec_timer))
        {
            timer_reset(&sec_timer);
            LED_Toggle(1);

            if(--game.next_mode_change <= 0)
            {
                game.mode = (game.mode == 1)?2:1;
                game.next_mode_change = rand()%25+3;
            }
        }
        if(timer_expired(&fast_timer))
        {
            timer_reset(&fast_timer);

            if(ledstripe.mode == 0x4)
            {
                ++ledstripe.pos;
                memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[(ledstripe.pos+1)*9], (uint8_t*)&SPI_MASTER_Buffer_Tx[ledstripe.pos*9], 9*5);
            }
            //cicle
            else if(ledstripe.mode == 0x5)
            {
                int i;
                uint8_t tmp[9];
                if(Button_GetState(1))
                    memcpy((uint8_t*)&tmp[0], (uint8_t*)&SPI_MASTER_Buffer_Tx[BufferSize-17-9], 9);
                else
                    set_rgb_led(&tmp[0], 0, 0, 0, 0);

                if(Button_GetState(2))
                    memcpy((uint8_t*)&tmp[0], (uint8_t*)&SPI_MASTER_Buffer_Tx[BufferSize-17-9], 9);
                for(i=0; i < BufferSize-17-9; i++) {
                    memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[i+9], (uint8_t*)&SPI_MASTER_Buffer_Tx[i], 9);
                } 
                //memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[9], (uint8_t*)&SPI_MASTER_Buffer_Tx[0], BufferSize-17-9);
                memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[1], (uint8_t*)&tmp[0], 9);
            }
            //random
            else if(ledstripe.mode == 0x6)
            {
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledstripe.pos , 0,0,0);
                ledstripe.pos = (rand())%NUMBER_OF_LEDS;
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos , ledstripe.data[0],ledstripe.data[1],ledstripe.data[2]);
//                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledstripe.pos , 255,255,255);
            }
            //moving dot
            else if(ledstripe.mode == 0x7) //TODO
            {
                if(ledstripe.pos > NUMBER_OF_LEDS*2+8)
                {
                    ledstripe.pos = 0;
                }
                else if(ledstripe.pos > NUMBER_OF_LEDS+4)
                {
                    //set last led
                    if(ledstripe.pos-5 <= NUMBER_OF_LEDS+4)
                        memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[BufferSize-17], (uint8_t*)&ledstripe.data[ (ledstripe.pos-1-NUMBER_OF_LEDS-4)*9 ], 9);
                    //save first led
                    else if(ledstripe.pos+1 > NUMBER_OF_LEDS*2)
                    // 42*2+8
                    // 92 = 4
                    // 91 = 3
                    // 90 = 2 | 89 = 1 | 88 = 0 
                        memcpy((uint8_t*)&ledstripe.data[ (ledstripe.pos-NUMBER_OF_LEDS*2-4)*9 ], (uint8_t*)&SPI_MASTER_Buffer_Tx[0], 9);

                    memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[0], (uint8_t*)&SPI_MASTER_Buffer_Tx[9], NUMBER_OF_LEDS*9);//BufferSize-17);

                    if(ledstripe.pos+6 < NUMBER_OF_LEDS*2)
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS, 0,0,0);
                    //memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[BufferSize-17-9*5], (uint8_t*)&tmp[0], 9*5);
                }
                else if(ledstripe.pos <= NUMBER_OF_LEDS+9)
                {
/*/                    uint8_t tmp[9*5];
//      41,42, 43,44,45,46 | 41 -> 0 , 42 -> 1
                    //save last led
                    if(ledstripe.pos+2 > NUMBER_OF_LEDS)
                    {
                        LED_Toggle(2);
                        if( ((ledstripe.pos-NUMBER_OF_LEDS+1)) < 5 )
                            memcpy((uint8_t*)&ledstripe.data[ ((ledstripe.pos-NUMBER_OF_LEDS+1))*9 ], (uint8_t*)&SPI_MASTER_Buffer_Tx[NUMBER_OF_LEDS*9-9], 9);
                    }*/
                    // set first led
                    if(ledstripe.pos < 5)
                    {
                        memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[0], (uint8_t*)&ledstripe.data[ (ledstripe.pos)*9 ], 9);
                        memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[9], (uint8_t*)&SPI_MASTER_Buffer_Tx[0], 9*5);
                    }
                    else
                    {
                        if(ledstripe.pos == 5)
                            set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos-5 , 0,0,0);

                        memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[(ledstripe.pos-3)*9], (uint8_t*)&SPI_MASTER_Buffer_Tx[(ledstripe.pos-4)*9], 9*5);
                        //letzte led ausschalten
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos-4 , 0,0,0);

                    /*else
                    {
                        //ist teilweise auserhalb des bereichs
                        memcpy((uint8_t*)&ledstripe.data[ ((ledstripe.pos-NUMBER_OF_LEDS+1))*9 ], (uint8_t*)&SPI_MASTER_Buffer_Tx[NUMBER_OF_LEDS*9-9], 9);
                        memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[(ledstripe.pos+1)*9], (uint8_t*)&SPI_MASTER_Buffer_Tx[ledstripe.pos*9], 9*(ledstripe.pos-NUMBER_OF_LEDS+1));
                    }*/

                    }
                }
                ++ledstripe.pos;
            }
            else if(ledstripe.mode == 0x8)
            {
                while(Button_GetState(1));
                for(uint32_t i = 0 ; i < NUMBER_OF_LEDS ; i++)
                {
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i , 0,0,0);
                }

                ledstripe.pos = 0;
                float t = 0;
                while(!Button_GetState(1))
                {
                    t += 0.001;
                    ledstripe.pos = t;
                    if(ledstripe.pos > 1 && ledstripe.pos < NUMBER_OF_LEDS)
                    {
                        ledstripe.data[0] = rand()%255;
                        ledstripe.data[1] = rand()%255;
                        ledstripe.data[2] = rand()%255;
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos , ledstripe.data[0],ledstripe.data[1],ledstripe.data[2]);
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos-1 , 0,0,0);
                    }
                }
                
                ledstripe.pos = t;
                for(uint32_t i = 0 ; i < ledstripe.pos && i < NUMBER_OF_LEDS ; i++)
                {
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i , ledstripe.data[0]*i/ledstripe.pos,ledstripe.data[1]*i/ledstripe.pos,ledstripe.data[2]*i/ledstripe.pos );
                }
            }
            //bouncing
            else if(ledstripe.mode == 0x0A)
            {
                ledstripe.pos++;
                if(ledstripe.pos >= NUMBER_OF_LEDS*2-1) {
                    ledstripe.pos = 1;
                }
                //uint8_t tmp[9];
                //set_rgb_led(&tmp[0], 0, 0, 0, 0);
                //if(Button_GetState(1))
                //    memcpy((uint8_t*)&tmp[0], (uint8_t*)&SPI_MASTER_Buffer_Tx[BufferSize-17-9], 9);
                if(ledstripe.pos < NUMBER_OF_LEDS) {
                    for(int i = NUMBER_OF_LEDS-2; i >= 0; i--) {
                        memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[(i+1)*9], (uint8_t*)&SPI_MASTER_Buffer_Tx[i*9], 9);
                    }
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , 0 , 0,0,0);
                }
                else {
                    for(int i = 0; i < NUMBER_OF_LEDS-1; i++) {
                        memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[i*9], (uint8_t*)&SPI_MASTER_Buffer_Tx[(i+1)*9], 9);
                    }
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-1 , 0,0,0);
                }
                //memmove((uint8_t*)&SPI_MASTER_Buffer_Tx[9], (uint8_t*)&SPI_MASTER_Buffer_Tx[0], BufferSize-17-9);
                //memcpy((uint8_t*)&SPI_MASTER_Buffer_Tx[0], (uint8_t*)&tmp[0], 9);
            }
            else if(ledstripe.mode == 0x9)
            {
                if(!Button_GetState(1))
                {
                    //while(Button_GetState(1));
                    /*for(uint32_t i = 0 ; i < NUMBER_OF_LEDS ; i++)
                    {
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i , 0,0,0);
                    }*/

                    float t = ledstripe.pos;
                    while(!Button_GetState(1))
                    {
                        t += 0.001;
                        ledstripe.pos = t;
                        if(ledstripe.pos >= 1 && ledstripe.pos < NUMBER_OF_LEDS)
                        {
                            set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos , ledstripe.data[0],ledstripe.data[1],ledstripe.data[2]);
                            set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos-1 , 0,0,0);
                        }
                    }
                    timer_restart(&fast_timer);
                }
                else if(ledstripe.pos > 0)
                {
                    if(ledstripe.pos >= 1 && ledstripe.pos < NUMBER_OF_LEDS)
                    {
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , ledstripe.pos , 0,0,0 );
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , --ledstripe.pos , ledstripe.data[0],ledstripe.data[1],ledstripe.data[2]);
                    }
                    else
                        --ledstripe.pos;
                }
            }
            else if(ledstripe.mode == 0x10)
            {
                game_round();
                /*
                if(game_mode == 0) //reset
                {
                    for(int i = 0 ; i < NUMBER_OF_LEDS ; i++)
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 0,0,0);

                    for(int i = 0 ; i <  pos2_min ; i++)
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-i   , 20,0,0);
                    for(int i = 0 ; i < pos1_min ; i++)
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 0,0,20);

                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , pos1   , 255,0,0);
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-pos2   , 0,0,255);
                    game_mode = 1;
                    next_game_mode_change = rand()%10+15;
                    
                    if(pos1_min > init_center-loosing_score)
                    {
                        ledstripe.data[0] = 0;
                        ledstripe.data[1] = 0;
                        ledstripe.data[2] = 255;
                        ledstripe.mode = 6;
                    }
                    else if(NUMBER_OF_LEDS-pos2_min < init_center+loosing_score)
                    {
                        ledstripe.data[0] = 255;
                        ledstripe.data[1] = 0;
                        ledstripe.data[2] = 0;
                        ledstripe.mode = 6;
                    }

                }
                
                if(!Button_GetState(1))
                {
                    if(pos1 < NUMBER_OF_LEDS && pos1 <= NUMBER_OF_LEDS-pos2)
                    {
                        pos1+=2;
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , pos1   , 255,0,0);
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , pos1-2 , 0,0,0);
                    }
                }
                else
                {
                    if(pos1 > pos1_min)
                    {
                        pos1--;
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , pos1   , 255,0,0);
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , pos1+1 , 0,0,0);
                    }
                }

                if(!Button_GetState(2))
                {
                    if(pos2 < NUMBER_OF_LEDS && pos2 <= NUMBER_OF_LEDS-pos1)
                    {
                        pos2+=2;
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-pos2   , 0,0,255);
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-(pos2-2) , 0,0,0);
                    }
                }
                else
                {
                    if(pos2 > pos2_min)
                    {
                        pos2--;
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-pos2   , 0,0,255);
                        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-(pos2+1) , 0,0,0);
                    }
                }

                if( abs(pos1-(NUMBER_OF_LEDS-pos2)) <= 1 )
                {
                    if( ( game_mode == 1 && (center-pos1) > 0 ) || ( game_mode == 2 && (center-pos1) < 0 ) )
                    {
                        pos2_min += 4;//abs(pos1-center);
                        for(int i = center ; i < NUMBER_OF_LEDS ; i++)
                            set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 100,100,100);
                    }
                    else
                    {
                        pos1_min += 4;//abs((NUMBER_OF_LEDS-pos2)-center);
                        for(int i = 0 ; i < center ; i++)
                            set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 100,100,100);
                    }
                    center=init_center+(rand()%20);
                    game_mode = 0;
                    pos1 = pos1_min;
                    pos2 = pos2_min;
                }

                
                if(game_mode == 1)
                {
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , center-1 , 20,0,0);
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , center+1 , 0,0,20);
                }
                else if(game_mode == 2)
                {
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , center-1 , 0,0,20);
                    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , center+1 , 20,0,0);
                }
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , center , 0,255,0);

                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , init_center-loosing_score , 20,20,0);
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , init_center+loosing_score , 20,20,0); */
            }

/*
            if(ledPos >= NUMBER_OF_LEDS*2-1)
                ledPos = 1;
            else if(ledPos >= NUMBER_OF_LEDS)
            {
                int ledPosBack = NUMBER_OF_LEDS-(ledPos-NUMBER_OF_LEDS);
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPosBack  , 0,0,0);
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPosBack-1, 64,0,0);
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPosBack-2,  255,0,0);
            }
            else
            {
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPos  , 255,0,0);
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPos-1, 64,0,0);
                set_rgb_led(&SPI_MASTER_Buffer_Tx[0], ledPos-2,  0,0,0);
            }
            }
*/
        }
    }
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
