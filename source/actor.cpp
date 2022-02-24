#include "location.hpp"
#include "actor.hpp"
#include "resourcemanager.hpp"

ActorComponent::ActorComponent(Actor *actorParent) :
      GameObject(),
      owner(actorParent)
{
    owner->components.push_back(this);
}

Actor *ActorComponent::getOwner() const noexcept {return owner;}


Actor::Actor()
{}

Actor::~Actor()
{    
    for(auto component : components)
        delete component;
    GameObject::~GameObject();
}


FRectangle2D Actor::getCollisionBody() const noexcept
{
    Rectangle2D<TReal> rect;
    rect.topleft = position - collisionRect;
    rect.botright = position + collisionRect;

    return rect;
}

void Actor::update(TReal dt)
{
    (void)dt;//Unused
}


MovementComponent::MovementComponent(Actor *parent) :
      ActorComponent(parent)
{}

void MovementComponent::update(TReal dt)
{
    if(!owner->location) return;
    TReal &dir = owner->direction;
    Vector2D &pos = owner->position;
    Location &map = *owner->location;



    dir += float(turn)*maxSpeed*dt;
    Vector2D rotator = {std::cos(dir),
                        std::sin(dir)};

    const Vector2D newPos = pos + rotator*speed*runSpeedMultiplier*dt;

    auto sign = (speed > 0) ? 1.f : -1.f;
    const float dimensions = owner->collisionRect;
    Vector2D border =  newPos + sign * rotator * dimensions;

    if(int(newPos.x()) >= 0 && newPos.x() < map.size.x() &&
       int(newPos.y()) >= 0 && newPos.y() < map.size.y())
    {
        const size_t posX = size_t(border.x())+size_t(pos.y())*map.size.x();
        const size_t posY = size_t(pos.x())+size_t(border.y())*map.size.x();

        if(map[posX] == ' ')
            pos.x() = newPos.x();
        if(map[posY] == ' ')
            pos.y() = newPos.y();
    }
}

CameraComponent::CameraComponent(Actor *parent) :
      ActorComponent(parent)
{}

void CameraComponent::update(TReal dt)
{
    (void)dt;
    camera.direction = owner->direction;
    camera.origin    = owner->position;
}


SpriteComponent::SpriteComponent(Actor *parent) :
      ActorComponent(parent),
      sprite(RM().texture("default"),{0.f,0.f})
{}

void SpriteComponent::setTexture(const PTexture &text)
{
    sprite.tex = text;
}

void SpriteComponent::update(TReal dt)
{
    (void)dt;
    sprite.pos = owner->position;
}

Pawn::Pawn() :
    Actor()
{
    movementComponent = new(this) MovementComponent(this);
    spriteComponent = new(this) SpriteComponent(this);
}


Player::Player() :
    Pawn()
{
    camera = new(this) CameraComponent(this);
}

void Player::doInteract()
{
    if(!location) return;
    auto actor = location->trace(position, direction, 2, this);
    if(actor) actor->onInterract(this);
}
