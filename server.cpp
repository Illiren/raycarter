#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"
#include "network.hpp"
#include <thread>

class CustomServer : public Server<CustomMsgTypes>
{
public:
    CustomServer(uint16_t nPort);
    TString playername = "Object0";


    std::unordered_map<uint32_t, PlayerDescription> _mapPlayerRoster;
    std::vector<uint32_t> _vGarbageIDs;

protected:
    bool onClientConnect(std::shared_ptr<Connection<CustomMsgTypes>> client) override;
    void onClientDisconnect(std::shared_ptr<Connection<CustomMsgTypes>> client) override;
    void onMessage(std::shared_ptr<Connection<CustomMsgTypes>> client, Message<CustomMsgTypes> &msg) override;


    // Server interface
public:
    void onClientValidated(std::shared_ptr<Connection<CustomMsgTypes>> client) override;
};

CustomServer::CustomServer(uint16_t nPort) :
      Server<CustomMsgTypes>(nPort)
{}

bool CustomServer::onClientConnect(std::shared_ptr<Connection<CustomMsgTypes> > client)
{
    //Message<CustomMsgTypes> msg;
    //msg.header.id = CustomMsgTypes::ServerAccept;
    //client->send(msg);
    return true;
}

void CustomServer::onClientValidated(std::shared_ptr<Connection<CustomMsgTypes>> client)
{
    Message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ClientAccepted;
    client->send(msg);
}

void CustomServer::onClientDisconnect(std::shared_ptr<Connection<CustomMsgTypes> > client)
{
    std::cout << "Someone disconnected\n";
    if(client)
    {
        if(_mapPlayerRoster.find(client->getID()) == _mapPlayerRoster.end())
        {

        }
        else
        {
            auto &pd = _mapPlayerRoster[client->getID()];
            std::cout << "[UNGRACEFUL REMOVAL]:" + playername + "\n";
            _mapPlayerRoster.erase(client->getID());
            _vGarbageIDs.push_back(client->getID());
        }
    }
}

void CustomServer::onMessage(std::shared_ptr<Connection<CustomMsgTypes>> client, Message<CustomMsgTypes> &msg)
{
    if(!_vGarbageIDs.empty())
    {
        for(auto pid : _vGarbageIDs)
        {
            Message<CustomMsgTypes> m;
            m.header.id = CustomMsgTypes::GameRemovePlayer;
            m << pid;
            std::cout << "Removing " << pid << "\n";
            messageAllClients(m);
        }
        _vGarbageIDs.clear();
    }

    switch(msg.header.id)
    {
    case CustomMsgTypes::ClientRegister:
    {
        PlayerDescription desc;
        msg >> desc;
        desc.uniqueID = client->getID();
        _mapPlayerRoster.insert_or_assign(desc.uniqueID,desc);

        Message<CustomMsgTypes> msgSendId;
        msgSendId.header.id = CustomMsgTypes::ClientAssignID;
        msgSendId << desc.uniqueID;
        messageClient(client,msgSendId);

        Message<CustomMsgTypes> msgAddPlayer;
        msgAddPlayer.header.id = CustomMsgTypes::GameAddPlayer;
        msgAddPlayer << desc;
        messageAllClients(msgAddPlayer);

        for(const auto &player : _mapPlayerRoster)
        {
            Message<CustomMsgTypes> addOtherPlayers;
            addOtherPlayers.header.id = CustomMsgTypes::GameAddPlayer;
            addOtherPlayers << player.second;
            messageClient(client, addOtherPlayers);
        }

        break;
    }
    case CustomMsgTypes::ClientUnregister:
    {
        break;
    }
    case CustomMsgTypes::GameUpdatePlayer:
    {
        messageAllClients(msg,client);
        break;
    }
    default:
        break;
    }
}




int main()
{
    bool debug = true;
    CustomServer sever(1234);
    sever.start();

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&debug](){debug = false;};

    while(debug)
    {
        GetInputManager().update();
        sever.update(-1,true);
    }

    return 0;
}
