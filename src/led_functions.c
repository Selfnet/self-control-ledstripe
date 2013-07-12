
#include "led_functions.h"

//wird am anfang aufgerufen
void init_function_3(void)
{


}

//wird alle x sekunden aufgerufen
void function_3(void)
{
    double pi = 3.141592654, max_speed = 0.1, direction_change_speed = 2*pi / 1000;
    static double foo1 = 0, foo2 = 0, foo3 = 0, speed;
    foo1 += sin(speed) * max_speed;
    foo2 += sin(speed+pi) * max_speed;
    foo3 += cos(speed) * max_speed;
    speed += direction_change_speed;
    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        double red   = sin(0.1*i+foo1);
        double green = sin(0.1*i+foo2);
        double blue  = sin(0.1*i+foo3);
        set_rgb_led(SPI_MASTER_Buffer_Tx, i, red * 255, green * 255, blue * 255);
    }
}




void function_move_up(void)
{
    uint8_t r,g,b;
    get_rgb_led(SPI_MASTER_Buffer_Tx, NUMBER_OF_LEDS-1, &r,&g,&b);
    if(r | g | b)
    {
        ledstripe.mode = 0x42;
    }
    else
    {
        memmove(SPI_MASTER_Buffer_Tx + 9, SPI_MASTER_Buffer_Tx, (NUMBER_OF_LEDS - 1) * 9);
        set_rgb_led(SPI_MASTER_Buffer_Tx, 0, 0, 0, 0);
    }
}


void function_move_down(void)
{
    uint8_t r,g,b;
    get_rgb_led(SPI_MASTER_Buffer_Tx,0, &r,&g,&b);
    if(r | g | b)
    {
        ledstripe.mode = 0x41;
    }
    else
    {
        memmove(SPI_MASTER_Buffer_Tx, SPI_MASTER_Buffer_Tx + 9, (NUMBER_OF_LEDS - 1) * 9);
        set_rgb_led(SPI_MASTER_Buffer_Tx, NUMBER_OF_LEDS-1, 0, 0, 0);
    }
}

void function_circle(void)
{
    uint8_t tmp[9];
    set_rgb_led(tmp, 0, 0, 0, 0);
    if(Button_GetState(1))
        memcpy(tmp, SPI_MASTER_Buffer_Tx + BufferSize-17-9, 9);
        
    for(int i = NUMBER_OF_LEDS-1; i >= 0; i--) {
        memcpy(SPI_MASTER_Buffer_Tx + (i+1)*9, SPI_MASTER_Buffer_Tx + i*9, 9);
    }
    
    if(ledstripe.data[0] != 0)
    {
        set_rgb_led(SPI_MASTER_Buffer_Tx, 1, ledstripe.data[0], ledstripe.data[1], ledstripe.data[2]);
        ledstripe.data[0] = 0;
    }
    
    memcpy(SPI_MASTER_Buffer_Tx, tmp, 9);
}
