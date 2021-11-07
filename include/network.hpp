#pragma once


#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <vector>
#include <functional>
#include <thread>

#include <queue>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename T>
class TSQueue
{
public:
    TSQueue() = default;
    TSQueue(const TSQueue<T> &) = delete;
    virtual ~TSQueue() {clear();}

public:
    const T& front()
    {
        std::scoped_lock lock(_queueMutex);
        return _data.front();
    }

    const T& back()
    {
        std::scoped_lock lock(_queueMutex);
        return _data.back();
    }

    T pop_front()
    {
        std::scoped_lock lock(_queueMutex);
        auto t = std::move(_data.front());
        _data.pop_front();
        return t;
    }

    T pop_back()
    {
        std::scoped_lock lock(_queueMutex);
        auto t = std::move(_data.back());
        _data.pop_back();
        return t;
    }

    void push_back(const T& item)
    {
        std::scoped_lock lock(_queueMutex);
        _data.emplace_back(std::move(item));

        std::unique_lock<std::mutex> ul(_blockMutex);
        _cvBlock.notify_one();
    }

    void push_front(const T& item)
    {
        std::scoped_lock lock(_queueMutex);
        _data.emplace_front(std::move(item));

        std::unique_lock<std::mutex> ul(_blockMutex);
        _cvBlock.notify_one();
    }

    bool empty() const
    {
        std::scoped_lock lock(_queueMutex);
        return _data.empty();
    }

    auto count() const
    {
        std::scoped_lock lock(_queueMutex);
        return _data.size();
    }

    void clear()
    {
        std::scoped_lock lock(_queueMutex);
        _data.clear();
    }

    void wait()
    {
        while(empty())
        {
            std::unique_lock<std::mutex> ul(_blockMutex);
            _cvBlock.wait(ul);
        }
    }

protected:
    mutable std::mutex _queueMutex;
    std::deque<T> _data;
    std::condition_variable _cvBlock;
    std::mutex _blockMutex;


};

template<typename T>
struct MessageHeader
{
    T id{};
    size_t size = 0;
};

template<typename T>
struct Message
{
    MessageHeader<T> header{};
    std::vector<uint8_t> data;

    size_t size() const
    {
        return data.size();
    }

    friend std::ostream &operator << (std::ostream &os, const Message<T> &msg)
    {
        os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
        return os;
    }

    template<typename DataType>
    friend Message<T> &operator << (Message<T> &msg, const DataType &data)
    {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");
        size_t i = msg.data.size();
        msg.data.resize(msg.data.size() + sizeof(DataType));
        std::memcpy(msg.data.data() + i, &data, sizeof(DataType));
        msg.header.size = msg.size();
        return msg;
    }

    template<typename DataType>
    friend Message<T> &operator >> (Message<T>& msg, DataType& data)
    {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");
        size_t i = msg.data.size() - sizeof(DataType);
        std::memcpy(&data, msg.data.data() + i, sizeof(DataType));
        msg.data.resize(i);
        msg.header.size = msg.size();
        return msg;
    }
};

template<typename T>
class Connection;

template<typename T>
struct OwnedMessage
{
    std::shared_ptr<Connection<T>> remote = nullptr;
    Message<T> msg;

    friend std::ostream &operator<<(std::ostream &os, const OwnedMessage<T> &msg)
    {
        os << msg.msg;
        return os;
    }
};


template<typename T>
class Connection : public std::enable_shared_from_this<Connection<T>>
{
public:
    enum class Owner
    {
        Server,
        Client
    };

public:
    Connection(Owner owner, asio::io_context &contect, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<T>> &qIn) :
          _owner(owner),
          _asioContext(contect),
          _socket(std::move(socket)),
          _qMessageIn(qIn)
    {}

    virtual ~Connection()
    {}

    uint32_t getID() const
    {
        return _id;
    }

    void connectToClient(uint32_t uid = 0)
    {
        if(_owner == Owner::Server)
        {
            if(_socket.is_open())
            {
                _id = uid;
                readHeader();
            }
        }
    }

