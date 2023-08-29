#include "states.h"
#include "resources.h"
#include "records.h"

/*******************************
 * Definições                  *
 *******************************/
#define DEMO_MODE FALSE

#define SCREEN_WIDTH 320

#define INITIAL_SPEED 2
#define SPEED_STEP    0.5
// #define INITIAL_SPEED 0
// #define SPEED_STEP    0

/* Gameover */
#define GAMEOVER_MSG_LINE 12
#define GAMEOVER_OPTION_1 "NEW GAME"
#define GAMEOVER_OPTION_2 "EXIT"

const short GAMEOVER_MENU_OPTIONS_SIZE = 2;
const char* GAMEOVER_MENU_OPTIONS[] = {GAMEOVER_OPTION_1, GAMEOVER_OPTION_2};
short gameover_menu_selected_option = 0;

/* Pause */
#define PAUSE_MSG_LINE 11
#define PAUSE_OPTION_1 "CONTINUE"
#define PAUSE_OPTION_2 "NEW GAME"
#define PAUSE_OPTION_3 "EXIT"

const short PAUSE_MENU_OPTIONS_SIZE = 3;
const char* PAUSE_MENU_OPTIONS[] = {PAUSE_OPTION_1, PAUSE_OPTION_2, PAUSE_OPTION_3};
short pause_menu_selected_option = 0;

/* Ranking */
typedef struct
{
    char letter;
    int x;
} t_key;

t_key keys_lines[3][10] = {
    {{'Q',80},{'W',96},{'E',112},{'R',128},{'T',144},{'Y',160},{'U',176},{'I',192},{'O',208},{'P',224}},
    {{'A',88},{'S',104},{'D',120},{'F',136},{'G',152},{'H',168},{'J',184},{'K',200},{'L',216},{0}},
    {{'Z',104},{'X',120},{'C',136},{'V',152},{'B',168},{'N',184},{'M',200},{0},{0},{0}}
};
const short keys_lines_size[] = {10, 9, 7};

short key_line = 0;
short key_col  = 0;

Sprite* keys_sprite[RECORD_NAME_SIZE-1] = {0};
Sprite* key_sel_sprite = NULL;

char new_ranking[RECORD_NAME_SIZE] = {0};

/* Áudios */
#define KICK_EFFECT   64
#define STEP_EFFECT   65
#define SPLASH_EFFECT 66
#define BONUS_EFFECT  67

/* Game state */
#define GAME_NONE    0
#define GAME_RUNNING 1
#define GAME_PAUSE   2
#define GAME_OVER    3
#define GAME_RECORD  4

#define STATUS_LINE 1
#define SCORE_MSG_STR  "SCORE"

short game_state = GAME_NONE;
long score = 0;

// Controle do srcoll horizontal
fix16 scrollspeed = FIX16(INITIAL_SPEED);
fix16 offset = FIX16(0);

// Período do dia (manhã, tarde e noite)
unsigned short period = 0;

// Posição do jogador na tela
Sprite* player = NULL;
fix16 player_x = FIX16(320);
fix16 player_y = FIX16(144);
short player_direction = 0;
short player_timer = 0;

fix16 player_x_speed = FIX16(2);
fix16 player_y_speed = FIX16(0);

#define PLAYER_RUNNING 0
#define PLAYER_JUMPING 1
#define PLAYER_CATCHED 2
#define PLAYER_STOPPED 3
#define PLAYER_WET     4

// Posição do boss na tela
Sprite* boss = NULL;
fix16 boss_x = FIX16(0);
fix16 boss_y = FIX16(64);
bool boss_visible = TRUE;
unsigned short boss_timer = 0;

#define BOSS_X_CATCH 64

// Controle de salto
#define FLOOR_HEIGHT 160
#define PLAYER_WIDTH  16
#define PLAYER_HEIGHT 16

fix16 gravity = FIX16(0.4);
bool jumping = FALSE;
bool elevated = FALSE;
unsigned short  wet_level = 0;
fix16 elevated_y = FIX16(0);

typedef struct {
    int x;
    int y;
} t_point;

/* Obstáculos */
#define MAX_OBSTACLES   8
#define NUM_OBSTACLES_TYPES 4

#define OBSTACLE_SINGLE 0
#define OBSTACLE_DOUBLE 1
#define OBSTACLE_STEP   2
#define OBSTACLE_WATER  3

#define OBSTACLE_SINGLE_W  32
#define OBSTACLE_SINGLE_H  32
#define OBSTACLE_DOUBLE_W  32
#define OBSTACLE_DOUBLE_H  64
#define OBSTACLE_STEP_W    64
#define OBSTACLE_STEP_H    64
#define OBSTACLE_WATER_W   48
#define OBSTACLE_WATER_H   16

#define COLISION_BLOCK 1
#define COLISION_WATER 2

typedef struct
{
    int w;
    int h;
    t_point polygons[2][4];
    unsigned short num_polygons;
    int x_offset;
    int y_offset;
    SpriteDefinition sprite;
    unsigned short crashed_anim;
    unsigned short colision_type;
} t_obstacle_type;

typedef struct
{
    Sprite* obstacle;
    fix16 obstacle_x;
    fix16 obstacle_y;
    t_obstacle_type* type;
    bool crashed;
    bool hit;
} t_obstacle;

t_obstacle_type obstacles_types[NUM_OBSTACLES_TYPES];
t_obstacle obstacles[MAX_OBSTACLES] = {0};
unsigned short num_obstacles = 0;
fix16 last_obstacle_x = FIX16(0);

