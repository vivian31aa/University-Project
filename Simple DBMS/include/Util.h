#ifndef DB_UTIL_H
#define DB_UTIL_H
#include "Command.h"
#include "Table.h"

typedef struct State {
    int saved_stdout;
} State_t;

State_t* new_State();
void print_prompt(State_t *state);
void print_user(User_t *user, selarg_t *sel_args);
void print_user2(Table_t *table, int *idxlist, size_t idxlistlength, Command_t *cmd);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd(Table_t *table, Command_t *cmd);
int handle_insert_cmd(Table_t *table, Command_t *cmd);
int handle_select_cmd(Table_t *table, Command_t *cmd);
//add new void to handle update and delete
void handle_update_cmd(Table_t *table, Command_t *cmd);
void handle_delete_cmd(Table_t *table, Command_t *cmd);
void print_help_msg();

#endif
