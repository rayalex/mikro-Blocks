#include "game_board.h"
#include "built_in.h"
#include "stdbool.h"
#include "stdint.h"
#include "random.h"
#include "geometry.h"
#include "tetrominoes.h"

void tgb_blit(GameBoard *const board, uint16_t shape, uintptr_t const *const image, uint8_t x, uint8_t y)
{
    uint8_t offsetX, offsetY, mask = 0;
    bool filled = false;

    for (offsetX = x; offsetX < x + SET_DIMENSION; ++offsetX)
    {
        for (offsetY = y; offsetY < y + SET_DIMENSION; ++offsetY)
        {
            filled = (shape >> (SET_DIMENSION * SET_DIMENSION - 1 - mask++)) & 1;

            board->board[offsetX][offsetY].filled = filled;
            board->board[offsetX][offsetY].image = image;
        }
    }
}

void tgb_blit_tetromino(GameBoard *const board, Tetromino const *const t)
{
    int8_t x, y;

    Rect boardRect = {0, 0, BOARD_WIDTH, BOARD_HEIGHT};
    Point blitPoint;

    for (x = 0; x < SET_DIMENSION; ++x)
    {
        for (y = 0; y < SET_DIMENSION; ++y)
        {
            if (t->buffer[x][y])
            {
                blitPoint.x = t->x + x;
                blitPoint.y = t->y + y;

                if (rect_intersects_point(boardRect, blitPoint))
                {
                    board->board[blitPoint.x][blitPoint.y].filled = true;
                    board->board[blitPoint.x][blitPoint.y].image = t->image;
                }
            }
        }
    }
}

bool tgb_collides(GameBoard const *const board, Tetromino const *const t)
{
    int8_t x, y;

    Rect boardRect = {0, 0, BOARD_WIDTH, BOARD_HEIGHT};
    Rect tetrominoGlobal;

    // compute position-adjusted AABB
    tetrominoGlobal = t->bounds;
    tetrominoGlobal.x += t->x;
    tetrominoGlobal.y += t->y;
    tetrominoGlobal.width++;
    tetrominoGlobal.height++;

    // in case AABB is outside the bucket, return immidiately
    if (!rect_contains_rect(boardRect, tetrominoGlobal))
        return true;

    // check the bit-map collision
    for (x = 0; x < SET_DIMENSION; ++x)
    {
        for (y = 0; y < SET_DIMENSION; ++y)
        {
            if (t->buffer[x][y] & board->board[x + t->x][y + t->y].filled)
                return true;
        }
    }

    return false;
}

void tgb_init(GameBoard *const board)
{
    tgb_clear(board);
}

void tgb_clear(GameBoard *const board)
{
    uint8_t i, x, y;

    for (x = 0; x < BOARD_WIDTH; x++)
        for (y = 0; y < BOARD_HEIGHT; y++)
            board->board[x][y].filled = false;
}

bool tgb_is_line_filled(GameBoard const *const board, uint8_t line)
{
    int i;

    for (i = 0; i < BOARD_WIDTH; ++i)
    {
        if (!board->board[i][line].filled)
            return false;
    }

    return true;
}