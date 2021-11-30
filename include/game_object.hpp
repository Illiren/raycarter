#pragma once

#include "stddefines.hpp"
#include "geometry.hpp"
#include "map.hpp"
#include "camera.hpp"
#include <list>


class GameObject
{
public:
     GameObject();
     virtual ~GameObject();
     virtual void update(TReal dt) = 0;
     TString name;
     bool isTickable = true;

     static const std::list<GameObject*> &getRegister() {return _objectRegister;}

     bool isValid() const noexcept {return this != nullptr;}

private:
     static std::list<GameObject*> _objectRegister;
};


class Actor;

class ActorComponent : public GameObject
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
    virtual void interract(Actor *causer) {}
    virtual void collision(Actor *causer) {}
    TReal    direction;
    Vector2D position;
    Location *location;


private:
    ComponentList components;

    friend class ActorComponent;

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
    CameraComponent(Actor *parent);

    Camera camera;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct SpriteComponent : public ActorComponent
{
    SpriteComponent(PTexture text, Actor *parent);
    Sprite sprite;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct Pawn : public Actor
{
public:
    Pawn(PTexture text);

    MovementComponent *movementComponent;
    SpriteComponent   *spriteComponent;
};


struct Player : public Pawn
{
public:
    Player(PTexture text);

    CameraComponent *camera;

    void doInteract();
};
