#pragma once
#define DEF_PORT 53000
#include <iostream>
#include <SFML/Network.hpp>
#include <list>

class ServerManager{

public:
    ServerManager();
    void runServer();
private:

    sf::SocketSelector selector;
    sf::TcpListener listener;
    std::list<sf::TcpSocket*> clients;
};