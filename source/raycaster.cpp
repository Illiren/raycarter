#include "raycaster.hpp"
#include "game_object.hpp"
#include <iostream>


CustomServer::CustomServer(uint16_t nPort) :
      Server<CustomMsgTypes>(nPort)
{}

bool CustomServer::onClientConnect(std::shared_ptr<Connection<CustomMsgTypes> > client)
{
    Message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerAccept;
    client->send(msg);
    return true;
}

void CustomServer::onClientDisconnect(std::shared_ptr<Connection<CustomMsgTypes> > client)
{
    std::cout << "Removing client [" << client->getID() << "]\n";
}

void CustomServer::onMessage(std::shared_ptr<Connection<CustomMsgTypes>> client, Message<CustomMsgTypes> &msg)
{
    switch(msg.header.id)
    {
    case CustomMsgTypes::ServerPing:
    {
        std::cout << "[" << client->getID() << "]: Server Ping\n";
        client->send(msg);
    }
    break;
    case CustomMsgTypes::MessageAll:
    {
        std::cout << "[" << client->getID() << "]: Message All\n";
        Message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerMessage;
        msg << client->getID();
        messageAllClients(msg, client);
    }
    break;
    default:
        break;
    }
}

void CustomClient::pingServer()
{
    Message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerPing;

    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
    msg << timeNow;
    send(msg);
}

void CustomClient::messageAll()
{
    Message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::MessageAll;
    send(msg);
}



RayCarter::RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit, Window *parent) :
      Window(winSize,parent),
      GameLoop(fpsLimit),
      screen(GetScreen()),
      keyMap(GetInputManager()),
      _map(mapdata,mapW,mapH,"walltext.bmp"),
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

    _camera.direction = 3.f*static_cast<float>(M_PI)/2.f;
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


void RayCarter::draw()
{
    //screen.clear(Color(100,100,100));

    const size_t mapRectW = _mapSize.x()/_map.width();
    const size_t mapRectH = _mapSize.y()/_map.height();
    std::vector<float> depthBuffer(_width, 1e3);

    Point2D org = {mapRectW,_height-_guiHeight};

    screen.drawRectangle(org,_width,_guiHeight,Color(150,150,150));

    //#pragma omp parallel for collapse(2)
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

//#pragma omp parallel for
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
            screen[pixX+(_height-pixY-1)*screen.width()] = Color::White;//Color(160,160,160,0);

            if(_map[mapPos] != ' ') // if something on the way
            {
                const auto itex = _map[mapPos] - '0';

                const auto dist = t*std::cos(angle-_camera.direction);
                depthBuffer[i] = dist;
                const TSize columnH = TSize(_height/dist);
                auto xtex = _map.wall2texcoord(cx,cy);

                TArray<uint32_t> column = _map.getTexture(itex, xtex, columnH);

                TSize wallTop = _height/2 - columnH/2;
                TSize wallBottom = wallTop+columnH;

                for(TSize j=0;j<_height;++j)
                {
                    if(j >= _height-_guiHeight) break;
                    if(j < wallTop)
                    {
                        Color color(150*(j/(float)_height*2), 50 + 150 * (j/(float)_height*2), 200);
                        screen.drawPoint({i,j},color);
                    }
                    else if((j >= wallBottom) && (j < _height-_guiHeight))
                    {
                        Color color(120 - 70 * ((_height - j)/ (float)_height * 2), 150 - 50 * ((_height - j)/(float)_height*2), 20);
                        screen.drawPoint({i,j},color);
                    }
                }
                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + screen.height()/2 - columnH/2;                    
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

    for(auto &drawable : drawList)
    {
        drawable->updateDist(_camera.origin.x(),_camera.origin.y());
        Sprite* s = dynamic_cast<Sprite*>(drawable);
        if(s)
        {
            drawSprite(*s,depthBuffer);
            continue;
        }
        NSprite *n = dynamic_cast<NSprite*>(drawable);
        drawSprite(*n,depthBuffer);
    }

    drawGUI();
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

//#pragma omp parallel for
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
                if(y < _height-_guiHeight)
                    screen.drawPoint({x,y},color);
            }
        }
    }
}

void RayCarter::drawSprite(const NSprite &sprite, const TArray<float> &depthBuffer)
{
    auto texture = sprite.tex.lock();
    float spriteDir = atan2(sprite.y - _camera.origin.y(), sprite.x - _camera.origin.x());
    while(spriteDir - _camera.direction > M_PI) spriteDir -= 2*M_PI;
    while(spriteDir - _camera.direction < - M_PI) spriteDir += 2*M_PI;

    auto dist = sprite.dist < 1 ? 1 : sprite.dist;
    TSize spriteScreenSize = std::min(static_cast<int>(_width/dist), static_cast<int>(_height/dist));
    int hOffset = (spriteDir - _camera.direction)*(_width)/(_camera.fov) + (_width)/2 - spriteScreenSize/2;
    int vOffset = _height/2 - spriteScreenSize/2;

    //#pragma omp parallel for
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
            if(intersect(actor->getRect(),actor2->getRect()))
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
    //_player.update(lag);
}

