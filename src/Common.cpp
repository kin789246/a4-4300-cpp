#include <cmath>
#include "Common.h"


namespace GameMath {
    Vec2 getSpeedAB(const Vec2& posA, const Vec2& posB, float speed) {
        float theta = std::atan2(posB.y - posA.y, posB.x - posA.x);
        return Vec2{ speed * std::cos(theta), speed * std::sin(theta) };
    }
}
