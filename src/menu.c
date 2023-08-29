#include "states.h"
#include "resources.h"
#include <strings.h>

#define MENU_OPTION_1 "NEW GAME"
#define MENU_OPTION_2 "BEST PLAYERS"

static void draw_menu_options();

const short MENU_OPTIONS_SIZE = 2;
const char* MENU_OPTIONS[] = {MENU_OPTION_1, MENU_OPTION_2};
short menu_selected_option = 0;

Sprite* sprite_logo;

void menu_create()
{
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
    PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));

    VDP_loadTileSet(initial_bkg.tileset, 1,  DMA);
    PAL_setPalette(PAL1, initial_bkg.palette->data, DMA);

    VDP_setTileMapEx(BG_A, initial_bkg.tilemap, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 0, 0, 0, 40, 40, DMA);

    PAL_setPalette(PAL2, logo.palette->data, DMA);
    sprite_logo = SPR_addSprite(&logo, 152, 60, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(sprite_logo, 0);

    draw_menu_options();
}

void menu_destroy()
{
    XGM_stopPlay();
    SPR_releaseSprite(sprite_logo);
    SPR_update();
}

void menu_update()
{
    SPR_update();
}

void menu_joystick(u16 joy, u16 changed, u16 state)
{
    if (state & BUTTON_START || state & BUTTON_A)
    {
        if (menu_selected_option == 0)
        {
            change_state(STATE_GAME);
        }
        else
        {
            change_state(STATE_RECORDS);
        }
    }
    else if (state & BUTTON_UP || state & BUTTON_DOWN)
    {
        if (state & BUTTON_UP)
        {
            menu_selected_option--;
        }
        else
        {
            menu_selected_option++;
        }

        menu_selected_option = max(0, menu_selected_option);
        menu_selected_option = min(MENU_OPTIONS_SIZE-1, menu_selected_option);

        draw_menu_options();
    }
}

void draw_menu_options()
{
    for (int idx = 0; idx < MENU_OPTIONS_SIZE; idx++)
    {
        char option[64] = {0};

        VDP_clearTextLine(17+2*idx);

        if (menu_selected_option == idx)
        {
            sprintf(option, "[%s]", MENU_OPTIONS[idx]);
        }
        else
        {
            sprintf(option, "%s", MENU_OPTIONS[idx]);
        }
        VDP_drawText(option, 20 - strlen(option)/2, 17+2*idx);
    }
}