#include "raycaster.hpp"
#include "game_object.hpp"
#include <iostream>



RayCarter::RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit) :
      GameLoop(fpsLimit),
      viewport(winSize),
      keyMap(GetInputManager()),
      rm(RM()),
      currentMode(None),
      ss(soundSystem())
{
    GetInputManager()['a'].keydownEvent = [this](){turnLeft();};
    GetInputManager()['a'].keyupEvent = [this](){turnStop();};
    GetInputManager()['d'].keydownEvent = [this](){turnRight();};
    GetInputManager()['d'].keyupEvent = [this](){turnStop();};

    GetInputManager()['w'].keydownEvent = [this](){moveForward();};
    GetInputManager()['w'].keyupEvent = [this](){moveStop();};
    GetInputManager()['s'].keydownEvent = [this](){moveBack();};
    GetInputManager()['s'].keyupEvent = [this](){moveStop();};

    GetInputManager()['e'].keyupEvent = [this](){interact();};
    GetInputManager()['c'].keydownEvent = [this](){playMusic();};
    //GetInputManager()['c'].keyupEvent = [this](){pauseMusic();};

    GetInputManager()[SDLK_LSHIFT].keydownEvent = [this](){sprintOn(); scream();};
    GetInputManager()[SDLK_LSHIFT].keyupEvent = [this](){sprintOff(); shutup();};


    rm.loadTexture("monsters.bmp",4,"avatar");
    rm.loadTexture("player.bmp", "player");
    rm.loadTexture("weapon.bmp", "weapon");

    rm.loadAudio("test.wav","test");
    //rm.loadAudio("aaaaa.wav","aaaa");
    //rm.loadAudio("01.wav", "music");

    viewport.playerFace = *rm.texture("player").lock().get();
    viewport.hudWeapon = *rm.texture("weapon").lock().get();

    //auto drake = new NPC_Drake(textureDB["avatar0"]);
    //drake->position = {4.0f, 13.4f};

#ifdef DEBUG
    viewport.dt = 0.1f;
#endif

    auto sound = rm.sound("test").lock().get();
    ss.setVolume(0.1f);

    se.setSound(sound);
    //se.setLooping(false);
    //se.setGlobal(false);
    se.setVolume(1.f);


    //auto m = rm.sound("music").lock().get();
    //music.setSound(m);
}

RayCarter::~RayCarter()
{
    if(isConnected())
        disconnect();
}


void RayCarter::start(Mode mode)
{
    if(mode == Singleplayer)
    {
        initPlayer(mode);
    }
    else if(mode == Multiplayer)
    {
        if(!connect("127.0.0.1", 1234))
            std::cout << "Can't connect ot server" << std::endl;

    }
    currentMode = mode;
    GameLoop::start();
}

void RayCarter::addLocation(Location *location, TString locationName)
{
    world.addLocation(std::move(location), locationName);
}

void RayCarter::initPlayer(Mode mode)
{
    if(mode == Singleplayer)
    {
        auto player = world["level0"]->spawn<Player>({3.4f, 3.4f});
        viewport.setCamera(player->camera);
        controller = player->movementComponent;
        player->camera->camera.fov = 1.f;
        player->camera->camera.distance = 20;
        player->direction = 1;

        world["level0"]->addActor(player);
    }

    NPC_Drake *drake = world["level0"]->spawn<NPC_Drake>({4.0f,13.4f});
    drake->spriteComponent->setTexture(rm.texture("avatar2"));
    drakecontroller = new NPC_Drake_Controller(drake);
}

void RayCarter::physX()
{
    auto &list = world["level0"]->actorList;
    for(auto &actor : list)
    {        
        if(!actor) continue;

        for(auto &actor2 : list)
        {
            if(actor == actor2) continue;
            if(!actor2) continue;
            if(intersect(actor->getCollisionBody(),actor2->getCollisionBody()))
            {
                actor->onCollision(actor2);
                actor2->onCollision(actor);
            }
        }
    }
}


void RayCarter::init()
{

}

void RayCarter::input()
{
    keyMap.update();
}

void RayCarter::render()
{
    viewport.fps = fps();
    viewport.draw();
}

