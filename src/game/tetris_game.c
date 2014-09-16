#include "config.h"
#include "stdint.h"
#include "tetris_game.h"
#include "game_board.h"
#include "tetrominoes.h"
#include "tetromino.h"
#include "graphics.h"
#include "input.h"
#include "random.h"
#include "twister.h"

#define TIME_BASE           100
#define UPDATE_TICK         TIME_BASE * 200
#define MAX_UPDATE_TICK     TIME_BASE * 50
#define MOVE_TICK           TIME_BASE * 30
#define MOVE_DELAY          (-2 * MOVE_TICK)

static const uint8_t LevelCap = 10;
static uint16_t UpdateTick = UPDATE_TICK;

static bool isRunning = false; //TODO: maybe move to TetrisGame?

static uint32_t updateCounter;
static int32_t moveCounter;
static uint32_t clearCounter;

static uint16_t (*piecePointer)[BOARD_WIDTH *BOARD_HEIGHT] = SET_CLASSIC_DATA;

static void _tg_move(Tetromino *t, Direction dir);
static void _tg_rotate(Tetromino *t, Rotation r);
static void _tg_drop(TetrisGame *game);
static uint8_t _tg_clear_lines(TetrisGame *game);

static uint32_t _tg_get_line_score(uint8_t lines, uint8_t level)
{
    uint32_t scores[] = {0, 40, 100, 300, 1200};

    if (lines <= 4)
        return scores[lines] * (level + 1);

    return 2000 * lines * (level + 1);
}

void tg_init(TetrisGame *const game, GameConfig const *const config)
{
    uint32_t seed;

    GameBoard *board;
    Tetromino *tetromino;

    // game setup TODO: remove malloc
    board = (GameBoard *)Malloc(sizeof (GameBoard));
    tetromino = (Tetromino *)Malloc(sizeof (Tetromino));

    game->board = board;
    game->currentPiece = tetromino;
    game->config = config;

    // set up board defaults
    tgb_init(game->board);
    board->x = 80;
    board->y = 80;
    board->config = config;

    // seed our random generator. We're using Mersenne twister
    // here as it produces more "random" results as opposed
    // to the built-in one, which was biased at generating
    // same value couple of times in sequence
    seed = seed_random();
    init_genrand(seed);
    srand(seed);

#ifdef DEBUG
    {
        char buffer[32];

        sprinti(buffer, "Seed: %x", seed);

        TFTIntern_Set_Font(TFTIntern_defaultFont, CL_WHITE_Intern, FO_HORIZONTAL_Intern);
        TFTIntern_Write_Text(buffer, 0, 0);
    }
#endif
}

// TODO: Ideally this should be called by the timer and/or update time tracked
// in-between the calls, but as the VisualTFT already provides main update loop
// which seems to stable enough we're using that instead.
void tg_update(TetrisGame *game)
{
    uint8_t clearedLines;

    if (!isRunning)
        return;

    if (moveCounter++ >= MOVE_TICK)
    {
        if (inputStatus != 0)
        {
            // apply the transforms
            tg_piece_move(game, inputStatus);
        }

        moveCounter = 0;
    }

    // skip the update
    if (updateCounter++ < UpdateTick)
        return;

    // move the piece if possible, if not
    // apply it to the board, and update
    // then release the new one
    if (!tg_piece_move(game, DOWN))
    {
        // apply the piece onto the board
        tgb_blit_tetromino(game->board, game->currentPiece);

        // update drop score
        game->score += game->currentPiece->y;

        // resolve any clear lines
        clearedLines = _tg_clear_lines(game);
        game->score += _tg_get_line_score(clearedLines, game->level);

        clearCounter += clearedLines;
        if (clearCounter >= LevelCap)
        {
            clearCounter %= LevelCap;
            game->level++;

            // difficulty is incremented by decreasing update tick by 8th of it's value
            if (UpdateTick > MAX_UPDATE_TICK)
                UpdateTick -= UpdateTick / 8; // TODO: ( >> 3?)
        }

        // create new piece
        _tg_drop(game);

        // if the new piece fails collision check, the game is over.
        if (tgb_collides(game->board, game->currentPiece))
        {
            game->endHandler();
            return;
        }
    }
    tg_draw(game);

    // reset
    updateCounter = 0;
}

