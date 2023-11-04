#pragma once
#include <iostream>
#include <SFML/Network.hpp>

enum class chess_color{white,black};
enum class pieceType{king,queen,knight,bishop,rook,pawn};


class Piece {

public:
    Piece(chess_color color, pieceType t){clr = color; type = t;wasMovedPreviously = false;}
    const chess_color& getColor(){return clr;}
    const pieceType& getType(){return type;}
    const bool wasMoved(){return wasMovedPreviously;}
    void pieceWasMoved(){if(!wasMovedPreviously) wasMovedPreviously = true;}
private:  
    chess_color clr;
    pieceType type;
    bool wasMovedPreviously;
};

class Square {

public:
    Square(chess_color clr){color = clr; resident = nullptr;}
    void addResident(chess_color color, pieceType t){if(resident==nullptr) resident = new Piece(color,t);}
    Piece* getResident(){return resident;}

private:
    friend void movePiece(Square*, Square*);
    chess_color color;
    Piece* resident;
};

void movePiece(Square* from, Square* to);

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
    void updateBoard(Square* origin, Square* target, sf::Vector2u end_pos);
    //------------// i have cool idea how to implement this way better but im too lazy (and idk if its correct anyways)
    bool pawnMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    bool knightMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    bool bishopMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    bool rookMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    bool queenMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    bool kingMove(sf::Vector2u start_pos, sf::Vector2u end_pos);
    //variable overflow is not a problem here ^^
    //need to implement special moves, such as castle and promotion

    User* usw; //user white
    User* usb; //user black
    User* whoseTurn;
    Square* board[8][8];
    Square* w_kingptr; //this and below should be a structure i guess
    sf::Vector2u w_king_pos;
    Square* b_kingptr;
    sf::Vector2u b_king_pos;
};