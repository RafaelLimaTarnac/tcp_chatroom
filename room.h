#pragma once

#define RSIZENAME 25
#define RSIZETHEME 12
#define RMAXUSERS 8

struct room{
  int id;
  char theme[RSIZETHEME];
  char name[RSIZENAME];
  int max_users;
  int current_users;
  struct room *next;
};

struct room *add_room(struct room **list, char *name, char *theme);
int rmv_room(struct room **list, int id);
void rmv_empty_rooms(struct room *list);
struct room *get_room(struct room *list, int id);
char *room_list_str(struct room *list);
char *troom_list_str(struct room *list, char *theme);
void set_room_limit(struct room *room, int limit);
void set_room_users(struct room *room, int value);

