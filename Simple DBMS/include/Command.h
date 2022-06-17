#ifndef COMMAND_H
#define COMMAND_H

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD = 101,
    UPDATE_CMD = 102,
    DELETE_CMD = 103,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct{
    char *field,*instr;
    int innum, cmptype,intype;
    //to record whether the input is int or char
} cond_t;

typedef struct selarg{
    //select struct
    char **fields;
    size_t fields_len;
    int offset,limit;
    //to record whether offset or limit
} selarg_t;

typedef union{
    //cmd's argument struct
    selarg_t sel_args;
} cmdarg_t;

typedef struct Command {
    //record command's condition struct
    unsigned char type;
    char **args;
    size_t args_len, args_cap;
    int condcount, condtype;
    //if there isn't any => 0, and => 2, or => 1
    cmdarg_t cmd_args;
    cond_t cond1,cond2; //where clauses
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *arg);
void cleanup_Command(Command_t *cmd);

#endif
