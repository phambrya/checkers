#include "checkBoard.h"
#include "globals.h"
#include "turnLogic.h"
#include "networkUtilities.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

int playCheckers(int redPieces, int whitePieces, int myConcede, int opponentConcede);
void endGame(int redPieces, int whitePieces, int redConcede, int whiteConcede);
int getCoordInput(char *userBuf, int type);
void sig_handler(int sigNum);

char myColor;
char curTurn;
char receive_buffer[RECEIVE_BUFFER_SIZE];
int sock_desc;

int main(){
	// Create an action handler as users tend to panic Ctrl + C upon poor input, want to make sure to recover
	// Properly if it happens during input
	struct sigaction sa = {
		.sa_handler = sig_handler,
		.sa_flags = SA_RESTART
	};
	sigaction(SIGINT, &sa, NULL);
	// Setup a host or connect to a pre-existing host
	printf("Would you like to host or connect? (h/c) ");
	char serv = tolower(getchar());
	while(getchar() != '\n');
	printf("\n");
	// Make sure no garbage in receive buffer
	memset(receive_buffer, 0, RECEIVE_BUFFER_SIZE);

	// Whether user running this is hosting or connecting, sock_desc will be the fd they write to and read from
	if(serv == 'h'){
		// Init server
		sock_desc = createServerAndWait();
		receiveAck(sock_desc);
	} else if(serv == 'c') {
		// Connect to server
		sock_desc = connectToServer();
		sendAck(sock_desc);
	} else {
		printf("Bad input, closing.\n");
	}
	if(sock_desc == -1){
		printf("Fatal error during connection, there is no host on this machine.\n");
		exit(-1);
	}
	char choice;
	// Give client the choice to choose what color they would like
	if(serv == 'c'){
		while((choice != 'r') && (choice != 'w')){
			printf("What color would you like to play as? (r/w) ");
			choice = tolower(getchar());
			// Clear buffer
			while(getchar() != '\n');
			printf("\n");
		}
		myColor = choice;
		if(choice == 'r'){
			sprintf(receive_buffer, "w\n");
			send_message(sock_desc, receive_buffer);
		} else {
			sprintf(receive_buffer, "r\n");
			send_message(sock_desc, receive_buffer);
		}
		receive_board(sock_desc, receive_buffer);
		sendAck(sock_desc);
	} else {
		receive(sock_desc, receive_buffer, RECEIVE_BUFFER_SIZE);
		myColor = receive_buffer[0];
		printf("Client chose their color so I am %c!\n", myColor);
		// Have only the server create the board and then pass it to the client
		createBoard();
		send_board(sock_desc, receive_buffer);
		receiveAck(sock_desc);
	}
	int redPieces = 12;
	int whitePieces = 12;
	int redConcede = 0;
	int whiteConcede = 0;
	playCheckers(redPieces, whitePieces, redConcede, whiteConcede);
	return 0;
}

