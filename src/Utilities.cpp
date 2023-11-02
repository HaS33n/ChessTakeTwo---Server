#include "../include/Utilities.h"

void Game::testSockets(const sf::SocketSelector& slctr){
    if(!slctr.isReady(*usw->sckt) && !slctr.isReady(*usb->sckt))
        return;

    if(slctr.isReady(*whoseTurn->sckt))
        processInput();

    User* ptr = usw; //ptr points to user who doesnt have right to move now
    if(ptr == whoseTurn)
        ptr = usb;
    
    if(slctr.isReady(*ptr->sckt)){
        sf::Packet pack;
        if(ptr->sckt->receive(pack) == sf::Socket::Done)
            pack.clear(); //i dont care what they send, its not their turn
    }
}