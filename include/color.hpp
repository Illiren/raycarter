#pragma once

#include "stddefines.hpp"


class Color
{

public:
    Color() noexcept
    {}

    constexpr Color(TUint32 rawColor) noexcept :
        _rawData(rawColor)
    {}

    constexpr Color(TUInt8 r, TUInt8 g, TUInt8 b, TUInt8 a=255) noexcept :
        _red(r),
        _green(g),
        _blue(b),
        _alpha(a)
    {}

    Color(const Color &color) noexcept;

    Color &operator=(const Color &color) noexcept;

    //TODO: Move operations

    auto red() const {return _red;}
    auto green() const {return _green;}
    auto blue() const {return _blue;}
    auto alpha() const {return _alpha;}

    inline operator TUint32() const noexcept {return _rawData;}

private:
    union
    {
        TUint32 _rawData;
        struct
        {
            TUInt8 _red;
            TUInt8 _green;
            TUInt8 _blue;
            TUInt8 _alpha;
        };
    };

public: //Color defines
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Black;
};

constexpr Color Color::White = Color(255,255,255);
constexpr Color Color::Red = Color(255,0,0);
constexpr Color Color::Green = Color(0,255,0);
constexpr Color Color::Blue = Color(0,0,255);
constexpr Color Color::Black = Color(0,0,0);

