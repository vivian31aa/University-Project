#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Command.h"
#include "SelectState.h"


void field_state_handler(Command_t *cmd, size_t idxarg){
	//initialize all the signals
	cmd->cmd_args.sel_args.fields = NULL;
	cmd->cmd_args.sel_args.fields_len = 0;//fields_len=>0
	cmd->cmd_args.sel_args.limit = -1;
	cmd->cmd_args.sel_args.offset = -1;
	//scan the arguments
	while(idxarg<cmd->args_len){
		if(!strncmp(cmd->args[idxarg],"*",1)){
			add_select_field(cmd, cmd->args[idxarg]);
		}
		else if(!strncmp(cmd->args[idxarg], "id",2)){
			add_select_field(cmd, cmd->args[idxarg]);
		}
		else if(!strncmp(cmd->args[idxarg], "name",4)){
			add_select_field(cmd, cmd->args[idxarg]);
		}
		else if(!strncmp(cmd->args[idxarg], "email",5)){
			add_select_field(cmd, cmd->args[idxarg]);
		}
		else if(!strncmp(cmd->args[idxarg], "age",3)){
			add_select_field(cmd, cmd->args[idxarg]);
		}
		//above are fields after select
		else if(!strncmp(cmd->args[idxarg], "from",4)){
			table_state_handler(cmd, idxarg+1);
			return;
		}
		//from table set
		else{
			cmd->type = UNRECOG_CMD;
			return;
		}
		idxarg++;
	}
	cmd->type = UNRECOG_CMD;
	return;
}

void table_state_handler(Command_t *cmd, size_t idxarg){
	if(idxarg< cmd->args_len){
		//more argu than idxarg
		idxarg++;
		//add 1 to idxarg
		if(idxarg == cmd->args_len){
			return;
		}
		//start checking
		else if(!strncmp(cmd->args[idxarg],"offset",6)){
			offset_state_handler(cmd, idxarg+1);
			//enter offset
			return;
		}
		else if(!strncmp(cmd->args[idxarg], "limit", 5)){
			limit_state_handler(cmd, idxarg+1);
			//enter limit
			return;
		}
		else if(!strncmp(cmd->args[idxarg], "where",5)){
			where_state_handler(cmd, idxarg+1);
			//enter where
			//return;
		}

	}
	cmd->type = UNRECOG_CMD;
	return;
}

void substr(char *result, const char *src, unsigned int a, unsigned int tmp){
	//if there is sub string
	strncpy(result, src+a, tmp);
	result[tmp]=0;
}

void addcond(Command_t *cmd, char *fieldn, char *cmp, char *inall){
	int intype, cmptype;
	cond_t condtmp;
	condtmp.field = fieldn;
	//put field value into condtmp
	if(cmp[0] == '<' || cmp[0] == '>'){
		if(cmp[1] && cmp[1] == '='){
			if(cmp[0] == '<') cmptype = 4; //<= type4
			else cmptype = 6;//>= type6
		}
		else{
			if(cmp[0] == '<') cmptype =3;//< type3
			else cmptype = 5;//> type5
		}
	}
	else if(cmp[0] == '!'){
		cmptype = 2;//!= type2
	}
	else{
		cmptype = 1;//== type1
	}
	//give every possible situation a type
	condtmp.cmptype = cmptype;

	if(inall[0] == '\"'){
		//input is string
		intype = 0;//string type = 0
		condtmp.instr = inall;
		condtmp.innum = 0;
		//clear innum
	}
	else{
		//input is number
		intype = 1;
		//int type = 1
		condtmp.innum = 0;
		//clear innum
		for(int i=0;inall[i];i++){
			condtmp.innum = condtmp.innum*10 + inall[i] - '0' ; 
		}
		condtmp.instr = "";
		//clear instr
	}
	condtmp.intype = intype;
	if(cmd->condcount == 0)
		cmd->cond1 = condtmp;
		//put into cond1
	else
		cmd->cond2 = condtmp;
		//more than 1 condition so put it into cond2
}

void where_state_handler(Command_t *cmd, size_t idxarg){
	cmd->condcount=0;
	//initialize
	addcond(cmd,cmd->args[idxarg+0],cmd->args[idxarg+1],cmd->args[idxarg+2]);
	//put arg value into condition
	cmd->condcount+=1; //add 1 condition
	if(idxarg<cmd->args_len-3){
		//if there are more arg then there are 2 conditions
		if(!strncmp(cmd->args[idxarg+3],"or",2)){
			addcond(cmd,cmd->args[idxarg+4],cmd->args[idxarg+5],cmd->args[idxarg+6]);
			cmd->condtype = 1;
			//or type1
			cmd->condcount+=1;
		}
		else if(!strncmp(cmd->args[idxarg+3],"and",3)){
			addcond(cmd,cmd->args[idxarg+4],cmd->args[idxarg+5],cmd->args[idxarg+6]);
			cmd->condtype = 2;
			//and type2
			cmd->condcount+=1;
		}
	}
	return;
}
void limit_state_handler(Command_t *cmd, size_t idxarg){
	if(idxarg<cmd->args_len){
		cmd->cmd_args.sel_args.limit = atoi(cmd->args[idxarg]);
		idxarg+=1;
		if(idxarg == cmd->args_len)//no limit
			return;
		//no enough arg for limit
	}
	cmd->type = UNRECOG_CMD;
	return;
}

void offset_state_handler(Command_t *cmd, size_t idxarg){
	if(idxarg < cmd->args_len){
		cmd->cmd_args.sel_args.offset = atoi(cmd->args[idxarg]);
		idxarg+=1;
		if(idxarg == cmd->args_len) //no offset
			return;
		else if(idxarg < cmd->args_len && !strncmp(cmd->args[idxarg],"limit",5)){
			//check if there are less arg than return required
			limit_state_handler(cmd, idxarg+1);
			//put into  limit
			return;
		}
	}
	cmd->type = UNRECOG_CMD;
	return;
}