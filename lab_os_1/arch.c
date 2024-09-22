#include "arch.h"


int readFiles(const char* dirPath, FileHeader** files,
              int* alloc_FileHeader, int* index) {
    //Откроем директорию
    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Ошибка при открытии директории");
        exit(EXIT_FAILURE);
    }
    struct dirent* entry;  //Структура из dirent.h
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (*index >= *alloc_FileHeader)
            {
                int new_size = *alloc_FileHeader + BLOCK_SIZE;
                FileHeader* new_fileHeader = realloc(files, sizeof(FileHeader) * new_size);
                if (new_fileHeader == NULL)
                {
                    free(files);
                    closedir(dir);
                    perror("Ошибка выделения памяти\n");
                    exit(EXIT_FAILURE);
                }
                *files = new_fileHeader;
                *alloc_FileHeader = new_size;
            }
            FileHeader fileHeader;
            strncpy(fileHeader.filename, entry->d_name, sizeof(fileHeader.filename) - 1);
            fileHeader.filename[sizeof(fileHeader.filename) - 1] = '\0';
            snprintf(fileHeader.path,
                     strlen(dirPath) + strlen(entry->d_name) + 2, "%s/%s",
                     dirPath, entry->d_name);

            //получаем информацию о размере
            struct stat st;  //Структура из sys/stat.h
            // printf("Проверка пути: %s\n", fileHeader.path);
            if (stat(fileHeader.path, &st) == -1) {
                perror("Ошибка при получении информации о файле");
                continue;
                // exit(EXIT_FAILURE);
            } else {
                fileHeader.size = st.st_size;
                // printf("Путь к файлу %s: %s и его размер: %ld\n", fileHeader.filename,
                //        fileHeader.path, fileHeader.size);
            }
            (*files)[(*index)++] = fileHeader;
        }
        else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
                 strcmp(entry->d_name, "..") != 0) 
        {
            char subDirPath[1024];
            snprintf(subDirPath, strlen(dirPath) + strlen(entry->d_name) + 2,
                     "%s/%s", dirPath, entry->d_name);
            int error = readFiles(subDirPath, files, alloc_FileHeader, index);
            if (error == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
        }
    }
    closedir(dir);
    return EXIT_SUCCESS;
}

void free_fileHeader(FileHeader* fileHeader) {
    if (fileHeader != NULL) {
        free(fileHeader);
        fileHeader = NULL;
    }
}

void create_arch(int index, char* path_for_arch, char* path_to_arch, FileHeader *files) {
    FILE* archiveFile = fopen(path_to_arch, "wb");
    fprintf(archiveFile, "%s\n", "#arch.bin");
    fprintf(archiveFile, "%d\n", index);
    //Запись информации о файлах
    for (int i = 0; i < index; i++)
    {
        fwrite(files[i].filename, sizeof(char), strlen(files[i].filename) + 1, archiveFile);
        fprintf(archiveFile, "\n%ld\n", files[i].size);
    }

    for (int i = 0; i < index; i++)
    {
        FILE* file = fopen(files[i].path, "rb");

        if (file)
        {
            char buffer[4096];
            size_t bytesRead;
            while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
            {
                fwrite(buffer, 1, bytesRead, archiveFile);
            }
            fclose(file);
        }
        else
        {
            perror("Ошибка при открытии файла для записи");
            fclose(file);
            free_fileHeader(files);
            exit(EXIT_FAILURE);
        }
    }
    fclose(archiveFile);
}