#pragma once

#include <string>
#include "Vec2.h"

class Action
{
    std::string m_name;
    std::string m_type;
    Vec2 m_pos;

    public:
        Action();
        Action(const std::string& name, const std::string& type);
        Action(const std::string& name, Vec2 pos);
        Action(const std::string& name, const std::string& type, Vec2 pos);
        
        const std::string& name() const;
        const std::string& type() const;
        const Vec2& pos() const;

        std::string toString() const;
};
