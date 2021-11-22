#include "raycaster.hpp"
#include "game_object.hpp"
#include <iostream>


const char RayCarter::mapdata[] =
                "1111111111111111"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "3              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1              1"\
                "1111111111111111";



RayCarter::RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit, Window *parent) :
      Window(winSize,parent),
      GameLoop(fpsLimit),
      screen(GetScreen()),
      keyMap(GetInputManager()),
      _map(mapdata,mapW,mapH),
      _player(_camera,_map),
      _mapOrigin({0,0}),
      _mapSize({100,100}),
      _guiHeight(96),
      _font("smallfont.bmp",15,15,16,16,1,' ')
{
    screen.resize(width(),height());
    if(!screen.init())
    {
        std::cout << "Can't initialize screen fb\n" << std::endl;
        return;
    }

    _camera.direction = 4.5f*static_cast<float>(M_PI);
    _camera.origin.x() = 3.456f;
    _camera.origin.y() = 2.345f;
    _camera.fov = 1;// static_cast<float>(M_PI)/3.14f;
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

RayCarter::~RayCarter() {}

Map &RayCarter::map() {return _map;}


void RayCarter::draw()
{
    //screen.clear(Color(100,100,100));
    const size_t mapRectW = _mapSize.x()/_map.width();
    const size_t mapRectH = _mapSize.y()/_map.height();
    std::vector<float> depthBuffer(_width, 1e3);

    Point2D org = {mapRectW,_height-_guiHeight};

    const Texture &floor = _map.getTexture(1);
    auto flw = floor.w;
    auto flh = floor.h;
    const Texture &ceiling = _map.getTexture(1);

    float dirX = std::cos(_camera.direction),
          dirY = std::sin(_camera.direction);

    constexpr float PX = 0,
                    PY = 0.56;

    const auto pos = _player.position();
    const Vector2D rayDir0(dirX + PY*dirY, dirY - PY*dirX);
    const Vector2D rayDir1(dirX - PY*dirY, dirY + PY*dirX);
    const Math::Vector2D<int> texsize(floor.w, floor.h);
    auto diff =  (rayDir1 - rayDir0)/_width;

    for(TSize i=_height/2+1; i<_height;++i)
    {
        float posZ = _height/2;
        float rowDistance = posZ/(i-_height/2);
        Vector2D floorStep = rowDistance * diff;
        Vector2D floorPos = pos + rayDir0 * rowDistance;

#ifndef DEBUG
#pragma omp parallel for
#endif
        for(TSize j=0; j<_width; ++j)
        {
            Vector2D floorjpos = floorPos + (floorStep*j);
            Math::Vector2D<int> cellTexPos = floorjpos;
            int tx = (int)(texsize.x() * (floorjpos.x() - cellTexPos.x())) & (texsize.x() - 1);
            int ty = (int)(texsize.y() * (floorjpos.y() - cellTexPos.y())) & (texsize.y() - 1);

            //Math::Vector2D<int> t = static_cast<Math::Vector2D<int>>(static_cast<Vector2D>(texsize)*(floorjpos-static_cast<Vector2D>(cellTexPos))) & (texsize-1);

            auto color = floor.get(tx,ty);
            color = (color>>1) & 8355711;// darkness
            screen.drawPoint({j,i},color);
            color = ceiling.get(tx,ty);
            screen.drawPoint({j,_height-i-1},color);
        }
    }


    screen.drawRectangle(org,_width,_guiHeight,Color(150,150,150,0));

#ifndef DEBUG
#pragma omp parallel for collapse(2)
#endif
    for(TSize j=0; j<_map.height(); ++j)
        for(TSize i=0; i<_map.width(); ++i)
        {
            if(_map[i+j*_map.width()]==' ')
                continue;

            const TSize rectX = i*mapRectW;
            const TSize rectY = _height-6-j*mapRectH;

            //const auto icolor = _map[i+j*_map.width()] - '0';

            screen.drawRectangle({rectX,rectY},mapRectW,mapRectH,Color::Black);
        }
#ifndef DEBUG
#pragma omp parallel for
#endif
    for(size_t i=0;i<_width;++i) // Wall Casting
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
            screen.drawPoint({pixX, (_height-pixY-1)}, Color::White);

            if(_map[mapPos] != ' ') // if something on the way
            {
                const auto dist = t*std::cos(angle-_camera.direction);
                depthBuffer[i] = dist;

                const auto itex = _map[mapPos] - '0';
                auto tex = _map.getTexture(itex);

                const TSize columnH = TSize(_height/dist);
                auto xtex = wall2texcoord(cx,cy,tex.w);

                TArray<uint32_t> column = tex.getScaledColumn(xtex, columnH);

                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + _height/2 - columnH/2;
                    if(py>=0 && py < _height-_guiHeight)
                    {
                        Color c(column[j]);
                        //if(py < _height-_guiHeight)
                            screen.drawPoint({i,py},Color(c));
                    }
                }

                break;
            }
        }        
    }

    const TSize playerPosX = _camera.origin.x()*mapRectW-2;
    const TSize playerPosY = _height-_camera.origin.y()*mapRectH-2;
    screen.drawRectangle({playerPosX,playerPosY},5,5,Color::Black);

    auto &drawList = Drawable::getRegister();
    Drawable::sort();

    for(auto &drawable : drawList)
    {
        //drawable->updateDist(_camera.origin.x(),_camera.origin.y());
        Sprite *n = dynamic_cast<Sprite*>(drawable);
        if(n)
            drawSprite(*n,depthBuffer);
    }

    drawGUI();
    screen.update();    
}


