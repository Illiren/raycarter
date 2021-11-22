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


class NActor;

class ActorComponent : public GameObject
{
public:
    ActorComponent(NActor *actorParent);
    NActor *getOwner() const noexcept;

protected:
    NActor *owner;
};


class NActor : public GameObject
{
    using ComponentList = TArray<ActorComponent>;
public:
    NActor();

    FRectangle2D getCollisionBody() const noexcept;

    TReal    direction;
    Vector2D position;
    Location *location;


private:
    ComponentList _actorList;
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

    MovementComponent(NActor *parent);

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
    CameraComponent(NActor *parent);

    Camera camera;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct SpriteComponent : public ActorComponent
{
    SpriteComponent(PTexture text, NActor *parent);
    Sprite sprite;

    // GameObject interface
public:
    void update(TReal dt) override;
};
