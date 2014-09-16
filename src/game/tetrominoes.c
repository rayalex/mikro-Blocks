#include "stdint.h"
#include "tetrominoes.h"
#include "tetris_resources.h"

// shapes in 4x4 matrices, encoded as a row-first 16-bit numbers
uint16_t const SET_CLASSIC_DATA[SET_CLASSIC_SIZE][SET_ROTATIONS] =
{
    {0xf000, 0x2222, 0x0f00, 0x4444},
    {0x8e00, 0x6440, 0x0e20, 0x44c0},
    {0x2e00, 0x4460, 0x0e80, 0xc440},
    {0x6600, 0x6600, 0x6600, 0x6600},
    {0x6c00, 0x4620, 0x06c0, 0x8c40},
    {0x4e00, 0x4640, 0x0e40, 0x4c40},
    {0xc600, 0x2640, 0x0c60, 0x4c80}
};

uintptr_t const *const IMG_DOTS[SET_CLASSIC_SIZE + 1] =
{
    &dots_02_bmp,
    &dots_03_bmp,
    &dots_04_bmp,
    &dots_05_bmp,
    &dots_06_bmp,
    &dots_07_bmp,
    &dots_08_bmp,
    &dots_01_bmp,
};

uintptr_t const *const IMG_FLAT[SET_CLASSIC_SIZE + 1] =
{
    &flat_02_bmp,
    &flat_03_bmp,
    &flat_04_bmp,
    &flat_05_bmp,
    &flat_06_bmp,
    &flat_07_bmp,
    &flat_08_bmp,
    &flat_01_bmp,
};

uintptr_t const *const IMG_CLASSIC[SET_CLASSIC_SIZE + 1] =
{
    &classic_02_bmp,
    &classic_03_bmp,
    &classic_04_bmp,
    &classic_05_bmp,
    &classic_06_bmp,
    &classic_07_bmp,
    &classic_08_bmp,
    &classic_01_bmp,
};