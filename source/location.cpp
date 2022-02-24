#include "location.hpp"
#include "geometry.hpp"
#include "actor.hpp"
#include <queue>
#include <iostream>
#include <stack>

Location::Location(const char *m, Vector2U s) :
      map(s.x()*s.y()),
      size(s)
{
    const auto mapSize = s.x()*s.y();
    for(auto i = mapSize;i--;map[i] = m[i]);
}


Actor *Location::trace(Vector2D origin, TReal direction, TReal d, Actor *actorToIgnore)
{
    const auto rotator = Vector2D{std::cos(direction),
                                  std::sin(direction)};

    for(TReal t = 0; t <= d; t+=.01f)
    {
        const auto pos = origin + t*rotator;

        for(auto &actor : actorList)
        {
            if(actor == nullptr) continue;
            if(actor == actorToIgnore) continue;

            if(intersect(pos, actor->getCollisionBody()))
            {
                return actor;
            }
        }
    }
    return nullptr;
}

const char &Location::operator [](TSize pos) const
{
    return map[pos];
}

const char &Location::operator [](Vector2U pos) const
{
    return map[pos.x()+pos.y()*size.x()];
}

const Texture &Location::getWallText(TSize texId) const
{
    return textureDB[texId];
}

void Location::addWallText(PTexture txt)
{
    auto t = txt.lock();
    textureDB.push_back(*t);
}

void Location::addActor(Actor *actor)
{
    assert(actor != nullptr && "Actor is invalid");
    actor->location = this;
    actorList.push_back(actor);
}

void Location::removeActor(Actor *actor)
{
    assert(actor != nullptr && "Actor is invalid");
    actorList.remove(actor);
    actor->location = nullptr;
}

void Location::setMap(const char m[], Vector2U pos)
{
    map.resize(pos.x()*pos.y());
    for(auto i=pos.x();i--;)
        for(auto j=pos.y();j--;)
            map[i+pos.x()*j] = m[i+pos.x()*j];
}

Texture Location::generateMinimap(Vector2U pos)
{
    const auto mapRect = pos/size;

    Texture txt(pos);

    for(Vector2U it{0,0};it.y()<size.y();++it.y())
        for(it.x()=0;it.x()<size.x();++it.x())
        {
            const auto mapPos = it.x()+it.y()*size.x();
            if(map[mapPos] == ' ') continue;

            const auto rect = txt.size - (it+1)*mapRect + 1;

            for(Vector2U k{0,0};k.x()<mapRect.x();++k.x())
                for(k.y()=0;k.y()<mapRect.y();++k.y())
                    txt.set(rect-k,Color::Black);
        }

    return txt;
}

void Location::update(TReal)
{}

void World::addLocation(Location *l, TString name)
{
    locationMap[name] = l;
}

Location *World::operator [](TString name)
{
    return locationMap[name];
}



struct Cell
{
    Vector2I parent;
    TReal f,g,h;

    Cell() :
          parent(-1,-1),
          f(-1.f),
          g(-1.f),
          h(-1.f)
    {}
};

struct CellPriority
{
    TReal weight;
    Vector2I pos;

    friend bool operator > (const CellPriority &lhs, const CellPriority &rhs)
    {
        if(lhs.weight < rhs.weight) return false;
        return true;
    }
};


bool isDestination(const Vector2I &position, const Vector2I &dest)
{
    return position == dest;
}

TReal euclideanDist(const Vector2I &src, const Vector2I &dest)
{
    using namespace std;
    return sqrt(pow((src.x() - dest.x()),2.f) + pow((src.y() - dest.y()),2.f));
}

TReal chebyshevDist(const Vector2I &src, const Vector2I &dest)
{
    return std::max(std::abs(src.x()-dest.x()),std::abs(src.y()-dest.y()));
}

TReal manhattanDist(const Vector2I &src, const Vector2I &dest)
{
    return std::abs(src.x()-dest.x())+std::abs(src.y()-dest.y());
}

bool Location::isValid(const Vector2I &point)
{
    return (point.x() >= 0) &&
           (point.x() < static_cast<int>(size.x())) &&
           (point.y() >= 0) &&
           (point.y() < static_cast<int>(size.y()));
}

bool Location::isBlocked(const Vector2I &point)
{
    return !isValid(point) || ((*this)[point] != ' ');
}

template<typename T>
T &atPos(TArray<T> &data, const Vector2U &size, const Vector2U &pos)
{
    return data[pos.x()+pos.y()*size.x()];
}

Location::Path Location::pathfinder(Vector2I src, Vector2I dst)
{    
    using namespace std;
    stack<Vector2I> path;

    if(!isValid(src))
    {
        //std::cout << "Source is invalid" << std::endl;
        return path;
    }
    if(!isValid(dst))
    {
        //std::cout << "Destination is invalid" << std::endl;
        return path;
    }

    if(isBlocked(src) || isBlocked(dst))
    {
        //std::cout << "Source or destination is blocked" << std::endl;
        return path;
    }

    if(isDestination(src,dst))
    {
        //std::cout << "Source is destination" << std::endl;
        return path;
    }

    bool closedList[size.x()][size.y()]; //TODO: allocate on heap
    memset(closedList, false, sizeof(closedList));

    TArray<Cell> cellDetails(size.x()*size.y());

    auto pos = src;

    auto &cell = atPos(cellDetails,size,pos);

    cell.f = 0.f;
    cell.g = 0.f;
    cell.h = 0.f;
    cell.parent = pos;

    std::priority_queue<CellPriority, std::vector<CellPriority>, std::greater<CellPriority>> openList;

    openList.emplace(0.0f,pos);

    while(!openList.empty())
    {
        const auto &p = openList.top();
        auto posU = p.pos;
        openList.pop();
        closedList[posU.x()][posU.y()] = true;

        for(Vector2I it{-1,-1};it.x() <= 1; it.x()++)
            for(it.y() = -1; it.y()  <= 1; it.y()++)
            {
                auto neighbour = posU + it;

                if(isValid(neighbour))
                {
                    if(isDestination(neighbour,dst))
                    {
                        atPos(cellDetails,size,neighbour).parent = posU;

                        path.push(dst);

                        Vector2I curr = atPos(cellDetails,size,Vector2U(dst.x(), dst.y())).parent;
                        Vector2I next;
                        do
                        {
                            path.push(curr);
                            curr = atPos(cellDetails,size,curr).parent;
                            next = atPos(cellDetails,size,curr).parent;
                        }
                        while(next != curr);

                        path.push(next);

                        return path;
                    }
                    else if(!closedList[neighbour.x()][neighbour.y()] && !isBlocked(neighbour))
                    {
                        TReal gNew, hNew, fNew;

                        gNew = atPos(cellDetails,size,posU).g+1.f;
                        hNew = euclideanDist(neighbour,dst);
                        fNew = gNew + hNew;

                        auto &npos = atPos(cellDetails,size,neighbour);
                        if(npos.f == -1 || npos.f > fNew)
                        {
                            openList.emplace(fNew,neighbour);

                            npos.g = gNew;
                            npos.h = hNew;
                            npos.f = fNew;
                            npos.parent = posU;
                        }
                    }
                }
            }
    }

    //std::cout << "Path not found " << std::endl;
    return path;
}

