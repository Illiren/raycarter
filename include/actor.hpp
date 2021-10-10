#pragma once

#include "drawable.hpp"
#include "game_object.hpp"
#include "map.hpp"

struct Actor : public GameObject
{
public:
    Actor();
    ~Actor() override;

    void setPos(const Vector2D &v);
    virtual Vector2D position() const;
    Drawable *drawable;
    Map *map;
    Vector2D rectSize;

    Rectangle getRect() const;

    virtual void collision(Actor *another);
    virtual void interract(Actor *causer);


    //void
    // GameObject interface
public:
    void update(TReal dt) override;
};
