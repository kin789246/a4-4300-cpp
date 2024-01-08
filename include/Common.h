#include "Vec2.h"

struct Intersect {
    bool result;
    Vec2 pos;
};

enum struct ODirection {
    UP,
    Down,
    LEFT,
    RIGHT,
    NONE
};

struct RectOverlap {
    ODirection direction;
    Vec2 overlap;
};
