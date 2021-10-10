#include "raycaster.hpp"
#include "game_object.hpp"
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
    keyMap['a'].keyupEvent = [this](){ _player.turn = 0; };

    keyMap['d'].keydownEvent = [this](){ _player.turn = 1; };
    keyMap['d'].keyupEvent = [this](){ _player.turn = 0; };

    keyMap['w'].keydownEvent = [this](){ _player.speed = _player.maxSpeed; };
    keyMap['w'].keyupEvent = [this](){ _player.speed = 0; };

    keyMap['s'].keydownEvent = [this](){ _player.speed = -_player.maxSpeed; };
    keyMap['s'].keyupEvent = [this](){ _player.speed = 0; };

    keyMap[SDLK_LSHIFT].keydownEvent = [this](){ _player.runningSpeed = 2; };
    keyMap[SDLK_LSHIFT].keyupEvent = [this](){ _player.runningSpeed = 1; };

    keyMap['r'].keydownEvent = [this](){ _camera.origin.x() = 3.456f;
                                         _camera.origin.y() = 2.345f; };

    keyMap['e'].keydownEvent = [this](){ _player.doInteract(); };
}


void RayCarter::draw()
{
    screen.clear(Color(100,100,100));

    const size_t mapRectW = _mapSize.x()/_map.width();
    const size_t mapRectH = _mapSize.y()/_map.height();
    std::vector<float> depthBuffer(_width, 1e3);

#pragma omp parallel for
    for(size_t i=0;i<_width;++i)
    {
        const float angle = _camera.direction - _camera.fov/2 + _camera.fov*i/float(_width);
        const auto acos = std::cos(angle);
        const auto asin = std::sin(angle);

        for(float t=0;t<_camera.distance; t+=.01f)
        {
            const float cx = _camera.origin.x() + t*acos;
            const float cy = _camera.origin.y() + t*asin;

            const TSize pixX = static_cast<TSize>(cx*mapRectW);
            const TSize pixY = static_cast<TSize>(cy*mapRectH);

            const auto mapPos = static_cast<int>(cx) + static_cast<int>(cy) * static_cast<int>(_map.width());
            screen[pixX+(pixY-1)*screen.width()] = Color(160,160,160,0);

            if(_map[mapPos] != ' ') // if something on the way
            {
                const auto itex = _map[mapPos] - '0';

                const auto dist = t*std::cos(angle-_camera.direction);
                depthBuffer[i] = dist;
                const TSize columnH = TSize(_height/dist);
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
                        Color c(column[j]);

                        screen.drawPoint({px,py},Color(c));
                    }
                }

                break;
            }
        }
    }

    //screen.drawRectangle(_mapOrigin,_mapSize.x(),mapRectW,Color::White);

#pragma omp parallel for collapse(2)
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

    auto &drawList = Drawable::getRegister();

    for(auto &drawable : drawList)
    {
        Sprite* s = dynamic_cast<Sprite*>(drawable);
        s->updateDist(_camera.origin.x(),_camera.origin.y());
        drawSprite(*s,depthBuffer);
    }

    screen.update();
}

void RayCarter::drawSprite(const Sprite &sprite, const TArray<float> &depthBuffer)
{
    float spriteDir = atan2(sprite.y - _camera.origin.y(), sprite.x - _camera.origin.x());
    while(spriteDir - _camera.direction > M_PI) spriteDir -= 2*M_PI;
    while(spriteDir - _camera.direction < - M_PI) spriteDir += 2*M_PI;

    auto dist = sprite.dist < 1 ? 1 : sprite.dist;
    TSize spriteScreenSize = std::min(static_cast<int>(_width/dist), static_cast<int>(_height/dist));
    int hOffset = (spriteDir - _camera.direction)*(_width)/(_camera.fov) + (_width)/2 - spriteScreenSize/2;
    int vOffset = _height/2 - spriteScreenSize/2;

#pragma omp parallel for
    for(TSize i=0;i<spriteScreenSize;++i)
    {
        if(hOffset+int(i)<0 || hOffset+i >= _width) continue;
        if(depthBuffer[hOffset + i]<sprite.dist) continue;
        for(TSize j=0;j<spriteScreenSize;++j)
        {
            if(vOffset+int(j)<0 || vOffset+j>=_height) continue;
            Color color = sprite.get(i,j,spriteScreenSize);
            if(color.alpha() > 128)
            {
                const auto x = hOffset+i;
                const auto y = vOffset+j;

                screen.drawPoint({x,y},color);
            }
        }
    }
}

void RayCarter::physX()
{
    auto list = GameObject::getRegister();
    for(auto &o : list)
    {
        auto actor = dynamic_cast<Actor*>(o);
        if(!actor) continue;

        for(auto &o2 : list)
        {
            if(o == o2) continue;
            auto actor2 = dynamic_cast<Actor*>(o2);
            if(!actor2) continue;
            if(collisionCheck(*actor, *actor2))
            {
                actor->collision(actor2);
                actor2->collision(actor);
            }
        }
    }
}

bool RayCarter::collisionCheck(const Actor &rect1, const Actor &rect2)
{
    const auto x1 = rect1.position().x() - rect1.rectSize.x();
    const auto w1 = rect1.rectSize.x() * 2;

    const auto y1 = rect1.position().y() - rect1.rectSize.y();
    const auto h1 = rect1.rectSize.y() * 2;

    const auto x2 = rect2.position().x() - rect2.rectSize.x();
    const auto w2 = rect2.rectSize.x() * 2;

    const auto y2 = rect2.position().y() - rect2.rectSize.y();
    const auto h2 = rect2.rectSize.y() * 2;

    if( (x1 < x2 + w2) &&
        (x1 + w1 > x2) &&
        (y1 < y2 + h2) &&
        (y1 + h1 > y2) )
        return true;
    return false;
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
    physX();
    auto list = GameObject::getRegister();
    for(const auto &o : list)
        o->update(lag);
    //_player.update(lag);
}
