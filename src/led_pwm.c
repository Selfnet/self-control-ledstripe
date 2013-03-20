
#include "led_pwm.h"

#include <string.h>
#include "uip.h"

RGB_Led_State led;

void init_timer(void)
{
    /* TIM6 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM9 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel =  TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Compute the prescaler value */
    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 1000) - 1;

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 10;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM6, ENABLE);

    /* TIM IT enable */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /*Update Interupt (URS bit) Enable */
    //TIM_UpdateRequestConfig(TIM6, TIM_UpdateSource_Regular); 
}

void enable_PWM(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    GPIO_InitTypeDef GPIO_InitStructure;
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    // Time base layout settings: TIM3 works because f = 72KHz 72M divided by 72000000, we subtract 1 and prescaler (0)
    // 72000000 on share for a period of 1 (999 +1) and get some 72000
    TIM_TimeBaseStructure.TIM_Period = PWM_STEPS-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 2;  //fclk = 72M/72M - 1 = 0
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    //0 = do not share the clock
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up mode

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);


    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Setup channel 3 who is led to PB0 (green)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = 0;//(int)led->cur_g;
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // high state
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB0


    // Setup Channel 4 who is led to the PB1 (red)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = 0;//(int)led->cur_r;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB1


    // Setup Channel 2 who is led to the PB5 (blue)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB5

    TIM_ARRPreloadConfig(TIM3,ENABLE);
    //  Enable the timer
    TIM_Cmd(TIM3, ENABLE);


    led.mode = 5;
    led.tmp = NULL;
    led.r = 0;
    led.g = 0;
    led.b = 0;
    led.cur_r = 0;
    led.cur_g = 0;
    led.cur_b = 0;
    led.time = 256;
    led.std_time = 50;
    fade_rnd_RGB(&led);

    init_timer(); //fürs faden
}

void set_RGB(RGB_Led_State *led)
{
    if(led->r <= 255 && led->r >= 0 && led->g <= 255 && led->g >= 0 && led->b <= 255 && led->b >= 0)
        update_PWM(led);
}

void start_fade(RGB_Led_State *led)
{
    led->change_r = ((led->target_r - led->r)/led->time);
    led->change_g = ((led->target_g - led->g)/led->time);
    led->change_b = ((led->target_b - led->b)/led->time);
}

void fade_RGB(RGB_Led_State *led)
{
    if(led->time > 1)
    {
        led->r += led->change_r;
        led->g += led->change_g;
        led->b += led->change_b;

        if( (led->change_r > 0 && led->r > led->target_r) || (led->change_r < 0 && led->r < led->target_r) )
            led->r = led->target_r;

        if( (led->change_g > 0 && led->g > led->target_g) || (led->change_g < 0 && led->g < led->target_g) )
            led->g = led->target_g;

        if( (led->change_b > 0 && led->b > led->target_b) || (led->change_b < 0 && led->b < led->target_b) )
            led->b = led->target_b;
        led->time--;
    }
    else
    {
        led->r = led->target_r;
        led->g = led->target_g;
        led->b = led->target_b;
        led->time = 0;
    }
    update_PWM( led );
}


void ftoc(float a, char *s)
{
    int t = (int)a;
    s[3] = (char)(t%10 +48);
    t /= 10;
    s[2] = (char)(t%10 +48);
    t /= 10;
    s[1] = (char)(t%10 +48);
    t /= 10;
    s[0] = (char)(t%10 +48);
    s[4] = '.';
    
    t = (a-(int)a)*100;
    s[6] = (char)(t%10 +48);
    t /= 10;
    s[5] = (char)(t%10 +48);

}

int limit(float v, int min, int max)
{
    if(v > max)
        return max;
    else if( v < min)
        return min;

    return (int)v;
}

void fade_rnd_RGB(RGB_Led_State *led)
{
    #define RAND_VAL 3 + 1
    
    led->time++;
    if( led->time >= led->std_time )
    {
        //fertig mit faden --> auf ziel setzen
        led->r += led->change_r;
        led->g += led->change_g;
        led->b += led->change_b;

        //checken ob ziel innerhalb der werte
        if( led->r >= 255 )
        {
            led->r = 255;
            led->change_r = (rand()% RAND_VAL)*-1;
        }
        else if( led->r <= 0 )
        {
            led->r = 0;
            led->change_r = (rand()% RAND_VAL);
        }

        if( led->g >= 255 )
        {
            led->g = 255;
            led->change_g = (rand()% RAND_VAL)*-1;
        }
        else if( led->g <= 0 )
        {
            led->g = 0;
            led->change_g = (rand()% RAND_VAL);
        }

        if( led->b >= 255 )
        {
            led->b = 255;
            led->change_b = (rand()% RAND_VAL)*-1;
        }
        else if( led->b <= 0 )
        {
            led->b = 0;
            led->change_b = (rand()% RAND_VAL);
        }

        // cur bestimmen
        led->cur_r = LED_PWM_LOOKUP_TABLE[ (int)led->r ];
        led->cur_g = LED_PWM_LOOKUP_TABLE[ (int)led->g ];
        led->cur_b = LED_PWM_LOOKUP_TABLE[ (int)led->b ];

        led->target_r = (float)(LED_PWM_LOOKUP_TABLE[ limit(led->r + led->change_r,0,255) ] - led->cur_r) / led->std_time;
        led->target_g = (float)(LED_PWM_LOOKUP_TABLE[ limit(led->g + led->change_g,0,255) ] - led->cur_g) / led->std_time;
        led->target_b = (float)(LED_PWM_LOOKUP_TABLE[ limit(led->b + led->change_b,0,255) ] - led->cur_b) / led->std_time;

        //wieder bei 0 anfangen
        led->time = 0; // zaehlt hoch bis led->std_time;
    }
    
    // aktuellen wert setzen (wird linear zwischen den einzelnen rgb werten gefaded)
    led->cur_r += led->target_r;
    led->cur_g += led->target_g;
    led->cur_b += led->target_b;

/*
    if( led->tmp != NULL )
    {
        char text[50];
        text[0] = ' ';
        ftoc(led->cur_r, text+1);
        text[8] = ' ';
        ftoc(led->target_r, text+9);
        text[16] = ' ';

        ftoc(led->cur_g, text+17);
        text[24] = ' ';
        ftoc(led->target_g, text+25);
        text[32] = ' ';
        
        ftoc(led->cur_b, text+33);
        text[40] = ' ';
        ftoc(led->target_b, text+41);
        
        text[48] = '\n';
        text[49] = 0;
        struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);
        //strcpy(s->outputbuf , "huhu");
        strcpy(s->outputbuf , text);
    }
*/

    _update_PWM( led );

}

void update_PWM(RGB_Led_State *led)
{
    led->cur_r = LED_PWM_LOOKUP_TABLE[ led->r ];
    led->cur_g = LED_PWM_LOOKUP_TABLE[ led->g ];
    led->cur_b = LED_PWM_LOOKUP_TABLE[ led->b ];
    
    _update_PWM(led);
}


// set values between 0 and 2047
void _update_PWM(RGB_Led_State *led)
{
    /* Set the Capture Compare Register value */
    TIM3->CCR4 = (int)led->cur_r;
    TIM3->CCR3 = (int)led->cur_g;
    TIM3->CCR2 = (int)led->cur_b;
}