/* Bônus */
#define WATER_PENALTY -10

#define MAX_BONUS   8
#define NUM_BONUS_TYPES 1
#define BONUS_SCORE 100

#define BONUS_FISH 0

#define BONUS_FISH_W  16
#define BONUS_FISH_H  16

#define MIN_BONUS_H   64

typedef struct
{
    int w;
    int h;
    t_point polygons[2][4];
    unsigned short num_polygons;
    int x_offset;
    int y_offset;
    SpriteDefinition sprite;
} t_bonus_type;

typedef struct
{
    Sprite* bonus;
    fix16 bonus_x;
    fix16 bonus_y;
    t_bonus_type* type;
} t_bonus;

#define NUM_BONUS_LEVELS 5
unsigned short bonus_levels[] = {0, 10, 20, 30, 40};

t_bonus_type bonus_types[NUM_BONUS_TYPES];
t_bonus bonus[MAX_BONUS] = {0};
unsigned short num_bonus = 0;
fix16 last_bonus_x = FIX16(0);
unsigned short bonus_timer = 0;

/* Level */
#define NUM_LEVELS 6

typedef struct
{
    short obstacles_rand[10];
    short bonus_rand[10];
} t_level;

unsigned int level = 1;
t_level levels[NUM_LEVELS];

static void reset_game();
static void pause_game();
static void unpause_game();
static void show_ranking_input();
static void update_ranking_input();
static void hide_ranking_input();
static void draw_pause_menu_options();
static void draw_gameover_menu_options();
static t_level* get_level();
static void load_obstacles();
static void load_bonus();
static void handle_colisions(int, int);
static void update_score(int);
static void display_score(bool);
static void update_boss();
static void update_obstacles();
static void update_bonus();
static void end_game();
static void light_day();
static void middle_day();
static void dark_night();

/*******************************
 * Funções Globais             *
 *******************************/
