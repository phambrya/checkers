#include "networkUtilities.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int receivePieces(int fd, int *redPieces, int *whitePieces);
int sendPieces(int fd, int *redPieces, int *whitePieces);
int receiveConcedeStatus(int fd, int *redConcede, int *whiteConcede);
int sendConcedeStatus(int fd, int *redConcede, int *whiteConcede);

int create_server_socket(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, in_port_t port, int type)
{
  int fd = socket(sa_family, type, 0);
  int optval = SO_REUSEPORT_OPTION_VALUE;
  setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));

  init_sock_addr_in(sock_addr, sa_family, ip_addr, port);

  int result = bind(fd, (struct sockaddr *)sock_addr, sizeof(struct sockaddr_in));
  if(result == -1){
    printf("Error on bind, returning\n");
    return -1;
  }
  result = listen(fd, 10);

  return fd;
}

int connect_server(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, in_port_t port, int type){
  int fd = socket(sa_family, type, 0);
  init_sock_addr_in(sock_addr, sa_family, ip_addr, port);
  int result = connect(fd, (struct sockaddr *)sock_addr, sizeof(struct sockaddr_in));
  if(result == -1){
    printf("Error on connect, returning\n");
    return -1;
  }
  return fd;
}

int receive(int fd, char * receive_buffer, int buffer_size){
  memset(receive_buffer, 0, buffer_size);
  read(fd, receive_buffer, buffer_size);
  return 0;
}

int send_board(int fd, char *receive_buffer){
  memset(receive_buffer, 0, RECEIVE_BUFFER_SIZE);
  int bufIndex = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      receive_buffer[bufIndex] = board[i][j];
      bufIndex++;
    }
  }
  write(fd, receive_buffer, RECEIVE_BUFFER_SIZE);
  return 0;
}

int receive_board(int fd, char * receive_buffer){
  memset(receive_buffer, 0, RECEIVE_BUFFER_SIZE);
  read(fd, receive_buffer, RECEIVE_BUFFER_SIZE);
  int bufIndex = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      board[i][j] = receive_buffer[bufIndex];
      bufIndex++;
    }
  }
  return 0;
}

int send_message(int fd, char * message){
  write(fd, message, strlen(message));
  return 0;
}

int sendAck(int fd){
  write(fd, ACKNOWLEDGE_MESSAGE, strlen(ACKNOWLEDGE_MESSAGE));
  return 0;
}

int receiveAck(int fd){
  char myBuf[RECEIVE_BUFFER_SIZE];
  read(fd, myBuf, RECEIVE_BUFFER_SIZE);
  return 0;
}

/* Helper functions for sending and receiving turn data */
int receiveTurnData(int fd, char * receive_buffer, int *redPieces, int *whitePieces, int *redConcede, 
int *whiteConcede, char *curTurn){
  receive_board(fd, receive_buffer);
  sendAck(fd);
  receivePieces(fd, redPieces, whitePieces);
  sendAck(fd);
  receiveConcedeStatus(fd, redConcede, whiteConcede);
  sendAck(fd);
  receiveCurTurn(fd, curTurn);
  sendAck(fd);
  return 0;
}

int sendTurnData(int fd, char * receive_buffer, int *redPieces, int *whitePieces, int *redConcede, 
int *whiteConcede, char *curTurn){
  send_board(fd, receive_buffer);
  receiveAck(fd);
  sendPieces(fd, redPieces, whitePieces);
  receiveAck(fd);
  sendConcedeStatus(fd, redConcede, whiteConcede);
  receiveAck(fd);
  sendCurTurn(fd, curTurn);
  receiveAck(fd);
  return 0;
}

int receivePieces(int fd, int *redPieces, int *whitePieces){
  read(fd, redPieces, sizeof(int));
  sendAck(fd);
  read(fd, whitePieces, sizeof(int));
  sendAck(fd);
  return 0;
}

int sendPieces(int fd, int *redPieces, int *whitePieces){
  write(fd, redPieces, sizeof(int));
  receiveAck(fd);
  write(fd, whitePieces, sizeof(int));
  receiveAck(fd);
  return 0;
}

int receiveConcedeStatus(int fd, int *redConcede, int *whiteConcede){
  read(fd, redConcede, sizeof(int));
  sendAck(fd);
  read(fd, whiteConcede, sizeof(int));
  sendAck(fd);
  return 0;
}

int sendConcedeStatus(int fd, int *redConcede, int *whiteConcede){
  write(fd, redConcede, sizeof(int));
  receiveAck(fd);
  write(fd, whiteConcede, sizeof(int));
  receiveAck(fd);
  return 0;
}

int sendCurTurn(int fd, char *curTurn){
	write(fd, curTurn, sizeof(char));
	receiveAck(fd);
	return 0;
}

int receiveCurTurn(int fd, char *curTurn){
	read(fd, curTurn, sizeof(char));
	sendAck(fd);
	return 0;
}

void init_sock_addr_in(struct sockaddr_in* sock_addr, sa_family_t sa_family, const char * ip_addr, 
in_port_t port){
  memset(sock_addr, 0, sizeof(struct sockaddr_in));
  sock_addr->sin_family = sa_family;
  sock_addr->sin_addr.s_addr = inet_addr(ip_addr);
  sock_addr->sin_port = htons(port);
}

/* Handle creating a server and waiting for client to connect, or connect to preexisting server */
int createServerAndWait(void){
  struct sockaddr_in server_endpoint;
  struct sockaddr_in client_endpoint;
  int client_sock_desc = -1;
  int server_sock_desc = create_server_socket(&server_endpoint, AF_INET, HOST_IP_ADDRESS, 
  SERVER_PORT,SOCK_STREAM);
  printf("Listening on Network Interface: %s Network Port: %d \n", HOST_IP_ADDRESS, SERVER_PORT);
  int connected = 0;
  while(!connected){
    printf("Waiting for client connection...\n");
    socklen_t sock_len = sizeof(struct sockaddr_in);
    client_sock_desc = accept(server_sock_desc, (struct sockaddr *)&client_endpoint, &sock_len);
    char *client_ip = inet_ntoa(client_endpoint.sin_addr);
    printf("Accepted connection: %sL%d\n", client_ip, ntohs(client_endpoint.sin_port));
    connected = 1;
  }
  return client_sock_desc;
}

int connectToServer(void){
  struct sockaddr_in server_endpoint;
  int sock_desc = connect_server(&server_endpoint, AF_INET, HOST_IP_ADDRESS, SERVER_PORT, 
  SOCK_STREAM);
  return sock_desc;
}
