#pragma once

#include <memory>
#include "Entity.h"
#include "Vec2.h"
#include "Common.h"

class Physics
{
    public:
        Vec2 GetOverlap(
            std::shared_ptr<Entity> a,
            std::shared_ptr<Entity> b
        );

        Vec2 GetPreviousOverlap(
            std::shared_ptr<Entity> a,
            std::shared_ptr<Entity> b
        );

        bool IsInside(const Vec2& pos, std::shared_ptr<Entity> e);
        Intersect LineInIntersect(
            const Vec2& a, 
            const Vec2& b, 
            const Vec2& c, 
            const Vec2& d
        );
        bool EntityIntersect(
            const Vec2& a, 
            const Vec2& b, 
            std::shared_ptr<Entity> e
        );
};
