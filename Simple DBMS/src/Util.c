#include <stdio.h>
#include "stdio_ext.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, selarg_t *selarg) {
    //hw2
    //print selected fields only
    size_t idx;
    printf("("); //print the front (
    for(idx = 0; idx <selarg->fields_len; idx++){
        if(!strncmp(selarg->fields[idx],"*", 1)){
            //when input is select *
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        }
        else{
            if(idx>0)   printf(", ");
            if(!strncmp(selarg->fields[idx],"id",2)){
                printf("%d",user->id); //select id
            }
            else if(!strncmp(selarg->fields[idx],"name",4))
                printf("%s",user->name);//select name
            else if(!strncmp(selarg->fields[idx],"email",5))
                printf("%s",user->email); //select email
            else if(!strncmp(selarg->fields[idx],"age",3))
                printf("%d", user->age); //select age
        }
    }
    printf(")\n");
}

void print_user2(Table_t *table, int *idxlist, size_t idxlistlength, Command_t *cmd) {
	//hw2
    //print when offset or limit
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;
    if(offset == -1){
        //deal with special case
        offset = 0;
    }
    if(idxlist){
        for(idx=offset;idx<idxlistlength;idx++){
            if(limit!=-1 && (idx-offset)>=limit){
                //to make sure limit is not negative
                //after idxlistlength-offset will be bigger thann limit
                break;
            }
            print_user(get_User(table, idxlist[idx]), &(cmd->cmd_args.sel_args));
        }
        
    }
    else{
            for(idx=offset; idx<table->len;idx++){
                if(limit!=-1 && (idx-offset)>=limit){
                    break;
                }
                print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
            }
    }
    /*printf("(");
	for(int i=0;i<t;i++){
		if(c[i]==1)	printf("%d", user->id);
		else if(c[i]==2)	printf("%s", user->name);
		else if(c[i]==3)	printf("%s", user->email);
		else if(c[i]==4)	printf("%d",user->age);
		if(i==t-1)	printf(")\n");
		else	printf(", ");
	}*/
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    cmd->condcount=0;
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);//enter insert
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);//enter select
        return SELECT_CMD;
    } 
    else if(!strncmp(cmd->args[0], "update", 6))
        handle_update_cmd(table,cmd);//enter update
    else if(!strncmp(cmd->args[0], "delete", 6))
        handle_delete_cmd(table, cmd);//enter delete
    else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
	// for(int i=0;i<table->len;i++){
	// 	if(user->id==table->users[i].id)
	// 		return ret;
	// }
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

int condcheck(cond_t condtmp, User_t *usertmp){
    int innum;
    char *instr;
    if(!strncmp(condtmp.field, "id", 2))
        innum = usertmp->id;
    else if(!strncmp(condtmp.field, "name", 4))
        instr = usertmp->name;
    else if(!strncmp(condtmp.field, "email", 5))
        instr = usertmp->email;
    else
        innum = usertmp->age;
    //put values into innum or instr
    if(condtmp.intype==1){
        if(condtmp.cmptype == 1)
            return (innum == condtmp.innum?1:0);
        //= type1
        else if(condtmp.cmptype == 2)
            return (innum != condtmp.innum?1:0);
        //!= type2
        else if(condtmp.cmptype == 3)
            return (innum < condtmp.innum?1:0);
        //< type3
        else if(condtmp.cmptype == 4)
            return (innum <= condtmp.innum?1:0);
        //<= type4
        else if(condtmp.cmptype == 5)
            return (innum > condtmp.innum?1:0);
        //> type5
        else if(condtmp.cmptype == 6)
            return (innum >= condtmp.innum?1:0);
        //>= type6
        //give all possible type a number
    }
    else{
        char *str = malloc(strlen(condtmp.instr)+1);
        for(int i=0;i<strlen(condtmp.instr);i++){
            str[i] = condtmp.instr[i];
        }
        str[strlen(condtmp.instr)] = 0;
        if(condtmp.cmptype == 1){
            return (strcmp(instr, str)==0?1:0);
        }
        //compare
        else if(condtmp.cmptype == 2){
            return (strcmp(instr, str)!=0?1:0);
        }
    }
    return 1;
}
int idxlength;//put this outside to make it a global variable

