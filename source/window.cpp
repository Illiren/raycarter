#include "window.hpp"

Window::Window(Rectangle2D<TSize> rect, Window *parent) :
      _parent(parent),
      _origin(rect.topleft)
{
    assert(rect.botright.x() > _origin.x() &&
           rect.botright.y() > _origin.y() &&
           "Something is wrong"); //TODO: Assert message

    _width = rect.botright.x() - _origin.x();
    _height = rect.botright.y() - _origin.y();
}

TSize Window::width() const {return _width;}

TSize Window::height() const {return _height;}

Point2D Window::origin() const {return _origin;}