void tg_pause(TetrisGame *const game)
{
    isRunning = false;
}

void tg_start(TetrisGame *game)
{
    tg_reset(game);
    _tg_drop(game);

    isRunning = true;
}

void tg_reset(TetrisGame *game)
{
    Rect cleanRect = {0, 0, BOARD_WIDTH, BOARD_HEIGHT};

    isRunning = false;

    // reset
    UpdateTick = UPDATE_TICK;

    updateCounter = 0;
    moveCounter = 0;
    clearCounter = 0;

    game->score = 0;
    game->level = 0;

    // make entire board dirty
    tgb_clear(game->board);
    tgb_invalidate_rect(game->board, cleanRect);
}

void tg_draw(TetrisGame *game)
{
    tgb_draw_board(game->board);

    tgb_draw_tetromino(
        game->board,
        game->currentPiece
    );
    
    tg_draw_extra(game);
}

// TODO: these two functions share large portion of the
// code and should be merged into one.
bool tg_piece_move(TetrisGame *const game, Direction dir)
{
    bool moved = false;

    Tetromino *current; // current tetromino
    GameBoard *board;
    Tetromino pending; // next transformation

    current = game->currentPiece;
    board = game->board;
    pending = *current;

    tgb_invalidate_blit_tetromino(board, current, INV_MODE_SET);

    _tg_move(&pending, dir);
    if (!tgb_collides(board, &pending))
    {
        _tg_move(current, dir);
        moved = true;
    }

    tgb_invalidate_blit_tetromino(board, current, INV_MODE_XOR);
    tg_draw(game);

    return moved;
}

bool tg_piece_rotate(TetrisGame *const game, Rotation rot)
{
    bool rotated = false;

    Tetromino *current; // current tetromino
    GameBoard *board;
    Tetromino pending; // next transformation

    current = game->currentPiece;
    board = game->board;
    pending = *current;

    tgb_invalidate_blit_tetromino(board, current, INV_MODE_SET);

    _tg_rotate(&pending, rot);
    if (!tgb_collides(board, &pending))
    {
        _tg_rotate(current, rot);
        rotated = true;
    }

    tgb_invalidate_blit_tetromino(board, current, INV_MODE_XOR);
    tg_draw(game);

    return rotated;
}

void tg_timer_reset()
{
    moveCounter = MOVE_DELAY;
}

static void _tg_move(Tetromino *t, Direction dir)
{
    //TODO: replace swithc-case with something more pleasing
    switch (dir)
    {
    case LEFT:
        t->x--;
        break;

    case RIGHT:
        t->x++;
        break;

    case DOWN:
        t->y++;
        break;
    };
}

static void _tg_rotate(Tetromino *t, Rotation rot)
{
    switch (rot)
    {
    case CW:
        tetromino_rotate_cw(t);
        break;

    case CCW:
        tetromino_rotate_ccw(t);
        break;
    }
}

static void _tg_drop(TetrisGame *game)
{
    uint8_t tId = genrand_int32() % SET_CLASSIC_SIZE;

    // create random tetoromino
    Tetromino *t = game->currentPiece;
    tetromino_create(&piecePointer[0][4 * tId], 4, game->config->tileset[tId], t);

    // 'center' it
    t->x = 3;
}

static uint8_t _tg_clear_lines(TetrisGame *game)
{
    uint8_t i, j;
    uint8_t cleaned = 0;
    bool invalidated = false;
    int8_t currentHeight = BOARD_HEIGHT - 1;
    Rect invalidateRect;

    GameBoard *board = game->board;

    do
    {
        if (!tgb_is_line_filled(board, currentHeight))
        {
            currentHeight--;
            continue;
        }

        // invalidate just once
        if (!invalidated)
        {
            tgb_invalidate(board);
            invalidated = true;
        }

        // push the complete rows down
        // TODO: we can probably achieve the same by mem-copying the rows
        for (i = currentHeight; i > 0; --i)
        {
            for (j = 0; j < BOARD_WIDTH; ++j)
            {
                board->board[j][i] = board->board[j][i - 1];
            }
        }

        // clear top row
        for (i = 0; i < BOARD_WIDTH; ++i)
        {
            board->board[i][0].filled = false;
        }

        cleaned++;
    }
    while (currentHeight != -1);

    return cleaned;
}