#pragma once

#include "window.hpp"
#include "color.hpp"
#include "screen.hpp"
#include "location.hpp"
#include "camera.hpp"
#include "gameloop.hpp"
#include "input.hpp"
#include "drawable.hpp"
#include "network.hpp"
#include "audio.hpp"
#include "actor.hpp"
#include "resourcemanager.hpp"
#include <functional>
#include <stack>

struct PlayerDescription
{
    uint32_t uniqueID = 0;
    uint32_t avatarID = 0;
    uint32_t locactionID = 0;

    Vector2D pos;
    char walk;
    float speed;
    float runSpeedMul;
};

struct Action
{
    uint32_t playerID = 0;
    uint32_t actionID = 0;
};


enum class CustomMsgTypes : uint32_t
{
    ServerStatus,
    ServerPing,

    ClientAccepted,
    ClientAssignID,
    ClientRegister,
    ClientUnregister,

    GameAddPlayer,
    GameRemovePlayer,
    GameUpdatePlayer,

    PlayerAction,
};

//Test Controller for Test npc
class NPC_Drake_Controller : public GameObject
{

public:
    NPC_Drake_Controller(Pawn *owner) :
          GameObject(),
          _owner(owner)
    {}


    void goTo(const Vector2I &dest)
    {
        if(!_owner && !_owner->location) return;
        auto loc = _owner->location;
        Vector2I pos = _owner->position;
        _path = loc->pathfinder(pos,dest);
    }

    // GameObject interface
public:
    void update(TReal dt) override
    {
        (void)dt;
        if(!_owner) return;

        auto mc = _owner->movementComponent;

        if(!mc) return;

        if(!_path.empty())
        {            
            auto &target = _path.top();
            const auto &position = _owner->position;

            if((floor(position.x()) == target.x()) &&
               (floor(position.y()) == target.y()))
            {
                _path.pop();
                target = _path.top();
            }

            auto diff = target-position;
            _owner->direction = atan2(diff.y(),diff.x());
            mc->speed = mc->maxSpeed;
        }
        else
        {
            mc->speed = 0;            
        }
    }

private:
    Pawn *_owner;
    Location::Path _path;
};


//Test NPC
class NPC_Drake : public Pawn
{
public:
    NPC_Drake() :
          Pawn()
    {
        sound.setSound(RM().sound("test").lock().get());
        sound.setLooping(true);
        sound.setGlobal(false);
        collisionRect = 0.9f;
    }

    void update(float ms) override
    {
        Pawn::update(ms);
        sound.setPosition({position.x(), position.y(), 0.f});
        sound.setDirection({std::cos(direction),std::sin(direction),0.f});
        sound.update(ms);
    }

private:
    SoundEmitter sound;

    // NActor interface
public:
    void onInterract(Actor *causer) override
    {
        (void)causer; //Unused
        sound.play();
    }
};

class RayCarter : public Client<CustomMsgTypes>, public GameLoop<RayCarter>
{
public:
    enum Mode
    {
        None,
        Singleplayer,
        Multiplayer,
    };

public:
    RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit = 60.f);
    ~RayCarter();

    void start(Mode mode);
    void addLocation(Location *location, TString locationName);
    void addActor(Actor *actor, TString locationName);


    World world;
    Viewport viewport;
    MovementComponent *controller = nullptr;

    TString  host;
    uint16_t port;
    uint32_t playerID;


private:
    InputManager &keyMap;
    ResourceManager &rm;

    void initPlayer(Mode mode);
    Mode currentMode;

    bool isWaitingForConnection;

    // Player Controller
    void moveForward();
    void moveBack();
    void moveStop();
    void turnRight();
    void turnLeft();
    void turnStop();
    void sprintOn();
    void sprintOff();
    void interact();
    void scream();
    void shutup();
    void playMusic();

    THashMap<uint32_t, Player*> playerMap;


    // Network serialization
    Player *desc2Actor(const PlayerDescription &desc);
    void actor2Desc(const Player &player, PlayerDescription &desc);
    void updateActor(Player *player, const PlayerDescription &desc);
    Player *findPlayer(uint32_t playerID);


    //INFO Sound System Test
    SoundSystem &ss;
    SoundEmitter se;
    SoundEmitter music;


    NPC_Drake_Controller *drakecontroller;

private:
    void physX();

    // GameLoop interface
public:
    void init();
    void input();
    void render();
    void update(TReal lag);
};
