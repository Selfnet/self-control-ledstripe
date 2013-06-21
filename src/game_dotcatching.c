
#include "stm32f10x.h"

#include "includes.h"

#include <string.h>
#include <stdlib.h>

#include "rgb_led.h"

#include "game_dotcatching.h"

void init_game(sGame *game)
{
    game->mode = 1;
    game->center = 160;
    game->init_center = 160;
    game->loosing_distance = 28;
    game->score_increase = 4;
    game->next_mode_change = rand()%10+40;



    game->player[0].min_pos = game->init_center*2 - NUMBER_OF_LEDS;
    game->player[0].pos = game->player[0].min_pos;
    game->player[0].max_pos = NUMBER_OF_LEDS-1;
    game->player[0].score = 0;
    game->player[0].speed_forward   = 2.0;
    game->player[0].speed_backward  = 1.0;

    game->player[0].dot_color[0]    = 255;
    game->player[0].dot_color[1]    = 0;
    game->player[0].dot_color[2]    = 0;

    game->player[0].score_color[0]  = 0;
    game->player[0].score_color[1]  = 0;
    game->player[0].score_color[2]  = 20;


    game->player[1].min_pos = 1;
    game->player[1].pos = game->player[1].min_pos;
    game->player[1].max_pos = NUMBER_OF_LEDS-1;
    game->player[1].score = 0;
    game->player[1].speed_forward   = 2.0;
    game->player[1].speed_backward  = 1.0;
    
    game->player[1].dot_color[0]    = 0;
    game->player[1].dot_color[1]    = 0;
    game->player[1].dot_color[2]    = 255;

    game->player[1].score_color[0]  = 20;
    game->player[1].score_color[1]  = 0;
    game->player[1].score_color[2]  = 0;
    new_game_round(game);
}


void new_game_round(sGame *game)
{
    //alles auf schwarz setzen
    for(int i = 0 ; i < NUMBER_OF_LEDS ; i++)
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 0,0,0);

    //min pos der spiler setzen
    for(int i = 0 ; i < game->player[1].min_pos ; i++)
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS-i  ,  game->player[1].score_color[0], game->player[1].score_color[1], game->player[1].score_color[2]);
    for(int i = 0 ; i < game->player[0].min_pos ; i++)
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i                 ,  game->player[0].score_color[0], game->player[0].score_color[1], game->player[0].score_color[2]);

    //display player dots
    display_player(game);

    game->mode = 1;
    game->next_mode_change = rand()%10+15;

    //check if player 1 has lost
    if(game->player[0].min_pos > game->init_center - game->loosing_distance)
    {
        ledstripe.data[0] = game->player[1].dot_color[0];
        ledstripe.data[1] = game->player[1].dot_color[1];
        ledstripe.data[2] = game->player[1].dot_color[2];
        ledstripe.mode = 6;
    }
    else if(NUMBER_OF_LEDS - game->player[1].min_pos < game->init_center + game->loosing_distance)
    {
        ledstripe.data[0] = game->player[0].dot_color[0];
        ledstripe.data[1] = game->player[0].dot_color[1];
        ledstripe.data[2] = game->player[0].dot_color[2];
        ledstripe.mode = 6;
    }
}

//move and display player dot
void move_player(sGame *game, int pid, uint32_t button_state)
{
    if(button_state == 1)
    {
        if(game->player[pid].pos < game->player[pid].max_pos)// && p1.pos <= p1.max_pos-p2.pos)
        {
            game->player[pid].pos += game->player[pid].speed_forward;
            if(pid == 0)
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->player[pid].pos - game->player[pid].speed_forward , 0,0,0);
            else
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS - (game->player[pid].pos - game->player[pid].speed_forward) , 0,0,0);
        }
    }
    else
    {
        if(game->player[pid].pos > game->player[pid].min_pos)
        {
            game->player[pid].pos -= game->player[pid].speed_backward;
            if(pid == 0)
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->player[pid].pos + game->player[pid].speed_backward , 0,0,0);
            else
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS - (game->player[pid].pos + game->player[pid].speed_backward) , 0,0,0);


        }
    }
}

void display_player(sGame *game)
{
    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->player[0].pos ,                 game->player[0].dot_color[0], game->player[0].dot_color[1], game->player[0].dot_color[2]);
    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , NUMBER_OF_LEDS - game->player[1].pos, game->player[1].dot_color[0], game->player[1].dot_color[1], game->player[1].dot_color[2]);
}

void check_collision(sGame *game)
{
    if( abs(game->player[0].pos - (NUMBER_OF_LEDS - game->player[1].pos)) <= 1 )
    {
        if( ( game->mode == 1 && (game->center - game->player[0].pos) > 0 ) || ( game->mode == 2 && (game->center - game->player[0].pos) < 0 ) )
        {
            game->player[1].min_pos += game->score_increase;
            for(int i = game->center ; i < NUMBER_OF_LEDS ; i++)
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 100,100,100);
        }
        else
        {
            game->player[0].min_pos += game->score_increase;
            for(int i = 0 ; i < game->center ; i++)
                set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , i   , 100,100,100);
        }
        game->center= game->init_center + (rand()%20);
        game->mode = 0;
        game->player[0].pos = game->player[0].min_pos;
        game->player[1].pos = game->player[1].min_pos;
    }
}

void update_display(sGame *game)
{
    if(game->mode == 1)
    {
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->center-1 , 20,0,0);
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->center+1 , 0,0,20);
    }
    else if(game->mode == 2)
    {
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->center-1 , 0,0,20);
        set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->center+1 , 20,0,0);
    }
    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->center , 0,255,0);

    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->init_center - game->loosing_distance , 20,20,0);
    set_rgb_led( (uint8_t*)&SPI_MASTER_Buffer_Tx[0] , game->init_center + game->loosing_distance , 20,20,0);
}

void game_round(void)
{
    move_player(&game, 0, !Button_GetState(1));
    move_player(&game, 1, !Button_GetState(2));

    display_player(&game);

    if(game.mode == 0)
        new_game_round(&game);
    else
        check_collision(&game);

    update_display(&game);
}
