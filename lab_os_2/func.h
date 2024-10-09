#ifndef FUNC
#define FUNC

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

struct PID_list *head_list;

int check_background(char **args);

void add_elem(pid_t new_PID);

void delete_head_list();

void Ctrl_plus_c(int sig);

int my_cd(char **args);

int my_help(char **args);

int my_exit(char **args);

void signal_handler(int sig);

int num_builtins();

int launch(char **args, int background);

int execute(char **args);

char** split_line(char *line);

char *readLine(void);

void loop(void);

#endif FUNC