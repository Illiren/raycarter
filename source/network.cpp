#include "network.hpp"
#include <iostream>
#include <chrono>

using namespace std;

struct Stun
{
    std::string url;
    std::string port;
};




Network::Network() :
      vBuffer(20*1024)
{}

bool Network::open(const asio::ip::address &addr, int &port)
{

    return true;
}

void Network::close()
{

}

void Network::setPeer(const asio::ip::udp::endpoint &ep)
{

}

void Network::send(void *buffer, size_t size)
{

}

void Network::asyncReceive(std::function<void (void *, size_t)> cb)
{

}

void Network::arrrComeAndGetSome(asio::ip::tcp::socket  &sock)
{
    sock.async_read_some(asio::buffer(vBuffer.data(),vBuffer.size()),
                           [&](std::error_code ec, std::size_t length)
                           {
                                if(!ec)
                                {
                                    cout <<"\n\nRead " << length << " bytes\n\n";
                                    for(int i=0;i<length;++i)
                                        std::cout << vBuffer[i];
                                    std::cout << std::endl;
                                    arrrComeAndGetSome(sock);
                                }
                           });
}

void Network::testSend()
{
    asio::error_code ec;

    asio::io_context context;

    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34",ec),80);

    asio::ip::tcp::socket socket(context);

    asio::ip::udp::socket usock(context);



    socket.connect(endpoint,ec);

    if(!ec)
    {
        std::cout << "Connected" << endl;
    }
    else
    {
        std::cout << "Not Connected " << ec.message() << endl;
    }

    if(socket.is_open())
    {
        string sRequest =
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";

        arrrComeAndGetSome(socket);

        socket.write_some(asio::buffer(sRequest.data(),sRequest.size()),ec);

    }


}
