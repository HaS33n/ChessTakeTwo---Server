#pragma once
#include <iostream>
#include <SFML/Network.hpp>

enum class chess_color{white,black};
enum class pieceType{king,queen,knight,bishop,rook,pawn};

class Piece {

public:
    Piece(chess_color color, pieceType t){clr = color; type = t;}
private:  
    chess_color clr;
    pieceType type;
};

class Square {

public:
    Square(chess_color color, sf::Vector2u crds);
    void addResident(chess_color color, pieceType t){resident = new Piece(color,t);}

private:
    friend void movePiece(Square*, Square*);
    chess_color clr;
    Piece* resident;
    sf::Vector2u coords;
};

class User{
public:
    User();
private:
    sf::TcpSocket sckt;
    chess_color color;
};

class Game{
public:
    Game(User* u1, User* u2);
private:
    User* usw; //user white
    User* usb; //user black
    chess_color whoseTurn;
};

void movePiece(Square* from, Square* to);