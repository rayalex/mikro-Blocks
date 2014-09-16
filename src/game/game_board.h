#ifndef _GAMEBOARD_H_INCLUDED
#define _GAMEBOARD_H_INCLUDED
#include "stdbool.h"
#include "stdint.h"
#include "geometry.h"
#include "tetromino.h"
#include "game_config.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

typedef struct
{
    bool filled;
    uintptr_t const *const image;
} Cell;

typedef struct
{
    uint8_t x, y;
    Cell board[BOARD_WIDTH][BOARD_HEIGHT];
    GameConfig *config;
}
GameBoard;

/**
 * Decodes and blits the shape onto a board. Provided image is assigned as a reference for
 * a filled cells.
 * @param board Board to apply blit to.
 * @param shape 16-bit number representing the 4x4 shape.
 * @param image Pointer to the image.
 * @param x     X Position
 * @param y     Y Position.
 */
void tgb_blit(GameBoard *const board, uint16_t shape, uintptr_t const *const image, uint8_t x, uint8_t y);

/**
 * Blits the tetromino onto the specified board.
 * @param board Board to apply blit.
 * @param t     Tetromino to blit from.
 */
void tgb_blit_tetromino(GameBoard *const board, Tetromino const *const t);

/**
 * Tests if the tetromino collides with the board.
 * @param  board Board to check collsiion with.
 * @param  t     Tetromino to check collision for.
 * @return       true if the tetromino colides, otherwise, false.
 */
bool tgb_collides(GameBoard const *const board, Tetromino const *const t);

/**
 * Initializes the GameBoard
 * @param board GameBoard to init.
 */
void tgb_init(GameBoard *const board);

/**
 * Clears the game board.
 * @param board GB to be cleared.
 */
void tgb_clear(GameBoard *const board);

/**
 * Checks if GameBoard has a completly filled horizontal line.
 * @param  board Board to check.
 * @param  line  Line number, starting from top line with index 0.
 * @return       true if the line is filled, otherwise false.
 */
bool tgb_is_line_filled(GameBoard const *const board, uint8_t line);

#endif