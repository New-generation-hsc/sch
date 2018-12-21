#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sch.h"

/**
 * the command might use '<' to redirect the input
 * @filename the file name
 * @cmd the command
*/
void redirect_input(char* filename, Cmd* cmd){
    int file = open(filename, O_RDONLY);
    if(file < 0){
        printf("Can't open the %s. please check the file exists.\n", filename);
        set_status(FAILURE);
        return;
    }
    cmd->in_desc = file;
}

/**
 * the command might use '>' to redirect the output
 * @filename the ouput file
 * @cmd the command
*/
void redirect_output(char* filename, Cmd* cmd){
    int file = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if(file < 0){
        printf("Can't write to %s. please check whther have the permission", filename);
        set_status(FAILURE);
        return;
    }
    cmd->out_desc = file;
}


/**
 * A command can be combined with other commands using pipe
 * This function deals with just one command
 * @str command string
 * @len the command length
 * @cmd where the command store
 * @return indicate the command is valid
*/
int parse_args(char* str, Cmd* cmd){
    int n_args = 0;
    char* arg, *space_str;

    arg = strtok_r(str, __ARG_DELIM, &space_str);
    
    while(arg != NULL && status){
        if(strcmp("<", arg) == 0){
            arg = strtok_r(NULL, __ARG_DELIM, &space_str);
            if(arg != NULL) redirect_input(arg, cmd);
            else set_status(FAILURE);
        }else if(strcmp(">", arg) == 0){
            arg = strtok_r(NULL, __ARG_DELIM, &space_str);
            if(arg != NULL) redirect_output(arg, cmd);
            else set_status(FAILURE);
        }else{
            cmd->cmd_args[n_args] = arg;
            n_args++;
        }
        arg = strtok_r(NULL, __ARG_DELIM, &space_str);
    }
    cmd->cmd_args[n_args] = NULL;
    return n_args;
}


/**
 * parse the whole command which might contain several command using pipe for connection
 * @line the whole command string
 * @cmd_list the command array, split the whole command into a single command
 * @return the number of commands
*/
int parse_command(char* line, Cmd* cmd_list){
    char *token, *pipe_str;
    int n_cmd = 0;
    token = strtok_r(line, __PIPE_DELIM, &pipe_str);
    while(token != NULL && status){
        parse_args(token, &cmd_list[n_cmd]);
        token = strtok_r(NULL, __PIPE_DELIM, &pipe_str);
        n_cmd++;
    }
    return n_cmd;
}

/**
 * set the parse command status
 * @s status
*/
void set_status(int s){
    status = s;
}

void print_cmd(Cmd *cmd){
    char **arg = cmd->cmd_args;
    while(*arg != NULL){
        printf("args:-> %s\n", *arg);
        arg++;
    }
    printf("in:-> %d, out:-> %d\n", cmd->in_desc, cmd->out_desc);
}