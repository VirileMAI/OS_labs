#include "unarch.h"

void create_dir(const char* path) {
  char command[1024];
  sprintf(command, "mkdir -p %s", path);
  system(command);
}

void delete_file(const char* path) {
  char command[1024];
  sprintf(command, "rm-rf %s", path);
  system(command);
}

int unarch(char* path_to_arch,char* path_for_unarch)
{
    FILE* archive_file = fopen(path_to_arch, "rb");
    if (!archive_file)
    {
        perror("Ошибка при открытии архивного файла");
        exit(EXIT_FAILURE);
    }
    char str[256];
    fgets(str, sizeof(str), archive_file);
    int a = strcmp(str, "#arch.bin\n");
    if (a!=0)
    {
        printf("%s", "Данный файл не является архивом");
        exit(EXIT_FAILURE);
    }
    int num_files;
    fscanf(archive_file, "%d\n", &num_files);
    printf("%d\n", num_files);

    FileHeader* files = malloc(num_files * sizeof(FileHeader));

    for (int i = 0; i < num_files; i++) 
    {
        fscanf(archive_file, "%s\n%ld\n", files[i].path, &files[i].size);
        printf("%s\n%ld\n", files[i].path, files[i].size);
    }
    for (int i = 0; i < num_files; i++)
    {
      char buf[4096];
      // printf("Файл %d: %s\n", i, files[i].path);
      snprintf(buf, sizeof(buf), "%s%s", path_for_unarch, files[i].path);
      printf("%s\n", buf);

      char dir_path[4096];
      strncpy(dir_path, buf, sizeof(dir_path) - 1);
      dir_path[sizeof(dir_path) - 1] = '\0';
    
      // Убираем имя файла, чтобы получить путь к директории
      char* last_slash = strrchr(dir_path, '/');
      if (last_slash != NULL) {
          *last_slash = '\0';  // Заменяем последний слеш на конец строки
      }

      // Создаем директорию
      create_dir(dir_path);

      FILE* output_file = fopen(buf, "wb");
      if (!output_file)
      {
        perror("Ошибка при создании файла");
        exit(EXIT_FAILURE);
      }
      char* buffer = malloc(files[i].size);
      fread(buffer, 1, files[i].size, archive_file);
      fwrite(buffer, 1, files[i].size, output_file);

      fclose(output_file);
      free(buffer);
    }
    fclose(archive_file);
    free(files);
    return EXIT_SUCCESS;
}