void RayCarter::update(TReal lag)
{
    if(currentMode == Multiplayer)
    {
        if(isConnected())
        {
            while(!incoming().empty())
            {
                auto msg = incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgTypes::ClientAccepted:
                {
                    PlayerDescription desc;
                    std::cout << "Server accepted client - you're in!\n";

                    Message<CustomMsgTypes> msg;
                    msg.header.id = CustomMsgTypes::ClientRegister;
                    desc.pos = {1.f,1.f};
                    desc.avatarID = 1;

                    msg << desc;
                    send(msg);
                    break;
                }
                case CustomMsgTypes::ClientAssignID:
                {
                    msg >> playerID;
                    std::cout << "Assigned Client ID = " << playerID << "\n";
                    break;
                }
                case CustomMsgTypes::GameAddPlayer:
                {
                    PlayerDescription d;
                    msg >> d;
                    Player *actor = desc2Actor(d);
                    if(actor)
                    {
                        playerMap.insert({d.uniqueID, actor});
                        world["level"+std::to_string(d.locactionID)]->addActor(actor);
                    }
                    std::cout << "Game Add Player id:" << d.uniqueID << std::endl;


                    if(d.uniqueID == playerID)
                    {
                        std::cout << "Player id ack, id=" << playerID << std::endl;
                        viewport.setCamera(actor->camera);
                        controller = actor->movementComponent;
                        actor->camera->camera.fov = M_PI/3.f;
                        actor->camera->camera.distance = 20;
                        actor->direction = 1;

                        isWaitingForConnection = false;
                    }
                    else
                    {
                        std::cout << "Player id nak, id=" << playerID << " in id: " << d.uniqueID << std::endl;
                    }

                    break;
                }
                case CustomMsgTypes::GameRemovePlayer:
                {
                    uint32_t removalID = 0;
                    msg >> removalID;
                    Actor *actor = playerMap[removalID];
                    if(actor)
                        actor->location->removeActor(actor);
                    playerMap.erase(removalID);
                    break;
                }
                case CustomMsgTypes::GameUpdatePlayer:
                {
                    PlayerDescription d;
                    msg >> d;
                    if(playerMap.contains(d.uniqueID))
                    {
                        updateActor(playerMap[d.uniqueID],d);
                    }
                    break;
                }
                case CustomMsgTypes::PlayerAction:
                {
                    break;
                }
                default:
                    break;
                }
            }
        }
    }


    physX();
    GC().update(lag);
    GC().collect(false);

    auto pos = controller->getOwner()->position;
    auto rot = controller->getOwner()->direction;

    se.update(lag);
    music.update(lag);
    ss.update(lag);
    drakecontroller->update(lag);    
    drakecontroller->goTo(pos);

    se.setPosition({pos.x(),pos.y(), 0.f});
    ss.setListenerPos({pos.x(), pos.y(), 0.f});
    ss.setListenerDir({std::cos(rot),std::sin(rot),0.f});


    if(currentMode == Multiplayer && controller)
    {        
        auto player = dynamic_cast<Player *>(controller->getOwner());

        if(player)
        {
            PlayerDescription d;
            Message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::GameUpdatePlayer;
            actor2Desc(*player,d);
            d.uniqueID = playerID;
            msg << d;
            send(msg);
        }
    }
}


void RayCarter::moveForward()
{
    if(controller) controller->speed = controller->maxSpeed;
}

void RayCarter::moveBack()
{
    if(controller) controller->speed = -controller->maxSpeed;
}

void RayCarter::moveStop()
{
    if(controller) controller->speed = 0;
}

void RayCarter::turnRight()
{
    if(controller) controller->turn = 1;
}

void RayCarter::turnLeft()
{
    if(controller) controller->turn = -1;
}

void RayCarter::turnStop()
{
    if(controller) controller->turn = 0;
}

void RayCarter::sprintOn()
{
    if(controller) controller->runSpeedMultiplier = 2;
}

void RayCarter::sprintOff()
{
    if(controller) controller->runSpeedMultiplier = 1;
}

void RayCarter::interact()
{
    if(controller) dynamic_cast<Player*>(controller->getOwner())->doInteract();
}

void RayCarter::scream()
{
    se.play();
}

void RayCarter::shutup()
{
    se.stop();
}

void RayCarter::playMusic()
{
    /*if(music.state() == SoundEmitter::Playing)
        music.pause();
    else
        music.play();*/
}

Player *RayCarter::desc2Actor(const PlayerDescription &desc)
{
    Player *a = new Player();
    a->position = desc.pos;


    return a;
}

void RayCarter::actor2Desc(const Player &player, PlayerDescription &desc)
{
    desc.avatarID = 2;
    desc.locactionID = 0;
    desc.pos = player.position;
}

void RayCarter::updateActor(Player *player, const PlayerDescription &desc)
{
    player->position = desc.pos;
}

Player *RayCarter::findPlayer(uint32_t playerID)
{
    Player *result = nullptr;
    return result;
}
