 #include "color.hpp"


Color::Color(const Color &color) noexcept:
      _rawData(color._rawData)
{}

Color &Color::operator=(const Color &color) noexcept
{
    _rawData = color._rawData;
    return *this;
}
