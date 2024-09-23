#include "arch.h"

int main(int argc, char* argv[])
{
    if (argc == 3)
    {
        int index = 0;
        // char path_for_arch[1024] = "/mnt/a/LR_OS/lab_1";
        char* path_for_arch = argv[1];
        char* path_to_arch = argv[2];
        char* lastSlash = strrchr(path_for_arch, '/');
        char* baseName;
        if (lastSlash) {
        // Выделяем память для baseName и копируем нужную часть
          size_t length = strlen(lastSlash + 1) + 1;  // +1 для нулевого терминатора
          baseName = malloc(length);
          if (baseName) {
            strcpy(baseName, lastSlash + 1);  // Копируем строку
            printf("Полученное имя: %s\n", baseName);
          } else {
              perror("Ошибка выделения памяти");
              return EXIT_FAILURE;
          }
        } else {
        printf("Ошибка: символ '/' не найден.\n");
        return EXIT_FAILURE;
        }
        strcat(path_to_arch, ".bin");
        int alloc_fileInfos = 30;
        FileHeader* FileHeaderList = malloc(sizeof(FileHeader) * alloc_fileInfos);
        readFiles(path_for_arch, &FileHeaderList, &alloc_fileInfos, &index, baseName, "");
        create_arch(index, path_for_arch, path_to_arch, FileHeaderList);
        free_fileHeader(FileHeaderList);
        FileHeaderList = NULL;
        printf("Архив создан успешно\n");
        exit(EXIT_SUCCESS);
    }
}