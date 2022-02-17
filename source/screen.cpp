#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "screen.hpp"


Screen::~Screen()
{
    if(framebuffer_texture)
        SDL_DestroyTexture(framebuffer_texture);
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void Screen::resize(TSize width, TSize height)
{
    _width = width;
    _height = height;
    _fb[0] = std::vector<TUint32>(_width*_height, Color::White);
    _fb[1] = std::vector<TUint32>(_width*_height, Color::White);
}

bool Screen::init()
{
    std::cout << "Initialize screen..." << std::endl;
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    if(SDL_CreateWindowAndRenderer(_width, _height, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer))
    {
        std::cout << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    //SDL_PIXELFORMAT_ARGB8888
    framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888 , SDL_TEXTUREACCESS_STREAMING, _width, _height);

    if(!framebuffer_texture)
    {
        std::cout << "Failed to create framebuffer texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Screen::clear(Color defaultColor)
{
    //_framebuffer = std::vector<TUint32>(_width*_height, defaultColor);
    std::fill(_framebuffer->begin(),_framebuffer->end(),defaultColor);
}

void Screen::fill(Point2D start, Point2D end, std::function<Color (Point2D)> coloralg)
{
    for(;start!=end;inc(start))
        (*_framebuffer)[pointToIndex(start)] = coloralg(start);
}

void Screen::clearRectacngle(Point2D origin, Point2D end, Color defaultColor)
{
    const auto startpoint = origin.x() + (origin.y())*_width;
    const auto endpoint = end.x() + end.y()*_width;

    std::fill(_framebuffer->begin()+startpoint, _framebuffer->begin()+endpoint, defaultColor);
}

void Screen::clearOffbuffer(Color defaultColor)
{
    std::fill(_fb[_currentFB^1].begin(),_fb[_currentFB^1].end(),defaultColor);
}

void Screen::update()
{
    _updating=true; //NOTE: For future use

    SDL_UpdateTexture(framebuffer_texture,nullptr,reinterpret_cast<void *>(_framebuffer->data()),static_cast<int>(_width*4));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,framebuffer_texture,nullptr,nullptr);
    SDL_RenderPresent(renderer);

    _currentFB^=1;
    _framebuffer = &_fb[_currentFB];

    _updating = false;
}


void Screen::drawRectangle(Point2D p1, Point2D p2, Color color)
{
    assert(p1.x()<p2.x() && p1.y()<p2.y());
    assert(_framebuffer->size() == _height*_width);

    while(p1.x()<p2.x())
    {
        while(p1.y()<p2.y())
        {
            drawPoint({p1.x(),p1.y()},color);
            ++p1.y();
        }
        ++p1.x();
    }
}

void Screen::drawRectangle(Point2D origin, TSize width, TSize height, Color color)
{
    assert(_framebuffer->size() == _height*_width);

#if __PARALLEL == 0
#pragma omp parallel for collapse(2)
#endif
    for(TSize i = 0; i<width; ++i)
        for(TSize j=0; j<height; ++j)
        {
            TSize cx = origin.x()+i;
            TSize cy = origin.y()+j;
            if(cx>=_width || cy >= _height) continue;
            drawPoint({cx,cy},color);
        }
}

void Screen::drawLine(Point2D start, Point2D end, Color color)
{
    int x0 = static_cast<int>(start.x());
    int y0 = static_cast<int>(start.y());

    int x1 = static_cast<int>(end.x());
    int y1 = static_cast<int>(end.y());

    bool steep = false;

    if(std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0,y0);
        std::swap(x1,y1);
        steep = true;
    }

    if(x0 > x1)
    {
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    const int dx = x1 - x0;
    const int dy = y1 - y0;
    const int derror2 = std::abs(dy)*2;
    int error2 = 0;
    TSize y = y0;

    const auto dty = (y1>y0?1:-1);
    if(steep)
        for (TSize x = x0;x<=x1;++x)
        {
            drawPoint({y,x},color);

            error2 += derror2;
            if(error2 > dx)
            {
                y += dty;
                error2 -= dx*2;
            }
        }
    else
        for (TSize x = x0;x<=x1;++x)
        {
            drawPoint({x,y},color);

            error2 += derror2;
            if(error2 > dx)
            {
                y += dty;
                error2 -= dx*2;
            }
        }


}

void Screen::drawTriangle(Point2D t0, Point2D t1, Point2D t2, Color color)
{
    if(t0.y() == t1.y() && t0.y() == t2.y())
        return;

    if(t0.y() > t1.y())
        std::swap(t0,t1);
    if(t0.y() > t2.y())
        std::swap(t0,t2);
    if(t1.y() > t2.y())
        std::swap(t1,t2);

    auto totalHeight = t2.y() - t0.y();

#if __PARALLEL == 0
#pragma omp parallel for
#endif
    for(int i=0;i<totalHeight;++i)
    {
        const bool secondHalf = i>t1.y()-t0.y() || t1.y() == t0.y();
        auto segmentHeight = secondHalf ? t2.y() - t1.y() : t1.y() - t0.y();
        assert(segmentHeight != 0 && "Zero division");

        TReal alpha = static_cast<TReal>(i)/totalHeight;
        TReal beta = static_cast<TReal>(i-(secondHalf ? t1.y()-t0.y() : 0))/segmentHeight;

        Point2D A = (t2-t0);
        A.x()*=alpha;
        A.y()*=alpha;
        A += t0;

        Point2D B = secondHalf ? (t2-t1) : (t1-t0);
        B.x()*=beta;
        B.y()*=beta;
        B += secondHalf ? t1 : t0;

        if(A.x() > B.x())
            std::swap(A,B);

        for(auto j=A.x();j<B.x();++j)
            drawPoint({j,t0.y()+i},color);
    }
}

void Screen::drawPoint(const Point2D &p, Color color)
{    
    (*_framebuffer)[pointToIndex(p)] = color;
}

void Screen::drawTexture(const Texture &text, Point2D p, TReal hs, TReal ws)
{
    //const auto w = text.w;
    //const auto h = text.h;
    const auto w = text.size.x();
    const auto h = text.size.y();
    const auto hscale = static_cast<int>(h/hs);
    const auto wscale = static_cast<int>(w/ws);
    const auto xoffset = p.x();
    const auto yoffset = p.y();

#if __PARALLEL == 0
#pragma omp parallel for collapse(2)
#endif
    for(TSize i = 0;i<wscale;++i)
        for(TSize j=0;j<hscale;++j)
        {
            //Color color = text.get(i*h/hscale,j*w/wscale);
            Color color = text.get({i*h/hscale,j*w/wscale});
            if(color.a() > 127)
            {
                const auto x = xoffset+i;
                const auto y = yoffset+j;
                if(x < _width && y < _height)
                    drawPoint({x,y},color);
            }
        }
}

void Screen::drawTexture(const Texture &text, Rectangle2D<TSize> p, TReal hs, TReal ws)
{
    //const auto w = text.w;
    //const auto h = text.h;
    const auto w = text.size.x();
    const auto h = text.size.y();
    const auto orig = p.topleft;
    const auto end = p.botright;
    assert(orig.x() > 0 && orig.y() > 0 && "Out of range");
    assert(end.x() < _width && end.y() < _height && "Out of range too");
    const auto hscale = static_cast<int>(h/hs);
    const auto wscale = static_cast<int>(w/ws);

#if __PARALLEL == 0
#pragma omp parallel for collapse(2)
#endif
    for(TSize i = 0; i < wscale; ++i)
        for(TSize j = 0; j < hscale; ++j)
        {
            //Color color = text.get(i*h/hscale,j*w/wscale);
            Color color = text.get({i*h/hscale,j*w/wscale});
            const auto x = orig.x() + i;
            const auto y = orig.y() + j;

            const bool cond = (color.a() > 128) && (x < end.x()) && (y < end.y());
            if(cond)
                drawPoint({x,y}, color);
        }
}


Screen &GetScreen()
{
    static Screen screen;
    return screen;
}
