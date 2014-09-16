#include "stdint.h"
#include "Tetrominoes.h"
#include "Tetromino.h"

static void update_shape_aabb(Tetromino *t)
{
    t->bounds = tetromino_compute_aabb(t);
}

static void apply_shape(Tetromino *t, uint16_t shape)
{
    uint8_t x, y;
    uint8_t mask = 0;

    // decode the shape into the tetromino, row first order
    for (y = 0; y < SET_DIMENSION; ++y)
    {
        for (x = 0; x < SET_DIMENSION; ++x)
        {
            t->buffer[x][y] = (shape >> (SET_DIMENSION * SET_DIMENSION - 1 - mask++)) & 1;
        }
    }

    update_shape_aabb(t);
}

static void update_shape(Tetromino *t)
{
    apply_shape(t, t->shapes[t->shapeId]);
}

Rect tetromino_compute_aabb(Tetromino const *const t)
{
    uint8_t x, y;

    uint8_t
    top     = UINT8_MAX,
    left    = UINT8_MAX,
    bottom  = 0,
    right   = 0;

    Rect out;
    for (x = 0; x < SET_DIMENSION; ++x)
    {
        for (y = 0; y < SET_DIMENSION; ++y)
        {
            if (!t->buffer[x][y])
                continue;

            top = min(top, y);
            left = min(left, x);

            right = max(right, x);
            bottom = max(bottom, y);
        }
    }

    out.x = left;
    out.y = top;
    out.width = right - left;
    out.height = bottom - top;
    return out;
}

void tetromino_create(uint16_t *sourceSet, uint8_t setLength, uintptr_t *image, Tetromino *out)
{
    // initial shape
    out->shapeCount = setLength;
    out->shapeId = 0;
    out->image = image;
    out->x = 0;
    out->y = 0;

    // copy shape data
    memcpy(&out->shapes, sourceSet, setLength * sizeof(uint16_t));

    // apply the shape
    update_shape(out);
}

void tetromino_rotate_cw(Tetromino *const t)
{
    uint8_t shapeId = t->shapeId;
    if (++shapeId == t->shapeCount)
        shapeId = 0;

    t->shapeId = shapeId;
    update_shape(t);
}

void tetromino_rotate_ccw(Tetromino *const t)
{
    uint8_t shapeId = t->shapeId;
    if (shapeId == 0)
        shapeId = t->shapeCount - 1;
    else shapeId--;

    t->shapeId = shapeId;
    update_shape(t);
}