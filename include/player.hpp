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
    ~Player() override;

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

