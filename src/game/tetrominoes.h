#ifndef _TETROMINOES_H
#define _TETROMINOES_H

#include "stdint.h"

#define SET_DIMENSION 4
#define SET_ROTATIONS 4
#define SET_CLASSIC_SIZE 7


extern uint16_t const SET_CLASSIC_DATA[SET_CLASSIC_SIZE][SET_ROTATIONS];

extern uintptr_t const *const IMG_DOTS[SET_CLASSIC_SIZE + 1];

extern uintptr_t const *const IMG_FLAT[SET_CLASSIC_SIZE + 1];

extern uintptr_t const *const IMG_CLASSIC[SET_CLASSIC_SIZE + 1];

#endif