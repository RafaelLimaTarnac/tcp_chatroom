#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<sys/socket.h>
#include<poll.h>
#include<unistd.h>

#define PORT "1234"

#define GETERR(fdb, cmp, msg)				\
  if(fdb == cmp){					\
    fprintf(stderr, strcat(msg, strerror(errno)));	\
    exit(1);						\
  }

// ------------------------------------------------------------------------------------------
// ------------------------------- NETWORK / UTILS FUNCTIONS --------------------------------
// ------------------------------------------------------------------------------------------

int connect_server(const char *ip){
  int sockfd, status;
  struct addrinfo hints, *server;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;

  status = getaddrinfo(ip, PORT, &hints, &server);
  GETERR(status, -1, "Error on getaddrinfo()\n");
  
  sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  GETERR(sockfd, -1, "Erorr on socket()\n");
  
  status = connect(sockfd, server->ai_addr, server->ai_addrlen);
  GETERR(status, -1, "Error on connect()\n");
  
  freeaddrinfo(server);

  return sockfd;
}

// ------------------------------------------------------------------------------------------
// ----------------------------------------- MAIN -------------------------------------------
// ------------------------------------------------------------------------------------------

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("usage: ./client (connection ipv4)");
    exit(1);
  }

  struct pollfd pfds[2];
  pfds[0].fd = 0;
  pfds[0].events = POLLIN;
  
  int sockfd, nbytes;
  char buff[2048];

  sockfd = connect_server(argv[1]);
  pfds[1].fd = sockfd;
  pfds[1].events = POLLIN;
  
  while(1){
    poll(pfds, 2, -1);

    for(int i = 0; i < 2; i++){
      if(pfds[i].revents && POLLIN){
	if(pfds[i].fd == 0){ // stdin
	  memset(buff, 0, sizeof(buff));
	  nbytes = read(0, buff, sizeof(buff));
	  GETERR(nbytes, -1, "Error on read()\n");

	  nbytes = send(sockfd, buff, strlen(buff) + 1, 0);
	  GETERR(nbytes, -1, "Error on send()\n");
	}
	else{ // server
	  memset(buff, 0, sizeof(buff));
	  nbytes = recv(sockfd, buff, sizeof(buff), 0);
	  GETERR(nbytes, -1, "Error on recv()\n");

	  if(nbytes == 0){
	    printf("Server closed connection.\n");
	    close(sockfd);
	    exit(0);
	  }
	  else{
	    printf("%s", buff);
	  }
	}
      }
    }
  }

  return 0;
}