// Main driver of the game
int playCheckers(int redPieces, int whitePieces, int redConcede, int whiteConcede){
	//curTurn = 'r';
	curTurn = 'r';
	char fromCoord[3] = {'h'};
	char toCoord[3] = {'h'};
	// Drive the game, loop until an exit condition is met
	while((redPieces > 0) && (whitePieces > 0) && (redConcede != 1) && (whiteConcede != 1)){
		printMyBoard(myColor);
		printf("It is %c's turn!\n", curTurn);
		int take = canTake(myColor);
		// Begin turn logic, loop until player executes a move and is unable to do more
		int moveMade = 0;
		while(!moveMade && curTurn == myColor){
			// Get from coordinate
			int inputStatus = getCoordInput(fromCoord, 1);
			// Check if a player chose to concede
			if(!inputStatus){
				if(curTurn == 'r'){
					redConcede = 1;         
				} else {
					whiteConcede = 1;
				}
				sendTurnData(sock_desc, receive_buffer, &redPieces, &whitePieces, &redConcede, &whiteConcede, &curTurn);
				endGame(redPieces, whitePieces, redConcede, whiteConcede);
			}
			// Get to coordinate
			inputStatus = getCoordInput(toCoord, 2);
			if(!inputStatus){
				if(curTurn == 'r'){
					redConcede = 1;         
				} else {
					whiteConcede = 1;
				}
				sendTurnData(sock_desc, receive_buffer, &redPieces, &whitePieces, &redConcede, &whiteConcede, &curTurn);
				endGame(redPieces, whitePieces, redConcede, whiteConcede);
			}
			// try to validate input move, otherwise loop and get new coords
			moveMade = validateMove(fromCoord, toCoord, take, curTurn, &redPieces, &whitePieces);
			// Reset input buffers
			fromCoord[0] = 'h';
			toCoord[0] = 'h';
			// If we have already taken a piece need to check to see if we can take again for double
			if(take && moveMade){
				take = canTake(curTurn);
				if(take){
					moveMade = 0;
					printMyBoard(curTurn);
				}
			}
			// Need to make sure to alternate turns
			if(moveMade == 1){
				if(curTurn == 'r'){
					curTurn = 'w';
				} else {
					curTurn = 'r';
				}
			}
			// Send turn data to opponent after every move is made, but don't transmit on bad input 
			if(take || moveMade){
				printMyBoard(myColor);
				sendTurnData(sock_desc, receive_buffer, &redPieces, &whitePieces, &redConcede, &whiteConcede, &curTurn);
			}
		}
		// If it is not our turn, wait to receive items
		if(curTurn != myColor){
			while(curTurn != myColor){
				printf("Not my turn!\n");
				receiveTurnData(sock_desc, receive_buffer, &redPieces, &whitePieces, &redConcede, &whiteConcede, &curTurn);
				// If still not my turn print the board so I can see what is happening
				if(whiteConcede || redConcede){
					endGame(redPieces, whitePieces, redConcede, whiteConcede);
				}
				if(curTurn != myColor){
					printMyBoard(myColor);
				}
			}
		}
	}
	endGame(redPieces, whitePieces, redConcede, whiteConcede);
	return 0;
}

void endGame(int redPieces, int whitePieces, int redConcede, int whiteConcede){
	printMyBoard(myColor);
	if(redPieces < 1){
		printf("Red has no pieces, white wins!\n");
	} else if(whitePieces < 1){
		printf("White has no pieces, red wins!\n");
	} else if(redConcede){
		printf("Red conceded, white wins!\n");
	} else if(whiteConcede){
		printf("White conceded, red wins!\n");
	}
	exit(0);
}

// Type will either be 1 or 2, 1 represents entering source coord, 2 represents dest
int getCoordInput(char *userBuf, int type){
	while((userBuf[0] == 'h') || (userBuf[0] == 'p')|| (userBuf[0] == 'c')){
		int inFD = fileno(stdin);
		tcflush(inFD, TCIFLUSH);
		if(type == 1){
			printf("Enter coordinate of piece to move, type h for help, type p to print the board: ");
		} else {
			printf("Enter the destination coordinate of piece to move, type h for help, type p to print the board: ");
		}
		// Check to see if a user tries to enter a Ctrl + D and recover properly
		char *readStatus = fgets(userBuf, 3, stdin);
		if(!readStatus){
			clearerr(stdin);
		}
		printf("\n");
		if(tolower(userBuf[0]) == 'h'){
			help();
		} else if(tolower(userBuf[0]) == 'p'){
			printMyBoard(myColor);
			printf("Press ENTER to continue\n");
		} else if(tolower(userBuf[0]) == 'c'){
			if(concede()){
				return 0;
			}
		}
		while(getchar() != '\n');
	}
	return 1;
}

// Make sure that users don't panic Ctrl + C.
void sig_handler(int sigNum){
	if(sigNum){
		return;
	}
}

