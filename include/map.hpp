#pragma once

#include "texture.hpp"
#include "geometry.hpp"


class Map
{
public:
    Map(const char *map, TSize width, TSize height, const TString &filename);

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
};
