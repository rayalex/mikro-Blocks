#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "stdint.h"
#include "stdbool.h"

// in case we need to redefine the type
typedef int16_t __numeric;

typedef struct
{
    __numeric x, y;
    __numeric width, height;
} Rect;

typedef struct
{
    __numeric x, y;
} Point;

/**
 * Determines if the point intersects the given rectangle.
 * True is returned if the point is inside or on the rect's border.
 * @param  rect Containing rectangle.
 * @param  pt   Test point.
 * @return True if point is contained within the rectangle.
 */
bool rect_intersects_point(Rect rect, Point pt);

/**
 * Tests if the container contains the rectangle.
 * @param  container	Bounding container rectangle.
 * @param  containee	Rectangle which is checked against the container.
 * @return
 */
bool rect_contains_rect(Rect container, Rect containee);

#endif