#pragma once

#include "stddefines.hpp"
#include "matrix.hpp"


struct Color : public Math::Vector<TByte,4>
{
    using Base = Math::Vector<TByte,4>;

    Color() {}
    Color(Math::Vector<TByte, 4> v) :
          Math::Vector<TByte,4>(std::forward<Math::Vector<TByte,4>>(v))
    {}
    Color(struct LinearColor color);
    Color(TByte r, TByte g, TByte b, TByte a = 0xFF) noexcept :
          Base(r,g,b,a)
    {}
    Color(uint32_t color)
    {
        *((uint32_t*)_data)=color;
    }
    Color(const Color &c) noexcept :
          Base(c)
    {}
    Color(Color &&c) noexcept :
          Base(c)
    {}

    Color &operator=(const Color &c) noexcept
    {
        static_cast<Base&>(*this) = static_cast<const Base&>(c);
        return *this;
    }

    Color &operator=(Color &&c) noexcept
    {
        static_cast<Base&>(*this) = static_cast<Base&&>(c);
        return *this;
    }

    inline TByte &r() noexcept {return x();}
    inline TByte &g() noexcept {return y();}
    inline TByte &b() noexcept {return z();}
    inline TByte &a() noexcept {return w();}

    inline const TByte &r() const noexcept {return x();}
    inline const TByte &g() const noexcept {return y();}
    inline const TByte &b() const noexcept {return z();}
    inline const TByte &a() const noexcept {return w();}


    inline operator const uint32_t &() const noexcept {return *((uint32_t*)_data);}
    inline operator uint32_t&() noexcept {return *((uint32_t*)_data);}


    //Any colour you like
    static const Color White; //stripes
    static const Color Black; //Sabbath
    static const Color Red; //hot chili peppers
    static const Color Green; // is the colour
    static const Color Blue; // Öyster Cult
    static const Color Cyan;
    static const Color Magenta;
    static const Color Yellow; //submarine
    static const Color Gray; //goes black

    static const Color Vermilion;
    static const Color Orange;
    static const Color Khaki;
    static const Color Lime;
    static const Color Olive;
    static const Color Grass;
    static const Color BluishGreen;
    static const Color Teal;
    static const Color BluishCyan;
    static const Color Azure;
    static const Color BlueViolet;
    static const Color Violet;
    static const Color PurpleViolet;
    static const Color Purple; //Rain
    static const Color Crimson; //and clover
    static const Color Scarlet;
    static const Color Carmine;
};




struct LinearColor : public Math::Vector<float,4>
{
    using Base = Math::Vector<float,4>;
    static constexpr float percent = 255.f;

    LinearColor() {}
    //LinearColor(Math::Vector<float,4> v) : Vector<float,4>(std::forward<Vector<float,4>>(v)) {}
    LinearColor(Color color) noexcept :
          Base(static_cast<float>(color.r())/percent,
               static_cast<float>(color.g())/percent,
               static_cast<float>(color.b())/percent,
               static_cast<float>(color.a())/percent)
    {
        r()=color.r()/percent;
    }
    LinearColor(float r, float g, float b, float a = 1.f) noexcept :
          Base(r,g,b,a)
    {}
    LinearColor(const LinearColor &c) noexcept :
          Base(c)
    {}
    LinearColor(LinearColor &&c) noexcept :
          Base(c)
    {}

    LinearColor &operator=(const LinearColor &c) noexcept
    {
        static_cast<Base&>(*this)=static_cast<const Base&>(c);
        return *this;
    }

    LinearColor &operator=(LinearColor &&c) noexcept
    {
        static_cast<Base&>(*this)=static_cast<Base&&>(c);
        return *this;
    }

    LinearColor &operator+=(LinearColor c) noexcept;
    LinearColor &operator-=(LinearColor c) noexcept;

    inline float &r() noexcept {return x();}
    inline float &g() noexcept {return y();}
    inline float &b() noexcept {return z();}
    inline float &a() noexcept {return w();}

    inline const float &r() const noexcept {return x();}
    inline const float &g() const noexcept {return y();}
    inline const float &b() const noexcept {return z();}
    inline const float &a() const noexcept {return w();}
};


LinearColor operator+(LinearColor lhs, LinearColor rhs);
LinearColor operator-(LinearColor lhs, LinearColor rhs);


//LinearColor blend(LinearColor a, LinearColor b);
LinearColor blend(LinearColor a, LinearColor b, float t);


//HUE
//CMYK

/*
class Color
{

public:
constexpr Color() noexcept {}
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
Color &operator-=(const Color &color) noexcept;
Color &operator+=(const Color &color) noexcept;
friend Color operator+(Color lhs, Color rhs) noexcept;

//TODO: Move operations

auto red() const noexcept {return _red;}
auto green() const noexcept {return _green;}
auto blue() const noexcept {return _blue;}
auto alpha() const noexcept {return _alpha;}

TUInt8& red() noexcept {return _red;}
TUInt8& green() noexcept {return _green;}
TUInt8& blue() noexcept {return _blue;}
TUInt8& alpha() noexcept {return _alpha;}

inline operator TUint32() const noexcept {return _rawData;}

private:
union //none standard way, need to remove
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

constexpr Color Color::White = Color(255,255,255,255);
constexpr Color Color::Red = Color(255,0,0,255);
constexpr Color Color::Green = Color(0,255,0,255);
constexpr Color Color::Blue = Color(0,0,255,255);
constexpr Color Color::Black = Color(0,0,0,255);

*/

