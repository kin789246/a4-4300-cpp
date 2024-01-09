#include "Physics.h"
#include "Entity.h"
#include <cstdlib>
#include <memory>
#include <iostream>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    // todo: return the overlap rectangle size of the bouding boxes of enetity a and b
    Vec2 posA = a->get<CBoundingBox>().center;
    Vec2 sizeA = a->get<CBoundingBox>().halfSize;
    Vec2 posB = b->get<CBoundingBox>().center;
    Vec2 sizeB = b->get<CBoundingBox>().halfSize;
    Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
    float ox = sizeA.x + sizeB.x - delta.x;
    float oy = sizeA.y + sizeB.y - delta.y;
    return Vec2(ox, oy);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    // todo: return the previous overlap rectangle size of 
    // the bouding boxes of enetity a and b
    // previous overlap uses the entity's previous position
    Vec2 posA = a->get<CBoundingBox>().prevCenter;
    Vec2 sizeA = a->get<CBoundingBox>().halfSize;
    Vec2 posB = b->get<CBoundingBox>().prevCenter;
    Vec2 sizeB = b->get<CBoundingBox>().halfSize;
    Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
    float ox = sizeA.x + sizeB.x - delta.x;
    float oy = sizeA.y + sizeB.y - delta.y;
    return Vec2(ox, oy);
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e) {
    // todo: implement this function
    Vec2 s = e->get<CAnimation>().animation.getSize();
    Vec2 ePos = e->get<CTransform>().pos;
    if (pos.x > ePos.x - s.x / 2 &&
        pos.x < ePos.x + s.x / 2 &&
        pos.y > ePos.y - s.y / 2 &&
        pos.y < ePos.y + s.y / 2
    ) {
        std::cout << e->get<CAnimation>().animation.getName() << std::endl;
        return true;
    }
    return false;
}

Intersect Physics::LineInIntersect(
    const Vec2& a, 
    const Vec2& b, 
    const Vec2& c, 
    const Vec2& d 
) {
    // todo: implement this function
    // where t u is scalar, t ∈ [0, 1] and u ∈ [0, 1]
    // t = ((c-a) X s) / (r X s)
    // u = ((c-a) X r) / (r X s)
    // intersection point = a + t*r or c + u*s
    Vec2 r = b - a;
    Vec2 s = d - c;
    float rxs = r.x * s.y - r.y * s.x;
    Vec2 cma = c - a;
    float t = (cma.x * s.y - cma.y * s.x) / rxs;
    float u = (cma.x * r.y - cma.y * r.x) / rxs;
    if (t >= 0 && t <= 1 && u >= 0 && u <=1) {
        return { true, Vec2(a.x + t*r.x, a.y + t*r.y) };
    }
    return { false, Vec2(0, 0) };
}

bool Physics::EntityIntersect(
    const Vec2& a, 
    const Vec2& b, 
    std::shared_ptr<Entity> e
) {
    // todo: implement this function
    auto boxC = e->get<CBoundingBox>().center;
    auto box = e->get<CBoundingBox>().halfSize;
    Vec2 e1{ boxC.x - box.x, boxC.y - box.y };
    Vec2 e2{ boxC.x + box.x, boxC.y - box.y };
    Vec2 e3{ boxC.x + box.x, boxC.y + box.y };
    Vec2 e4{ boxC.x - box.x, boxC.y + box.y };

    if (LineInIntersect(a, b, e1, e2).result ||
        LineInIntersect(a, b, e2, e3).result ||
        LineInIntersect(a, b, e3, e4).result ||
        LineInIntersect(a, b, e4, e1).result 
    ) {
        return true;
    }
    return false;
}

RectOverlap Physics::AisNearB(
            std::shared_ptr<Entity> a,
            std::shared_ptr<Entity> b,
            const Vec2& maxDist
) {
    ODirection dir = ODirection::NONE;
    Vec2 overlap = GetOverlap(a, b);
    Vec2 pOverlap = GetPreviousOverlap(a, b);
    float dy = b->get<CTransform>().pos.y - a->get<CTransform>().pos.y;
    if (0 < overlap.x && 
        -maxDist.y < overlap.y &&
        0 < overlap.y &&
        pOverlap.y <= 0
    ) {
        if (dy > 0) {
            dir = ODirection::UP;
        }
        else if (dy < 0){
            dir = ODirection::Down;
        }
    }

    float dx = b->get<CTransform>().pos.x - a->get<CTransform>().pos.x;
    if (0 < overlap.y &&
            -maxDist.x < overlap.x &&
            0 < overlap.x &&
            pOverlap.x <= 0
       ) {
        if (dx > 0) {
            dir = ODirection::LEFT;
        }
        else if (dx < 0) {
            dir = ODirection::RIGHT;
        }
    }
    return { dir, overlap };
}