    void connectToServer(const asio::ip::tcp::resolver::results_type &endpoints)
    {
        if(_owner == Owner::Client)
        {
            asio::async_connect(_socket, endpoints,
                                [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
                                {
                                    if(!ec)
                                    {
                                        readHeader();
                                    }
                                });
        }
    }

    void disconnect()
    {
        if(isConnected())
            asio::post(_asioContext,[this]() {_socket.close();});
    }

    bool isConnected() const
    {
        return _socket.is_open();
    }

    void send(const Message<T> &msg)
    {
        asio::post(_asioContext,
                   [this, msg]()
                   {
                       bool bWritingMessage = _qMessageOut.empty();
                       _qMessageOut.push_back(msg);
                       if(!bWritingMessage)
                       {
                           writeHeader();
                       }
                   });
    }


private:
    void writeHeader()
    {
        asio::async_write(_socket, asio::buffer(&_qMessageOut.front().header, sizeof(MessageHeader<T>)),
                          [this](std::error_code ec, std::size_t length)
                          {
                              if(!ec)
                              {
                                  if(_qMessageOut.front().data.size() > 0)
                                  {
                                      writeBody();
                                  }
                                  else
                                  {
                                      _qMessageOut.pop_front();

                                      if(!_qMessageOut.empty())
                                      {
                                          writeHeader();
                                      }
                                  }
                              }
                              else
                              {
                                  std::cout << "[" << _id << "] Write Header Fail.\n";
                                  _socket.close();
                              }
                          });
    }

    void writeBody()
    {
        asio::async_write(_socket, asio::buffer(_qMessageOut.front().data.data(),_qMessageOut.front().data.size()),
                          [this](std::error_code ec, std::size_t length)
                          {
                              if(!ec)
                              {
                                  _qMessageOut.pop_front();
                                  if(!_qMessageOut.empty())
                                      writeHeader();
                              }
                              else
                              {
                                  std::cout << "[" << _id << "] Write Body Fail.\n";
                                  _socket.close();
                              }
                          });
    }

    void readHeader()
    {
        asio::async_read(_socket, asio::buffer(&_msgTemporaryIn.header, sizeof(MessageHeader<T>)),
                         [this](std::error_code ec, std::size_t length)
                         {
                             if(!ec)
                             {
                                 if(_msgTemporaryIn.header.size > 0)
                                 {
                                     _msgTemporaryIn.data.resize(_msgTemporaryIn.header.size);
                                     readBody();
                                 }
                                 else
                                 {
                                     addToIncomingMessageQueue();
                                 }
                             }
                             else
                             {
                                 std::cout << "[" << _id << "] Read Header Fail\n";
                                 _socket.close();
                             }
                         });
    }

    void readBody()
    {
        asio::async_read(_socket, asio::buffer(_msgTemporaryIn.data.data(), _msgTemporaryIn.data.size()),
                         [this](std::error_code ec, std::size_t length)
                         {
                             if(!ec)
                             {
                                 addToIncomingMessageQueue();
                             }
                             else
                             {
                                 std::cout << "[" << _id << "] Read Body Fail\n";
                                 _socket.close();
                             }
                         });
    }

    void addToIncomingMessageQueue()
    {
        if(_owner == Owner::Server)
            _qMessageIn.push_back({this->shared_from_this(), _msgTemporaryIn});
        else
            _qMessageIn.push_back({nullptr, _msgTemporaryIn});
        readHeader();
    }


protected:
    Owner _owner;
    asio::io_context &_asioContext;
    TSQueue<Message<T>> _qMessageOut;
    TSQueue<OwnedMessage<T>> &_qMessageIn;
    Message<T> _msgTemporaryIn;
    asio::ip::tcp::socket _socket;
    uint32_t _id = 0;
};



template<typename T>
class Client
{
public:
    Client() {}
    virtual ~Client() {disconnect();}

    bool connect(const std::string &host, const uint16_t port)
    {
        try
        {
            asio::ip::tcp::resolver resolver(_context);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

            _connection = std::make_unique<Connection<T>>(Connection<T>::Owner::Client, _context, asio::ip::tcp::socket(_context),_messageIn);
            _connection->connectToServer(endpoints);
            _contextThread = std::thread([this]() {_context.run();});
        }
        catch(std::exception &e)
        {
            std::cout << "Client Exception: " << e.what() << "\n";
            return false;
        }
        return  true;
    }

    void disconnect()
    {
        if(isConnected())
        {
            _connection->disconnect();
        }

        _context.stop();

        if(_contextThread.joinable())
            _contextThread.join();

        _connection.release();
    }

