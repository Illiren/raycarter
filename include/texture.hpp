#pragma once

#include "geometry.hpp"
#include "SDL.h"

struct Texture
{
    TSize w,h;
    TSize count, size;
    TArray<uint32_t> img;

    Texture(const TString filename, const uint32_t format);
    uint32_t get(TSize i, TSize j, TSize id) const;
    TArray<uint32_t> getScaledColumn(TSize texId, TSize texCoord, TSize height) const;
};
