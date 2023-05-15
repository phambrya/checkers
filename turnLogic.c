#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "globals.h"
#include "checkBoard.h"

void help(void);
int concede(void);
int canTake(char curTurn);
int redTake(void);
int calcRedRegTake(int i, int j);
int calcRedKingTake(int i, int j);
int whiteTake(void);
int calcWhiteRegTake(int i, int j);
int calcWhiteKingTake(int i, int j);
int validateMove(char fromCoord[], char toCoord[], int ableTake, char myColor, int *redPieces,
int *whitePieces);
int processTake(int ix, int iy, int jx, int jy, int *redPieces, int *whitePieces);
int takeWithReg(int ix, int iy, int jx, int jy, int takeColor);
int takeWithKing(int ix, int iy, int jx, int jy, int takeColor);
int processJump(int ix, int iy, int jx, int jy, int takeColor);
int checkColor(int ix, int iy, int takeColor);
int processMove(int ix, int iy, int jx, int jy, char myColor);
int processRegMove(int ix, int jx, int myColor);
int doMove(int ix, int iy, int jx, int jy);
void turnToKing(int ix, int iy);

/* Group of functions that calculate if a piece can be taken */

void help(void){
  printf("If you cannot move any pieces, you must forfeit!\n");
  printf("If you are able to take a piece, you must.\n");
  printf("In checkers you can move regular pieces only diagonally forward.\n");
  printf("When you get your piece to the other side it is kinged and can move all directions diagonally.\n");
  printf("One twist on normal checkers though is that you can take more than two pieces a turn.\n");
  printf("When you are asked to enter a coordinate, enter as row-column pairs without spaces.\n");
  printf("For example, if I want to move my piece at row 1, column 5, I would enter it as: 15\n");
  printf("Additionally, if you would like to concede, press c on request for input\n");
  printf("Note that Ctrl + C is disabled. Upon mis-input, enter invalid second coordinate to reset inputs.\n");
  printf("If you truly desire to close the program, either concede or enter Ctrl + \\.\n");
  printf("Press ENTER to continue\n\n");
}

int concede(void){
  printf("Are you sure you want to concede? (y/n) ");
  char result = tolower(getchar());
  printf("\n");
  if(result == 'y'){
    return 1;
  } else {
    return 0;
  }
}

/* Functions to calculate if a player can take a piece */
int canTake(char curTurn){
  int take = 0;
  if(curTurn == 'r'){
    take = redTake();
  } else {
    take = whiteTake();
  }
  return take;
}

int redTake(void){
  // look for a red piece
  int canTake = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(board[i][j] == red){
        // we have found a red piece, need to calc if it can take
        canTake = calcRedRegTake(i, j);
        if(canTake){
          return canTake;
        }
      } else if (board[i][j] == redKing){
        canTake = calcRedKingTake(i, j);
        if(canTake){
          return canTake;
        }
      }
    }
  }
  return canTake;
}

int calcRedRegTake(int i, int j){
  // Must move to higher oreder i values
  // Therefore if i > 5, cannot take with given piece
  if(i < 6){
    // check if we will not walk out of bounds and if piece diagonal ahead in either direction is white and we will be landing on empty
    if((j < 6) && (((board[i+1][j+1] == white) || (board[i+1][j+1] == whiteKing)) && (board[i+2][j+2] == emptyChar))){
      return 1;
    }
    if((j > 1) && (((board[i+1][j-1] == white) || (board[i+1][j-1] == whiteKing)) && (board[i+2][j-2] == emptyChar))){
      return 1;
    } 
  }
  return 0;  
}

