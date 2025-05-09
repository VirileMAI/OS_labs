#ifndef FUNC_H
#define FUNC_H

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#define PATH_MAX 4095

struct PID_list
{
    pid_t PID;
    struct PID_list *next_process;
};

extern struct PID_list *head_list;

extern char *builtin_str[];

int check_background(char **args);

void add_elem(pid_t new_PID);

void delete_head_list();

void delete_last_elem();

void signal_handler_last_elem(int sig);

void Ctrl_plus_c(int sig);

int my_cd(char **args);

int my_help(char **args);

int my_exit(char **args);

int my_chsh(char **args);

extern int (*builtin_func[]) (char**);

void signal_handler(int sig);

int num_builtins();

int launch(char **args, int background);

int execute(char **args);

char** split_line(char *line);

char *readLine(void);

void loop(void);

#endif