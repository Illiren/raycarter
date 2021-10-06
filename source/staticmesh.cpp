#include "staticmesh.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


StaticMesh::StaticMesh(const char *filename) :
      _verts(),
      _faces()
{
    std::ifstream in;

    in.open(filename, std::ifstream::in);
    if(in.fail())
    {
        std::cout << "Can't open file" << std::endl;
        return;
    }

    std::string line;

    while(!in.eof())
    {
        std::getline(in,line);
        std::istringstream iss(line.c_str());
        char trash;

        if(!line.compare(0,2, "v "))
        {
            iss >> trash;
            Vector3D v;
            for (TSize i=0;i<3;++i)
                iss >> v[i];
            _verts.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            TArray<int> f;
            int itrash,
                idx;
            iss >> trash;
            while(iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                --idx;
                f.push_back(idx);
            }
            _faces.push_back(f);
        }
    }

    std::cerr << "# v# " << _verts.size() << " f# " << _faces.size() << std::endl;
}

StaticMesh::~StaticMesh()
{

}

TSize StaticMesh::nverts() const
{
    return _verts.size();
}

TSize StaticMesh::nfaces() const
{
    return _faces.size();
}

Vector3D StaticMesh::vert(TSize i) const
{
    return _verts[i];
}

TArray<int> StaticMesh::face(TSize idx) const
{
    return _faces[idx];
}
