#include "camera.hpp"
#include "game_object.hpp"
#include "color.hpp"
#include <map>
#include <iostream>

int Viewport::wall2texcoord(TReal hx, TReal hy, int tw)
{
    TReal x = hx - floor(hx+.5);
    TReal y = hy - floor(hy+.5);

    int tex = x * tw;
    if(abs(y) > abs(x))
        tex = y*tw;
    if(tex < 0)
        tex+=tw;
    return tex;
}

void Viewport::drawSprite(const Sprite &sprite)
{
    auto pos = camera->origin;
    auto dir = camera->direction;
    auto fov = camera->fov;
    auto texture = sprite.tex.lock();

    float spriteDir = atan2(sprite.pos.y() - pos.y(), sprite.pos.x() - pos.x());
    while(spriteDir - dir > M_PI) spriteDir -= 2*M_PI;
    while(spriteDir - dir < -M_PI) spriteDir += 2*M_PI;

    auto dist = sprite.dist < 1 ? 1 : sprite.dist;
    TSize spriteScreenSize = std::min(static_cast<int>(_width/dist),
                                      static_cast<int>(_height/dist));

    int hOffset = (spriteDir - dir)*(_width)/(fov) +
                  (_width)/2 - spriteScreenSize/2;
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
                if(y < _height)
                    screen.drawPoint({x,y},color);
            }
        }
    }
}

Viewport::Viewport(Rectangle2D<TSize> rect, Window *parent) :
      Window(rect,parent),
      depthBuffer(_width,1e3),
      screen(GetScreen()),
      _font("smallfont.bmp",15,15,16,16,1,' '),
      mapSize{100,100}
{
    screen.resize(_width,_height);
    if(!screen.init())
    {
        std::cout << "Can't initialize screen fb\n" << std::endl;
        return;
    }
}

void Viewport::setCamera(CameraComponent *cam)
{
    if(!cam) return;
    camera = &cam->camera;
    actor = cam->getOwner();
}

void Viewport::drawWorld()
{
    constexpr float dist = 0.52245f;

    auto location = actor->location;

    minimap = std::move(location->generateMinimap(mapSize));
    _mapRectH = mapSize.y()/location->height;
    _mapRectW = mapSize.x()/location->width;

    if(!actor->location) return;
    for(auto &i : depthBuffer)
        i = 1e3;

    const auto pos = camera->origin;
    const auto direction = camera->direction;
    const auto fov = camera->fov;
    const auto distance = camera->distance;

    // Draw Floor and Ceiling
    const float dirX = std::cos(direction),
                dirY = std::sin(direction);

    const Vector2D rayDir0(dirX + dist*dirY, dirY - dist*dirX);
    const Vector2D rayDir1(dirX - dist*dirY, dirY + dist*dirX);
    auto diff = (rayDir1 - rayDir0)/_width;
    const Math::Vector2D<int> texsize(location->floor.w, location->floor.h);

    const auto minHeight = static_cast<TSize>(_height/distance);

    screen.drawRectangle({0,_height/2-minHeight/2},_width,minHeight,Color(20,20,20));


    for(TSize i=_height/2+minHeight/2; i<_height;++i)
    {
        TReal posZ = _height/2;
        TReal rowDistance = posZ/(i-_height/2);

        const Vector2D floorStep = rowDistance * diff;
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

            auto color = location->floor.get(tx,ty);
            color = (color>>1) & 8355711;
            screen.drawPoint({j,i},color);
            color = location->ceil.get(tx,ty);
            screen.drawPoint({j,_height-i-1},color);
        }
    }



    // Wall Casting
#ifndef DEBUG
#pragma omp parallel for
#endif
    for(TSize i=0;i<_width;++i)
    {
        const float angle = direction - fov/2 + fov*i/float(_width);
        const auto acos = std::cos(angle);
        const auto asin = std::sin(angle);

        //Trace for distance of view
        for(TReal t=0;t<distance; t+=.01f)
        {
            const TReal cx = pos.x() + t*acos;
            const TReal cy = pos.y() + t*asin;
            const auto mapPos = static_cast<int>(cx) +
                                static_cast<int>(cy) *
                                static_cast<int>(location->width);

            //Mini Map Sight of View
            const TSize pixX = minimap.w-static_cast<TSize>((cx+1)*_mapRectW)+1;
            const TSize pixY = minimap.h-static_cast<TSize>((cy+1)*_mapRectH)+1;
            minimap.set(pixX,pixY,Color(160,160,160,160));

            // if something on the way. Wall most likely
            if((*location)[mapPos] != ' ')
            {
                const auto dist = t*std::cos(angle-direction);
                depthBuffer[i] = dist;
                const TSize columnH = TSize(_height/dist);

                const auto itex = (*location)[mapPos] - '0';
                auto tex = location->getWallText(itex);

                auto xtex = wall2texcoord(cx,cy,tex.w);
                TArray<uint32_t> column = tex.getScaledColumn(xtex, columnH);

                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + _height/2 - columnH/2;
                    if(py>=0 && py < _height)
                        screen.drawPoint({i,py},Color(column[j]));
                }
                break;
            }
        }
    }


    //Player on minimap
    const TSize playerPosX = minimap.w-pos.x()*(_mapRectW);
    const TSize playerPosY = minimap.h-pos.y()*(_mapRectH);
    for(TSize i=_mapRectW;i--;)
        for(TSize j=_mapRectH;j--;)
            minimap.set(playerPosX-i,playerPosY-j,Color::Black);
}

void Viewport::drawActors()
{
    auto pawn = dynamic_cast<Pawn*>(actor);
    Drawable *drawable = nullptr;
    if(pawn)
        drawable = &(pawn->spriteComponent->sprite);

    auto &list = Drawable::getRegister();
    const auto pos = camera->origin;

    for(auto &obj : list)
    {
        if(obj == drawable) continue;
        obj->updateDist(pos);
        auto spriteobj = dynamic_cast<Sprite*>(obj);
        drawSprite(*spriteobj);
    }
}

void Viewport::drawHUD()
{    
    screen.drawRectangle({0,_height-mapSize.y()+_mapRectH/2+1},_width,mapSize.y(),Color(100,200,100));
    screen.drawTexture(minimap,{0,_height-minimap.h+_mapRectH/2+1});

    const Point2D posA = {_width/2-(hudWeapon.w/2-20)*2,_height-minimap.h-static_cast<TSize>(hudWeapon.h*1.5f)};
    const Point2D posB = {_width/2+(hudWeapon.w/2)*2, _height-minimap.h+_mapRectH/2+1};
    screen.drawTexture(hudWeapon,{posA,posB},0.5f,0.5f);

    const Point2D posC {_width/2-playerFace.w/4,_height-static_cast<TSize>(playerFace.h/2.7f)};
    screen.drawTexture(playerFace,posC,2.6,2.6);

    Texture fpstext = _font.createText(std::to_string(fps)+" FPS");
    const Point2D posE {10,10};
    screen.drawTexture(fpstext,posE);
}

void Viewport::draw()
{
    if(!actor) return;
    if(!camera) return;

    drawWorld();
    drawActors();
    drawHUD();
    screen.update();
}
