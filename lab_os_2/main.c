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


int check_background(char **args)
{
    int i = 0;
    while(args[i] != NULL)
    {
        i++;
    }

    if (i > 0 && strcmp(args[i-1], "&") == 0)
    {
        args[i-1] = NULL;
        return 1; 
    }
    return 0;
}


void add_elem(pid_t new_PID)
{
    struct PID_list *new_list = (struct PID_list*)malloc(sizeof(struct PID_list));

    new_list->PID = new_PID;
    new_list->next_process = NULL;

    if(head_list == NULL)
    {
        head_list = new_list;
    }
    else
    {
        struct PID_list *current = head_list;
        while (current->next_process != NULL)
        {
            current = current->next_process;
        }
        current->next_process = new_list;
    }
    return;
}

void delete_head_list()
{
    if (head_list == NULL)
    {
        return;
    }
    struct PID_list *current = head_list;
    head_list = current->next_process;

    free(current);
    return;
}

void Ctrl_plus_c(int sig)
{
    if (head_list == NULL)
    {
        printf("\nРодительский процесс убит | PID: %d\n", getpid());
        kill(getpid(), SIGTERM);
        waitpid(-1, NULL, 0);
    }
    else
    {
        struct PID_list *current = head_list;
        int i = 1;
        printf("\n\033[38;5;161mДочерние процессы родительского процесса: \033[0m%d\n", getpid());
        while(current != NULL)
        {
            printf("\033[38;5;135m%i. PID дочернего процесса:\033[38;5;228;m%d\033[0m\n", i, current->PID);
            current = current->next_process;
            i++;
        }
        printf("Убит процесс: %d\n", head_list->PID);
        kill(head_list->PID, SIGTERM);

        waitpid(head_list->PID, NULL, 0);

        delete_head_list();
    }
    return;
}

// Прототипы функций
int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);
void signal_handler(int sig);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char**) = {
    &my_cd,
    &my_help,
    &my_exit
};

int num_builtins() {return sizeof(builtin_str) / sizeof(char *);}

int my_cd(char **args) {
    char * home = getenv("HOME");

    if (args[1] == NULL) {
        chdir(home);
    }
    else if (strcmp(args[1], "~"))
    {

    } 
    else 
    {
        if (chdir(args[1]) != 0) {
            perror("Ошибка при смене директории");
        }
    }
    return 1;
}

int my_help(char** args) {
    int i;
    printf("Терминал Кириллова А. А., группа М3О-319Бк-22\n");
    printf("Наберите название программы и ее аргументы и нажмите enter.\n");
    printf("Вот список встроенных команд:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }
    printf("Используйте команду man для получения информации по другим программам.\n");
    return 1;
}

void signal_handler(int sig) {
    if (head_list == NULL)
    {
        printf("\nРодительский процесс убит | PID %d\n", getpid());
        kill(getpid(),SIGTERM);
        waitpid(-1, NULL, 0);
    }
    else
    {
        struct PID_list *current = head_list;
        int i = 1;
        printf("\nДочерние процессы родительского процесса %d:\n", getpid());
        while(current != NULL)
        {
            printf("%i. PID дочернего процесса: %d\n", i, current->PID);
            current = current->next_process;
            i++;
        }
        printf("Убит процесс %d\n", head_list->PID);
        kill(head_list->PID, SIGTERM);

        waitpid(head_list->PID, NULL, 0);
        delete_head_list();
    }
    return;
}

int launch(char **args, int background) {
    pid_t pid;
    char *ignore_list[] = {"ps", "ls", "cat", "grep", "killall", NULL}; // Массив команд, которые не сохраняются
    int save_process = 1; // Флаг для сохранения PID
    int status;

    // Проверяем, является ли команда одной из тех, которые не нужно сохранять
    for (int i = 0; ignore_list[i] != NULL; i++) {
        if (strcmp(args[0], ignore_list[i]) == 0) {
            save_process = 0;
            break;
        }
    }

    pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        setpgid(0, 0); // Создайте новую группу процессов
        if (execvp(args[0], args) == -1) {
            perror("Ошибка при выполнении команды");
            exit(EXIT_FAILURE); // Выйти, если команда не удалась
        }
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("Ошибка при форкинге");
    } else {
        // Родительский процесс
        if (!background) {
            waitpid(pid, &status, 0);  // Ждем завершения дочернего процесса, если процесс не в фоне

            // Сохраняем PID только если команда завершилась успешно
            if (!WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                // add_elem(pid);  // Сохраняем PID только если команда выполнена без ошибок
                // printf("Дочерний процесс = %d\n", pid);
                printf("Процесс %d завершился с ошибкой, команда: %s\n", pid, args[0]);
            } 
            // else {
            //     printf("Процесс %d завершился с ошибкой, команда: %s\n", pid, args[0]);
            // }
        } else {
            // Процесс в фоне
            if (save_process) {
                add_elem(pid);  // Сохраняем PID, так как мы не будем ждать процесс в фоне
                
            }
            printf("Процесс запущен в фоне, PID: %d\n", pid);
        }
    }
    return 1;
}



int execute(char **args) {
    int i;
    int background = check_background(args);

    if (args[0] == NULL) {
        return 1; // Была введена пустая команда.
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch(args, background);
}

int my_exit(char **args) {
    return 0;
}

char** split_line(char *line) {
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Ошибка выделения памяти\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

char *readLine(void) {
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

void loop(void) {
    char *line;
    char **args;
    int status = 0;
    char cwd[PATH_MAX];
    char * user = getenv("USER");
    char * host = "virile";
    do {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Ошибка при получении директории");
            continue;
        }
        printf("\033[38;5;46m%s@%s\033[0m:\033[38;5;111m%s\033[0m $ ",user, host ,cwd);
        line = readLine();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    
    signal(SIGINT, signal_handler); 
    loop();

    return EXIT_SUCCESS;
}
