
#ifndef __LED_PWM_H__
#define __LED_PWM_H__


#include "stm32f10x.h"

struct rgb_led_state_t {
  float r;
  float g;
  float b;

  float change_r;
  float change_g;
  float change_b;

  float target_r;
  float target_g;
  float target_b;

  char mode;

  int time;
};

typedef struct rgb_led_state_t  RGB_Led_State;


void enable_PWM(void);
void update_PWM(int r,int g, int b);
int convert_color(unsigned char c);
void set_RGB(RGB_Led_State *led);
void start_fade(RGB_Led_State *led);
void fade_RGB(RGB_Led_State *led);

extern RGB_Led_State led;

#endif
