#pragma once

#include "drawable.hpp"
#include "game_object.hpp"
#include "map.hpp"

struct Actor : public GameObject
{
public:
    Actor();
    ~Actor() override;

    virtual void setPos(const Vector2D &v);
    virtual void setDirection(TReal dir);
    virtual Vector2D position() const;
    virtual TReal direction() const;
    virtual void collision(Actor *another);
    virtual void interract(Actor *causer);
    FRectangle2D getCollisionRect() const;

    std::shared_ptr<Drawable> drawable;
    Vector2D rectSize;
    Map *map;
    uint32_t id;


    // GameObject interface
public:
    void update(TReal dt) override;
};
