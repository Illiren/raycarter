#pragma once

#include "texture.hpp"
#include "geometry.hpp"
//#include "actor.hpp"


class Map
{
public:
    Map(const char *map, TSize width, TSize height, const TString &filename);

    class Actor *spawn();
    class Actor *trace(Vector2D origin, TReal agle, TReal dt, class Actor *actorToIgnore);

    TSize width() const;
    TSize height() const;

    const char &operator [](TSize pos) const;
    char operator[](TSize pos);
    TArray<uint32_t> getTexture(TSize texId, TSize texCoord, TSize height)  const;
    int wall2texcoord(float hx, float hy) const;



private:
    const char *mapRef = nullptr;
    TSize _width = 0,
          _height = 0;
    Texture _textureDB;

    constexpr static char empty = ' ';
    constexpr static char actor = 'a';
};
