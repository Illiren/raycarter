#include "camera.hpp"
#include "game_object.hpp"
#include "color.hpp"
#include <map>

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

    float spriteDir = atan2(sprite.y - pos.y(), sprite.x - pos.x());
    while(spriteDir - dir > M_PI) spriteDir -= 2*M_PI;
    while(spriteDir - dir < - M_PI) spriteDir += 2*M_PI;
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
      font("smallfont.bmp",15,15,16,16,1,' ')
{}

void Viewport::setCamera(CameraComponent *cam)
{
    if(!cam) return;
    camera = &cam->camera;
    actor = cam->getOwner();
}

void Viewport::drawWorld()
{
    constexpr float dist = 0.54;

    auto location = actor->location;
    if(!actor->location) return;
    for(auto &i : depthBuffer)
        i = 1e3;

    // Draw Floor and Ceiling
    const float dirX = std::cos(camera->direction),
                dirY = std::sin(camera->direction);

    const auto pos = actor->position;
    const auto direction = actor->direction;
    const auto fov = camera->fov;
    const auto distance = camera->distance;
    const Vector2D rayDir0(dirX + dist*dirY, dirY - dist*dirX);
    const Vector2D rayDir1(dirX - dist*dirY, dirY + dist*dirX);
    auto diff = (rayDir1 - rayDir0)/_width;
    const Math::Vector2D<int> texsize(location->floor.w, location->floor.h);

    for(TSize i=_height/2+1; i<_height;++i)
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
            color = location->ceiling.get(tx,ty);
            screen.drawPoint({j,_height-i-1},color);
        }
    }


#ifndef DEBUG
#pragma omp parallel for
#endif
    for(size_t i=0;i<_width;++i) // Wall Casting
    {
        const float angle = direction - fov/2 + fov*i/float(_width);
        const auto acos = std::cos(angle);
        const auto asin = std::sin(angle);

        for(TReal t=0;t<distance; t+=.01f)
        {
            const TReal cx = pos.x() + t*acos;
            const TReal cy = pos.y() + t*asin;
            const auto mapPos = static_cast<int>(cx) +
                                static_cast<int>(cy) *
                                static_cast<int>(location->width);

            if((*location)[mapPos] != ' ') // if something on the way
            {
                const auto dist = t*std::cos(angle-direction);
                depthBuffer[i] = dist;

                const auto itex = (*location)[mapPos] - '0';
                auto tex = location->getTexture(itex);
                const TSize columnH = TSize(_height/dist);
                auto xtex = wall2texcoord(cx,cy,tex.w);
                TArray<uint32_t> column = tex.getScaledColumn(xtex, columnH);

                for(TSize j=0;j<columnH;++j)
                {
                    TSize py = j + _height/2 - columnH/2;
                    if(py>=0 && py < _height)
                    {
                        Color c(column[j]);
                        screen.drawPoint({i,py},Color(c));
                    }
                }

                break;
            }
        }
    }
}

void Viewport::drawActors()
{
    //auto &drawList = Drawable::getRegister();
    auto &gameObjectList = GameObject::getRegister();

    for(auto &obj : gameObjectList)
    {
        //auto spritobj = dynamic_cast
    }

}

void Viewport::drawHUD()
{

}

void Viewport::draw()
{
    if(!actor) return;
    if(!camera) return;

    drawWorld();
    drawActors();
    drawHUD();
}
