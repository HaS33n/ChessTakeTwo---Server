#pragma once
#include <iostream>
#include <SFML/Network.hpp>

enum class chess_color{white,black};
enum class pieceType{king,queen,knight,bishop,rook,pawn};


class Piece {

public:
    Piece(chess_color color, pieceType t){clr = color; type = t;}
    const chess_color& getColor(){return clr;}
    const pieceType& getType(){return type;}
private:  
    chess_color clr;
    pieceType type;
};

//void movePiece(Square* from, Square* to); //<----TODO

class Square {

public:
    Square(chess_color clr){color = clr; resident = nullptr;}
    void addResident(chess_color color, pieceType t){if(resident==nullptr) resident = new Piece(color,t);}
    Piece* getResident(){return resident;}

private:
    //friend void movePiece(Square*, Square*); // <----uncomment when movePiece is defined <--- TODO
    chess_color color;
    Piece* resident;
};

class User{
public:
    User(sf::TcpSocket* s, chess_color clr){sckt =s; color = clr;}
private:
    friend class Game;
    sf::TcpSocket* sckt;
    chess_color color;
};

class Game{
public:
    Game(User* u1, User* u2);
    void testSockets(const sf::SocketSelector& slctr);
    
private:
    void processInput();
    bool checkMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    
    User* usw; //user white
    User* usb; //user black
    User* whoseTurn;
    Square* board[8][8];
};