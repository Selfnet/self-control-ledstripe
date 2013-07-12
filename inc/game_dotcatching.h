
#ifndef __GAME_DOTCATCHING_H__
#define __GAME_DOTCATCHING_H__

typedef struct 
{
    float pos;
    int min_pos;
    int max_pos; //NUMBER_OF_LEDS
    int score;
    float speed_forward;
    float speed_backward;
    uint8_t dot_color[3];
    uint8_t score_color[3];
} sPlayer;

typedef struct 
{
    uint8_t mode;
    uint32_t center;
    uint32_t init_center;
    uint32_t loosing_distance;
    uint32_t score_increase;
    uint32_t next_mode_change;
    sPlayer player[2];
} sGame;

extern sGame game;

void init_game(sGame *game);
void new_game_round(sGame *game);
void move_player(sGame *game, int pid, uint32_t button_state);
void check_collision(sGame *game);
void game_round(void);
void update_display(sGame *game);
void display_player(sGame *game);

#endif
