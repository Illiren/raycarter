#include "window.hpp"

Window::Window(Rectangle2D<TSize> rect, Window *parent) :
      _parent(parent),
      _origin(rect.topleft)
{
    _width = rect.width();
    _height = rect.height();
}

TSize Window::width() const {return _width;}

TSize Window::height() const {return _height;}

Point2D Window::origin() const {return _origin;}
