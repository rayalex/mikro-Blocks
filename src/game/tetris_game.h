#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "game_board.h"
#include "game_config.h"
#include "tetromino.h"
#include "stdbool.h"
#include "input.h"

typedef void (*GameEventHandler )(void);

typedef struct
{
    GameConfig  *config;
    GameBoard   *board;
    Tetromino   *currentPiece;
    Tetromino   *nextPiece;

    uint32_t    score;
    uint8_t     level;

    GameEventHandler endHandler;
} TetrisGame;

typedef enum
{
    LEFT    = KEY_LEFT,
    RIGHT   = KEY_RIGHT,
    DOWN    = KEY_DOWN,
    UP      = KEY_UP
} Direction;

typedef enum
{
    CW      = KEY_RCW,
    CCW     = KEY_RCCW
} Rotation;

/**
 * Initializes the game.
 * @param game Game to initialize.
 */
void tg_init(TetrisGame *const game, GameConfig const *const config);

/**
 * Pauses the game, stopping any updates.
 * @param game Game to pause.
 */
void tg_pause(TetrisGame *const game);

/**
 * Starts the game.
 * @param game Game to start.
 */
void tg_start(TetrisGame *const game);

/**
 * Resets all game parameters.
 * @param game Game to reset.
 */
void tg_reset(TetrisGame *const game);

/**
 * Draws the entire game contents onto the screen.
 * @param game Game to draw.
 */
void tg_draw(TetrisGame const *const game);

/**
 * Does an update step to the game.
 * @param game Game to update.
 */
void tg_update(TetrisGame *const game);

/**
 * Moves the current game piece in specified direction.
 * @param  game Game to move the piece from.
 * @param  dir Direction of movement.
 * @return True if the piece has been moved sucessfully, false otherwise.
 */
bool tg_piece_move(TetrisGame *const game, Direction dir);

/**
 * Rotates the current piece in place, with specified Rotation.
 * @param  game Game to rotate the piece from.
 * @param  rot Rotation direction.
 * @return True if the piece has been rotated sucessfully, otherwise, false.
 */
bool tg_piece_rotate(TetrisGame *const game, Rotation rot);

/**
 * Resets game's internal timer.
 */
void tg_timer_reset();

#endif