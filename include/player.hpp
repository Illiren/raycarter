#pragma once

#include "camera.hpp"
#include "map.hpp"
#include "actor.hpp"

struct Player : public Actor
{
    enum WalkState
    {
        Idle = 0,
        Forward = -1,
        Back = 1,
        Left = -1,
        Right = 1
    };

    Player(Camera &camera, Map &map);
    ~Player() override {}

    char walk = 0;
    char turn = 0;
    float runningSpeed = 1;
    float speed = .0f;
    float maxSpeed = 0.002f;

    void update(TReal dt) override;
    void doInteract();

    Map &map;
    Camera &camera;
    PTexture face;
    PTexture weapontext;

    // Actor interface
public:
    Vector2D position() const override;
    void collision(Actor *another) override;
};

struct MovementComponent : public GameObject
{
    enum WalkState
    {
        Idle = 0,
        Forward = -1,
        Back = 1,
        Left = -1,
        Right = 1
    };

    MovementComponent(Actor &parent) :
          owner(parent)
    {}

    char walk = 0;
    char turn = 0;
    float runSpeedMultiplier = 1;
    float speed = .0f;
    float maxSpeed = 0.002f;

    Actor &owner;

    // GameObject interface
public:
    void update(TReal dt) override
    {
        TReal dir = owner.direction();
        Vector2D pos = owner.position();
        Map &map = *owner.map;
        dir += float(turn)*maxSpeed*dt;

        constexpr float length=0.5;
        const auto cosa = std::cos(dir);
        const auto sina = std::sin(dir);

        float nx = pos.x() + cosa*speed*runSpeedMultiplier*dt;
        float ny = pos.y() + sina*speed*runSpeedMultiplier*dt;

        float dx = length*cosa+nx;
        float dy = length*sina+ny;

        if(int(nx) >= 0 && int(nx) < 16 && int(ny) >= 0 && int(ny)< 16)
        {
            const size_t posX = size_t(dx)+size_t(pos.y())*map.width();
            const size_t posY = size_t(pos.x())+size_t(dy)*map.width();

            if(map[posX] == ' ')
                pos.x() = nx;
            if(map[posY] == ' ')
                pos.y() = ny;

            owner.setPos(pos);
        }
        owner.setDirection(dir);
    }
};
