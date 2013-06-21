
#include "stm32f10x.h"
#include "includes.h"
#include "stm32f10x.h"

__attribute__ ((section("flash_f1"))) void flash_function_1(uint32_t ticks)
{
    LED_Toggle(1);
}
