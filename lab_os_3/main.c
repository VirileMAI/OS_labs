#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define NEW_ARR 256

int main(int argc, char* argv[])
{
    char *file_name = argv[1];
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }
    printf("Файл: %s\n", file_name);

    char* buff = malloc(NEW_ARR * sizeof(char));
    if (buff == NULL)
    {
        perror("Ошибка выделения памяти");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    int i = 0, c;
    int new_i = i;
    while ((c = fgetc(file)) != EOF)
    {
        if(i >= (new_i + NEW_ARR) - 1)
        {
            char* new_buff = realloc(buff, (i + NEW_ARR) * sizeof(char));
            new_i = i;
            if (new_buff == NULL)
            {
                perror("Ошибка перераспределения памяти");
                free(buff);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            buff = new_buff;
        }
        buff[i++] = c;
    }

    buff[i] = '\0';
    fclose(file);

    int dataProcessed;
    int fd;
    if (argv[2] != NULL)
    {
        fd = atoi(argv[2]);
    }
    else fd = 1;
    dataProcessed = write(fd, buff, strlen(buff));

    if (dataProcessed == -1)
    {
        perror("Ошибка при записи\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("\nЗаписано %d байтов\n", dataProcessed);
    }
    return EXIT_SUCCESS;
    free(buff);
}
