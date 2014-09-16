#include "stdint.h"
#include "game_board.h"
#include "geometry.h"
#include "graphics.h"
#include "tetromino.h"
#include "tetrominoes.h"
#include "config.h"
#include "stdint.h"
#include "tetris_resources.h"

#define CTX_DIRTY 1
#define CTX_CLEAN 0

//TODO: make part of Game/gameboard structure
#define CELL_SIZE 32
#define SCORE_PADDING 8
#define SCORE_DIGIT_W 20

//HACK: used to track the most recent score and not refresh it if not necessary
static uint32_t prevScore = -1;

static char __dirty[BOARD_WIDTH][BOARD_HEIGHT] = {0};

static uintptr_t const *const emptyTile = &flat_01_bmp;

static uintptr_t const *const digits[] =
{
    &digit_0_bmp,
    &digit_1_bmp,
    &digit_2_bmp,
    &digit_3_bmp,
    &digit_4_bmp,
    &digit_5_bmp,
    &digit_6_bmp,
    &digit_7_bmp,
    &digit_8_bmp,
    &digit_9_bmp,
    &digit_empty_bmp
};

static void draw_number_with_images(uint16_t number, char *format, uint16_t x, uint16_t y)
{
    const uint8_t space = 10;

    char buffer[32];
    uint8_t length;
    uint8_t i;
    uintptr_t *chr;

    length = sprinti(buffer, format, number);

    for (i = 0; i < length; ++i)
    {
        if (isspace(buffer[i]))
            chr = digits[space];
        else chr = digits[buffer[i] - '0'];

        TFTIntern_Image(x + i * SCORE_DIGIT_W, y, chr, 1);
    }
}

void g_draw_numeric_res(uint16_t number, uintptr_t *digits, char *format, Point position)
{
    draw_number_with_images(number, format, position.x, position.y);
}

void g_draw_numeric(uint16_t number, char *format, Point position)
{
    g_draw_numeric_res(number, digits, format, position);
}


void tg_draw_extra(TetrisGame const *const game)
{

    uint8_t lvlLength = 0;

    if (prevScore == game->score)
        return;

    // no support of %*d formatting, so we have to provide string
    // literal instead

    // draw the score
    draw_number_with_images(game->score, "%8d", 80, 48);

    // and level
    draw_number_with_images(game->level, "%3d", 340, 48);

    prevScore = game->score;

#ifdef DEBUG
    {
        char buffer[32];
        uint16_t xMin, xMax, yMin, yMax;
        TSC2006_GetCalibrationConsts(&xMax, &xMax, &yMin, &yMax);

        sprinti(buffer, "%d,%d,%d,%d", xMin, xMax, yMin, yMax);
        TFTIntern_Write_Text(buffer, 0, 0);
    }
#endif
}

void tgb_draw_board(GameBoard const *const board)
{
    uint8_t x, y;
    uint16_t top, left;
    uintptr_t *imgPointer;

    for (x = 0; x < BOARD_WIDTH; ++x)
    {
        for (y = 0; y < BOARD_HEIGHT; ++y)
        {
            left = board->x + x * CELL_SIZE;
            top = board->y + y * CELL_SIZE;

            if (__dirty[x][y] == CTX_CLEAN)
                continue;

            if (board->board[x][y].filled)
                imgPointer = board->board[x][y].image;
            else
                imgPointer = board->config->tileset[7];

            TFTIntern_Image(left, top, imgPointer, 1);

            // mark the cell as clean so it won't be rendered in the next pass
            __dirty[x][y] = CTX_CLEAN;
        }
    }
}

