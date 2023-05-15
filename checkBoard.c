#include <stdio.h>
#include "globals.h"

void printBoardForWhite(void);
void printBoardForRed(void);
void printCell(unsigned char val);
void printBound(void);
void printRowBound(void);
void colorRed(void);
void colorWhite(void);
void setBackground(int i, int j);
void resetBackground(void);
void createBoard(void);

unsigned char board[8][8];

/* Interface to print the board orientated towards the passed in color */
void printMyBoard(char myColor){
  printf("\n");
  if(myColor == 'r'){
    printBoardForRed();
  } else {
    printBoardForWhite();
  }
  printf("\n");
}

void printBoardForWhite(void){
  for(int i = 0; i < 8; i++){
    printBound();
    printRowBound();
    for(int j = 0; j < 8; j++){
      printCell(board[i][j]);
    }
    printf(" %d\n", i);
  }
  printBound();
  printf("  0   1   2   3   4   5   6   7\n");
}

void printBoardForRed(void){
  for(int i = 7; i > -1; i--){
    printBound();
    printRowBound();
    for(int j = 7; j > -1; j--){
      printCell(board[i][j]);
    }
    printf(" %d\n", i);
  }
  printBound();
  printf("  7   6   5   4   3   2   1   0 \n");
}

void printCell(unsigned char val){
  printf(" ");
  if((val == red) || (val == redKing)){
    colorRed();
  }
  if(val == emptyChar){
    printf("%c", val);
  }
   else if((val == redKing) || (val == whiteKing)){
    printf("X");
  } else {
    printf("0");
  }
  colorWhite();
  printf(" |");
}

void printBound(void){
  printf("|---|---|---|---|---|---|---|---|\n"); 
}

void printRowBound(void){
  printf("|");
}

void colorRed(void){
  printf("\033[0;31m");
}

void colorWhite(void){
  printf("\033[0;37m");
}

void createBoard(void){
  // Populate the board
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 8; j++){
      //isEven print on every even index
      if((i == 3) || (i == 4)){
        board[i][j] = emptyChar;
      }
       else if((i%2) == 0){
        if((j%2)==0){
          board[i][j] = red;
        } else {
          board[i][j] = emptyChar;
        }
      } else {
        // place in odd indexes
        if((j%2)==1){
          board[i][j] = red;
        } else {
          board[i][j] = emptyChar;
        }
      }
    }
  }
  for(int i = 5; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if((i%2)== 0){
        if((j%2) == 0){
        board[i][j] = white;
        } else {
          board[i][j] = emptyChar;
        }
      } else {
        if((j%2) == 1){
          board[i][j] = white;
        } else {
          board[i][j] = emptyChar;
        }
      }
    }
  }
}


void createTestBoard(void){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			board[i][j] = emptyChar;
		}
	}
	board[1][1] = red;
	board[2][2] = whiteKing;
}


