#include "../include/Utilities.h"

Game::Game(User* u1, User* u2){
    usw=u1;
    usb=u2;
    whoseTurn = usw;
	en_passant_sqr = nullptr;
	en_passant = false;
	current_move_by_2 = false;
	was_last_move_by_2 = false;

	bool isWhite = true;
	//send to u1 their color
	
	sf::Packet p1;
	p1<<isWhite;
	u1->sckt->send(p1);

	isWhite = false;
	sf::Packet p2;
	p2<<isWhite;
	u1->sckt->send(p2);

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
	w_kingptr = board[4][0];
	w_king_pos = sf::Vector2u(4,0);
	board[4][7]->addResident(chess_color::black,pieceType::king);
	b_kingptr = board[4][7];
	b_king_pos = sf::Vector2u(4,7);
}

void Game::testSockets(const sf::SocketSelector& slctr){
    if(!slctr.isReady(*usw->sckt) && !slctr.isReady(*usb->sckt))
        return;

    if(slctr.isReady(*whoseTurn->sckt)){
		processInput();
		return;
	}
        

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

    if(!checkMove(start_pos,end_pos)){
		//send move discard (should be a func)
		sf::Packet rPack;
		bool is_move_possible = false;
		rPack<<is_move_possible;
		whoseTurn->sckt->send(rPack);
		return;
	}
		

	else{
		//send move approval
		sf::Packet rPack;
		bool is_move_possible = true;
		rPack<<is_move_possible;
		whoseTurn->sckt->send(rPack);

		//send to other user move info
		sf::Packet u2_pack;
		u2_pack<<start_pos.x<<start_pos.y<<end_pos.x<<end_pos.y<<en_passant; //send additional info if he's checked + special moves TODO
		if(whoseTurn == usw)
			usb->sckt->send(u2_pack);
		
		else
			usw->sckt->send(u2_pack);

		updateBoard(board[start_pos.x][start_pos.y],board[end_pos.x][end_pos.y], end_pos);
		return; 
	}
		
	
}

bool Game::checkMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	//check if values are correct (in range) maybe TODO (idk if its necessary)
	
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

	switch (board[start_pos.x][start_pos.y]->getResident()->getType()) //this HAS TO be a function
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

	Square* kingptr;
	chess_color pieces_color;
	sf::Vector2u kingcrds;

	if(whoseTurn->color == chess_color::white){
		kingptr = w_kingptr;
		pieces_color = chess_color::black;
		kingcrds = w_king_pos;
	}
		

	else{
		kingptr = b_kingptr;
		pieces_color = chess_color::white;
		kingcrds = b_king_pos;
	}
		
	for(int i=0;i<8;i++){ //checking if king is checked, this should also be a function.....
		for(int j=0;j<8;j++){
			Square* checked_sqr = board[i][j];
			if(checked_sqr->getResident() != nullptr){
				if(checked_sqr->getResident()->getColor() == pieces_color && checked_sqr->getResident()->getType() != pieceType::king)
				{
					switch (checked_sqr->getResident()->getType()) //ugly code layout but whatever
	{
	case pieceType::pawn:
		if(pawnMove(sf::Vector2u(i,j),kingcrds)) // here need to add parameter, wheter its checking for check or not (you can only check side)
			return false;
		
		break;

	case pieceType::knight:
		if(knightMove(sf::Vector2u(i,j),kingcrds))
			return false;
		
		break;

	case pieceType::bishop:
		if(bishopMove(sf::Vector2u(i,j),kingcrds))
			return false;
		
		break;

	case pieceType::rook:
		if(rookMove(sf::Vector2u(i,j),kingcrds))
			return false;
		
		break;

	case pieceType::queen:
		if(queenMove(sf::Vector2u(i,j),kingcrds))
			return false;
		
		break;

	default:
		std::cout<<"??? \n";
		break;
	}
				}
			}
		}
	}
	

	return true;
}

bool Game::pawnMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
	bool was_moved = board[start_pos.x][start_pos.y]->getResident()->wasMoved();
if(board[start_pos.x][start_pos.y]->getResident()->getColor() == chess_color::white){
	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y + 1 && board[end_pos.x][end_pos.y]->getResident() == nullptr) //move by 1
		return true;
	
	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y + 2 && !was_moved && board[end_pos.x][end_pos.y]->getResident() == nullptr && board[end_pos.x][end_pos.y-1]->getResident() == nullptr){ //move by 2
		en_passant_sqr = board[end_pos.x][end_pos.y - 1];
		current_move_by_2 = true; //nie wiem kurwa
		return true;
	} 
		
	if((end_pos.x == start_pos.x +1 || end_pos.x == start_pos.x -1) && (end_pos.y == start_pos.y +1) && board[end_pos.x][end_pos.y] == en_passant_sqr){ //en passant
		en_passant = true;
		return true;
	}


	if((end_pos.x == start_pos.x +1 || end_pos.x == start_pos.x -1) && (end_pos.y == start_pos.y +1)){ // take (overflow isnt a problem here)
		if(board[end_pos.x][end_pos.y]->getResident() != nullptr)
			return true;
	}
}

else{
	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y - 1 && board[end_pos.x][end_pos.y]->getResident() == nullptr) //move by 1
		return true;
	
	if(start_pos.x == end_pos.x && end_pos.y == start_pos.y - 2 && !was_moved && board[end_pos.x][end_pos.y]->getResident() == nullptr && board[end_pos.x][end_pos.y-1]->getResident() == nullptr){ //move by 2
		en_passant_sqr = board[end_pos.x][end_pos.y + 1];
		return true;
	}

	if((end_pos.x == start_pos.x +1 || end_pos.x == start_pos.x -1) && (end_pos.y == start_pos.y -1)){ // take (overflow isnt a problem here)
		if(board[end_pos.x][end_pos.y]->getResident() != nullptr)
			return true;
	}
}
	//promotions + en passant TODO

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

	//checking for opposition + castles TODO


	return false;
}

void movePiece(Square* from, Square* to){
    if(from->resident == nullptr){
        std::cout<<"nullptr \n";
        return;
    }

    if(to->resident != nullptr)
        delete to->resident;

	from->resident->pieceWasMoved();
    to->resident = from->resident;
    from->resident = nullptr;
}

void Game::updateBoard(Square* origin, Square* target, sf::Vector2u end_pos){
	movePiece(origin,target);

	if(en_passant){
		if(whoseTurn->color == chess_color::white)
			board[end_pos.x][end_pos.y - 1] == nullptr;

		else
			board[end_pos.x][end_pos.y + 1] == nullptr;
	}
	en_passant = false;	

	if(whoseTurn == usw){
		if(origin->getResident()!= nullptr){
			if(origin->getResident()->getType() == pieceType::king)
				w_kingptr = target;
				w_king_pos = end_pos;
		}

		whoseTurn == usb;
	}

	else{
		if(origin->getResident()!= nullptr){
			if(origin->getResident()->getType() == pieceType::king)
				b_kingptr = target;
				b_king_pos = end_pos;
		}

		whoseTurn == usw;
	}
}