void game_create()
{
    setRandomSeed(getTick());

    /* Obstáculos */
    obstacles_types[0].w = OBSTACLE_SINGLE_W;
    obstacles_types[0].h = OBSTACLE_SINGLE_H;
    obstacles_types[0].x_offset = 0;
    obstacles_types[0].y_offset = 0;
    obstacles_types[0].sprite   = single_box;
    obstacles_types[0].crashed_anim  = 1;
    obstacles_types[0].colision_type = COLISION_BLOCK;
    obstacles_types[0].num_polygons = 1;
    obstacles_types[0].polygons[0][0].x = 0;
    obstacles_types[0].polygons[0][0].y = OBSTACLE_SINGLE_H;
    obstacles_types[0].polygons[0][1].x = OBSTACLE_SINGLE_W;
    obstacles_types[0].polygons[0][1].y = OBSTACLE_SINGLE_H;
    obstacles_types[0].polygons[0][2].x = OBSTACLE_SINGLE_W;
    obstacles_types[0].polygons[0][2].y = 0;
    obstacles_types[0].polygons[0][3].x = 0;
    obstacles_types[0].polygons[0][3].y = 0;

    obstacles_types[1].w = OBSTACLE_DOUBLE_W;
    obstacles_types[1].h = OBSTACLE_DOUBLE_H;
    obstacles_types[1].x_offset = 0;
    obstacles_types[1].y_offset = 0;
    obstacles_types[1].sprite   = double_box;
    obstacles_types[1].crashed_anim  = 1;
    obstacles_types[1].colision_type = COLISION_BLOCK;
    obstacles_types[1].num_polygons = 1;
    obstacles_types[1].polygons[0][0].x = 0;
    obstacles_types[1].polygons[0][0].y = OBSTACLE_DOUBLE_H;
    obstacles_types[1].polygons[0][1].x = OBSTACLE_DOUBLE_W;
    obstacles_types[1].polygons[0][1].y = OBSTACLE_DOUBLE_H;
    obstacles_types[1].polygons[0][2].x = OBSTACLE_DOUBLE_W;
    obstacles_types[1].polygons[0][2].y = 0;
    obstacles_types[1].polygons[0][3].x = 0;
    obstacles_types[1].polygons[0][3].y = 0;

    obstacles_types[2].w = OBSTACLE_STEP_W;
    obstacles_types[2].h = OBSTACLE_STEP_H;
    obstacles_types[2].x_offset = 0;
    obstacles_types[2].y_offset = 0;
    obstacles_types[2].sprite   = step_box;
    obstacles_types[2].crashed_anim  = 1;
    obstacles_types[2].colision_type = COLISION_BLOCK;
    obstacles_types[2].num_polygons = 2;
    obstacles_types[2].polygons[0][0].x = 0;
    obstacles_types[2].polygons[0][0].y = OBSTACLE_SINGLE_H;
    obstacles_types[2].polygons[0][1].x = OBSTACLE_SINGLE_W;
    obstacles_types[2].polygons[0][1].y = OBSTACLE_SINGLE_H;
    obstacles_types[2].polygons[0][2].x = OBSTACLE_SINGLE_W;
    obstacles_types[2].polygons[0][2].y = 0;
    obstacles_types[2].polygons[0][3].x = 0;
    obstacles_types[2].polygons[0][3].y = 0;
    obstacles_types[2].polygons[1][0].x = OBSTACLE_STEP_W/2;
    obstacles_types[2].polygons[1][0].y = OBSTACLE_DOUBLE_H;
    obstacles_types[2].polygons[1][1].x = OBSTACLE_STEP_W/2 + OBSTACLE_DOUBLE_W;
    obstacles_types[2].polygons[1][1].y = OBSTACLE_DOUBLE_H;
    obstacles_types[2].polygons[1][2].x = OBSTACLE_STEP_W/2 + OBSTACLE_DOUBLE_W;
    obstacles_types[2].polygons[1][2].y = 0;
    obstacles_types[2].polygons[1][3].x = OBSTACLE_STEP_W/2;
    obstacles_types[2].polygons[1][3].y = 0;

    obstacles_types[3].w = OBSTACLE_WATER_W;
    obstacles_types[3].h = OBSTACLE_WATER_H;
    obstacles_types[3].x_offset = 0;
    obstacles_types[3].y_offset = OBSTACLE_WATER_H;
    obstacles_types[3].sprite   = water;
    obstacles_types[3].crashed_anim  = 0;
    obstacles_types[3].colision_type = COLISION_WATER;
    obstacles_types[3].num_polygons = 1;
    obstacles_types[3].polygons[0][0].x = 0;
    obstacles_types[3].polygons[0][0].y = OBSTACLE_WATER_H;
    obstacles_types[3].polygons[0][1].x = OBSTACLE_WATER_W;
    obstacles_types[3].polygons[0][1].y = OBSTACLE_WATER_H;
    obstacles_types[3].polygons[0][2].x = OBSTACLE_WATER_W;
    obstacles_types[3].polygons[0][2].y = 0;
    obstacles_types[3].polygons[0][3].x = 0;
    obstacles_types[3].polygons[0][3].y = -1;

    /* Bônus */
    bonus_types[0].w = BONUS_FISH_W;
    bonus_types[0].h = BONUS_FISH_H;
    bonus_types[0].sprite = fish;
    bonus_types[0].num_polygons = 1;
    bonus_types[0].polygons[0][0].x = 0;
    bonus_types[0].polygons[0][0].y = BONUS_FISH_H;
    bonus_types[0].polygons[0][1].x = BONUS_FISH_W;
    bonus_types[0].polygons[0][1].y = BONUS_FISH_H;
    bonus_types[0].polygons[0][2].x = BONUS_FISH_W;
    bonus_types[0].polygons[0][2].y = 0;
    bonus_types[0].polygons[0][3].x = 0;
    bonus_types[0].polygons[0][3].y = 0;

    /* Levels */
    memcpy(levels[0].obstacles_rand, (short[]) {0, -1, 0, 3, -1, -1, -1, -1, -1, -1}, 10*sizeof(short));
    memcpy(levels[0].bonus_rand, (short[]) {0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 10*sizeof(short));

    memcpy(levels[1].obstacles_rand, (short[]) {0, 0, 0, 1, 1, 3, -1, -1, -1, -1}, 10*sizeof(short));
    memcpy(levels[1].bonus_rand, (short[]) {0, -1, -1, -1, -1, -1, -1, -1, -1, 0}, 10*sizeof(short));

    memcpy(levels[2].obstacles_rand, (short[]) {0, 0, 0, 1, 1, 2, 3, -1, -1, -1}, 10*sizeof(short));
    memcpy(levels[2].bonus_rand, (short[]) {0, -1, -1, -1, -1, 0, -1, -1, -1, 0}, 10*sizeof(short));

    memcpy(levels[3].obstacles_rand, (short[]) {0, 0, 1, 1, 2, 2, 3, 1, -1, -1}, 10*sizeof(short));
    memcpy(levels[3].bonus_rand, (short[]) {0, -1, 0, -1, -1, 0, -1, -1, -1, 0}, 10*sizeof(short));

    memcpy(levels[4].obstacles_rand, (short[]) {0, 0, 1, 1, 2, 2, 3, 2, -1, -1}, 10*sizeof(short));
    memcpy(levels[4].bonus_rand, (short[]) {0, -1, 0, -1, -1, 0, -1, 0, -1, 0}, 10*sizeof(short));

    memcpy(levels[5].obstacles_rand, (short[]) {0, 0, 1, 1, 2, 2, 3, 2, 1, -1}, 10*sizeof(short));
    memcpy(levels[5].bonus_rand, (short[]) {0, -1, 0, 0, -1, 0, -1, 0, -1, 0}, 10*sizeof(short));

    reset_game();
}

void game_destroy()
{
    XGM_stopPlay();
    for (int idx = 0; idx < MAX_OBSTACLES; idx++)
    {
        if (obstacles[idx].obstacle != NULL)
        {
            SPR_releaseSprite(obstacles[idx].obstacle);
            obstacles[idx].obstacle = NULL;
            num_obstacles--;
        }
    }

    for (int idx = 0; idx < MAX_BONUS; idx++)
    {
        if (bonus[idx].bonus != NULL)
        {
            SPR_releaseSprite(bonus[idx].bonus);
            bonus[idx].bonus = NULL;
            num_bonus--;
        }
    }

    if (player) SPR_releaseSprite(player);
    if (boss)   SPR_releaseSprite(boss);
    SPR_update();
}

void game_joystick(u16 joy, u16 changed, u16 state)
{
    if (game_state == GAME_PAUSE)
    {
        if (state & BUTTON_START)
        {
            if (pause_menu_selected_option == 0)
            {
                unpause_game();
            }
            else if (pause_menu_selected_option == 1)
            {
                reset_game();
            } 
            else if (pause_menu_selected_option == 2)
            {
                change_state(STATE_MENU);
            }
        }
        else if (state & BUTTON_UP || state & BUTTON_DOWN)
        {
            if (state & BUTTON_UP)
            {
                pause_menu_selected_option--;
            }
            else
            {
                pause_menu_selected_option++;
            }

            pause_menu_selected_option = max(0, pause_menu_selected_option);
            pause_menu_selected_option = min(PAUSE_MENU_OPTIONS_SIZE-1, pause_menu_selected_option);

            draw_pause_menu_options();
        }
    }
    else if (game_state == GAME_RECORD)
    {
        if (state & BUTTON_START)
        {
            if (strlen(new_ranking) > 0)
            {
                insert_record(new_ranking, score);
            }

            game_state = GAME_OVER;
            hide_ranking_input();
            gameover_menu_selected_option = 0;
            draw_gameover_menu_options();
        }
        else if (state & BUTTON_UP)
        {
            key_line--;
            key_line = max(0, key_line);
        }
        else if (state & BUTTON_DOWN)
        {
            key_line++;
            key_line = min(2, key_line);
            key_col = min(keys_lines_size[key_line]-1, key_col);
        }
        else if (state & BUTTON_LEFT)
        {
            key_col--;
            key_col = max(0, key_col);
        }
        else if (state & BUTTON_RIGHT)
        {
            key_col++;
            key_col = min(keys_lines_size[key_line]-1, key_col);
        }
        else if (state & BUTTON_A)
        {
            if (strlen(new_ranking) < RECORD_NAME_SIZE-1)
            {
                new_ranking[strlen(new_ranking)] = keys_lines[key_line][key_col].letter;
            }
        }
        else if (state & BUTTON_C)
        {
            if (strlen(new_ranking) > 0)
            {
                new_ranking[strlen(new_ranking)-1] = 0;
            }
        }
    }
    else if (game_state == GAME_OVER)
    {
        if (state & BUTTON_START)
        {
            if (gameover_menu_selected_option == 0)
            {
                reset_game();
            }
            else if (gameover_menu_selected_option == 1)
            {
                change_state(STATE_MENU);
            }
        }
        else if (state & BUTTON_UP || state & BUTTON_DOWN)
        {
            if (state & BUTTON_UP)
            {
                gameover_menu_selected_option--;
            }
            else
            {
                gameover_menu_selected_option++;
            }

            gameover_menu_selected_option = max(0, gameover_menu_selected_option);
            gameover_menu_selected_option = min(GAMEOVER_MENU_OPTIONS_SIZE-1, gameover_menu_selected_option);

            draw_gameover_menu_options();
        }
    }
    else if (game_state == GAME_RUNNING)
    {
        if (state & BUTTON_START)
        {
            pause_game();
        }
        else if (state & BUTTON_A) 
        {
            if (!jumping && (!elevated || elevated_y == player_y) && wet_level == 0)
            {
                elevated = FALSE;
                elevated_y = FIX16(0);
                jumping = TRUE;
                player_y_speed = FIX16(-8);
                SPR_setAnim(player, PLAYER_JUMPING);
            }
        }

        if (wet_level == 0)
        {
            if (state & BUTTON_RIGHT) 
            {
                player_x_speed = FIX16(1);
                player_direction = 1;
                if (jumping == FALSE) SPR_setAnim(player, PLAYER_RUNNING);
                SPR_setHFlip(player, FALSE);
            }
            else if (state & BUTTON_LEFT) 
            {
                player_x_speed = fix16Add(fix16Neg(scrollspeed), FIX16(-1));
                player_direction = -1;
                if (jumping == FALSE) SPR_setAnim(player, PLAYER_RUNNING);
                SPR_setHFlip(player, TRUE);
            }
            else
            {
                player_x_speed = fix16Neg(scrollspeed);
                player_direction = 0;
                if (!jumping) SPR_setAnim(player, PLAYER_STOPPED);
            }
        }
    }
}

void game_update()
{
    if (game_state == GAME_RUNNING)
    {
        int last_player_x = fix16ToInt(player_x);
        int last_player_y = fix16ToInt(player_y);

        offset = fix16Add(offset, fix16Neg(scrollspeed));
        if (fix16ToInt(offset) <= -256)
        {
            offset = FIX16(0);
        }
        VDP_setHorizontalScroll(BG_B, fix16ToInt(offset));

        load_obstacles();
        load_bonus();

        update_obstacles();
        update_bonus();

        period++;

        if (jumping || elevated) {
            player_y_speed = fix16Add(player_y_speed, gravity);
            player_y = fix16Add(player_y, player_y_speed);
        }

        player_x = fix16Add(player_x, player_x_speed);
        last_player_x -= fix16ToInt(scrollspeed);

        if (fix16ToInt(player_x) < 0)
        {
            player_x = FIX16(0);
        }
        else if (fix16ToInt(player_x) > SCREEN_WIDTH - PLAYER_WIDTH)
        {
            player_x = FIX16(SCREEN_WIDTH - PLAYER_WIDTH);
        }

        boss_x = FIX16(-48 + 200 - fix16ToInt(player_x));
        boss_x = FIX16(max(-48, fix16ToInt(boss_x)));
        boss_x = FIX16(min(16, fix16ToInt(boss_x)));

        if (!DEMO_MODE && fix16ToInt(player_x) <= BOSS_X_CATCH && fix16ToInt(player_y) >= 64)
        {
            end_game();
        }
        else
        {
            handle_colisions(last_player_x, last_player_y);

            SPR_setPosition(player, fix16ToInt(player_x), fix16ToInt(player_y));
            SPR_setPosition(boss, fix16ToInt(boss_x), fix16ToInt(boss_y));

            update_boss();

            if ((jumping || elevated) && fix16ToInt(player_y) + PLAYER_HEIGHT >= (FLOOR_HEIGHT))
            {
                elevated = FALSE;
                elevated_y = FIX16(0);
                jumping = FALSE;
                player_y_speed = FIX16(0);
                player_y = intToFix16(FLOOR_HEIGHT - PLAYER_HEIGHT);
                SPR_setAnim(player, fix16ToInt(player_x_speed) == fix16ToInt(fix16Neg(scrollspeed)) ? (wet_level == 0 ? PLAYER_STOPPED : PLAYER_WET) : PLAYER_RUNNING);
            }

            if (wet_level > 0)
            {
                wet_level--;
                if (wet_level == 0)
                {
                    SPR_setAnim(player, PLAYER_STOPPED);
                }
            }

            if (period % 1000 == 0)
            {
                level++;
            }

            if (period % 10 == 0)
            {
                update_score(level);
            }

            if (period == 1000)
            {
                middle_day();
            } 
            else if (period == 2000) 
            {
                dark_night();
                scrollspeed = fix16Add(scrollspeed, FIX16(SPEED_STEP));
                if (player_direction == -1)
                {
                    player_x_speed = fix16Add(fix16Neg(scrollspeed), FIX16(-1));
                }
            } 
            else if (period == 3000) 
            {
                period = 0;
                light_day();
            }
        }

        if (++bonus_timer % 9 == 0) bonus_timer = 0;

        player_timer++;
        if ((wet_level == 0 && player_timer >= 3) || (wet_level > 0 && player_timer >= 9))
        {
            SPR_nextFrame(player);
            player_timer = 0;
        }
        if (++boss_timer % 9 == 0)
        {
            SPR_nextFrame(boss);
            if (boss_timer >= 27)
            {
                boss_timer = 0;
                if (boss_visible) {
                    XGM_startPlayPCM(STEP_EFFECT, 10, SOUND_PCM_CH2);
                }
            }
        }
    }
    else if (game_state == GAME_RECORD)
    {
        update_ranking_input();
    }

    SPR_update();
}

/*******************************
 * Funções Locais              *
 *******************************/
t_level* get_level()
{
    return &levels[min(level, NUM_LEVELS) - 1];
}

void reset_game()
{
    game_destroy();

    period = 0;
    offset = FIX16(0);
    scrollspeed = FIX16(INITIAL_SPEED);
    player_x_speed = fix16Neg(scrollspeed);
    player_y_speed = FIX16(0);
    player_x = FIX16(260);
    player_y = FIX16(144);
    player_timer = 0;
    boss_x = FIX16(-48);
    boss_y = FIX16(64);
    boss_timer = 0;
    boss_visible = FALSE;
    gravity = FIX16(0.4);
    jumping = FALSE;
    elevated = FALSE;
    wet_level = 0;
    elevated_y = FIX16(0);
    num_obstacles = 0;
    score = 0;
    level = 1;

    VDP_loadTileSet(floor.tileset, 1,  DMA);
	VDP_loadTileSet(wall.tileset,  65,  DMA);
    VDP_loadTileSet(black.tileset, 129, DMA);
    VDP_loadTileSet(keyboard.tileset, 130, DMA);
    VDP_loadTileSet(rec_legend.tileset, 330, DMA);
    VDP_loadTileSet(new_record.tileset, 378, DMA);

    PAL_setPalette(PAL0, single_box.palette->data, DMA);
	PAL_setPalette(PAL1, giant.palette->data, DMA);
    PAL_setPalette(PAL2, nenem.palette->data, DMA);
    PAL_setPalette(PAL3, keyboard.palette->data, DMA);

	VDP_setTileMapEx(BG_B, floor.tilemap, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1),   0, 20, 0, 0, 64,  1, DMA);
    VDP_setTileMapEx(BG_B, wall.tilemap, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 65),   0, 21, 0, 0, 64,  7, DMA);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    VDP_setHorizontalScroll(BG_B, fix16ToInt(offset));

    player = SPR_addSprite(&nenem, fix16ToInt(player_x), fix16ToInt(player_y), TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(player, PLAYER_STOPPED);
    
    boss = SPR_addSprite(&giant, fix16ToInt(boss_x), fix16ToInt(boss_y), TILE_ATTR(PAL1, 0, FALSE, FALSE));
    SPR_setAnim(boss, 0);

    memset(obstacles, 0, sizeof(obstacles));
    memset(bonus, 0, sizeof(bonus));

    SPR_setAnim(player, PLAYER_STOPPED);
    SPR_setAnim(boss, 0);

    XGM_setPCM(KICK_EFFECT,   kick,   sizeof(kick));
    XGM_setPCM(STEP_EFFECT,   step,   sizeof(step));
    XGM_setPCM(SPLASH_EFFECT, splash, sizeof(splash));
    XGM_setPCM(BONUS_EFFECT,  ring,   sizeof(ring));

    VDP_clearPlane(BG_A, TRUE);

    display_score(TRUE);

    XGM_startPlay(music);

    game_state = GAME_RUNNING;

    light_day();
}

