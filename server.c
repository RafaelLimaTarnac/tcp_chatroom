#include"user.h"
#include"room.h"
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<poll.h>

#define PORT "1234"
#define BACKLOG 50

#define GETERR(fdb, cmp, msg)				\
  if(fdb == cmp){					\
    fprintf(stderr, strcat(msg, strerror(errno)));	\
    exit(1);						\
  }

#define HELPCMP(fdb) (strcmp(fdb, "-h") == 0 || strcmp(fdb, "--help") == 0)

struct user *users;
struct room *rooms;

// pollfd
void add_poll(struct pollfd **polls, int *len, int *index, int fd);
void rmv_poll(struct pollfd **polls, int *len, int *index, int fd);

// network / utils
int create_host();
void send_message(struct user *sender, char *buff);
void send_message_user(struct user *receiver, char *buff);
void user_commands(char *buff, struct user *user);
void set_room_user(struct user *user, struct room *room);

// ------------------------------------------------------------------------------------------
// ------------------------------------ POLL FUNCTIONS --------------------------------------
// ------------------------------------------------------------------------------------------

void add_poll(struct pollfd **polls, int *len, int *index, int fd){
  if(*len >= *index){
    (*len) *= 2;

    (*polls) = realloc((*polls), sizeof(struct pollfd) * (*len));
    if((*polls)==NULL){
      fprintf(stderr, "Error on realloc() of pollfd\n");
      exit(1);
    }
  }
  
  (*polls)[*index].fd = fd;
  (*polls)[*index].events = POLLIN;

  (*index) += 1;
}

void rmv_poll(struct pollfd **polls, int *len, int *index, int fd){
  for(int i = 0; i < (*index) ; i++){
    if((*polls)[i].fd == fd){
      (*polls)[i] = (*polls)[*index];
      close(fd);
      return;
    }
  }

  fprintf(stderr, "Couldn't find in poll fd:%d\n", fd);
}

// ------------------------------------------------------------------------------------------
// ------------------------------- NETWORK / UTILS FUNCTIONS --------------------------------
// ------------------------------------------------------------------------------------------

int create_host(){
  int sockfd, newfd, status;
  struct addrinfo hints, *server;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  hints.ai_flags = AI_PASSIVE;
  
  status = getaddrinfo(NULL, PORT, &hints, &server);
  GETERR(status, -1, "Error on getaddrinfo()\n");
  
  sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  GETERR(sockfd, -1, "Error on socket()\n");

  int yes = 1;
  status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  GETERR(status, -1, "Error on setsockopt()\n");
  
  status = bind(sockfd, server->ai_addr, server->ai_addrlen);
  GETERR(status, -1, "Error on bind()\n");

  status = listen(sockfd, BACKLOG);
  GETERR(status, -1, "Error on listen()\n");
  
  return sockfd;
}

void send_message(struct user *sender, char *buff){
  int status;
  int buff_size = strlen(buff);
  
  for(struct user *current = users; current != NULL; current = current->next){
    if(sender->room_id != current->room_id || sender->id == current->id)
      continue;

    status = send(current->id, buff, buff_size, 0);
  }
}

void send_message_user(struct user *receiver, char *buff){
  int status;
  int buff_size = strlen(buff);

  status = send(receiver->id, buff, buff_size, 0);
}

