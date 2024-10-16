#include "func.h"

struct PID_list *head_list;

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "chsh"
};


int (*builtin_func[]) (char**) = {
    &my_cd,
    &my_help,
    &my_exit,
    &my_chsh
};


// Функция проверки запуска приложения в фоне
int check_background(char **args)
{
    int i = 0;
    while(args[i] != NULL) {i++;}
    if (i > 0 && strcmp(args[i - 1], "&") == 0) //если последний аргумент &, то процесс запущен в фоне
    {
        args[i - 1] = NULL;
        return 1;
    }
    return 0;
}

//добавление нового элемемнта в конец списка
void add_elem(pid_t new_PID)
{
    struct PID_list *new_list = (struct PID_list*)malloc(sizeof(struct PID_list));

    new_list->PID = new_PID;
    new_list->next_process = NULL;

    if (head_list == NULL) {head_list = new_list;}
    else
    {
        struct PID_list *current = head_list;
        while (current->next_process != NULL) {current = current->next_process;}
        current->next_process = new_list;
    }
}

// Удаление первого элемента (в итоге удаление в порядке FIFO)
void delete_head_list()
{
    if (head_list == NULL) {return;}
    struct PID_list *current = head_list;
    head_list = current->next_process;

    free(current);
}

void delete_last_elem() {

    if (head_list == NULL) {return;}

    if (head_list->next_process == NULL) {
        free(head_list);
        head_list = NULL;
        return;
    }

    struct PID_list *current = head_list;
    struct PID_list *prev = NULL;

    while (current->next_process != NULL) {
        prev = current;
        current = current->next_process;
    }
    free(current);
    prev->next_process = NULL;
}

// Количество встроенных команд
int num_builtins() {return sizeof(builtin_str) / sizeof(char *);}

// Функция cd
int my_cd(char **args) {
    char * home = getenv("HOME");

    if (args[1] == NULL) {
        chdir(home);
    }
    else if (strcmp(args[1], "~") == 0)
    {
        chdir(home);
    } 
    else 
    {
        if (chdir(args[1]) != 0) {
            perror("Ошибка при смене директории");
        }
    }
    return 1;
}


// Функция доп. информации
int my_help(char** args) {
    int i;
    printf("Терминал Кириллова А. А., группа М3О-319Бк-22\n");
    printf("I <3 Katyushu!\n");
    printf("Наберите название программы и ее аргументы и нажмите enter.\n");
    printf("Вот список встроенных команд:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }
    printf("Используйте команду man для получения информации по другим программам.\n");
    return 1;
}

//функция выхода из терминала
int my_exit(char **args) {
    return 0;
}

// функция для возрата стандартного терминала
int my_chsh(char **args) {
    const char *new_shell = "/bin/bash";
    char *user = getenv("USER");

    if (user == NULL) {
        fprintf(stderr, "Ошибка получения имени пользователя\n");
        return 1;
    }

    char command[256];
    snprintf(command, sizeof(command), "chsh -s %s %s", new_shell, user);

    // Выполняем команду
    int result = system(command);
    if (result == -1) {
        perror("Ошибка при смене оболочки");
        return -1;
    }

    printf("Оболочка успешно изменена на %s. Пожалуйста, перезапустите терминал.\n", new_shell);
    return 1; 
}

// Функция обработки сигнала SIGINT(Ctrl+c)
void signal_handler(int sig) {
    char cwd[PATH_MAX];
    char* user = getenv("USER");
    char hostname[1024];
    gethostname(hostname, 1024);
    if (head_list == NULL)
    {
        printf("\n");
         if (getcwd (cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
        }
        printf("\033[38;5;46m%s@%s\033[0m:\033[38;5;111m%s\033[0m $ ",user, hostname ,cwd);
        fflush(stdout);
        return;
    }
    else
    {
        struct PID_list *current = head_list;
        int i = 1;
        printf("\nДочерние процессы родительского процесса %d:\n", getpid());
        // Печать дочерних процессов родительского процесса
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
}

// Функция запуска
int launch(char **args, int background)
{
    pid_t pid;
    int status;

    if (strcmp(args[0], "cat") == 0 && args[1] == NULL) {
        printf("Ошибка: команда 'cat' требует файл или ввод данных.\n");
        return 1;
    }

    pid = fork(); // Создаем новый процесс
    if (pid == 0)
    {
        // Дочерний процесс
        if (strcmp(args[0], "man") != 0)
        {
            setpgid(0, 0);
        }
        if (execvp(args[0], args) == -1) {
            exit(EXIT_FAILURE); 
        }
    }
    else if (pid < 0) {
        perror("Ошибка при форкинге");
    }
    else
    {
        // Родительский процесс
        if (!background)
        {
            // Если процесс не в фоне, ждем его завершения
            add_elem(pid);
            printf("Запущен дочерний процесс | PID: %d\n", pid);
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status); 
                if (exit_status != 0) {
                    printf("Процесс %d завершился с ошибкой, команда: %s, код: %d\n", pid, args[0], exit_status);
                    printf("Дочерний процесс %d удален\n", pid);
                    delete_last_elem();
                    return 1;
                } 
                printf("Дочерний процесс %d удален\n", pid);
                delete_last_elem();
            } 
        }
        else
        {
            add_elem(pid);
            printf("Процесс запущен в фоне | PID: %d\n", pid);
        }
    }
    return 1; 
}


//Функция запуска встроенных команд
int execute(char **args)
{
    int i;
    int background = check_background(args);

    if (args[0] == NULL) {return 1;}
    for (i = 0; i < num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    }

    return launch(args, background);
}

// Функция разделение команд
char** split_line(char *line)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) 
            {
                fprintf(stderr, "Ошибка выделения памяти\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// Функция считывания строки с терминала
char* readLine(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

// Основной цикл
void loop(void)
{
    char *line;
    char **args;
    int status = 0;
    char cwd[PATH_MAX];
    char* user = getenv("USER");
    char hostname[1024];
    gethostname(hostname, 1024);
    do {
        if (getcwd (cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
            continue;
        }
        printf("\033[38;5;46m%s@%s\033[0m:\033[38;5;111m%s\033[0m $ ",user, hostname ,cwd);
        line = readLine();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    }while (status);
}

