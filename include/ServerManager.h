#pragma once
#define DEF_PORT 53000
#include <iostream>
#include <SFML/Network.hpp>
#include <deque>
#include <vector>
#include "Utilities.h"

class ServerManager{

public:
    ServerManager();
    void runServer();
private:

    sf::SocketSelector selector;
    sf::TcpListener listener;
    std::deque<sf::TcpSocket*> clients;
    std::vector<Game*> games;
};