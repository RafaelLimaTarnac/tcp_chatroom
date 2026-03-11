#pragma once

#define USIZENAME 16

struct user{
  int id;
  char name[USIZENAME];
  int room_id;
  struct user *next;
};

struct user *add_user(struct user **list, const char *name, int id);
int rmv_user(struct user **list, int id);
struct user *get_user(struct user *list, int id);
void set_name_user(struct user *user, char *name);
char *user_list_str(struct user *list);
char *iuser_list_str(struct user *list, int room_id);