// Check in each diagonal direction if there is a white piece that then has an empty space "behind" it
int calcRedKingTake(int i, int j){
  // Check ahead squares 
  if(i < 6){
    // check left ahead
    if((j < 6) && (((board[i+1][j+1] == white) || (board[i+1][j+1] == whiteKing)) && (board[i+2][j+2] == emptyChar))){
      return 1;
    }
    // check right ahead
    if((j > 1) && (((board[i+1][j-1] == white) || (board[i+1][j-1] == whiteKing)) && (board[i+2][j-2] == emptyChar))){
      return 1;
    } 
  }
  // Check behind squares
  if(i > 1){
    // check left behind
    if((j < 6) && (((board[i-1][j+1] == white) || (board[i-1][j+1] == whiteKing)) && (board[i-2][j+2] == emptyChar))){
      return 1;
    }
    // check right behind
    if((j > 1) && (((board[i-1][j-1] == white) || (board[i-1][j-1] == whiteKing)) && (board[i-2][j-2] == emptyChar))){
      return 1;
    }
  }
  return 0;
}

int whiteTake(void){
  int canTake = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(board[i][j] == white){
        canTake = calcWhiteRegTake(i, j);
        if(canTake){
          return canTake;
        }
      } else if(board[i][j] == whiteKing){
        canTake = calcWhiteKingTake(i, j);
        if(canTake){
          return canTake;
        }
      }
    }
  }
  return canTake;
}

int calcWhiteRegTake(int i, int j){ 
  // Must move to higher oreder i values
  // Therefore if i > 5, cannot take with given piece
  if(i > 1){
    // check left ahead
    if((j > 1) && (((board[i-1][j-1] == red) || (board[i-1][j-1] == redKing)) && (board[i-2][j-2] == emptyChar))){
      return 1;
    }
    // check right ahead
    if((j < 6) && (((board[i-1][j+1] == red) || (board[i-1][j+1] == redKing)) && (board[i-2][j+2] == emptyChar))){
      return 1;
    } 
  }
  return 0;  
}

int calcWhiteKingTake(int i, int j){ 
  printf("Fell into calc white king take\n");
  printf("i: %d. j: %d\n", i, j);
  // Check behind squares 
  if(i < 6){
    // check left 
    if((j < 6) && (((board[i+1][j+1] == red) || (board[i+1][j+1] == redKing)) && (board[i+2][j+2] == emptyChar))){
      return 1;
    }
    // check right
    if((j > 1) && (((board[i+1][j-1] == red) || (board[i+1][j-1] == redKing)) && (board[i+2][j-2] == emptyChar))){
      return 1;
    } 
  }
  // Check ahead squares
  if(i > 1){
    // check left
    if((j < 6) && (((board[i-1][j+1] == red) || (board[i-1][j+1] == redKing)) && (board[i-2][j+2] == emptyChar))){
      return 1;
    }
    // check right
    printf("%c\n", board[i-1][j-1]);
    if((j > 1) && (board[i-1][j-1] == red)){
	printf("valid j and red piece in direction i-1 j-1\n");
    }
    if((j > 1) && (((board[i-1][j-1] == red) || (board[i-1][j-1] == redKing)) && (board[i-2][j-2] == emptyChar))){
      return 1;
    }
  }
  return 0;
}

/* Move taking logic and some input validation */
int validateMove(char fromCoord[], char toCoord[], int ableTake, char myColor, int *redPieces,
  int *whitePieces){
  printf("From coord is %s\n", fromCoord);
  printf("To coord is %s\n", toCoord);
  printf("Take status is %d\n", ableTake);
  int status = 0;
  int ix = fromCoord[0] - 48;
  int iy = fromCoord[1] - 48;
  int jx = toCoord[0] - 48;
  int jy = toCoord[1] - 48;
  if((ix < 0) || (ix > 7) || (iy < 0) || (iy > 7) || (jx < 0) || (jx > 7) || (jy < 0) || (jy > 7)){
    printf("Invalid coordinates entered, please try again.\n");
    return 0;
  }
  // Make sure the player is trying to move rows/columns
  if((ix == jx) || (iy == jy)){
    printf("Invalid move. Trying to move vertically or horizontally. Please try again\n");
    return status;
  }
  // Need to see if we must take and if attempted move is a take
  if((abs(ix - jx) == 2) && (abs(iy - jy) == 2)){
    // Need to validate that a take is possible
    if(!ableTake){
      printf("Attempting to take when a take is not possible. Please try again.\n");
      return status;
    }
    // process coordinates for a take
    status = processTake(ix, iy, jx, jy, redPieces, whitePieces);
    if(!status){
      printf("Invalid input for a take.\n");
    }
  } else if((abs(ix - jx) == 1) && (abs(iy - jy) == 1)){
    // Need to validate that a take is not possible
    if(ableTake){
      printf("Attempting a move when a take is possible. Please try again\n");
      return status;
    }
    // process coordinates for a move
    status = processMove(ix, iy, jx, jy, myColor);
    if(!status){
      printf("Invalid input for a move.\n");
    }
  } else {
    printf("Coordinate too distant for a legal move. Please try again\n");
    return status;
  }
  // If status, we did a move or take, see if we can turn piece into king
  if(status){
    turnToKing(jx, jy);
  }
  return status;
}

