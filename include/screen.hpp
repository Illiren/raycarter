#pragma once

#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <functional>
#include "stddefines.hpp"
#include "color.hpp"
#include "geometry.hpp"
#include "texture.hpp"


//TODO: Double buffer and swap buffers

class Screen
{
public:
    ~Screen(); //I hope no one will call it

    friend Screen &GetScreen();

    void resize(TSize width, TSize height);

    bool init();

    void clear(Color defaultColor = Color::White);
    void fill(Point2D start, Point2D end, std::function<Color (Point2D)> coloralg);
    void clearRectacngle(Point2D origin, Point2D end, Color defaultColor = Color::White);
    void clearOffbuffer(Color defaultColor = Color::White);

    void update();

    TUint32 &operator[](TSize pos)
    {
        return (*_framebuffer)[pos];
    }

    const TUint32 &operator[](TSize pos) const
    {
        return (*_framebuffer)[pos];
    }


    inline TSize pixCount() const noexcept {return _framebuffer->size();}

    TSize width() const noexcept {return _width;}
    TSize height() const noexcept {return _height;}

    void drawRectangle(Point2D p1, Point2D p2, Color color);
    void drawRectangle(Point2D origin, TSize height, TSize width, Color color);
    void drawLine(Point2D start, Point2D end, Color color);
    void drawTriangle(Point2D t0, Point2D t1, Point2D t2, Color color);
    void drawPoint(const Point2D &p, Color color);
    void drawTexture(const Texture &text, Point2D p, TReal hscale = 1.f, TReal wscale = 1.f);
    void drawTexture(const Texture &text, Rectangle2D<TSize> p, TReal hscale = 1.f, TReal wscale = 1.f);


    auto rawData() {return _framebuffer->data();}
    inline bool isBusy() const {return _updating;}


protected:
    Screen()
    {}

    Screen(TSize width, TSize height) :
        _width(width),
        _height(height)
    {
        _fb[0] = std::vector<TUint32>(_width*_height, Color::White);
        _fb[1] = std::vector<TUint32>(_width*_height, Color::White);
        //clear();
    }

    inline TSize pointToIndex(const Point2D &p) const noexcept
    {
        const auto index = p.x() + (p.y())*_width;
        assert(index<_framebuffer->size() && "Out of range");
        return index;
    }

    inline void inc(Point2D &p) const noexcept
    {
        if(p.x() + 1 == _width)
        {
            p.x() = 0;
            p.y()++;
        }
        else
        {
            p.x()++;
        }
    }

private:
    bool _updating = false;
    bool _currentFB = 0;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *framebuffer_texture = nullptr;
    TArray<TUint32> *_framebuffer = &_fb[_currentFB];
    TArray<TUint32> _fb[2];

    TSize _width,
          _height;
};

Screen &GetScreen();
