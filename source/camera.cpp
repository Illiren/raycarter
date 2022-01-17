#include "camera.hpp"
#include "actor.hpp"
#include "color.hpp"
#include "location.hpp"
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

#if __PARALLEL == 1
#pragma omp parallel for
#endif
    for(TSize i=0;i<spriteScreenSize;++i)
    {
        if(hOffset+int(i)<0 || hOffset+i >= _width) continue;
        if(depthBuffer[hOffset + i]<sprite.dist) continue;
        for(TSize j=0;j<spriteScreenSize;++j)
        {
            if(vOffset+int(j)<0 || vOffset+j>=_height) continue;
            //Color color = texture->get(i,j,spriteScreenSize);
            Color color = texture->get({i,j},spriteScreenSize);
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
    auto location = actor->location;

    _minimap = std::move(location->generateMinimap(mapSize.x(), mapSize.y()));
    _mapRect = mapSize/Vector2U{location->width, location->height};
    auto mapRectSize = mapSize/Math::Vector2D<TSize>(location->height,location->width);
    //Math::Vector2D<TSize> minimapSize(_minimap.w,_minimap.h);
    Math::Vector2D<TSize> minimapSize(_minimap.size);

    if(!actor->location) return;
    for(auto &i : depthBuffer)
        i = 1e3;

    const auto pos = camera->origin;
    const auto direction = camera->direction;
    const auto fov = camera->fov;
    const auto dist = fov/2.f; //TODO: Rename
    const auto distance = camera->distance;

    // Draw Floor and Ceiling
    const auto dirRotator = Vector2D(std::cos(direction),std::sin(direction));

    const Vector2D rayDir0(dirRotator.x() + dist*dirRotator.y(),
                           dirRotator.y() - dist*dirRotator.x());

    const Vector2D rayDir1(dirRotator.x() - dist*dirRotator.y(),
                           dirRotator.y() + dist*dirRotator.x());

    auto diff = (rayDir1 - rayDir0)/_width;
    //const Math::Vector2D<int> texsize(location->floor.w, location->floor.h);
    const Math::Vector2D<int> texsize(location->floor.size.x(), location->floor.size.y());

    const auto minHeight = static_cast<TSize>(_height/distance);
    screen.drawRectangle({0,_height/2-minHeight/2},_width,minHeight,Color(20,20,20));

    // Wall Casting
#if __PARALLEL == 1
#pragma omp parallel for
#endif
    for(TSize i=0;i<_width;++i)
    {
        const float angle = direction - fov/2 + fov*i/float(_width);
        Vector2D rotator(std::cos(angle),
                         std::sin(angle));

        TSize columnH = minHeight; //if not hit, then draw froor and cells for minHeight
        //Trace for distance of view
        for(TReal t=0.f; t<distance; t+=.01f)
        {
            const auto distPos = pos + rotator*t;

            const auto mapPos = static_cast<int>(distPos.x()) +
                                static_cast<int>(distPos.y()) *
                                static_cast<int>(location->width);

            //Mini Map Sight of View
            const auto pixPos = minimapSize-static_cast<Math::Vector2D<TSize>>((distPos+1)*static_cast<Vector2D>(mapRectSize))+2;
            //_minimap.set(pixPos.x(),pixPos.y(),Color(160,160,160,160));
            _minimap.set({pixPos.x(),pixPos.y()},Color(160,160,160,160));

            // if something on the way. Wall most likely
            if((*location)[mapPos] != ' ')
            {
                const auto dist = t*std::cos(angle-direction);
                depthBuffer[i] = dist;
                columnH = TSize(_height/dist);

                const auto itex = (*location)[mapPos] - '0';
                auto tex = location->getWallText(itex);

                //auto xtex = wall2texcoord(distPos.x(),distPos.y(),tex.w);
                auto xtex = wall2texcoord(distPos.x(),distPos.y(),tex.size.x());

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

        //Around walls draw cells and floors
        for(TSize j=_height/2+columnH/2; j<_height; ++j)
        {
            TReal rowDistance = _height/(2.f*(j-_height/2)); //hyperbola, close to middle - farest

            const auto floorStep = rowDistance * diff;
            auto floorPos = pos + rayDir0 * rowDistance;

            auto floorjpos = floorPos + (floorStep*i);
            Math::Vector2D<int> cellTexPos = floorjpos;
            Math::Vector2D<int> t = static_cast<Vector2D>(texsize)*(floorjpos-static_cast<Vector2D>(cellTexPos));
            t&=(texsize-1); // limit by texsize size. if texsize == 64 then only 5 bit integer will be used ( 64-1 = 0x00111111), otherwise it will be overflow in some conditions

            //auto color = location->floor.get(t.x(),t.y());
            auto color = location->floor.get(t);

            color = (color>>1) & 8355711; //Darker
            screen.drawPoint({i,j},color);
            //color = location->ceil.get(t.x(),t.y());
            color = location->ceil.get(t);
            screen.drawPoint({i,_height-j-1},color);
        }
    }


    //Player on minimap
    const auto playerPos = minimapSize-pos*mapRectSize;
    for(TSize i=_mapRect.x();i--;)
        for(TSize j=_mapRect.y();j--;)
            //_minimap.set(playerPos.x()-i,playerPos.y()-j,Color::Black);
            _minimap.set({playerPos.x()-i,playerPos.y()-j},Color::Black);
}

void Viewport::drawActors()
{
    auto pawn = dynamic_cast<Pawn*>(actor);
    Drawable *drawable = nullptr;
    if(pawn) drawable = &(pawn->spriteComponent->sprite);

    auto &list = Drawable::getRegister();
    const auto pos = camera->origin;

    for(auto &obj : list)
    {
        if(obj == drawable) continue;
        obj->updateDist(pos);
        if(obj->dist > camera->distance) continue;
        auto spriteobj = dynamic_cast<Sprite*>(obj);
        drawSprite(*spriteobj);
    }
}

void Viewport::drawHUD()
{
    /*
    screen.drawRectangle({0,_height-mapSize.y()+_mapRect.y()/2+1},_width,mapSize.y(),Color(100,200,100));
    screen.drawTexture(_minimap,{0,_height-_minimap.h+_mapRect.y()/2+1});

    const Point2D posA = {_width/2-(hudWeapon.w/2-20)*2,_height-_minimap.h-static_cast<TSize>(hudWeapon.h*1.5f)};
    const Point2D posB = {_width/2+(hudWeapon.w/2)*2, _height-_minimap.h+_mapRect.y()/2+1};
    screen.drawTexture(hudWeapon,{posA,posB},0.5f,0.5f);

    const Point2D posC {_width/2-playerFace.w/4,_height-static_cast<TSize>(playerFace.h/2.7f)};*/
    screen.drawRectangle({0,_height-mapSize.y()+_mapRect.y()/2+1},_width,mapSize.y(),Color(100,200,100));
    screen.drawTexture(_minimap,{0,_height-_minimap.size.y()+_mapRect.y()/2+1});

    const Point2D posA = {_width/2-(hudWeapon.size.x()/2-20)*2,_height-_minimap.size.y()-static_cast<TSize>(hudWeapon.size.y()*1.5f)};
    const Point2D posB = {_width/2+(hudWeapon.size.x()/2)*2, _height-_minimap.size.y()+_mapRect.y()/2+1};
    screen.drawTexture(hudWeapon,{posA,posB},0.5f,0.5f);

    const Point2D posC {_width/2-playerFace.size.x()/4,_height-static_cast<TSize>(playerFace.size.y()/2.7f)};
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




