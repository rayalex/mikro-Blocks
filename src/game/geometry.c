#include "geometry.h"

bool rect_intersects_point(Rect rect, Point pt)
{
    return !(pt.x < rect.x || pt.y < rect.y || pt.x > rect.x + rect.width || pt.y > rect.y + rect.height);
}

bool rect_contains_rect(Rect r1, Rect r2)
{
    return !(r1.x > r2.x || r1.y > r2.y || r1.x + r1.width < r2.x + r2.width || r1.y + r1.height < r2.y + r2.height);
}