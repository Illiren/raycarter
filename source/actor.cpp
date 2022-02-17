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
    rect.topleft = position - 0.4f;
    rect.botright = position + 0.4f;

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

    constexpr float length=0.5;
    const auto cosa = std::cos(dir);
    const auto sina = std::sin(dir);

    float nx = pos.x() + cosa*speed*runSpeedMultiplier*dt;
    float ny = pos.y() + sina*speed*runSpeedMultiplier*dt;

    float dx = length*cosa+nx;
    float dy = length*sina+ny;

    if(int(nx) >= 0 && int(nx) < 16 && int(ny) >= 0 && int(ny)< 16)
    {
        const size_t posX = size_t(dx)+size_t(pos.y())*map.size.x();
        const size_t posY = size_t(pos.x())+size_t(dy)*map.size.x();

        if(map[posX] == ' ')
            pos.x() = nx;
        if(map[posY] == ' ')
            pos.y() = ny;
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
