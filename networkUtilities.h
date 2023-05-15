#ifndef UTILITIES_H
#define UTILITIES_H

#include "checkBoard.h"
#include <arpa/inet.h>

#define RECEIVE_BUFFER_SIZE 200
#define MESSAGE_BUFFER_SIZE 200
#define ACKNOWLEDGE_MESSAGE "ACKNOWLEDGE_MESSAGE"
#define SEND_MESSAGE_AFFIRMATION  "y"
#define STATUS_MESSAGE_SUCCESS "STATUS_MESSAGE_SUCCESS"
#define STATUS_MESSAGE_FAIL "STATUS_MESSAGE_FAIL"
#define SO_REUSEPORT_OPTION_VALUE 1
#define HOST_IP_ADDRESS "0.0.0.0"
#define CONNECT_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 1234

//TCP
int create_server_socket(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, in_port_t port, int type);
int connect_server(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, in_port_t port, int type);
int receive(int fd, char * receive_buffer, int buffer_size);
int send_board(int fd, char * receive_buffer);
int receive_board(int fd, char * receive_buffer);
int send_message(int fd, char * message);
int sendAck(int fd);
int receiveAck(int fd);
int receiveTurnData(int fd, char * receive_buffer, int *redPieces, int *whitePieces, int *redConcede, 
int *whiteConcede, char *curTurn);
int sendTurnData(int fd, char * receive_buffer,int *redPieces, int *whitePieces, int *redConcede, 
int *whiteConcede, char *curTurn);
int sendCurTurn(int fd, char *curTurn);
int receiveCurTurn(int fd, char *curTurn);

//TCP
void init_sock_addr_in(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, in_port_t port);
void read_user_input(char* prompt, char* read_buffer, int read_buffer_size);

int createServerAndWait(void);
int connectToServer(void);
#endif