// i expect to receive a string with '/' as first character
void user_commands(char *buff, struct user *user){
  char *argv[4] = {NULL, NULL, NULL, NULL};
  char cpy[156];
  char *token;
  if(strlen(buff) >= 156){
    send_message_user(user, "buffer exceeds 155 characters\n");
    return;
  }
  
  strcpy(cpy, buff);
  token = strtok(cpy, " \n");
  
  for(int i = 0; i < 4 && token; i++){
    argv[i] = token;
    token = strtok(NULL, " \n");
  }
  
  if(strcmp(argv[0], "/rooms") == 0){
    // -t (by theme)
    if(argv[1] == NULL){
      char *msg = room_list_str(rooms);
      send_message_user(user, msg);
      free(msg);
    }
    else if(HELPCMP(argv[1])){
      send_message_user(user, "usage: /rooms (-t theme)\n");
    }
    else if(strcmp(argv[1], "-t") == 0 && argv[2] != NULL){
      char *msg = troom_list_str(rooms, argv[2]);
      send_message_user(user, msg);
      free(msg);
    }
  }
  else if(strcmp(argv[0], "/change-room") == 0){
    //set_room_user(user, argument);
    if(argv[1] == NULL || HELPCMP(argv[1])){
      send_message_user(user, "/change-room room-id");
    }
    else{
      int num = strtol(argv[1], NULL, 10);
      struct room *new_room = get_room(rooms, num);
      if(new_room != NULL){
	set_room_user(user, new_room);
	send_message_user(user, "\tCHANGED ROOM\n");
      }
      else{
	send_message_user(user, "\tRoom either non-existent or full\n");
      }
    }
    
    rmv_empty_rooms(rooms);
  }
  else if(strcmp(argv[0], "/change-name") == 0){
    //set_name_user(user, argument);

    if(argv[1] == NULL || HELPCMP(argv[1])){
      send_message_user(user, "usage: /change-name new-name\n");
    }
    else{
      set_name_user(user, argv[1]);
      send_message_user(user, "\tCHANGED NAME\n");
    }
  }
  else if(strcmp(argv[0], "/users") == 0){
    // (default lists all users inside current room) -a (all) -i (room id)
    
    if(argv[1] == NULL){
      char *msg = iuser_list_str(users, user->room_id);
      send_message_user(user, msg);      
    }
    else if(HELPCMP(argv[1])){
      send_message_user(user, "/users (shows all users in current room by default)\n\t-a (shows all users)\n\t-i room_id\n");
      
    }
    else if(strcmp(argv[1], "-a") == 0){
      char *msg = user_list_str(users);
      send_message_user(user, msg);
      
    }
    else if(strcmp(argv[1], "-i") == 0 && argv[2] != NULL){
      // strtol
      int room_id = strtol(argv[2], NULL, 10);
      char *msg = iuser_list_str(users, room_id);
      send_message_user(user, msg);
      
    }
  }
  else if(strcmp(argv[0], "/create-room") == 0){    
    if(argv[1] == NULL || argv[2] == NULL || HELPCMP(argv[1])){
      send_message_user(user, "usage: /create-room name theme\n");
    }
    else{
      struct room *new_room = add_room(&rooms, argv[1], argv[2]);
      set_room_user(user, new_room);
      rmv_empty_rooms(rooms);
      char *msg = "new room created successfully!\n";
      send(user->id, msg, strlen(msg)+1, 0);
    }
  }
  else{
    send_message_user(user, "Commands:\n\t/rooms\n\t/users\n\t/change-room\n\t/change-name\n\t/create-room\n");
  }

  memset(buff, 0, sizeof(buff));
}

void set_room_user(struct user *user, struct room *room){
  struct room *old_room = get_room(rooms, user->room_id);
  if(old_room != NULL)
    old_room->current_users -= 1;

  user->room_id = room->id;
  room->current_users += 1;
}

// ------------------------------------------------------------------------------------------
// ----------------------------------------- MAIN -------------------------------------------
// ------------------------------------------------------------------------------------------

int main(void){
  int p_len = 5, p_index = 0;
  struct pollfd *pfds = malloc(sizeof(struct pollfd) * p_len);

  int sockfd, newfd, status, user_size, nbytes;
  struct sockaddr new_user;
  char buff[2048];
  
  sockfd = create_host();
  add_poll(&pfds, &p_len, &p_index, sockfd);
  printf("Server is now open!\n");
  
  struct room *public_room = add_room(&rooms, "Public", "All");
  set_room_limit(public_room, 9999);
  
  while(1){
    poll(pfds, p_index + 1, -1);
    
    for(int i = 0; i < p_index; i++){
      if(pfds[i].revents && POLLIN){

	if(pfds[i].fd == sockfd){ // SERVER SOCKET
	  user_size = sizeof(new_user);
	  status = accept(sockfd, &new_user, &user_size);
	  GETERR(status, -1, "Error on accept()\n");

	  add_poll(&pfds, &p_len, &p_index, status);
	  struct user *new_user = add_user(&users, "Midget", status);
	  set_room_user(new_user, public_room);
	  
	  printf("new connection, fd: %d\n", status);
	  break;
	}
	else{ // USER SOCKET
	  nbytes = recv(pfds[i].fd, buff, sizeof(buff), 0);
	  printf("(%d bytes) data received\n", nbytes);
	  struct user *sender = get_user(users, pfds[i].fd);
	  GETERR(nbytes, -1, "Error on recv()\n");

	  if(nbytes == 0){ // user disconected
	    printf("User with fd:%d left\n", sender->id);
	    
	    struct room *sender_room = get_room(rooms, sender->room_id);
	    set_room_users(sender_room, sender_room->current_users - 1);
	    rmv_empty_rooms(rooms);
	    
	    char msg[80];
	    strcpy(msg, sender->name);
	    strcat(msg, " disconnected from chat\n");
	    send_message(sender, msg);
	    
	    status = rmv_user(&users, pfds[i].fd);
	    GETERR(status, -1, "Error on rmv_user()\n");	    
	    rmv_poll(&pfds, &p_len, &p_index, pfds[i].fd);

	    break;
	  }
	  else if(buff[0] == '/'){
	    user_commands(buff, sender);
	  }
	  else{ // message received
	    int size_msg = (sizeof(char) * strlen(buff)) + (sizeof(char) * strlen(sender->name)) + 3;
	    char *msg = malloc(size_msg);
	    memset(msg, 0, size_msg);
	    
	    strcat(msg, sender->name);
	    strcat(msg, ": ");
	    strcat(msg, buff);
	    
	    send_message(sender, msg);
	    free(msg);
	  }
	}
      }
    }
  }
  
  close(sockfd);
  return 0;
}
