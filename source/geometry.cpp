#include "geometry.hpp"



// Old implementation of point rectangle test
/*
    Vector2D p1 = Vector2D(apos.x() - rectsize.x(), apos.y() - rectsize.y());
    Vector2D p2 = Vector2D(apos.x() + rectsize.x(), apos.y() - rectsize.y());
    Vector2D p3 = Vector2D(apos.x() + rectsize.x(), apos.y() + rectsize.y());
    Vector2D p4 = Vector2D(apos.x() - rectsize.x(), apos.y() + rectsize.y());
//---------- 1
    Vector2D min = p1;
    Vector2D max = p1;
//---------- 2
    if(p2.x() < min.x())
        min.x() = p2.x();
    else if(p2.x() > max.x())
        max.x() = p2.x();

if(p2.y() < min.y())
    min.y() = p2.y();
else if(p2.y() > max.y())
    max.y() = p2.y();
//---------- 3
if(p3.x() < min.x())
    min.x() = p3.x();
else if(p3.x() > max.x())
    max.x() = p3.x();

if(p3.y() < min.y())
    min.y() = p3.y();
else if(p3.y() > max.y())
    max.y() = p3.y();
//---------- 4
if(p4.x() < min.x())
    min.x() = p4.x();
else if(p4.x() > max.x())
    max.x() = p4.x();

if(p4.y() < min.y())
    min.y() = p4.y();
else if(p4.y() > max.y())
    max.y() = p4.y();

//---------- Check

if(point.x() > min.x() && point.x() < max.x() &&
    point.y() > min.y() && point.y() < max.y())
    return true;

return false;*/
