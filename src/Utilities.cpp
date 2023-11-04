#include "../include/Utilities.h"

Game::Game(User* u1, User* u2){
	//send users their color <--- TODO
    usw=u1;
    usb=u2;
    whoseTurn = usw;


    //init the board
	chess_color previous; 

	for(int i=0;i<8;i++){
		if(i%2 == 0)
			previous = chess_color::white;

		else
			previous = chess_color::black;

		
		for(int j=0;j<8;j++){
			if(previous == chess_color::white){
				board[i][j] = new Square(chess_color::black);
				previous = chess_color::black;
			}

			else{
				board[i][j] = new Square(chess_color::white);
				previous = chess_color::white;
			}
		}
	}

    //populate the board with pieces
	//pawns
	for(int i=0;i<8;i++)
		board[i][1]->addResident(chess_color::white,pieceType::pawn);

	for(int i=0;i<8;i++)
		board[i][6]->addResident(chess_color::black,pieceType::pawn);	

	//knights
	board[1][0]->addResident(chess_color::white,pieceType::knight);
	board[6][0]->addResident(chess_color::white,pieceType::knight);
	board[1][7]->addResident(chess_color::black,pieceType::knight);
	board[6][7]->addResident(chess_color::black,pieceType::knight);

	//rooks
	board[0][0]->addResident(chess_color::white,pieceType::rook);
	board[7][0]->addResident(chess_color::white,pieceType::rook);
	board[0][7]->addResident(chess_color::black,pieceType::rook);
	board[7][7]->addResident(chess_color::black,pieceType::rook);

	//bishops
	board[2][0]->addResident(chess_color::white,pieceType::bishop);
	board[5][0]->addResident(chess_color::white,pieceType::bishop);
	board[2][7]->addResident(chess_color::black,pieceType::bishop);
	board[5][7]->addResident(chess_color::black,pieceType::bishop);

	//queens
	board[3][0]->addResident(chess_color::white,pieceType::queen);
	board[3][7]->addResident(chess_color::black,pieceType::queen);

	//kings
	board[4][0]->addResident(chess_color::white,pieceType::king);
	board[4][7]->addResident(chess_color::black,pieceType::king);
}

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

void Game::processInput(){ //need proper error handling
    sf::Packet pack;
    if(whoseTurn->sckt->receive(pack) != sf::Socket::Done)
        std::cout<<"failed receiving \n";

    sf::Vector2u start_pos, end_pos;
    if(!pack>>start_pos.x>>start_pos.y>>end_pos.x>>end_pos.y){
        std::cout<<"failed reading \n";
        return;
    }
    if(!checkMove(start_pos,end_pos))
		return; //send decline to user TODO

	else
		return; //send accept to user and update the board TODO
	

    //.... MORE
    
}

bool Game::checkMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	//check if values are correct (in range)
	
    if(board[start_pos.x][start_pos.y]->getResident() == nullptr) //move from empty square
        return false;

    if(board[start_pos.x][start_pos.y]->getResident()->getColor() != whoseTurn->color) //move using opponents piece
        return false;

    if(board[end_pos.x][end_pos.y]->getResident() != nullptr){ // take own piece
        if(board[end_pos.x][end_pos.y]->getResident()->getColor() == whoseTurn->color)
        return false;
    }

	if(start_pos == end_pos) //move in place (?)
		return false;

	switch (board[start_pos.x][start_pos.y]->getResident()->getType())
	{
	case pieceType::pawn:
		if(!pawnMove(start_pos,end_pos))
			return false;
		
		break;

	case pieceType::knight:
		if(!knightMove(start_pos,end_pos))
			return false;
		
		break;

	case pieceType::bishop:
		if(!bishopMove(start_pos,end_pos))
			return false;
		
		break;

	case pieceType::rook:
		if(!rookMove(start_pos,end_pos))
			return false;
		
		break;

	case pieceType::queen:
		if(!queenMove(start_pos,end_pos))
			return false;
		
		break;

	case pieceType::king:
		if(!kingMove(start_pos,end_pos))
			return false;
		
		break;

	default:
		std::cout<<"??? \n";
		break;
	}
    

	//check for checks

	return true;
}