// Interface to process the input for a player trying to take a piece
int processTake(int ix, int iy, int jx, int jy, int *redPieces, int *whitePieces){
  int takeColor;
  int status = 0;
  if((board[ix][iy] == red) || (board[ix][iy] == redKing)){
    takeColor = takeWhite;
  } else if ((board[ix][iy] == white) || (board[ix][iy] == whiteKing)){
    takeColor = takeRed;
  } else {
    printf("CRITICAL ERROR\n");
    exit(-1);
  }
  if((board[ix][iy] == red) || (board[ix][iy] == white)){
    status = takeWithReg(ix, iy, jx, jy, takeColor);
  } else {
    status = takeWithKing(ix, iy, jx, jy, takeColor);
  }
  if(status){
    // If take successful, decrement appropriate pieces
    if(takeColor == takeRed){
      *redPieces -= 1;
    } else {
      *whitePieces -= 1;
    }
  }
  return status;
}

// Input validation for a regular piece take
int takeWithReg(int ix, int iy, int jx, int jy, int takeColor){
  int status = 0;
  // Need to validate we are moving in the right direction
  if(takeColor == takeRed){
    // If we are red taking white, moving from low rows to high rows
    if(ix - jx < 0){
      printf("Trying to move a regular piece backwards!\n");
      return status;
    }
  } else {
    // If we are white taking red, moving from high rows to low rows
    if(ix - jx > 0){
      printf("Trying to move a regular piece backwards!\n");
      return status;
    }
  }
  status = processJump(ix, iy, jx, jy, takeColor);
  return status;
}

int takeWithKing(int ix, int iy, int jx, int jy, int takeColor){
  // Taking with a king is simpler because we don't need to pre-validate the directions 
  return processJump(ix, iy, jx, jy, takeColor);
}

// Do the actual taking of a piece and updating the board
//TODO refactor this to remove some of the nested if statements
int processJump(int ix, int iy, int jx, int jy, int takeColor){
  int status = 0;
  // Need to check if we're going left or right
  if(iy - jy > 0){
     // We are going from high to low in the row
     // Need to check if we're going up or down in the column
     if(ix - jx > 0){
       // high low row, high low col 
       if(board[ix-1][iy-1] != emptyChar){
         status = checkColor(ix-1, iy-1, takeColor); 
         if((board[jx][jy] == emptyChar) && status){
           // We are clear, do our jump
           board[ix-1][iy-1] = emptyChar;
           board[jx][jy] = board[ix][iy];
           board[ix][iy] = emptyChar;
           status = 1;
         } else {
           printf("Can't take!\n");
           status = 0;
         }
       }
     } else {
       // Going low to high in the column
       // high low row, low high col
       if(board[ix+1][iy-1] != emptyChar){
         status = checkColor(ix+1, iy-1, takeColor);
         if((board[jx][jy] == emptyChar) && status){
           board[ix+1][iy-1] = emptyChar;
           board[jx][jy] = board[ix][iy];
           board[ix][iy] = emptyChar;
           status = 1;
         } else {
           printf("Can't take!\n");
           status = 0;
         }
       }
     }
  } else {
    // We are going low to high in the row
    // Need to check if we're going up or down in the column
    if(ix - jx > 0){
      // Going high to low in the column
      // low high row, high low col
      if(board[ix-1][iy+1] != emptyChar){
        status = checkColor(ix-1, iy+1, takeColor);
        if((board[jx][jy] == emptyChar) && status){
          board[ix-1][iy+1] = emptyChar;
          board[jx][jy] = board[ix][iy];
          board[ix][iy] = emptyChar;
          status = 1;
        } else {
          printf("Can't take!\n");
          status = 0;
        }
      }
    } else {
      // Going low to high in the column
      // low high row, low high col
      if(board[ix+1][iy+1] != emptyChar){
        status = checkColor(ix+1, iy+1, takeColor);
        if((board[jx][jy] == emptyChar) && status){
          board[ix+1][iy+1] = emptyChar;
          board[jx][jy] = board[ix][iy];
          board[ix][iy] = emptyChar;
          status = 1;
        } else {
          printf("Can't take!\n");
          status = 0;
        }
      }
    }
  } 
  return status;
}

