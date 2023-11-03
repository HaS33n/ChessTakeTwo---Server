#include "../include/Utilities.h"

Game::Game(User* u1, User* u2){
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
    checkMove(start_pos,end_pos);
    
    
}

bool Game::checkMove(sf::Vector2u start_pos, sf::Vector2u end_pos){
    if(board[start_pos.x][start_pos.y]->getResident() == nullptr) //move from empty square
        return false;

    if(board[start_pos.x][start_pos.y]->getResident()->getColor() != whoseTurn->color) //move using opponents piece
        return false;

    if(board[end_pos.x][end_pos.y]->getResident() != nullptr){ // take own piece
        if(board[end_pos.x][end_pos.y]->getResident()->getColor() == whoseTurn->color)
        return false;
    }

    //proper calcs
    
}