#include "actor.hpp"

Actor::Actor() :
      rectSize{0.3f,0.3f}
{}

Actor::~Actor()
{}

void Actor::setPos(const Vector2D &v)
{
    if(drawable)
    {
        drawable->x = v.x();
        drawable->y = v.y();
    }
}

Vector2D Actor::position() const
{
    if(drawable)
    {
        return {drawable->x,drawable->y};
    }

    return {-1.f,-1.f};
}

Rectangle2D<TReal> Actor::getRect() const
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
    auto pos = position();

}
