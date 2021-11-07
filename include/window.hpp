#pragma once

#include "geometry.hpp"

class Window
{
public:
    Window(Rectangle2D<TSize> rect, Window *parent = nullptr);
    virtual ~Window() {}

    inline virtual void draw() = 0;

    TSize width() const;
    TSize height() const;
    Point2D origin() const;

protected:
    Window* _parent;
    Point2D _origin;
    TSize   _width,
            _height;
};
