#pragma once

#include "geometry.hpp"

class StaticMesh
{
public:
    StaticMesh(const char *filename);
    ~StaticMesh();

    TSize nverts() const;
    TSize nfaces() const;

    Vector3D vert(TSize i) const;
    TArray<int> face(TSize idx) const;


private:
    TArray<Vector3D>    _verts;
    TArray<TArray<int>> _faces;
};



