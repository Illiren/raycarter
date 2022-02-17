#pragma once

#include "texture.hpp"
#include "geometry.hpp"
#include "game_object.hpp"
#include "actor.hpp"



struct Location : public GameObject
{
    Location() = default;
    Location(const char *m, Vector2U s);
    Location(const Location &) = default;
    Location(Location &&) = default;
    Location &operator=(const Location &) = default;
    Location &operator=(Location &&) = default;

    template<typename T>
    T *spawn(Vector2D pos = {0.f,0.f}) requires std::is_base_of_v<Actor, T>
    {
        auto actor = new(this) T();
        actor->position = pos;
        actor->location = this;
        actorList.push_back(actor);
        return actor;
    }

    Actor *trace(Vector2D origin, TReal direction, TReal dt, Actor *actorToIgnore);
    const char &operator [](TSize pos) const;
    const char &operator [](Vector2U pos) const;
    const Texture &getWallText(TSize texId) const;
    void addWallText(PTexture txt);
    void addActor(Actor *actor);
    void removeActor(Actor *actor);
    void setMap(const char m[], Vector2U pos);
    Texture generateMinimap(Vector2U pos);

    TList<Actor*>   actorList;
    Texture         floor;
    Texture         ceil;
    TArray<Texture> textureDB;
    TArray<char>    map;
    Vector2U        size;

    // GameObject interface
public:
    void update(TReal dt) override;
};

struct World
{
    void addLocation(Location *l, TString name);
    Location *operator[](TString name);

    //Texture map();
    THashMap<TString, Location*> locationMap;
};


