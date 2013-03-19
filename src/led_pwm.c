
#include "led_pwm.h"

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
    TIM_TimeBaseStructure.TIM_Prescaler = 0;  //fclk = 72M/72M - 1 = 0
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    //0 = do not share the clock
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up mode

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    led.mode = 5;
    led.r = 256;
    led.g = 256;
    led.b = 256;
    led.time = 256;
    led.std_time = 250;
    fade_rnd_RGB(&led);
//    update_PWM( led.r, led.g, led.b);

    TIM_ARRPreloadConfig(TIM3,ENABLE);
    //  Enable the timer
    TIM_Cmd(TIM3, ENABLE);
    
    init_timer(); //fürs faden
}

void set_RGB(RGB_Led_State *led)
{
    update_PWM( led->r, led->g, led->b);
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
    update_PWM( led->r, led->g, led->b);
}

void fade_rnd_RGB(RGB_Led_State *led)
{
    #define RAND_VAL 7 + 1
    
    // aktuellen wert setzen (wird linear zwischen den einzelnen rgb werten gefaded)
    _update_PWM(    (float)(LED_PWM_LOOKUP_TABLE[ (int)led->r ] + led->target_r * led->time) , 
                    (float)(LED_PWM_LOOKUP_TABLE[ (int)led->g ] + led->target_g * led->time) , 
                    (float)(LED_PWM_LOOKUP_TABLE[ (int)led->b ] + led->target_b * led->time) );
    
    led->time++;
    if(led->time >= led->std_time)
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

        //neues ziel bestimmen
        led->target_r = (float)(LED_PWM_LOOKUP_TABLE[ (int)(led->r + led->change_r) ] - LED_PWM_LOOKUP_TABLE[ (int)led->r ]) / led->std_time;
        led->target_g = (float)(LED_PWM_LOOKUP_TABLE[ (int)(led->g + led->change_g) ] - LED_PWM_LOOKUP_TABLE[ (int)led->g ]) / led->std_time;
        led->target_b = (float)(LED_PWM_LOOKUP_TABLE[ (int)(led->b + led->change_b) ] - LED_PWM_LOOKUP_TABLE[ (int)led->b ]) / led->std_time;

        //die einzelnen schritte im 2048 bereich ausrechnen
        //led->change_r = (LED_PWM_LOOKUP_TABLE[ (int)led->target_r ] - LED_PWM_LOOKUP_TABLE[ (int)led->r ])/led->std_time;
        //led->change_g = (LED_PWM_LOOKUP_TABLE[ (int)led->target_g ] - LED_PWM_LOOKUP_TABLE[ (int)led->g ])/led->std_time;
        //led->change_b = (LED_PWM_LOOKUP_TABLE[ (int)led->target_b ] - LED_PWM_LOOKUP_TABLE[ (int)led->b ])/led->std_time;

        //wieder bei 0 anfangen
        led->time = 0; // zaehlt hoch bis led->std_time;
    }
}


/* IDEE:

beim hoch faden:
(LED_PWM_LOOKUP_TABLE[g+1] - LED_PWM_LOOKUP_TABLE[g]) / zeit für steps
und in jedem step den TIM_OCInitStructure.TIM_Pulse += ((LED_PWM_LOOKUP_TABLE[g+1] - LED_PWM_LOOKUP_TABLE[g]) / tick-zeit)*tick

*/

void update_PWM(int r,int g, int b)
{
    _update_PWM( LED_PWM_LOOKUP_TABLE[ (int)r ] , LED_PWM_LOOKUP_TABLE[ (int)g ] , LED_PWM_LOOKUP_TABLE[ (int)b ]);
//    _update_PWM( r*2048/255 , g*2048/255 , b*2048/255);
}

// set values between 0 and 2047
void _update_PWM(int r,int g, int b)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Setup channel 3 who is led to PB0 (green)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = g;
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // high state
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB0


    // Setup Channel 4 who is led to the PB1 (red)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = r;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB1


    // Setup Channel 2 who is led to the PB5 (blue)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = b;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // low state
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB5

    TIM_ARRPreloadConfig(TIM3,ENABLE);
    //  Enable the timer
    TIM_Cmd(TIM3, ENABLE);
}



