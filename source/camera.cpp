#include "camera.hpp"
#include "actor.hpp"
#include "color.hpp"
#include "location.hpp"
#include <map>
#include <iostream>

int Viewport::wall2texcoord(Vector2D h, int tw)
{
    auto pos = h - Vector2D{floor(h.x()+.5f),floor(h.y()+.5f)};

    int tex = pos.x()*tw;
    if(abs(pos.y()) > abs(pos.x()))
        tex = pos.y()*tw;
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
            Color color = texture->get({i,j},spriteScreenSize);
            if(color.a() > 128)
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
      mapSize{100,100},
      depthBuffer(_width,1e3),
      screen(GetScreen()),      
      _font("smallfont.bmp",15,15,16,16,1,' ')

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

    _minimap = location->generateMinimap(mapSize);
    _mapRect = mapSize/location->size;
    auto mapRectSize = mapSize/location->size;

    if(!actor->location) return;
    for(auto &i : depthBuffer)
        i = 1e3;

    const auto pos = camera->origin;
    const auto direction = camera->direction;
    const auto fov = camera->fov;
    const auto dist = fov/2.f; //TODO: Rename
    const auto distance = camera->distance;

    // Draw floor and ceiling
    const auto dirRotator = Vector2D(std::cos(direction),
                                     std::sin(direction));


    const Vector2D rayDir0(dirRotator.x() + dist*dirRotator.y(),
                           dirRotator.y() - dist*dirRotator.x());

    const Vector2D rayDir1(dirRotator.x() - dist*dirRotator.y(),
                           dirRotator.y() + dist*dirRotator.x());

    auto diff = (rayDir1 - rayDir0)/_width;

    const auto minHeight = static_cast<TSize>(_height/distance)+5;
    screen.drawRectangle({0,_height/2-minHeight/2},_width,minHeight,Color(20,20,20));


    // Wall Casting
#if __PARALLEL == 1
#pragma omp parallel for
#endif

    for(TSize i=0;i<_width;++i)
    {
        LinearColor darkness(0.f,0.f,0.f,0.0f);
        const float angle = direction - fov/2 + fov*i/float(_width);
        Vector2D rotator(std::cos(angle),
                         std::sin(angle));

        TSize columnH = minHeight; //if not hit, then draw froor and cells for minHeight        

        //Trace for distance of view
        TReal t=0.f;
        for(;t<distance; t+=dt)
        {
            const auto distPos = pos + rotator*t;

            //Mini Map Sight of View
            const auto pixPos = _minimap.size-((distPos+1)*mapRectSize)+2;

            _minimap.set(pixPos,Color(160,160,160,160));

            // if something on the way. Wall most likely
            if((*location)[distPos] != ' ')
            {
                const auto dist = t*std::cos(angle-direction);
                depthBuffer[i] = dist;
                columnH = TSize(_height/dist);

                const auto itex = (*location)[distPos] - '0';
                auto tex = location->getWallText(itex);

                auto xtex = wall2texcoord(distPos,tex.size.x());

                TArray<uint32_t> column = tex.getScaledColumn(xtex, columnH);

                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + _height/2 - columnH/2;
                    if(py>=0 && py < _height)
                    {
                        const TReal percent = (10/distance)*t;
                        darkness.a()=percent;
                        LinearColor color = Color(column[j]);
                        color.a() = 0.5f;
                        color+=darkness;
                        screen.drawPoint({i,py},color);
                    }
                }

                break;
            }
        }


        //Around walls draw ceiling and floors
        const TReal range = _height - _height/2;
        for(TSize j=_height/2+columnH/2; j<_height; ++j)
        {            
            const TReal rowDistance = _height/(2.f*(j-_height/2)); //hyperbola, close to middle - farest
            const TReal percent = (100.f/range)*(j-_height/2.f);
            darkness.a()=1.f-percent/100.f;

            const auto floorStep = rowDistance * diff;
            auto floorPos = pos + rayDir0 * rowDistance;

            auto floorjpos = floorPos + (floorStep*i);
            const Vector2I cellPos = floorjpos;
            Vector2I texPos = static_cast<Vector2F>(location->floor.size)*(floorjpos-cellPos);
            texPos&=(location->floor.size-1); // limit by texsize size. if texsize == 64 then only 5 bit integer will be used ( 64-1 = 0x00111111), otherwise it will be overflow in some conditions

            //Floor
            LinearColor color = location->floor.get(texPos);
            color.a() = 0.1f;
            color+=darkness;
            screen.drawPoint({i,j},color);

            //Ceil
            color = location->ceil.get(texPos);
            color.a() = 0.1f;
            color+=darkness;
            screen.drawPoint({i,_height-j-1},color);
        }
    }


    //Player on minimap
    const auto playerPos = _minimap.size-pos*mapRectSize;
    Vector2U it;
    for(it.x()=_mapRect.x();it.x()--;)
        for(it.y()=_mapRect.y();it.y()--;)
            _minimap.set(playerPos-it,Color::Black);
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




