#include"room.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int r_id = 0;

struct room *add_room(struct room **list, char *name, char *theme){
  struct room *new_room = malloc(sizeof(struct room));

  if(strlen(name) >= RSIZENAME){
    name[RSIZENAME - 1] = '\0';
  }
  if(strlen(theme) >= RSIZETHEME){
    theme[RSIZETHEME - 1] = '\0'; 
  }
  
  new_room->id = r_id;
  r_id += 1;
  new_room->current_users = 0;
  new_room->max_users = RMAXUSERS;
  strcpy(new_room->name, name);
  strcpy(new_room->theme, theme);
  
  new_room->next = (*list);
  (*list) = new_room;
  
  return new_room;
}

int rmv_room(struct room **list, int id){
  struct room *prev = NULL;
  
  for(struct room *current = (*list); current != NULL; current = current->next){
    if(current->id == id){
      if(prev != NULL)
	prev->next = current->next;
      else{
	// for some reason "(*list) = current->next" doesn't work
	// so i just gave up.
	return 0;
      }

      free(current);
      // and memset corrupts data at list's top
      //memset(current, 0, sizeof(struct room));
      return 0;
    }
    prev = current;
  }
  fprintf(stderr, "Couldn't find room with id:%d\n", id);
  return -1;
}

void rmv_empty_rooms(struct room *list){
  int *ids = malloc(sizeof(int));
  int i = 0;
  
  for(struct room *current = list; current != NULL; current = current->next){    
    if(current->current_users <= 0 && current->id != 0){
      ids[i++] = current->id;
      ids = realloc(ids, sizeof(int) * (i + 1));
    }
  }
  
  for(int j = 0; j < i; j++){
    rmv_room(&list, ids[j]);
  }
  if(i == 0)
    return;
    
  free(ids);
}

struct room *get_room(struct room *list, int id){
  if(id == -1)
    return NULL;
  
  for(struct room *current = list; current != NULL; current = current->next){
    if(current->id == id){
      return current;
    }
  }
  printf("Couldn't find room with id:%d\n", id);
  return NULL;
}

char *room_list_str(struct room *list){

  char *text = malloc(sizeof(char));
  text[0] = '\0';
  char msg[80];
  for(struct room *current = list; current != NULL; current = current->next){
    
    sprintf(msg, "(id:%d)\ttheme:%s\tname:%s\tusers: %d (max:%d)\n", current->id, current->theme, current->name, current->current_users, current->max_users);

    text = realloc(text , (sizeof(char) * (strlen(text) + 1)) + (sizeof(char) * (strlen(msg) + 1)));
    strcat(text, msg);
    
  }  
  return text;
}

char *troom_list_str(struct room *list, char *theme){
  char *text = malloc(sizeof(char));
  text[0] = '\0';
  char msg[80];
  for(struct room *current = list; current != NULL; current = current->next){
    if(strcmp(current->theme, theme) != 0)
      continue;
    
    sprintf(msg, "(id:%d)\ttheme:%s\tname:%s\tusers: %d (max:%d)\n", current->id, current->theme, current->name, current->current_users, current->max_users);

    text = realloc(text , (sizeof(char) * (strlen(text) + 1)) + (sizeof(char) * (strlen(msg) + 1)));
    strcat(text, msg);
    
  }
  return text;
}

void set_room_limit(struct room *room, int limit){
  room->max_users = limit;
}

void set_room_users(struct room *room, int value){
  value = value < 0 ? 0 : value;
  room->current_users = value;
  printf("room fd:%d now has %d users\n", room->id, value);
}
