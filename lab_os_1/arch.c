#include "arch.h"

int readFiles(const char* dirPath, FileHeader** files,
              int* alloc_FileHeader, int* index, const char* base_name, const char* relative_path) {
    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Ошибка при открытии директории");
        exit(EXIT_FAILURE);
    }
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            // Проверка выделения памяти для файлов
            if (*index >= *alloc_FileHeader) {
                int new_size = *alloc_FileHeader + BLOCK_SIZE;
                FileHeader* new_fileHeader = realloc(*files, sizeof(FileHeader) * new_size);
                if (new_fileHeader == NULL) {
                    free(*files);
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

            // Формируем полный путь к файлу
            snprintf(fileHeader.path, sizeof(fileHeader.path), "%s/%s", dirPath, entry->d_name);

            // Формируем относительный путь к файлу
            char relative_file_path[1024];
            snprintf(relative_file_path, sizeof(relative_file_path), "%s/%s", relative_path, entry->d_name);

            // Получаем информацию о размере
            struct stat st;
            if (stat(fileHeader.path, &st) == -1) {
                perror("Ошибка при получении информации о файле");
                continue;
            } else {
                fileHeader.size = st.st_size;
            }

            // Сохраняем относительный путь в filename
            strncpy(fileHeader.filename, relative_file_path, sizeof(fileHeader.filename) - 1);
            fileHeader.filename[sizeof(fileHeader.filename) - 1] = '\0';

            (*files)[(*index)++] = fileHeader;
        } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subDirPath[1024];
            snprintf(subDirPath, sizeof(subDirPath), "%s/%s", dirPath, entry->d_name);

            // Обновляем относительный путь для подкаталога
            char new_relative_path[1024];
            snprintf(new_relative_path, sizeof(new_relative_path), "%s/%s", relative_path, entry->d_name);

            // Рекурсивно обрабатываем подкаталоги
            readFiles(subDirPath, files, alloc_FileHeader, index, base_name, new_relative_path);
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