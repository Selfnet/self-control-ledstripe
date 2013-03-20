
#ifndef __LED_PWM_H__
#define __LED_PWM_H__


#include "stm32f10x.h"

#define PWM_STEPS 2048

struct rgb_led_state_t {
    int r;
    int g;
    int b;

    float cur_r;
    float cur_g;
    float cur_b;

    float change_r;
    float change_g;
    float change_b;

    float target_r;
    float target_g;
    float target_b;

    char mode;

    int time;
    int std_time;
    
    void *tmp;
};

typedef struct rgb_led_state_t RGB_Led_State;

//extern RGB_Led_State led;
extern struct rgb_led_state_t led;

void enable_PWM(void);
void _update_PWM(RGB_Led_State *led);
void update_PWM(RGB_Led_State *led);
int convert_color(unsigned char c);
void set_RGB(RGB_Led_State *led);
void fade_rnd_RGB(RGB_Led_State *led);
void start_fade(RGB_Led_State *led);
void fade_RGB(RGB_Led_State *led);



//generate lookup table
//width = 2048.0
//arraysize = 255
//gamma = 2.5
//print [ int(width*(((width/arraysize*(i+1))/width)**gamma)) for i in range(arraysize)]

static int LED_PWM_LOOKUP_TABLE[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31, 32, 34, 36, 38, 39, 41, 43, 45, 47, 50, 52, 54, 56, 59, 61, 64, 66, 69, 71, 74, 77, 80, 82, 85, 88, 92, 95, 98, 101, 104, 108, 111, 115, 118, 122, 126, 130, 133, 137, 141, 145, 150, 154, 158, 162, 167, 171, 176, 180, 185, 190, 195, 200, 205, 210, 215, 220, 225, 231, 236, 242, 247, 253, 259, 265, 271, 276, 283, 289, 295, 301, 308, 314, 321, 327, 334, 341, 348, 355, 362, 369, 376, 383, 390, 398, 405, 413, 421, 429, 436, 444, 452, 461, 469, 477, 486, 494, 503, 511, 520, 529, 538, 547, 556, 565, 574, 584, 593, 603, 612, 622, 632, 642, 652, 662, 672, 683, 693, 703, 714, 725, 735, 746, 757, 768, 780, 791, 802, 814, 825, 837, 849, 860, 872, 884, 896, 909, 921, 933, 946, 959, 971, 984, 997, 1010, 1023, 1037, 1050, 1063, 1077, 1091, 1104, 1118, 1132, 1146, 1160, 1175, 1189, 1204, 1218, 1233, 1248, 1263, 1278, 1293, 1308, 1323, 1339, 1354, 1370, 1386, 1402, 1418, 1434, 1450, 1466, 1483, 1499, 1516, 1533, 1549, 1566, 1584, 1601, 1618, 1635, 1653, 1671, 1688, 1706, 1724, 1742, 1761, 1779, 1797, 1816, 1835, 1853, 1872, 1891, 1910, 1930, 1949, 1968, 1988, 2008, 2028, 2047};

#endif