void tgb_draw_tetromino(GameBoard const *const board, Tetromino const *const t)
{
    int8_t x, y;
    int16_t top, left;
    int16_t i, j;
    Point current;

    x = t->x;
    y = t->y;

    // draw from tetromino internals
    TFTIntern_Set_Brush(1, CL_BLUE_Intern, 0, 0, 0, 0);
    TFTIntern_Set_Pen(0, 0);

    for (i = 0; i < SET_DIMENSION; ++i)
    {
        for (j = 0; j < SET_DIMENSION; ++j)
        {
            left = board->x + (i + x) * CELL_SIZE;
            top = board->y + (j + y) * CELL_SIZE;

#ifdef DEBUG
            current.x = i;
            current.y = j;

            if (rect_intersects_point(t->bounds, current))
                TFTIntern_Set_Pen(CL_FUCHSIA_Intern, 2);
            else TFTIntern_Set_Pen(0, 0);
#endif

            if (t->buffer[i][j])
                TFTIntern_Image(left, top, t->image, 1);
        }
    }

#ifdef DEBUG
    {
        char buffer[64];

        sprinti(buffer, "%d-%d-%d-%d", t->bounds.x, t->bounds.y, t->bounds.width, t->bounds.height);
        TFTIntern_Write_Text(buffer, 10, 10);
    }
#endif

}

void tgb_invalidate_rect(GameBoard const *const board, Rect const rect)
{
    //TODO: make exclude rect function (diff)
    int16_t i, j;
    int16_t left = max(0, rect.x);
    int16_t top = max(0, rect.y);
    int16_t right = min(rect.x + rect.width, BOARD_WIDTH);
    int16_t bottom = min(rect.y + rect.height, BOARD_HEIGHT);

    for (i = left; i < right; ++i)
    {
        for (j = top; j < bottom; ++j)
        {
            __dirty[i][j] = CTX_DIRTY;
        }
    }
}

void tgb_invalidate_blit(GameBoard *const board, uint8_t _data[SET_DIMENSION][SET_DIMENSION], Rect rect, InvalidateMode mode)
{

#ifdef DEBUG
    char buffer[32];
    uint16_t dbgTop = 20;
#endif

    int16_t i, j, x, y;

    Rect boardRect = {0, 0, BOARD_WIDTH, BOARD_HEIGHT};
    Point point;

    for (i = 0; i < SET_DIMENSION; ++i)
    {
        for (j = 0; j < SET_DIMENSION; ++j)
        {
            // board coordinates
            x = rect.x + i;
            y = rect.y + j;

            point.x = x;
            point.y = y;

            if (!rect_intersects_point(boardRect, point))
                continue;

#ifdef DEBUG
            sprinti(buffer, "%d,%d:%d", i, j, _data[i][j]);
            TFTIntern_Write_Text(buffer, 10, dbgTop);
            dbgTop += 12;
#endif

            if (_data[i][j])
            {
                switch (mode)
                {
                case INV_MODE_SET:
                    __dirty[x][y] = CTX_DIRTY;
                    break;

                case INV_MODE_CLR:
                    __dirty[x][y] = CTX_CLEAN;
                    break;

                case INV_MODE_XOR:
                    __dirty[x][y] ^= 1;
                    break;
                }
            }
        }
    }
}

void tgb_invalidate_blit_tetromino(GameBoard *const board, Tetromino const *const t, InvalidateMode mode)
{
    // tetromino data is completly held withing it's AABB
    // therefore we're just iterrating through it and setting
    // the fields as necessary

    // offset the position
    Rect computedAabb = {0, 0, SET_DIMENSION, SET_DIMENSION};
    computedAabb.x = t->x;
    computedAabb.y = t->y;
    tgb_invalidate_blit(board, t->buffer, computedAabb, mode);
}

void tgb_invalidate_filled(GameBoard *const board)
{
    uint8_t x, y;

    for (x = 0; x < BOARD_WIDTH; ++x)
    {
        for (y = 0; y < BOARD_HEIGHT; ++y)
        {
            if (board->board[x][y].filled)
                __dirty[x][y] = CTX_DIRTY;
        }
    }
}

void tgb_invalidate(GameBoard *const board)
{
    uint8_t x, y;

    for (x = 0; x < BOARD_WIDTH; ++x)
    {
        for (y = 0; y < BOARD_HEIGHT; ++y)
        {
            __dirty[x][y] = CTX_DIRTY;
        }
    }
}