void pause_game()
{
    game_state = GAME_PAUSE;
    pause_menu_selected_option = 0;
    draw_pause_menu_options();
    XGM_pausePlay();
}

void unpause_game()
{
    game_state = GAME_RUNNING;
    for (int idx = 0; idx < PAUSE_MENU_OPTIONS_SIZE; idx++)
    {
        VDP_clearTextLine(PAUSE_MSG_LINE+2*idx);
    }
    XGM_resumePlay();
}

void show_ranking_input()
{
    key_col = 0;
    key_line = 0;
    memset(new_ranking, 0, RECORD_NAME_SIZE);

    VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL3, -1, FALSE, FALSE, 129), 8, 7, 24, 14);
    VDP_setTileMapEx(BG_A, keyboard.tilemap, TILE_ATTR_FULL(PAL3, -1, FALSE, FALSE, 130), 10, 9, 0, 0, 20, 10, DMA);
    VDP_setTileMapEx(BG_A, rec_legend.tilemap, TILE_ATTR_FULL(PAL3, -1, FALSE, FALSE, 330), 8, 20, 0, 0, 24, 2, DMA);
    VDP_setTileMapEx(BG_A, new_record.tilemap, TILE_ATTR_FULL(PAL2, -1, FALSE, FALSE, 378), 12, 3, 0, 0, 16, 4, DMA);

    for (int idx = 0; idx < RECORD_NAME_SIZE - 1; idx++)
    {
        keys_sprite[idx] = SPR_addSprite(&keys, 80 + idx * 16, 72, TILE_ATTR(PAL3, -1, FALSE, FALSE));
        SPR_setDepth(keys_sprite[idx], SPR_MIN_DEPTH);
        SPR_setAnim(keys_sprite[idx], 0);
    }
    key_sel_sprite = SPR_addSprite(&key_sel, 80, 104, TILE_ATTR(PAL3, -1, FALSE, FALSE));
    SPR_setDepth(key_sel_sprite, SPR_MIN_DEPTH);
}

