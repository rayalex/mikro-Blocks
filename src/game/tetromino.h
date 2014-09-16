#ifndef _TETROMINO_H
#define _TETROMINO_H

#include "stdint.h"
#include "geometry.h"
#include "tetrominoes.h"

typedef struct
{
    // data buffer
    uint8_t buffer[SET_DIMENSION][SET_DIMENSION];

    uint16_t shapes[SET_ROTATIONS];     // all possible variants
    uint8_t shapeId;                    // tetromino shape
    uint8_t shapeCount;                 // length of the shapes array, to support variable number of rotations in the future;
    uintptr_t *image;

    int8_t x, y;                        // position within the bucket boundaries TODO: to Point

    Rect bounds;                        // shape bounds
} Tetromino;

/**
 * Creates tetromino from source bitmap.
 * @param source Source int_32 data pointer
 * @param setLength Lenght of the set.
 * @param out    Destination structure.
 */
void tetromino_create(uint16_t *sourceSet, uint8_t setLength, uintptr_t *image, Tetromino *out);

/**
 * Computes bounding-box for the tetromino. BB is returned in tetromino-local space.
 * @param  t Tetromino to compute BB for.
 * @return   Resulting Bounding box.
 */
Rect tetromino_compute_aabb(Tetromino const *const t);

/**
 * Rotates tetromino clockwise.
 * @param t Tetromino to rotate.
 */
void tetromino_rotate_cw(Tetromino *const t);

/**
 * Rotate tetromino counterclockwise.
 * @param t Tetromino to rotate.
 */
void tetromino_rotate_ccw(Tetromino *const t);

#endif