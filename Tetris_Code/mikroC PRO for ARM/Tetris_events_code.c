#include "Tetris_objects.h"
#include "Tetris_resources.h"

//--------------------- User code ---------------------//
#include "stdint.h"
#include "game.h"
#include "tetris_game.h"
#include "tetrominoes.h"
#include "input.h"
#include "graphics.h"

static TetrisGame game;
static GameConfig config = {
    7,          // tileset size
    &IMG_FLAT   // tileset reference
};

void End_Game(void)
{
    uint8_t len;
    char buffer[10];
    Point scorePosition;

    len = log10(game.score) + 1;

    DrawScreen(&EndScreen);

    scorePosition.x = 240 - (len * 20) / 2; //magic
    scorePosition.y = 435;
    g_draw_numeric(game.score, "%d", scorePosition);

    tg_reset(&game);
}

void Initialize_Game()
{
    // assign our end_game handler
    game.endHandler = End_Game;

    MM_Init();
    tg_init(&game, &config);
}

void Start_Game()
{
    tg_start(&game);
    tg_draw(&game);
}

void Update_Game()
{
    tg_update(&game);
}

static void keySet(uint8_t key)
{
    keyPress(key);
}

static void keyReset(uint8_t key)
{
    keyRelease(key);
}

//----------------- End of User code ------------------//

// Event Handlers

void ImgStartOnClick()
{
    DrawScreen(&GameScreen);
    Start_Game();
}

void ImgArrowLeftOnDown()
{
    keySet(KEY_LEFT);
    tg_piece_move(&game, LEFT);
    tg_timer_reset();
}

void ImgArrowLeftOnUp()
{
    keyReset(KEY_LEFT);
}

void ImgArrowRightOnDown()
{
    keySet(KEY_RIGHT);
    tg_piece_move(&game, RIGHT);
    tg_timer_reset();
}

void ImgArrowRightOnUp()
{
    keyReset(KEY_RIGHT);
}

void ImgArrowDownOnDown()
{
    keySet(KEY_DOWN);
    tg_piece_move(&game, DOWN);
    tg_timer_reset();
}

void ImgArrowDownOnUp()
{
    keyReset(KEY_DOWN);
}

void ImgRotateCCWOnDown()
{
    keySet(KEY_RCCW);
    tg_piece_rotate(&game, CCW);
    tg_timer_reset();
}

void ImgRotateCCWOnUp()
{
    keyReset(KEY_RCCW);
}

void ImgRotateCWOnDown()
{
    keySet(KEY_RCW);
    tg_piece_rotate(&game, CW);
    tg_timer_reset();
}

void ImgRotateCWOnUp()
{
    keyReset(KEY_RCW);
}

void ImgHomeOnClick()
{
    DrawScreen(&HomeScreen);
}

void FauxBoxOnUp()
{
    keyClear();
}