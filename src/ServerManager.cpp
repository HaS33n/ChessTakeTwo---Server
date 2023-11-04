#include "../include/ServerManager.h"

ServerManager::ServerManager()
{
    listener.listen(DEF_PORT);
    selector.add(listener);

}

void ServerManager::runServer(){
    while (true)
    {
        if (selector.wait(sf::seconds(1)))
        {
            if (selector.isReady(listener))
            {
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    clients.push_back(client);
                    //send client their position in queue <------TODO
                    selector.add(*client);
                }
                else
                    delete client;
            }
        
            for(auto& it : clients)
            {
                if (selector.isReady(*it))
                {
                    sf::Packet packet;
                    if (it->receive(packet) == sf::Socket::Done)
                        packet.clear(); //i dont care whats inside, they just have to wait lol
                }
            }

            for(auto& it : games)
                it->testSockets(selector);


            
        }

        while(clients.size()>=2){
            User* u1 = new User(clients.front(),chess_color::white);
            //selector.remove(*clients.front());
            clients.pop_front();
            
            User* u2 = new User(clients.front(),chess_color::white);
            //selector.remove(*clients.front());
            clients.pop_front();

            Game* ngame = new Game(u1,u2);
            games.push_back(ngame);
        }

    }

}
