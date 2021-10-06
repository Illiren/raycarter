#include "raycaster.hpp"
#include <iostream>


RayCarter::RayCarter(Rectangle winSize, TReal fpsLimit, Window *parent) :
      Window(winSize,parent),
      GameLoop(fpsLimit),
      screen(GetScreen()),
      keyMap(GetInputManager()),
      _map(mapdata,mapW,mapH,"walltext.bmp"),
      _player(_camera,_map),
      _mapOrigin({0,0}),
      _mapSize({100,100})
{
    screen.resize(width(),height());
    if(!screen.init())
    {
        std::cout << "Can't initialize screen fb\n" << std::endl;
        return;
    }

    _camera.direction = 3.f*static_cast<float>(M_PI)/2.f;
    _camera.origin.x() = 3.456f;
    _camera.origin.y() = 2.345f;
    _camera.fov = static_cast<float>(M_PI)/3.f;
    _camera.distance = 20;

    InputManager &keyMap = GetInputManager();
    keyMap['a'].keydownEvent = [this](){ _player.turn = -1; };
    keyMap['a'].keyupEvent = [this](){_player.turn = 0;};

    keyMap['d'].keydownEvent = [this](){ _player.turn = 1; };
    keyMap['d'].keyupEvent = [this](){ _player.turn = 0; };

    keyMap['w'].keydownEvent = [this](){ _player.walk = 1; };
    keyMap['w'].keyupEvent = [this](){ _player.walk = 0; };

    keyMap['s'].keydownEvent = [this](){ _player.walk = -1; };
    keyMap['s'].keyupEvent = [this](){ _player.walk = 0; };

    keyMap[SDLK_LSHIFT].keydownEvent = [this](){_player.runningSpeed = 2;};
    keyMap[SDLK_LSHIFT].keyupEvent = [this](){_player.runningSpeed = 1;};
}


void RayCarter::draw()
{
    screen.clear(Color(100,100,100));

    const size_t mapRectW = _mapSize.x()/_map.width();
    const size_t mapRectH = _mapSize.y()/_map.height();

#pragma omp parallel for
    for(size_t i=0;i<_width;++i)
    {
        const float angle = _camera.direction - _camera.fov/2 + _camera.fov*i/float(_width);
        const auto acos = std::cos(angle);
        const auto asin = std::sin(angle);

        for(float t=0;t<_camera.distance; t+=.03f)
        {
            const float cx = _camera.origin.x() + t*acos;
            const float cy = _camera.origin.y() + t*asin;

            const TSize pixX = static_cast<TSize>(cx*mapRectW);
            const TSize pixY = static_cast<TSize>(cy*mapRectH);

            const auto mapPos = static_cast<int>(cx) + static_cast<int>(cy) * static_cast<int>(_map.width());
            screen[pixX+(screen.height()-pixY-1)*screen.width()] = Color(160,160,160,0);

            if(_map[mapPos] != ' ')
            {
                //const auto icolor = _ref[mapPos] - '0';
                //assert(icolor<colors.size());

                const auto itex = _map[mapPos] - '0';

                const auto cos = t*std::cos(angle-_camera.direction);
                const TSize columnH = TSize(_height/cos);
                auto xtex = _map.wall2texcoord(cx,cy);

                TArray<uint32_t> column = _map.getTexture(itex, xtex, columnH);

                const auto x = i;
                const auto y = _height/2-columnH/2;

                TSize px = i;

                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + screen.height()/2 - columnH/2;
                    if(py>=0 && py < screen.height())
                    {
                        Color c(column[column.size()-j]);

                        screen.drawPoint({px,py},Color(c));
                    }
                }

                break;
            }
        }
    }

    screen.drawRectangle(_mapOrigin,_mapSize.x(),mapRectW,Color::White);

    for(TSize j=0; j<_map.height(); ++j)
        for(TSize i=0; i<_map.width(); ++i)
        {
            if(_map[i+j*_map.width()]==' ')
                continue;

            const TSize rectX = i*mapRectW;
            const TSize rectY = j*mapRectH;

            const auto icolor = _map[i+j*_map.width()] - '0';

            screen.drawRectangle({rectX,rectY},mapRectW,mapRectH,Color::Black);
        }

    const TSize playerPosX = _camera.origin.x()*mapRectW-2;
    const TSize playerPosY = _camera.origin.y()*mapRectH-2;
    screen.drawRectangle({playerPosX,playerPosY},5,5,Color::Black);

    screen.update();
}

void RayCarter::input()
{
    keyMap.update();
}

void RayCarter::render()
{
    draw();
}

void RayCarter::update(TReal lag)
{
    _player.update(lag);
}
