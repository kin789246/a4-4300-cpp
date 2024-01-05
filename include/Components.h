#pragma once

#include "Animation.h"
#include <vector>

// set a flag: flag |= (int)PlayerState
// unset a flag: flag &= ~(int)PlayerState
// flipping a flag: flag ^= (int)PlayerState
// checking if a flag set: return (flag & (int)PlayerState) == (int)PlayerState
// checking multiple flags set: return (flag &(int)PlayerState) != 0
enum struct PlayerState {
    STAND = 1 << 0,
    STANDSHOOT = 1 << 1,
    AIR = 1 << 2,
    AIRSHOOT = 1 << 3,
    RUN = 1 << 4,
    RUNSHOOT = 1 << 5
};

class Component
{
    public:
        bool has = false;
};

class CTransform : public Component
{
    public:
        Vec2 pos = { 0.0, 0.0 };
        Vec2 prevPos = { 0.0, 0.0 };
        Vec2 scale = { 1.0, 1.0 };
        Vec2 velocity = { 0.0, 0.0 };
        Vec2 facing = { 0.0, 1.0 };
        float angle = 0;

        CTransform() {}
        CTransform(const Vec2& p) : pos(p) {}
        CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
            : pos(p), prevPos(p), scale(sc), velocity(sp), angle(a) {}
};

class CLifespan : public Component
{
    public:
        int lifespan = 0;
        int frameCreated = 0;
        CLifespan() {}
        CLifespan(int duration, int frame) 
            : lifespan(duration), frameCreated(frame) {}
};

class CDamage : public Component
{
    public:

    int damage = 1;
    CDamage() {}
    CDamage(int d) : damage(d) {}
};


class CInvicibility : public Component
{
    public:

    int iframes = 1;
    CInvicibility() {}
    CInvicibility(int f) : iframes(f) {}
};

class CHealth : public Component
{
    public:

    int max = 1;
    int current = 1;
    CHealth() {}
    CHealth(int m, int c) : max(m), current(c) {}
};

class CInput : public Component
{
    public:

        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool attack = false;

        CInput() {}
};

class CBoundingBox : public Component
{
    public:
        Vec2 size;
        Vec2 halfSize;
        bool blockMove = false;
        bool blockVision = false;
        CBoundingBox() {}
        CBoundingBox(const Vec2& s, bool m, bool v) 
        : size(s)
          , halfSize(s.x / 2.0, s.y / 2.0)
          , blockMove(m)
          , blockVision(v)
    {}
};

class CAnimation : public Component
{
    public:
        Animation animation;
        bool repeat = false;
        CAnimation() {}
        CAnimation(const Animation& animation, bool r) 
        : animation(animation), repeat(r) {}
};

class CGravity : public Component
{
    public:
        float gravity = 0;
        CGravity() {}
        CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
    public:
    PlayerState state;
    PlayerState preState; 
    bool changeAnimate = false;
    CState() {}
    CState(const PlayerState s) : state(s), preState(s) {}
}; 

class CFollowPlayer : public Component
{
    public:

    Vec2 home = { 0, 0 };
    float speed = 0;
    CFollowPlayer() {}
    CFollowPlayer(Vec2 p, float s) : home(p), speed(s) {}
};

class CPatrol : public Component 
{
    public:

    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
    CPatrol() {}
    CPatrol(std::vector<Vec2>& pos, float s) : positions(pos), speed(s) {}
};
