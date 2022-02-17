#pragma once

#include <unordered_map>
#include <memory>
#include <SDL.h>
#include "geometry.hpp"
#include "color.hpp"

struct Texture
{
    Texture();
    Texture(Vector2U size);

    Texture(const Texture &txt);
    Texture(Texture &&txt);
    Texture &operator=(const Texture &txt);
    Texture &operator=(Texture &&txt);

    void set(Vector2U pos, uint32_t value);
    Color get(Vector2U pos) const;
    Color get(Vector2U pos, TSize spriteScreenSize) const;
    TArray<uint32_t> getScaledColumn(TSize texCoord, TSize height) const;

    explicit operator bool() const noexcept {return img.size() != 0;}
    Vector2U size;
    TArray<uint32_t> img;
};

using PTexture = std::weak_ptr<Texture>;

inline bool isValid(const PTexture &weak)
{
    return !weak.expired();
}

struct Font
{
    using ByteVec = Math::Vector2D<uint8_t>;

    Font(TString filename, uint8_t cw, uint8_t ch, uint8_t th, uint8_t tw, uint8_t i, char startChar = ' ');
    Font(const Font &) = default;
    Font(Font &&) = default;
    Font &operator = (const Font &) = default;
    Font &operator = (Font &&) = default;
    ~Font() = default;

    ByteVec   charSize;
    ByteVec   tableSize;
    uint8_t   indent      = 1;
    char      startCharacter = ' ';
    Texture  fontTable;

    Texture createText(TString text);
};
