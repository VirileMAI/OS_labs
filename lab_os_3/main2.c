#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BLOCK_SIZE 256

int main(int argc, char *argv[])
{
    int fd1[2], fd2[2];
    pid_t fork_res_1, fork_res_2;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    fork_res_1 = fork();
    if (fork_res_1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (fork_res_1 == 0) {
        // Дочерний процесс 1
        close(fd1[0]);  // Закрываем конец для чтения
        char buff[16];
        sprintf(buff, "%d", fd1[1]);
        execl(argv[1], argv[1], argv[2], buff, (char *)0);
        perror("execl");
        close(fd1[1]);  // Закрываем конец для записи, если произошла ошибка
        exit(EXIT_FAILURE);
    }

    fork_res_2 = fork();
    if (fork_res_2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (fork_res_2 == 0) {
        // Дочерний процесс 2
        close(fd2[0]);  // Закрываем конец для чтения
        char buff[16];
        sprintf(buff, "%d", fd2[1]);
        execl(argv[1], argv[1], argv[3], buff, (char *)0);
        perror("execl");
        close(fd2[1]);  // Закрываем конец для записи, если произошла ошибка
        exit(EXIT_FAILURE);
    }

    // Родительский процесс
    close(fd1[1]);  // Закрываем конец для записи
    close(fd2[1]);  // Закрываем конец для записи

    wait(NULL);  // Ожидание завершения дочерних процессов
    wait(NULL);

    // Динамическое чтение данных из pipe 1
    int dataProcessed1 = 0;
    int size1 = BLOCK_SIZE;
    char *buff_for_1 = malloc(size1);

    int bytes_read;
    while ((bytes_read = read(fd1[0], buff_for_1 + dataProcessed1, BLOCK_SIZE)) > 0) {
        dataProcessed1 += bytes_read;
        if (dataProcessed1 + BLOCK_SIZE > size1) {
            size1 *= 2;  // увеличиваем размер буфера в два раза
            buff_for_1 = realloc(buff_for_1, size1);
        }
    }

    // Динамическое чтение данных из pipe 2
    int dataProcessed2 = 0;
    int size2 = BLOCK_SIZE;
    char *buff_for_2 = malloc(size2);

    while ((bytes_read = read(fd2[0], buff_for_2 + dataProcessed2, BLOCK_SIZE)) > 0) {
        dataProcessed2 += bytes_read;
        if (dataProcessed2 + BLOCK_SIZE > size2) {
            size2 *= 2;  // увеличиваем размер буфера
            buff_for_2 = realloc(buff_for_2, size2);
        }
    }

    close(fd1[0]);  // Закрываем конец для чтения
    close(fd2[0]);  // Закрываем конец для чтения

    printf("Считано %d байтов: %s\n", dataProcessed1, buff_for_1);
    printf("Считано %d байтов: %s\n", dataProcessed2, buff_for_2);

    // XOR результат
    unsigned char *buf_for_res = malloc(dataProcessed1 > dataProcessed2 ? dataProcessed1 : dataProcessed2);

    FILE *output = fopen("result.txt", "w+");

    int max_len = (dataProcessed1 > dataProcessed2) ? dataProcessed1 : dataProcessed2;
    for (int i = 0; i < dataProcessed1; i++) {
        unsigned char char1 = buff_for_1[i];
        unsigned char char2 = buff_for_2[i % dataProcessed2];
        buf_for_res[i] = char1 ^ char2;
        if (buf_for_res[i] == '\0') buf_for_res[i] = char1;

        fprintf(output, "%c", buf_for_res[i]);
        printf("result[%d]: %d ^ %d = %d\n", i, char1, char2, buf_for_res[i]);
    }

    printf("Результат: ");
    for (int j = 0; j < max_len; j++) 
        printf("%d|%c ", buf_for_res[j], buf_for_res[j]);
    printf("\n");

    fclose(output);

    free(buff_for_1);
    free(buff_for_2);
    free(buf_for_res);

    exit(EXIT_SUCCESS);
}
