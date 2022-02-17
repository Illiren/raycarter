#pragma once

#include "camera.hpp"
#include "geometry.hpp"
#include "game_object.hpp"

class Actor;
struct Location;

struct ActorComponent : public GameObject
{
public:
    ActorComponent(Actor *actorParent);

    Actor *getOwner() const noexcept;

protected:
    Actor *owner;
};


class Actor : public GameObject
{
public:
    using ComponentList = TArray<ActorComponent*>;
public:
    Actor();
    ~Actor() override;

    FRectangle2D getCollisionBody() const noexcept;
    virtual void onInterract(Actor *causer) {(void)causer;} //Unused
    virtual void onCollision(Actor *causer) {(void)causer;} //Unused
    TReal    direction;
    Vector2D position;
    Location *location;


private:
    ComponentList components;

    friend struct ActorComponent;

    // GameObject interface
public:
    void update(TReal dt) override;
};


struct MovementComponent : public ActorComponent
{
    enum WalkState
    {
        Idle = 0,
        Forward = -1,
        Back = 1,
        Left = -1,
        Right = 1
    };

    MovementComponent(Actor *parent);

    char walk = 0;
    char turn = 0;
    float runSpeedMultiplier = 1;
    float speed = .0f;
    float maxSpeed = 0.002f;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct CameraComponent : public ActorComponent
{
public:
    CameraComponent(Actor *parent);

    Camera camera;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct SpriteComponent : public ActorComponent
{
    SpriteComponent(Actor *parent);
    Sprite sprite;
    void setTexture(const PTexture &text);

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct Pawn : public Actor
{
public:
    Pawn();

    MovementComponent *movementComponent;
    SpriteComponent   *spriteComponent;
};


struct Player : public Pawn
{
public:
    Player();

    CameraComponent *camera;

    void doInteract();
};
