#include "actor.hpp"

ActorComponent::ActorComponent(NActor *actorParent) :
      owner(actorParent)
{}


Actor::Actor() :
      rectSize{0.3f,0.3f}
{}

Actor::~Actor()
{
    GameObject::~GameObject();
}

void Actor::setPos(const Vector2D &v)
{
    if(drawable)
    {
        drawable->x = v.x();
        drawable->y = v.y();
    }
}

void Actor::setDirection(TReal dir)
{

}

Vector2D Actor::position() const
{
    if(drawable)
    {
        return {drawable->x,drawable->y};
    }

    return {-1.f,-1.f};
}

TReal Actor::direction() const
{
    return 1;
}

FRectangle2D Actor::getCollisionRect() const
{
    Rectangle2D<TReal> rect;
    rect.topleft.x() = position().x() - rectSize.x();
    rect.topleft.y() = position().y() - rectSize.y();

    rect.botright.x() = position().x() + rectSize.x();
    rect.botright.y() = position().y() + rectSize.y();

    return rect;
}

void Actor::collision(Actor *another) {}

void Actor::interract(Actor *causer)
{

}

void Actor::update(TReal dt)
{

}