void update_ranking_input()
{
    for (int idx = 0; idx < RECORD_NAME_SIZE - 1; idx++)
    {
        SPR_setAnim(keys_sprite[idx], new_ranking[idx] == 0 ? 0 : (int)(new_ranking[idx])-64);
    }
    SPR_setPosition(key_sel_sprite, keys_lines[key_line][key_col].x, 104 + key_line * 16);
}

void hide_ranking_input()
{
    VDP_clearPlane(BG_A, TRUE);
    display_score(TRUE);
    if (key_sel_sprite)
    {
        SPR_releaseSprite(key_sel_sprite);
        key_sel_sprite = NULL;
    }
    for (int idx = 0; idx < RECORD_NAME_SIZE - 1; idx++)
    {
        if (keys_sprite[idx])
        {
            SPR_releaseSprite(keys_sprite[idx]);
            keys_sprite[idx] = NULL;
        }
    }
}

void draw_pause_menu_options()
{
    for (int idx = 0; idx < PAUSE_MENU_OPTIONS_SIZE; idx++)
    {
        char option[64] = {0};

        VDP_clearTextLine(PAUSE_MSG_LINE+2*idx);

        if (pause_menu_selected_option == idx)
        {
            sprintf(option, "[%s]", PAUSE_MENU_OPTIONS[idx]);
        }
        else
        {
            sprintf(option, "%s", PAUSE_MENU_OPTIONS[idx]);
        }
        VDP_drawText(option, 20 - strlen(option)/2 , PAUSE_MSG_LINE+2*idx);
    }
}

