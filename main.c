#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "sch.h"

/**
 * read a line from stdin and is null terminated 
 * @buffer store the line content
 * @buf_size is the max capacity
*/
void readline(char* buffer, int buf_size){
    int cnt = 0; /* character count */
    char ch;
    while(cnt < buf_size - 1 && (ch = getchar()) != EOF && ch != '\n'){
        buffer[cnt] = ch;
        cnt++;
    }
    buffer[cnt] = '\0';
}

/**
 * fork a new process to exexute the command
 * @in_desc input file descriptor
 * @out_desc output file descriptor
 * @cmd the command
*/
int spawn_proc(int in_desc, int out_desc, Cmd* cmd){
    pid_t pid;

    if((pid = fork()) == 0){
        // child process

        if(in_desc != 0){
            dup2(in_desc, 0);
            close(in_desc);
        }

        if(out_desc != 1){
            dup2(out_desc, 1);
            close(out_desc);
        }

        if(cmd->in_desc != 0){
            dup2(cmd->in_desc, 0);
            close(cmd->in_desc);
        }

        if(cmd->out_desc != 1){
            dup2(cmd->out_desc, 1);
            close(cmd->out_desc);
        }

        execvp(cmd->cmd_args[0], cmd->cmd_args);
        printf("some error happen in child process.\n");
        exit(1);
    }

    wait(NULL); // wait for child process
    return pid;
}

/**
 * execute each command using a child process
 * @cmd_list command array
 * @cnt the number of command
*/
void fork_pipes(Cmd* cmd_list, int cnt){
    int pipefd[2];
    int in_desc = STDIN_FILENO;

    for(int i = 0; i < cnt - 1; i++){
        pipe(pipefd);

        // pipefd[1] is the write end of pipe
        spawn_proc(in_desc, pipefd[1], &cmd_list[i]);

        // No need for write end of the pipe, 
        close(pipefd[1]);

        // keep the read end of pipe
        in_desc = pipefd[0];
    }

    if(cnt > 0){
        spawn_proc(in_desc, STDOUT_FILENO, &cmd_list[cnt-1]);
    }
    
}

void init_command(Cmd* cmd_list){
    for(int i = 0; i < __N_CMD; i++){
        cmd_list[i].in_desc = STDIN_FILENO;
        cmd_list[i].out_desc = STDOUT_FILENO;
    }
}

/**
 * print help string
*/
void print_propmt(){
    printf("\033[1;32msch > \033[0m");
}

/**
 * change directory by using command 'cd'
*/
void cd_cmd(char* path){
    int ret = chdir(path);
    if(ret < 0){
        printf("change directory error. please check correct directory or access permission.\n");
    }
}

void sch_loop(){
    while(1){
        init_command(cmd_list);
        set_status(SUCCESS);
        print_propmt();
        readline(line, __N_CMD_CHAR);
        int cnt = parse_command(line, cmd_list);
        if(cnt == 1 && strcmp("cd", cmd_list[0].cmd_args[0]) == 0){
            cd_cmd(cmd_list[0].cmd_args[1]);
        }else if(cnt == 1 && strcmp("exit", cmd_list[0].cmd_args[0]) == 0){
            printf("Bye!\n");
            exit(0);
        }else{
            fork_pipes(cmd_list, cnt);
        }
    }
}

static void catch_stop(int signo){
    printf("\nInteractive attetion signal caught, Ctrl + C.\nBye!\n");
    exit(0);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, catch_stop);
    sch_loop();
    return 0;
}
