#include"user.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct user *add_user(struct user **list, const char *name, int id){
  struct user *new_user = malloc(sizeof(struct user));

  
  
  new_user->id = id;
  new_user->room_id = -1;
  strcpy(new_user->name, name);
  
  new_user->next = (*list);
  (*list) = new_user;

  return new_user;
}

int rmv_user(struct user **list, int id){
  struct user *prev = NULL;
  for(struct user *current = (*list); current != NULL; current = current->next){
    if(current->id == id){
      if(prev != NULL)
	prev->next = current->next;
      else
	(*list) = current->next;

      free(current);

      /*
       pollfd already closes my file descriptor
       if you want to close it via user, you can
       , just uncomment close(id).
       
       be sure to change pollfd accordingly tho
      */
      
      //close(id);
      memset(current, 0, sizeof(struct user));
      return 0;
    }
    prev = current;
  }
  
  fprintf(stderr, "Couldn't find user with id:%d\n", id);
  return -1;
}

struct user *get_user(struct user *list, int id){
  for(struct user *current = list; current != NULL; current = current->next){
    if(current->id == id)
      return current;
  }

  fprintf(stderr, "Couldn't find user with id:%d\n", id);
  return NULL;
}

void set_name_user(struct user *user, char *name){
  if(strlen(name) >= USIZENAME){
    name[USIZENAME - 1] = '\0';
  }
  
  strcpy(user->name, name);
}

char *user_list_str(struct user *list){
  char *text = malloc(sizeof(char));
  text[0] = '\0';
  char msg[30];
  for(struct user *current = list; current != NULL; current = current->next){
    sprintf(msg, "id:%d\tname:%s\troom_id:%d\n", current->id, current->name, current->room_id);

    text = realloc(text , (sizeof(char) * (strlen(text) + 1)) + (sizeof(char) * (strlen(msg) + 1)));
    strcat(text, msg);    
  }  
  return text;
}

char *iuser_list_str(struct user *list, int room_id){
    char *text = malloc(sizeof(char));
  text[0] = '\0';
  char msg[40];
  for(struct user *current = list; current != NULL; current = current->next){
    if(current->room_id != room_id)
      continue;
    
    sprintf(msg, "id:%d\tname:%s\n", current->id, current->name);

    text = realloc(text , (sizeof(char) * (strlen(text) + 1)) + (sizeof(char) * (strlen(msg) + 1)));
    strcat(text, msg);    
  }  
  return text;
}