void RayCarter::drawSprite(const Sprite &sprite, const TArray<float> &depthBuffer)
{
    auto texture = sprite.tex.lock();
    float spriteDir = atan2(sprite.y - _camera.origin.y(), sprite.x - _camera.origin.x());
    while(spriteDir - _camera.direction > M_PI) spriteDir -= 2*M_PI;
    while(spriteDir - _camera.direction < - M_PI) spriteDir += 2*M_PI;

    auto dist = sprite.dist < 1 ? 1 : sprite.dist;
    TSize spriteScreenSize = std::min(static_cast<int>(_width/dist), static_cast<int>(_height/dist));
    int hOffset = (spriteDir - _camera.direction)*(_width)/(_camera.fov) + (_width)/2 - spriteScreenSize/2;
    int vOffset = _height/2 - spriteScreenSize/2;

#ifndef DEBUG
#pragma omp parallel for
#endif
    for(TSize i=0;i<spriteScreenSize;++i)
    {
        if(hOffset+int(i)<0 || hOffset+i >= _width) continue;
        if(depthBuffer[hOffset + i]<sprite.dist) continue;
        for(TSize j=0;j<spriteScreenSize;++j)
        {
            if(vOffset+int(j)<0 || vOffset+j>=_height) continue;
            Color color = texture->get(i,j,spriteScreenSize);
            if(color.alpha() > 128)
            {
                const auto x = hOffset+i;
                const auto y = vOffset+j;
                if(y < _height-_guiHeight)
                    screen.drawPoint({x,y},color);
            }
        }
    }
}

void RayCarter::drawGUI()
{
    auto text = _player.face.lock();
    auto w = text->w;
    auto h = text->h;
    auto scaleMuliplier = 2.5;
    auto hscale = static_cast<int>(h/scaleMuliplier);
    auto wscale = static_cast<int>(w/scaleMuliplier);
    auto xoffset = _width/2 - wscale/2;
    auto yoffset = _height-_guiHeight;
    screen.drawTexture(*text, {xoffset, yoffset}, scaleMuliplier, scaleMuliplier);

    auto wtext = _player.weapontext.lock();
    w = wtext->w;
    h = wtext->h;
    auto weaponScale = 0.5;
    hscale = static_cast<int>(h/weaponScale);
    wscale = static_cast<int>(w/weaponScale);
    xoffset = _width/2-wscale/2;
    yoffset = _height-_guiHeight-hscale+60;

    screen.drawTexture(*wtext, {{xoffset, yoffset}, {_width-1,_height-_guiHeight}}, weaponScale, weaponScale);

    auto fpstext = _font.createText(std::to_string(fps()) + " FPS!");
    screen.drawTexture(fpstext,{10,10});
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
            //if(collisionCheck(*actor, *actor2))
            if(intersect(actor->getCollisionRect(),actor2->getCollisionRect()))
            {
                actor->collision(actor2);
                actor2->collision(actor);
            }
        }
    }
}

int RayCarter::wall2texcoord(float hx, float hy, int tw)
{
    float x = hx - floor(hx+.5);
    float y = hy - floor(hy+.5);

    int tex = x * tw;
    if(abs(y) > abs(x))
        tex = y*tw;
    if(tex < 0)
        tex+=tw;
    return tex;
}


void RayCarter::init()
{

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

    auto &drawList = Drawable::getRegister();
    for(auto &drawable : drawList)
        drawable->updateDist(_camera.origin.x(),_camera.origin.y());

    //_player.update(lag);
}

