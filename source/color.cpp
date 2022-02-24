#include "color.hpp"

Color::Color(LinearColor color) :
      Base(color.r()*0xFF, color.g()*0xFF, color.b()*0xFF, color.a()*0xFF)
{}

/*
Color &Color::blend(Color c) noexcept
{
    auto newAlpha = 1-(1-a())*(1-c.a());
    if(newAlpha == 0) return *this;

    auto coeff0 = a()/newAlpha;
    auto coeff1 = c.a()*(1-a())/newAlpha;

    r() = r()*coeff0 + c.r()*coeff1;
    g() = g()*coeff0 + c.g()*coeff1;
    b() = b()*coeff0 + c.b()*coeff1;
    a() = newAlpha;


    return *this;
}

Color blend(Color a, Color b)
{
    Color c;
    c.a() = 1-(1-a.a())*(1-b.a());
    if(c.a() == 0) return c;

    auto coeff0 = a.a()/b.a();
    auto coeff1 = b.a()*(1-a.a())/b.a();

    c.r() = a.r()*coeff0 + b.r()*coeff1;
    c.g() = a.g()*coeff0 + b.g()*coeff1;
    c.b() = a.b()*coeff0 + b.b()*coeff1;

    return c;
}

LinearColor blend(LinearColor a, LinearColor b)
{
    LinearColor c;

    c.a() = 1-(1-a.a())*(1-b.a());
    if(c.a() < 1.0e-6) return c;

    const auto coeff0 = a.a()/c.a();
    const auto coeff1 = b.a()*(1.f-a.a())/c.a();

    c.r() = a.r()*coeff0 + b.r()*coeff1;
    c.g() = a.g()*coeff0 + b.g()*coeff1;
    c.b() = a.b()*coeff0 + b.b()*coeff1;

    return c;
}*/

LinearColor &LinearColor::operator+=(LinearColor c) noexcept
{
    const auto newAlpha = 1-(1-a())*(1-c.a());
    if(newAlpha < 1.0e-6) return *this;

    const auto coeff0 = a()/newAlpha;
    const auto coeff1 = c.a()*(1.f-a())/newAlpha;

    r() = r()*coeff0 + c.r()*coeff1;
    g() = g()*coeff0 + c.g()*coeff1;
    b() = b()*coeff0 + c.b()*coeff1;

    return *this;
}

LinearColor &LinearColor::operator-=(LinearColor c) noexcept
{
    const auto newAlpha = 1-(1-a())*(1-c.a());
    if(newAlpha < 1.0e-6) return *this;

    const auto coeff0 = a()/newAlpha;
    const auto coeff1 = c.a()*(1.f-a())/newAlpha;

    r() = r()*coeff0 - c.r()*coeff1;
    g() = g()*coeff0 - c.g()*coeff1;
    b() = b()*coeff0 - c.b()*coeff1;

    return *this;
}

LinearColor operator+(LinearColor lhs, LinearColor rhs)
{
    return lhs+=rhs;
}
LinearColor operator-(LinearColor lhs, LinearColor rhs)
{
    return lhs-=rhs;
}

LinearColor blend(LinearColor a, LinearColor b, float t)
{
    constexpr auto blendColor = [] (float a, float b, float t) ->float
    {
        return std::sqrt((1-t)*a*a + t * b*b);
    };

    constexpr auto blendAlpha = [] (float a, float b, float t) ->float
    {
        return (1-t)*a + t*b;
    };

    t = std::min(1.f, std::max(0.f,t));

    a.r() = blendColor(a.r(),b.r(),t);
    a.g() = blendColor(a.g(),b.g(),t);
    a.b() = blendColor(a.b(),b.b(),t);
    a.a() = blendAlpha(a.a(),b.a(),t);
    return a;
}


const Color Color::White = {0xFF,0xFF,0xFF,0xFF};
const Color Color::Black = {0x00,0x00,0x00,0xFF};
const Color Color::Red   = {0xFF,0x00,0x00,0xFF};
const Color Color::Green = {0x00,0xFF,0x00,0xFF};
const Color Color::Blue  = {0x00,0x00,0xFF,0xFF};
const Color Color::Cyan  = {0x00,0xFF,0xFF,0xFF};
const Color Color::Magenta = {0xFF,0x00,0xFF,0xFF};
const Color Color::Yellow = {0xFF,0xFF,0x00,0xFF};
const Color Color::Gray   = {0x78,0x78,0x78,0xFF};


const Color Color::Vermilion = {0xFF,0x40,0x00,0xFF};
const Color Color::Orange = {0xFF,0x80,0x00,0xFF};
const Color Color::Khaki = {0xFF,0xBF,0x00,0xFF};
const Color Color::Lime = {0xBF,0xFF,0x00,0xFF};
const Color Color::Olive = {0x80,0xFF,0x00,0xFF};
const Color Color::Grass = {0x40,0xFF,0x00,0xFF};
const Color Color::BluishGreen = {0x00,0xFF,0x40,0xFF};
const Color Color::Teal = {0x00,0xFF,0x80,0xFF};
const Color Color::BluishCyan = {0x00,0xBF,0xFF,0xFF};
const Color Color::Azure = {0x00,0x80,0xFF,0xFF};
const Color Color::BlueViolet = {0x00,0x40,0xFF,0xFF};
const Color Color::Violet = {0x80,0x00,0xFF,0xFF};
const Color Color::PurpleViolet = {0x40,0x00,0xFF,0xFF};
const Color Color::Purple = {0xBF,0x00,0xFF,0xFF};
const Color Color::Crimson = {0xFF,0x00,0x80,0xFF};
const Color Color::Scarlet = {0xFF,0x00,0x80,0xFF};
const Color Color::Carmine = {0xFF,0x00,0x40,0xFF};






