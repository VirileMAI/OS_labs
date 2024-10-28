#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define NEW_ARR 256

int main(int argc, char* argv[])
{
    char *file_name = argv[1];
    FILE *file = fopen(file_name, "r");
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

// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <fcntl.h>


// #define NEW_ARR 256

// int main(int argc, char* argv[])
// {
//     char *file_name = argv[1];
//     int fd = open(file_name, O_RDONLY);
//     if (fd == -1)
//     {
//         perror("Ошибка открытия файла");
//         exit(EXIT_FAILURE);
//     }
//     printf("Файл: %s\n", file_name);

//     char* buff = malloc(256 * sizeof(char));
//     if (buff == NULL)
//     {
//         perror("Ошибка выделения памяти");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }
//     int i = 0, bytes_read;
//     while ((bytes_read = read(fd, &buff[i], NEW_ARR)) > 0)
//     {
//         i += bytes_read;

//         if (i >= NEW_ARR - 1)
//         {
//             char* new_buff = realloc(buff, (i + NEW_ARR) * sizeof(char));
//             if (new_buff == NULL)
//             {
//                 perror("ошибка перераспределения памяти");
//                 free(buff);
//                 close(fd);
//                 exit(EXIT_FAILURE);
//             }
//             buff = new_buff;
//         }
//     }

//     if(bytes_read == -1)
//     {
//         perror("ошибка выделения памяти");
//         free(buff);
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     buff[i] = '\0';
//     printf("Содержимое файла: \n%s\n", buff);
//     close(fd);

//     // int bytes_sent = 0;
//     // bytes_sent = write(fd, buff, strlen(buff));
//     // free(buff);


//     // if (bytes_sent == -1)
//     // {
//     //     perror("Ошибка при записи\n");
//     //     exit(EXIT_FAILURE);
//     // }
//     // else
//     // {
//     //     printf("\nЗаписано %d байтов\n", bytes_sent);
//     // }
//     printf("\nЗаписано %d байтов\n", i);
//     return EXIT_SUCCESS;
// }