int* makeidx(Table_t *table, int *idxlist, Command_t *cmd){
    idxlength=0;
    for(int i=0;i<table->len;i++){
        User_t *usertmp = get_User(table,i);
        if(cmd->condcount == 2){//check how many conditions there ate
            int first = condcheck(cmd->cond1, usertmp);
            int second = condcheck(cmd->cond2, usertmp);
            if(cmd->condtype == 1){
                //condtype=1 => or
                if(first || second){
                    idxlength += 1;
                    //calcute idx's length to malloc
                }
            }
            else if(cmd->condtype==2){
                //condtype=2 => and
                if(first && second ){
                   idxlength += 1; 
                }
            }
        }
        else{
            //only one condition
            int first = condcheck(cmd->cond1, usertmp);
            if(first){
                idxlength+=1;
            }
        }
    }
    //now we know how much memory we need to allocate    
    idxlist = malloc(idxlength * sizeof(int));
    int index = 0;
    for(int i=0;i<table->len;i++){
        User_t *usertmp = get_User(table,i);
        if(cmd->condcount == 2){
            int first = condcheck(cmd->cond1, usertmp);
            int second = condcheck(cmd->cond2, usertmp);
            if(cmd->condtype == 1){
                //condition is or
                if(first || second)
                    idxlist[index++] = i;
                    //save all the index that satisfisfy cond1 or cond2
            }
            else if(cmd->condtype == 2){
                //condition is and
                if(first && second)
                    idxlist[index++] = i;
                //save all the index that satisfisfy cond1 and cond2
            }
        }
        else{
            int first = condcheck(cmd->cond1, usertmp);
            if(first){
                idxlist[index++] = i;
                //save all the index that satisfisfy cond1
            }
        }
    }
    return idxlist;
}

void deleteuser(Table_t *table, int *idxlist, int idxlength){
    Table_t *newtab = new_Table(NULL);
    int index =0 ;
    for(int i=0;i<table->len;i++){
        if(index == idxlength || idxlist[index]!=i){
            add_User(newtab,get_User(table,i));
            //printf("%d %d\n", i ,idxlist[k]);
        }
        else{
            index++;
            //printf("%d\n",k);
        }
    }
    table->len = newtab->len;
    for(int i=0;i<table->len;i++){
        //for every users
        table->users[i].id = newtab->users[i].id;
        for(int j=0;j<strlen(newtab->users[i].name);j++){
            //every users' name
            table->users[i].name[j] = newtab->users[i].name[j];
        }
        table->users[i].name[strlen(newtab->users[i].name)]=0;
        for(int j=0;j<strlen(newtab->users[i].email);j++){
            //every users' email
            table->users[i].email[j]=newtab->users[i].email[j];
        }
        table->users[i].email[strlen(newtab->users[i].email)] = 0;
        table->users[i].age = newtab->users[i].age;
    }
}

void handle_delete_cmd(Table_t *table, Command_t *cmd){
    if(cmd->args_len >= 5){
        //enter where
        where_state_handler(cmd, 4);
        //use where_handler
        int *idxlist = NULL;
        idxlist = makeidx(table, idxlist, cmd);
        deleteuser(table, idxlist, idxlength);
        //enter delete
    }
    else{
        //arg doesn't contain where caluse
        int *idxlist = malloc((table->len) * sizeof(int));
        for(int i=0;i<table->len;i++){
            idxlist[i]=i;
            //create idxlist to save
        }
        idxlength = table->len;
        deleteuser(table, idxlist, idxlength); //enter delete
    }
}

void updateuser(Table_t *table, int *idxlist, int idxlength, char *field, char *newval){
    for(int i=0;i<idxlength;i++){
        int tmp = idxlist[i];
        User_t usertmp = *get_User(table, tmp);
        if(!strncmp(field,"id",2)){
            //if id field
            int value = 0;
            for(int j=0;j<strlen(newval);j++){
                //turn newval to int
                value = value*10 + newval[j]-'0';
            }
            usertmp.id =  value;
        }
        else if(!strncmp(field,"name",4)){
            //if name field
            char *s = newval;
            for(int j=0;j<strlen(s);j++){
                usertmp.name[j] = s[j];
            }
            usertmp.name[strlen(s)] = 0;
        }
        else if(!strncmp(field,"email",5)){
            //if email field
            char *s = newval;
            for(int j=0;j<strlen(s);j++){
                usertmp.email[j] = s[j];
            }
            usertmp.email[strlen(s)] = 0;
        }
        else if(!strncmp(field, "age", 3)){
            //if age field
            int value = 0;
            for(int i=0;i<strlen(newval);i++){
                //turn newval to int
                value = value * 10 + newval[i]-'0';
            }
            usertmp.age = value;
        }
        //users=>usertmp
        table->users[tmp] = usertmp;
    }
}

