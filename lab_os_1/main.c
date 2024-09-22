#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Определим структуру для хранения информации о файлах
typedef struct
{
    char filename[4097];
    long size;
} FileInfo;

// Функция для чтения файлов из директории и сохранения их информации
int readFiles(const char* dirPath, FileInfo* files, const int* alloc_fileInfos, int* index)
{
    // Откроем директорию
    DIR* dir = opendir(dirPath);
    if (!dir)
    {
        perror("Ошибка при открытии директории");
        exit(EXIT_FAILURE);
    }
    // char * mainDir = mainDirName(dirPath);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {  // Обычный файл
            FileInfo fileInfo;
            snprintf(fileInfo.filename, sizeof(dirPath) + sizeof(entry->d_name), "%s/%s", dirPath, entry->d_name);

            // Получим размер файла
            FILE* file = fopen(fileInfo.filename, "rb");
            if (file)
            {
                fseek(file, 0, SEEK_END);
                fileInfo.size = ftell(file);
                fclose(file);
            }
            else
            {
                perror("Ошибка при открытии файла");
                exit(EXIT_FAILURE);
            }

            // если файлов больше, чем заданно
            if (*index >= *alloc_fileInfos)
            {
                int new_size = *alloc_fileInfos + 10;
                FileInfo* new_fileInfoList = realloc(files, (sizeof(FileInfo)) * new_size);
                if (new_fileInfoList == NULL)
                {
                    free(files);
                    return -1;
                }
                files = new_fileInfoList;
            }
            // Добавим в список
            files[(*index)++] = fileInfo;
        }
        else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Рекурсивно обойдем поддиректории
            char subDirPath[4097];
            snprintf(subDirPath, sizeof(dirPath) + sizeof(entry->d_name), "%s/%s", dirPath, entry->d_name);
            int error_code = readFiles(subDirPath, files, alloc_fileInfos, index);
            if (error_code)
            {
                return error_code;
            }
        }
    }

    closedir(dir);
    return 0;
}

void free_file_info(FileInfo* fileInfo)
{
    if (fileInfo != NULL)
    {
        free(fileInfo);
        fileInfo = NULL;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Использование: %s <путь_к_входной_директории> <путь_к_выходной_директории_с_названием_архивного_файла_(без_расширения!)>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int index = 0;
    char* dirPath = argv[1];
    char* dirPath2 = argv[2];
    strcat(dirPath2, ".bin");
    int alloc_fileInfos = 30;
    FileInfo* FileInfoListBig = malloc(sizeof(FileInfo) * alloc_fileInfos);
    readFiles(dirPath, FileInfoListBig, &alloc_fileInfos, &index);

    // Создадим выходной архивный файл
    FILE* archiveFile = fopen(dirPath2, "wb");  ////////////
    if (!archiveFile)
    {
        perror("Ошибка при создании архивного файла");
        return EXIT_FAILURE;
    }

    // Запишем уникальный заголовок
    fprintf(archiveFile, "%s\n", "#archive.bin");
    // Запишем заголовок с количеством файлов

    fprintf(archiveFile, "%d\n", index);

    // Обрезаем изначальный путь до папки
    for (int i = strlen(dirPath) - 1; dirPath[i] != '/'; i--)
    {
        dirPath[i] = '\0';
    }

    // Запишем информацию о файлах
    for (int i = 0; i < index; ++i)
    {
        char buff[4097];
        for (int j = strlen(dirPath); j < strlen(dirPath) + strlen(FileInfoListBig[i].filename); ++j)
        {
            // for(int j = 0; j < strlen(FileInfoListBig[i].filename); ++j){
            buff[j - strlen(dirPath)] = FileInfoListBig[i].filename[j];
        }
        fwrite(buff, sizeof(char), strlen(FileInfoListBig[i].filename) - strlen(dirPath) + 1, archiveFile);
        // fwrite(buff, sizeof(char), strlen(FileInfoListBig[i].filename), archiveFile);
        fprintf(archiveFile, "\n%ld\n", FileInfoListBig[i].size);
    }
    // Запишем содержимое файлов
    for (int i = 0; i < index; ++i)
    {
        FILE* inputFile = fopen(FileInfoListBig[i].filename, "rb");
        if (inputFile)
        {
            char buffer[4097];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0)
            {
                fwrite(buffer, 1, bytesRead, archiveFile);
            }
            fclose(inputFile);
        }
        else
        {
            perror("Ошибка при открытии входного файла");
            fclose(inputFile);
            free_file_info(FileInfoListBig);
            return EXIT_FAILURE;
        }
    }

    fclose(archiveFile);
    free_file_info(FileInfoListBig);
    FileInfoListBig = NULL;
    printf("Архив успешно создан\n");
    return EXIT_SUCCESS;
}
