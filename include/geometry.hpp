#pragma once

#include "matrix.hpp"
#include "stddefines.hpp"

using Vector2D = Math::Vector2D<TReal>;
using Vector3D = Math::Vector3D<TReal>;
using Vector4D = Math::Vector4D<TReal>;
using Vector2D = Math::Vector2D<TReal>;

using Point2D = Math::Vector2D<TSize>;
using Point3D = Math::Vector3D<TSize>;

struct Line2D
{
    Point2D start;
    Point2D end;
};

struct Ray2D
{
    Point2D origin;
    Point2D direction;
};


struct Line3D
{
    Point3D start;
    Point3D end;
};

struct Rectangle
{
    Point2D topleft;
    Point2D botright;

    auto height() const;
    auto width() const;
};

struct Circle
{
    Point2D origin;
    TReal   radius;
};

struct Triangle
{
    Point2D t0;
    Point2D t1;
    Point2D t2;
};

struct Polygon2D
{
    TArray<Point2D> points;
};


struct Box
{
    TSize width,
          height,
          depth;
    Point3D pos;
};

struct Cube
{
    TSize size;
    Point3D pos;
};
