#ifndef LINEARCOLOR_HPP
#define LINEARCOLOR_HPP

#include "geometry.hpp"
#include "color.hpp"

class LinearColor
{
public:
    LinearColor(const Color &color) :
        _data(color.red()/255.99,color.green()/255.99,color.blue()/255.99,color.alpha()/255.99)
    {}
    LinearColor(TReal r, TReal g, TReal b, TReal a = 1.f) :
        _data(r,g,b,a)
    {}


    inline operator Color() const
    {
        return Color(255.99*_data[0],
                     255.99*_data[1],
                     255.99*_data[2],
                     255.99*_data[3]);
    }

private:
    Vector4D _data;
};



#endif //LINEARCOLOR_HPP
