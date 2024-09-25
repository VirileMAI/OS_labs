#include "unarch.h"

void create_dir(const char* path) {
  char command[1024];
  sprintf(command, "mkdir -p %s", path);
  system(command);
}

int unarch(char* path_to_arch, char* base_path_for_unarch) {
  FILE* archive_file = fopen(path_to_arch, "rb");
  if (!archive_file) {
    perror("Ошибка при открытии архивного файла");
    exit(EXIT_FAILURE);
  }

  char str[256];
  fgets(str, sizeof(str), archive_file);
  if (strcmp(str, "#arch.bin\n") != 0) {
    printf("%s", "Данный файл не является архивом");
    exit(EXIT_FAILURE);
  }
  char pass[256];
  fgets(pass, sizeof(pass), archive_file);

  char input_pass[256];
  printf("Введите пароль от архива: ");
  fgets(input_pass, sizeof(input_pass), stdin);

  if (strcmp(pass, input_pass) != 0) {
    printf("Неверный пароль!\n");
    fclose(archive_file);
    exit(EXIT_FAILURE);
  }

  int num_files;
  fscanf(archive_file, "%d\n", &num_files);

  FileHeader* files = malloc(num_files * sizeof(FileHeader));
  for (int i = 0; i < num_files; i++) {
    fscanf(archive_file, "%s\n%ld\n", files[i].path, &files[i].size);
  }

  // Создание директории с именем архива
  char archive_name[1024];
  snprintf(archive_name, sizeof(archive_name), "%s/%s", base_path_for_unarch,
           "archive_folder");  // Измените "archive_folder" на нужное имя
  create_dir(archive_name);

  for (int i = 0; i < num_files; i++) {
    char buf[4096];
    snprintf(buf, sizeof(buf), "%s/%s", archive_name, files[i].path);

    // Создание директории для каждого файла
    char dir_path[4096];
    strncpy(dir_path, buf, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';
    char* last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL) {
      *last_slash =
          '\0';  // Убираем имя файла, чтобы получить путь к директории
    }
    create_dir(dir_path);

    FILE* output_file = fopen(buf, "wb");
    if (!output_file) {
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