    void send(const Message<T> &msg)
    {
        if(isConnected())
            _connection->send(msg);
    }

    bool isConnected() const
    {
        if(_connection)
            return _connection->isConnected();
        else
            return false;
    }

    TSQueue<OwnedMessage<T>> &incoming()
    {
        return _messageIn;
    }

protected:
    asio::io_context _context;
    std::thread _contextThread;
    std::unique_ptr<Connection<T>> _connection;
    TSQueue<OwnedMessage<T>> _messageIn;
};


template<typename T>
class Server
{
public:
    Server(uint16_t port) :
          _asioAcceptor(_asioContext,asio::ip::tcp::endpoint(asio::ip::tcp::v4(),port))
    {}

    ~Server()
    {
        stop();
    }

    bool start()
    {
        try
        {
            waitForClientConnection();
            _contextThread = std::thread([this]() {_asioContext.run();});
        }
        catch(std::exception &e)
        {
            std::cout << "[SERVER] Exception: " << e.what() << "\n";
            return false;
        }
        std::cout << "[SERVER] Started!\n";
        return true;
    }

    void stop()
    {
        _asioContext.stop();
        if(_contextThread.joinable())
            _contextThread.join();
        std::cout << "[SERVER] Stopped!\n";
    }

    void waitForClientConnection()
    {
        _asioAcceptor.async_accept(
            [this](const std::error_code &ec, asio::ip::tcp::socket socket)
            {
                if(!ec)
                {
                    std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

                    std::shared_ptr<Connection<T>> newconn = std::make_shared<Connection<T>>(Connection<T>::Owner::Server, _asioContext, std::move(socket), _messagesIn);

                    if(onClientConnect(newconn))
                    {
                        _connections.push_back(std::move(newconn));

                        _connections.back()->connectToClient(_idCounter++);
                        std::cout << "[" << _connections.back()->getID() << "] Connection Approved\n";
                    }
                    else
                    {
                        std::cout << "[-----] Connection Denied\n";
                    }
                }
                else
                {
                    std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
                }

                waitForClientConnection();
            });
    }

    void messageClient(std::shared_ptr<Connection<T>> client, const Message<T> &msg)
    {
        if(client && client->isConnected())
        {
            client->send(msg);
        }
        else
        {
            onClientDisconnect(client);
            client.reset();
            _connections.erase(std::remove(_connections.begin(), _connections.end(), client), _connections.end());
        }
    }

    void messageAllClients(const Message<T> &msg, std::shared_ptr<Connection<T>> ignoreClient = nullptr)
    {
        bool invalidClientExists = false;

        for(auto &client : _connections)
        {
            if(client && client->isConnected())
            {
                if(client != ignoreClient)
                    client->send(msg);
            }
            else
            {
                onClientDisconnect(client);
                client.reset();
                invalidClientExists = true;
            }
        }

        if(invalidClientExists)
            _connections.erase(std::remove(_connections.begin(), _connections.end(), nullptr), _connections.end());
    }

    void update(size_t maxMessages = -1, bool wait = false)
    {
        if(wait)
            _messagesIn.wait();

        size_t messageCount = 0;

        while((messageCount < maxMessages) && !_messagesIn.empty())
        {
            auto msg = _messagesIn.pop_front();

            onMessage(msg.remote,msg.msg);

            ++messageCount;
        }
    }

protected:
    virtual bool onClientConnect(std::shared_ptr<Connection<T>> client) {return false;}
    virtual void onClientDisconnect(std::shared_ptr<Connection<T>> client) {}
    virtual void onMessage(std::shared_ptr<Connection<T>> client, Message<T> &msg) {}


private:
    TSQueue<OwnedMessage<T>> _messagesIn;
    std::deque<std::shared_ptr<Connection<T>>> _connections;
    asio::io_context _asioContext;
    std::thread _contextThread;
    asio::ip::tcp::acceptor _asioAcceptor;
    uint32_t _idCounter = 10000;
};

class Network
{
public:
    Network();

    bool open(const asio::ip::address &addr, int &port);
    void close();
    void setPeer(const asio::ip::udp::endpoint &ep);
    void send(void *buffer, size_t size);
    void asyncReceive(std::function<void (void*,size_t)> cb);


    void testSend();

    void arrrComeAndGetSome(asio::ip::tcp::socket &sock);

private:
    std::vector<char> vBuffer;

};