void draw_gameover_menu_options()
{
    for (int idx = 0; idx < GAMEOVER_MENU_OPTIONS_SIZE; idx++)
    {
        char option[64] = {0};

        VDP_clearTextLine(GAMEOVER_MSG_LINE+2*idx);

        if (gameover_menu_selected_option == idx)
        {
            sprintf(option, "[%s]", GAMEOVER_MENU_OPTIONS[idx]);
        }
        else
        {
            sprintf(option, "%s", GAMEOVER_MENU_OPTIONS[idx]);
        }
        VDP_drawText(option, 20 - strlen(option)/2 , GAMEOVER_MSG_LINE+2*idx);
    }
}

void update_score(int to_add)
{
    long old_score = score;
    score = max(0, score+to_add);
    display_score(old_score > score);
}

void display_score(bool clear)
{
    char score_msg[20] = {0};
    sprintf(score_msg, "%ld", score);
    if (clear)
    {
        VDP_clearText(30, STATUS_LINE, 10);
        VDP_clearText(30, STATUS_LINE + 1, 10);
        VDP_drawText(SCORE_MSG_STR, 40 - strlen(SCORE_MSG_STR) - 1, STATUS_LINE);
    }
    VDP_drawText(score_msg, 40 - strlen(score_msg) - 1, STATUS_LINE + 1);
}