// Check the color of a piece to validate that it is of the appropriate color to be jumped
int checkColor(int ix, int iy, int takeColor){
  if(takeColor == takeRed){
    if((board[ix][iy] == red) || (board[ix][iy] == redKing)){
      return 1;
    } else {
      printf("Trying to take your own piece!\n");
      return 0;
    }
  } else {
    if((board[ix][iy] == white) || (board[ix][iy] == whiteKing)){
      return 1;
    } else {
      printf("Trying to take your own piece!\n");
      return 0;
    }
  }
}

/* Interface to process moving a piece, also does some input validation */
int processMove(int ix, int iy, int jx, int jy, char myColor){
  int status = 0;
  // Check to see if there is a piece at the coordinate
  if(board[ix][iy] == emptyChar){
    printf("Trying to move a piece that doesn't exist!\n");
    return status;
  }
  // Check if source coordinate has a piece of the proper color
  if(myColor == 'r'){
    if((board[ix][iy] == white) || (board[ix][iy] == whiteKing)){
      printf("Trying to move a piece that is not yours!\n");
      return status;
    }
  } else {
    if((board[ix][iy] == red) || (board[ix][iy] == redKing)){
      printf("Trying to move a piece that is not yours!\n");
      return status;
    }
  }
  // Need to tell if piece that is moving is regular or king
  if((board[ix][iy] == red) || (board[ix][iy] == white)){
    status = processRegMove(ix, jx, myColor);
    if(status){
      status = doMove(ix, iy, jx, jy);
    }
  } else {
    // If we are a king, don't need to validate direction
    status = doMove(ix, iy, jx, jy);
  }
  return status;
}

// Further input validation for moving a regular piece
int processRegMove(int ix, int jx, int myColor){
  // Need to make sure that the piece is moving in the proper direction
  if(myColor == 'r'){
    // If we are red, moving from low rows to high rows
    if(ix - jx > 0){
      printf("Trying to move a regular piece backwards!\n");
      return 0;
    }
  } else {
    // If we are white, moving from high rows to low rows
    if(ix - jx < 0){
      printf("Trying to move a regular piece backwards!\n");
      return 0;
    }
  }
  return 1;
}

// Do the actual move and update the board
int doMove(int ix, int iy, int jx, int jy){
  if(board[jx][jy] == emptyChar){
    board[jx][jy] = board[ix][iy];
    board[ix][iy] = emptyChar;
    return 1;
  } else {
    printf("Trying to move to a space with a piece on it!\n");
    return 0;
  }
}

// If a piece has made it to it's opposite side, update its value of not a king
void turnToKing(int ix, int iy){
  //Check to see if we are at either end of the board
  if(ix == 0){
    if(board[ix][iy] == white){
      board[ix][iy] = whiteKing;
    }
  } else if (ix == 7){
    if(board[ix][iy] == red){
      board[ix][iy] = redKing;
    }
  }
}
