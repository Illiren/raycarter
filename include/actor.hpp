#pragma once

#include "camera.hpp"
#include "geometry.hpp"
#include "game_object.hpp"

class Actor;
struct Location;

//Actor component is Game Object with pointer to owner
struct ActorComponent : public GameObject
{
public:
    ActorComponent(Actor *actorParent);
    Actor *getOwner() const noexcept;

protected:
    Actor *owner;
};

//Actor is Game Object with position, direction on location
//It's has List of it's components
class Actor : public GameObject
{
public:
    using ComponentList = TArray<ActorComponent*>;
public:
    Actor();
    ~Actor() override;

    FRectangle2D getCollisionBody() const noexcept;
    virtual void onInterract(Actor *) {}
    virtual void onCollision(Actor *) {}

    TReal    collisionRect = 0.5f;
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

//Movement component represent movement of Actor
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

//Camera connect actor with camera
struct CameraComponent : public ActorComponent
{
public:
    CameraComponent(Actor *parent);

    Camera camera;

    // GameObject interface
public:
    void update(TReal dt) override;
};

//Sprite connect actor with sprite
struct SpriteComponent : public ActorComponent
{
    SpriteComponent(Actor *parent);
    Sprite sprite;
    void setTexture(const PTexture &text);

    // GameObject interface
public:
    void update(TReal dt) override;
};

//Pawn is Actor that can move and has some graphical representation
struct Pawn : public Actor
{
public:
    Pawn();

    MovementComponent *movementComponent;
    SpriteComponent   *spriteComponent;
};

//Player is Pawn with camera
struct Player : public Pawn
{
public:
    Player();

    CameraComponent *camera;

    void doInteract();
};


