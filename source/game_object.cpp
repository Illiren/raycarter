#include "game_object.hpp"

std::list<GameObject*> GameObject::_objectRegister;

GameObject::GameObject()
{
    static TSize i = 0; //Bad UID
    name = "Object " + std::to_string(i++);
    _objectRegister.push_back(this);
}

GameObject::~GameObject()
{
    _objectRegister.remove(this);
}

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
    GameObject::~GameObject();
    for(auto component : components)
        delete component;
}

FRectangle2D Actor::getCollisionBody() const noexcept
{
    Rectangle2D<TReal> rect;
    rect.topleft.x() = position.x() - 0.4f;
    rect.topleft.y() = position.y() - 0.4f;

    rect.botright.x() = position.x() + 0.4f;
    rect.botright.y() = position.y() + 0.4f;

    return rect;
}

void Actor::update(TReal dt)
{

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
        const size_t posX = size_t(dx)+size_t(pos.y())*map.width;
        const size_t posY = size_t(pos.x())+size_t(dy)*map.width;

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
    camera.direction = owner->direction;
    camera.origin    = owner->position;
}


SpriteComponent::SpriteComponent(PTexture text, Actor *parent) :
      ActorComponent(parent),
      sprite(text, {0.f,0.f})
{

}

void SpriteComponent::update(TReal dt)
{
    sprite.pos = owner->position;
}

Pawn::Pawn(PTexture text) :
      Actor()
{
    movementComponent = new MovementComponent(this);
    spriteComponent = new SpriteComponent(text,this);
}


Player::Player(PTexture text) :
      Pawn(text)
{
    camera = new CameraComponent(this);
}

void Player::doInteract()
{
    if(!location) return;
    auto actor = location->trace(position, direction, 2, this);
    if(actor) actor->interract(this);
}
