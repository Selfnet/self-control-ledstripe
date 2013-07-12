
#ifndef __LED_FUNCTIONS_H__
#define __LED_FUNCTIONS_H__

#include "rgb_led.h"
#include "io-helper.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

//wird am anfang aufgerufen
void init_function_3(void);

//wird alle x sekunden aufgerufen
void function_3(void);


void function_move_up(void);
void function_move_down(void);
void function_circle(void);


#endif