void handle_update_cmd(Table_t *table, Command_t *cmd){
    char *newval = cmd->args[5]; //where the newval is
    char *field = cmd->args[3]; //where field is
    if(cmd->args_len>6){
        //there is where caluse
        where_state_handler(cmd, 7);
        //use where_handler
        int *idxlist = NULL;
        idxlist = makeidx(table, idxlist, cmd);
        updateuser(table, idxlist, idxlength, field, newval);
    }
    else{
        int *idxlist = malloc((table->len)*sizeof(int));
        //create idxlist to save
        for(int i=0;i<table->len;i++){
            idxlist[i] = i;
            //save the index
        }
        idxlength = table->len;
        updateuser(table, idxlist, idxlength, field, newval);
    }
    //just like delete
}
///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd,1);
    //use field_state_handler
    for(int i=0;i<cmd->args_len;i++){
        table_state_handler(cmd,i);
        //use table_state_handler
    }
    if(cmd->condcount == 0){
        int sum = 0;
        if(!strncmp(cmd->args[1], "sum(id)",7)){
            //if the second arg is sum(id)
            for(int i=0;i<table->len;i++){
                User_t *usertmp = get_User(table,i);
                sum+= usertmp->id;
                //sum all ids in the table
            }
            printf("(%d)\n",sum);
        }
        else if(!strncmp(cmd->args[1], "avg(id)",7)){
            //if the second arg is avg(id)
            for(int i=0;i<table->len;i++){
                User_t *usertmp = get_User(table,i);
                sum+= usertmp->id;
            }
            //.3f=>three digits after .
            //divide sum
            printf("(%.3f)\n",(double)sum / (double)table->len);
        }
        else if(!strncmp(cmd->args[1], "sum(age)", 8)){
            //if the second arg is sum(age)
            for(int i=0;i<table->len;i++){
                User_t *usertmp = get_User(table,i);
                sum+= usertmp->age;
                //sum all ages in the table
            }
            printf("(%d)\n",sum);
        }
        else if(!strncmp(cmd->args[1], "avg(age)", 8)){
            // if the second arg is avg(id)
            for(int i=0;i<table->len;i++){
                User_t *usertmp = get_User(table,i);
                sum+= usertmp->age;
            }
            //.3f=>three digits after .
            //divide sum
            printf("(%.3f)\n",(double)sum / (double)table->len);
        }
        else if((!strncmp(cmd->args[1], "count(*)", 8))|| (!strncmp(cmd->args[1],"count(name)",11)) || (!strncmp(cmd->args[1],"count(id)",9)) || (!strncmp(cmd->args[1],"count(email)",12)) || (!strncmp(cmd->args[1],"count(age)",10))){
            //if the second arg is count(*)
            printf("(%d)\n",table->len);
        }
        else{
            //don't input value
            print_user2(table, NULL, 0, cmd);
        }
    }
    else{
        int sum = 0;
        int *idxlist = NULL;
        idxlist = makeidx(table,idxlist, cmd);
        
        if(!strncmp(cmd->args[1], "sum(id)",7)){
            //if the second arg is sum(id)
            for(int i=0;i<idxlength;i++){
                User_t *usertmp = get_User(table,idxlist[i]);
                sum+= usertmp->id;
                //sum all ids in the table
            }
            printf("(%d)\n",sum);
        }
        else if(!strncmp(cmd->args[1], "avg(id)",7)){
            //if the second arg is avg(id)
            for(int i=0;i<idxlength;i++){
                User_t *usertmp = get_User(table,idxlist[i]);
                sum+= usertmp->id;
            }
            //.3f=>three digits after .
            //divide sum
            printf("(%.3f)\n",(double)sum / (double)idxlength);
        }
        else if(!strncmp(cmd->args[1], "sum(age)", 8)){
            //if the second arg is sum(age)
            for(int i=0;i<idxlength;i++){
                User_t *usertmp = get_User(table,idxlist[i]);
                sum+= usertmp->age;
                //sum all ages in the table
            }
            printf("(%d)\n",sum);
        }
        else if(!strncmp(cmd->args[1], "avg(age)", 8)){
            // if the second arg is avg(id)
            for(int i=0;i<idxlength;i++){
                User_t *usertmp = get_User(table,idxlist[i]);
                sum+= usertmp->age;
            }
            //.3f=>three digits after .
            //divide sum
            printf("(%.3f)\n",(double)sum / (double)idxlength);
        }
        else if((!strncmp(cmd->args[1], "count(*)", 8)) || (!strncmp(cmd->args[1],"count(id)",9)) || (!strncmp(cmd->args[1],"count(name)",11)) || (!strncmp(cmd->args[1],"count(email)",12)) || (!strncmp(cmd->args[1],"count(age)",10))){
            printf("(%d)\n", idxlength);
        }
        else{
            print_user2(table, idxlist, idxlength,cmd);
        }
    }
    return table->len;
}


///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

