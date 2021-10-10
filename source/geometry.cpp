#include "geometry.hpp"

auto Rectangle::height() const
{
    return botright.x() - topleft.x();
}

auto Rectangle::width() const
{
    return botright.x() - topleft.x();
}
