/**
 * This is a miniShell writed by c code.
 * @Author hsc
 * @date 2018-12-20
 * @version 0.1 
*/

#ifndef _SCH_H_
#define _SCH_H_

#define __N_ARG 100      /* define the largest number of arguments */
#define __N_CMD 100      /* define the largest number of command can combine */
#define __N_CMD_CHAR 128 /* define the largest number of character that commnd can contain */
#define __N_FILENAME 32  /* define the largest number of character that file name */

#define SUCCESS 1
#define FAILURE 0

#define __ARG_DELIM "\t "
#define __PIPE_DELIM "|"

struct _Cmd {
    char* cmd_args[__N_ARG]; /* the command arguments */
    int in_desc;             /* the input file descriptor default is the STDIN */
    int out_desc;            /* the output file descriptor deault is the STDOUT */
};

typedef struct _Cmd Cmd;
Cmd cmd_list[__N_CMD];
char line[__N_CMD_CHAR];

int status; /* indicate the command is valid */

void init_command(Cmd*);
int parse_command(char*, Cmd*);
void readline(char*, int);
void set_status(int);

#endif