bool point_inside_polygon(t_point polygon[], int n, t_point point, t_point base_point) {
    int intersections = 0;

    for (int i = 0, j = n - 1; i < n; j = i++) {
        t_point p1 = {polygon[i].x + base_point.x, polygon[i].y + base_point.y};
        t_point p2 = {polygon[j].x + base_point.x, polygon[j].y + base_point.y};

        if (((p1.y >= point.y && p2.y < point.y) || (p2.y >= point.y && p1.y < point.y)) &&
            (point.x < (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x))
            intersections++;
    }

    return (intersections % 2 == 1);
}

void handle_colisions(int last_player_x, int last_player_y)
{
    t_point top_left;
    t_point bottom_left;
    t_point top_right;
    t_point bottom_right;
    int p_x;
    int p_y;

    p_x = fix16ToInt(player_x);
    p_y = fix16ToInt(player_y);
    
    top_left.x     = p_x;
    top_left.y     = p_y;
    bottom_left.x  = p_x;
    bottom_left.y  = p_y + PLAYER_HEIGHT;
    top_right.x    = p_x + PLAYER_WIDTH - 1;
    top_right.y    = p_y;
    bottom_right.x = p_x + PLAYER_WIDTH - 1;
    bottom_right.y = p_y + PLAYER_HEIGHT;

    for (int idx = 0; idx < MAX_OBSTACLES; idx++)
    {
        t_obstacle* obs = &obstacles[idx];
        if (obs->obstacle != NULL && !obs->crashed&& !obs->hit)
        {
            t_point base_point = {fix16ToInt(obs->obstacle_x), fix16ToInt(obs->obstacle_y)};

            for (int pidx = 0; pidx < obs->type->num_polygons; pidx++)
            {
                if (point_inside_polygon(obs->type->polygons[pidx], 4, bottom_right, base_point) ||
                    point_inside_polygon(obs->type->polygons[pidx], 4, top_right, base_point) ||
                    point_inside_polygon(obs->type->polygons[pidx], 4, bottom_left, base_point) ||
                    point_inside_polygon(obs->type->polygons[pidx], 4, top_left, base_point))
                {
                    if (obs->type->colision_type == COLISION_BLOCK)
                    {
                        if (fix16ToInt(player_y_speed) > 0 && (elevated || (jumping && last_player_y < base_point.y)) && (p_x + PLAYER_WIDTH/2) >= base_point.x && p_x + PLAYER_WIDTH/2 <= base_point.x + obs->type->w)
                        {
                            player_y = FIX16(base_point.y - PLAYER_HEIGHT);
                            player_y_speed = FIX16(1);
                            elevated_y = player_y;

                            if (jumping)
                            {
                                jumping = FALSE;
                                elevated = TRUE;
                                SPR_setAnim(player, fix16ToInt(player_x_speed) == fix16ToInt(fix16Neg(scrollspeed)) ? PLAYER_STOPPED : PLAYER_RUNNING);
                            }
                        }
                        else if (fix16ToInt(player_y_speed) < 0 && jumping && last_player_y > base_point.y && (p_x + PLAYER_WIDTH/2) >= base_point.x && p_x + PLAYER_WIDTH/2 <= base_point.x + obs->type->w)
                        {
                            player_y = FIX16(last_player_y);
                            player_y_speed = FIX16(0);
                        }
                        else
                        {
                            fix16 next_x = fix16Add(obs->obstacle_x, FIX16(obs->type->w));
                            fix16 last_x = fix16Sub(obs->obstacle_x, FIX16(PLAYER_WIDTH));

                            last_x = fix16Add(last_x, FIX16(obs->type->polygons[pidx][0].x));
                            player_x = abs(fix16ToInt(player_x) - fix16ToInt(next_x)) < abs(fix16ToInt(player_x) - fix16ToInt(last_x)) ? next_x : last_x;
                        }
                    }
                    else if (obs->type->colision_type == COLISION_WATER)
                    {
                        if (((fix16ToInt(player_x) != last_player_x - fix16ToInt(scrollspeed)) || (fix16ToInt(player_y) != last_player_y))
                            && wet_level == 0 && p_x >= base_point.x && p_x + PLAYER_WIDTH <= base_point.x + obs->type->w)
                        {
                            player_x_speed = fix16Neg(scrollspeed);
                            player_x = FIX16(last_player_x);
                            SPR_setAnim(player, PLAYER_WET);
                            wet_level = 30;
                            obs->hit = TRUE;
                            update_score(WATER_PENALTY);
                        }
                    }
                }
            }
        }
    }

    for (int idx = 0; idx < MAX_BONUS; idx++)
    {
        t_bonus* b = &bonus[idx];
        if (b->bonus != NULL)
        {
            t_point base_point = {fix16ToInt(b->bonus_x), fix16ToInt(b->bonus_y)};

            for (int pidx = 0; pidx < b->type->num_polygons; pidx++)
            {
                if (point_inside_polygon(b->type->polygons[pidx], 4, bottom_right, base_point) ||
                    point_inside_polygon(b->type->polygons[pidx], 4, top_right, base_point) ||
                    point_inside_polygon(b->type->polygons[pidx], 4, bottom_left, base_point) ||
                    point_inside_polygon(b->type->polygons[pidx], 4, top_left, base_point))
                {
                    XGM_startPlayPCM(BONUS_EFFECT, 10, SOUND_PCM_CH4);
                    SPR_releaseSprite(b->bonus);
                    b->bonus = NULL;
                    num_bonus--;
                    update_score(BONUS_SCORE);
                }
            }
        }
    }
}

void load_obstacles()
{
    if (num_obstacles < MAX_OBSTACLES && last_obstacle_x <= FIX16(256) && fix16ToInt(offset) % 32 == 0)
    {
        t_level* curr_level = get_level();
        short obs_type_idx = curr_level->obstacles_rand[random() % 10];
        t_obstacle_type* type = obs_type_idx >= 0 ? &obstacles_types[obs_type_idx] : NULL;

        if (type)
        {
            for (int idx = 0; idx < MAX_OBSTACLES; idx++)
            {
                if (obstacles[idx].obstacle == NULL)
                {
                    obstacles[idx].obstacle_x = FIX16(SCREEN_WIDTH);
                    last_obstacle_x = FIX16(SCREEN_WIDTH + 64);
                    obstacles[idx].obstacle_y = FIX16(FLOOR_HEIGHT - type->h + type->y_offset);
                    obstacles[idx].type = type;
                    obstacles[idx].crashed = FALSE;
                    obstacles[idx].hit = FALSE;
                    obstacles[idx].obstacle = SPR_addSprite(&(type->sprite), fix16ToInt(obstacles[idx].obstacle_x), fix16ToInt(obstacles[idx].obstacle_y), TILE_ATTR(PAL0, 0, FALSE, FALSE));
                    SPR_setAnim(obstacles[idx].obstacle, 0);
                    SPR_setDepth(obstacles[idx].obstacle, SPR_MIN_DEPTH+1);
                    num_obstacles++;
                    break;
                }
            }
        }
    }
}

void load_bonus()
{
    if (num_bonus < MAX_BONUS && last_bonus_x <= FIX16(256) && fix16ToInt(offset) % 24 == 0)
    {
        t_level* curr_level = get_level();
        short bonus_type_idx = curr_level->bonus_rand[random() % 10];
        t_bonus_type* type = bonus_type_idx >= 0 ? &bonus_types[bonus_type_idx] : NULL;

        if (type)
        {
            for (int idx = 0; idx < MAX_BONUS; idx++)
            {
                if (bonus[idx].bonus == NULL)
                {
                    bonus[idx].bonus_x = FIX16(SCREEN_WIDTH);
                    last_bonus_x = FIX16(SCREEN_WIDTH + 64);
                    bonus[idx].bonus_y = FIX16(FLOOR_HEIGHT - MIN_BONUS_H - type->h - bonus_levels[random() % NUM_BONUS_LEVELS]);
                    bonus[idx].type = type;
                    bonus[idx].bonus = SPR_addSprite(&(type->sprite), fix16ToInt(bonus[idx].bonus_x), fix16ToInt(bonus[idx].bonus_y), TILE_ATTR(PAL2, 0, FALSE, FALSE));
                    SPR_setAnim(bonus[idx].bonus, 0);
                    SPR_setDepth(bonus[idx].bonus, SPR_MIN_DEPTH+1);
                    num_bonus++;
                    break;
                }
            }
        }
    }
}

void update_obstacles()
{
    last_obstacle_x = FIX16(0);
    for (int idx = 0; idx < 8; idx++)
    {
        if (obstacles[idx].obstacle != NULL)
        {
            obstacles[idx].obstacle_x = fix16Add(obstacles[idx].obstacle_x, fix16Neg(scrollspeed));

            if (fix16ToInt(obstacles[idx].obstacle_x) > fix16ToInt(last_obstacle_x))
            {
                last_obstacle_x = fix16Add(obstacles[idx].obstacle_x, FIX16(64));
            }

            if (obstacles[idx].obstacle_x > FIX16(-obstacles[idx].type->w))
            {
                if (obstacles[idx].obstacle_x <= boss_x + FIX16(40) && obstacles[idx].crashed == FALSE && boss_x > FIX16(-48))
                {
                    obstacles[idx].crashed = TRUE;
                    SPR_setAnim(obstacles[idx].obstacle, obstacles[idx].type->crashed_anim);
                    XGM_startPlayPCM(obstacles[idx].type->colision_type == COLISION_WATER ? SPLASH_EFFECT : KICK_EFFECT, 10, SOUND_PCM_CH3);
                }

                SPR_setPosition(obstacles[idx].obstacle, fix16ToInt(obstacles[idx].obstacle_x), fix16ToInt(obstacles[idx].obstacle_y));
            }
            else
            {
                SPR_releaseSprite(obstacles[idx].obstacle);
                obstacles[idx].obstacle = NULL;
                num_obstacles--;
            }
        }
    }
}

void update_bonus()
{
    last_bonus_x = FIX16(0);
    for (int idx = 0; idx < 8; idx++)
    {
        if (bonus[idx].bonus != NULL)
        {
            bonus[idx].bonus_x = fix16Add(bonus[idx].bonus_x, fix16Neg(scrollspeed));

            if (fix16ToInt(bonus[idx].bonus_x) > fix16ToInt(last_bonus_x))
            {
                last_bonus_x = fix16Add(bonus[idx].bonus_x, FIX16(64));
            }

            if (bonus[idx].bonus_x > FIX16(-bonus[idx].type->w))
            {
                if (bonus[idx].bonus_x <= boss_x + FIX16(40) && boss_x > FIX16(-48) && bonus[idx].bonus_y >= FIX16(60))
                {
                    SPR_releaseSprite(bonus[idx].bonus);
                    bonus[idx].bonus = NULL;
                    num_bonus--;
                }
                else
                {
                    if (bonus_timer % 9 == 0) SPR_nextFrame(bonus[idx].bonus);
                    SPR_setPosition(bonus[idx].bonus, fix16ToInt(bonus[idx].bonus_x), fix16ToInt(bonus[idx].bonus_y));
                }
            }
            else
            {
                SPR_releaseSprite(bonus[idx].bonus);
                bonus[idx].bonus = NULL;
                num_bonus--;
            }
        }
    }
}

void update_boss()
{
    boss_visible = boss_x > FIX16(-48);
}

void end_game()
{
    if (is_record(score))
    {
        game_state = GAME_RECORD;
        show_ranking_input();
    } 
    else
    {
        game_state = GAME_OVER;
        gameover_menu_selected_option = 0;
        draw_gameover_menu_options();
    }

    SPR_setAnim(player, 2);
    SPR_setAnim(boss, 1);
    SPR_setPosition(player, 54, 144);
    SPR_setPosition(boss, 16, 64);
    XGM_stopPlay();
}

void light_day()
{
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x6dc2ca));
}

void middle_day() {
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x1d484e));
}

void dark_night() {
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000024));
}