bool Game::pawnMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	bool was_moved = board[start_pos.x][start_pos.y]->getResident()->wasMoved();

	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y + 1 && board[end_pos.x][end_pos.y]->getResident() == nullptr) //move by 1
		return true;
	
	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y + 2 && !was_moved && board[end_pos.x][end_pos.y]->getResident() == nullptr && board[end_pos.x][end_pos.y-1]->getResident() == nullptr) //move by 2
		return true;

	if((end_pos.x == start_pos.x +1 || end_pos.x == start_pos.x -1) && (end_pos.y == start_pos.y +1)){ // take (overflow isnt a problem here)
		if(board[end_pos.x][end_pos.y]->getResident() != nullptr)
			return true;
	}

	//promotions + en passant

	return false;
}

bool Game::knightMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	if(end_pos.x == start_pos.x +2 || end_pos.x == start_pos.x -2){
		if(end_pos.y == start_pos.y + 1 || end_pos.y == start_pos.y - 1)
			return true;
	}

	if(end_pos.x == start_pos.y +2 || end_pos.x == start_pos.y -2){
		if(end_pos.y == start_pos.x + 1 || end_pos.y == start_pos.x - 1)
			return true;
	}

	return false;
}

bool Game::bishopMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	short int dirX, dirY; //directions (signs)

	if(start_pos.x < end_pos.x)
		dirX = 1;

	else 
		dirX = -1;

	if(start_pos.y < end_pos.y)
		dirY = 1;
	
	else
		dirX = -1;

	int i=1;
	while(start_pos.x + dirX*i >= 0 && start_pos.x + dirX*i <= 7 && start_pos.y + dirY*i >= 0 && start_pos.y + dirY*i <= 7 ){
		if(start_pos.x + dirX*i == end_pos.x && start_pos.y + dirY*i == end_pos.y)
			return true;

		if(board[start_pos.x + dirX*i][start_pos.y + dirY*i]->getResident() != nullptr) //something in the waaaaaay hmmmmmmmmmmmmmmm
			return false;
		i++;
	}

	return false;
}

bool Game::rookMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	if(start_pos.x != end_pos.x && start_pos.y != end_pos.y)
		return false;

	if(start_pos.x == end_pos.x){
		int dir;
		if(start_pos.y < end_pos.y)
			dir=1;
		
		else
			dir = -1;
		int i=1;
		while(start_pos.y + dir *i >= 0 && start_pos.y + dir *i <=7){
			if(start_pos.y + dir *i == end_pos.y)
				return true;
			
			if(board[start_pos.x][start_pos.y + dir *i]->getResident() != nullptr)
				return false;

			i++;
		}
		return false; //if this gets called, something went wrong
	}

	else{
		int dir;
		if(start_pos.x < end_pos.x)
			dir=1;
		
		else
			dir = -1;
		int i=1;
		while(start_pos.x + dir *i >= 0 && start_pos.x + dir *i <=7){
			if(start_pos.x + dir *i == end_pos.x)
				return true;
			
			if(board[start_pos.x + dir *i][start_pos.y]->getResident() != nullptr)
				return false;

			i++;
		}
		return false; //if this gets called, something went wrong
	}

	return false; //if this gets called, something went wrong
}

bool Game::queenMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	if(bishopMove(start_pos,end_pos))
		return true;
	
	if(rookMove(start_pos,end_pos))
		return true;
	
	return false;
}

bool Game::kingMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	//i will just make braindead checking

	//moves into the left
	if(start_pos.x + -1 == end_pos.x){
		if(start_pos.y + 1 == end_pos.y || start_pos.y - 1 == end_pos.y || start_pos.y == end_pos.y )
			return true;
	}

	//const x 
	if(start_pos.x == end_pos.x){
		if(start_pos.y + 1 == end_pos.y || start_pos.y - 1 == end_pos.y)
			return true;
	}

	//moves into the right
	if(start_pos.x + +1 == end_pos.x){
		if(start_pos.y + 1 == end_pos.y || start_pos.y - 1 == end_pos.y || start_pos.y == end_pos.y )
			return true;
	}

	return false;
}
