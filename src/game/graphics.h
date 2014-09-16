#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "stdint.h"
#include "tetris_game.h"
#include "game_board.h"
#include "geometry.h"
#include "tetromino.h"

typedef enum
{
    INV_MODE_SET,	// bit is set
    INV_MODE_CLR,	// bit is cleared
    INV_MODE_XOR	// bit is XORed with the current value
} InvalidateMode;

/**
 * Draws the number using the preformatted string, using the internal bitmap.
 * @param number Number to write.
 * @param format Number format.
 * @param position Top left corner of the drawing origin.
 */
void g_draw_numeric(uint16_t number, char *format, Point position);

/**
 * Draws the number using the preformatted string, with provided array of images.
 * @param number Number to write.
 * @param digits Pointer to the array of images containing all the digits.
 * The array should contain 11 bitmaps, for ditigs 0-9, plus one for empty place.
 * @param format Number format.
 * @param position Tope left corner of the drawing origin.
 */
void g_draw_numeric_res(uint16_t number, uintptr_t *digits, char *format, Point position);

/**
 * Draws Game's extras.
 * @param game Game we're drawing the extras from.
 */
void tg_draw_extra(TetrisGame const *const game);

/**
 * Draws the board and it's contents.
 * @param board Board to draw.
 */
void tgb_draw_board(GameBoard const *const board);

/**
 * Draws the tetromino on screen.
 * @param board Owning board, used for offset calculation.
 * @param t Tetromino we're drawing.
 */
void tgb_draw_tetromino(GameBoard const *const board, Tetromino const *const t);

/**
 * Invalidates rectangle on the game board, making it redraw on the next update.
 * @param board Board we're invalidating.
 * @param rect Rectangle to invalidate.
 */
void tgb_invalidate_rect(GameBoard const *const board, Rect const rect);

/**
 * Invalidates the game board with given 2D matrix.
 * @param board Game board we're invalidating.
 * @param _data 2D matrix we're applying to invalidate map.
 * @param dimension 2D matrix dimension.
 * @param mode Invalidation mode.
 */
void tgb_invalidate_blit(GameBoard *const board, uint8_t _data[][], Rect dimension, InvalidateMode mode);

/**
 * Blits the tetromino shape on the board's invalidation map. Each filled cell in the tetromino
 * is combined using the mode function, productig the resulting bit on the board's map.
 * @param board Board we're invalidating.
 * @param t Tetromino to apply to invalidation map.
 * @param mode Mode we're using to invalidate the board.
 */
void tgb_invalidate_blit_tetromino(GameBoard *const board, Tetromino const *const t, InvalidateMode mode);

/**
 * Invalidates all filled cells in the game board.
 * @param board Board to invalidate.
 */
void tgb_invalidate_filled(GameBoard *const board);

/**
 * Invalidates the entire game board.
 * @param board Board to invalidate.
 */
void tgb_invalidate(GameBoard *const board);

#endif