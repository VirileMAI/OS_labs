#include "unarch.h"
#include <ctype.h>

void create_dir(const char* path) {
  char command[1024];
  sprintf(command, "mkdir -p %s", path);  // Создание директории
  system(command);
}

// Функция для проверки, является ли строка числом
int is_numeric(const char* str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) {
      return 0;  // Не число
    }
  }
  return 1;  // Число
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
    fclose(archive_file);
    exit(EXIT_FAILURE);
  }

  char pass_or_num[256];
  fgets(pass_or_num, sizeof(pass_or_num), archive_file);

  // Убираем символ новой строки '\n' из прочитанного значения
  pass_or_num[strcspn(pass_or_num, "\n")] = '\0';

  int num_files;  // Объявляем переменную num_files

  if (is_numeric(pass_or_num)) {
    // Число было строкой, это количество файлов в архиве
    printf("Пароль не установлен для этого архива. Продолжаем распаковку.\n");
    num_files = atoi(pass_or_num);  // Присваиваем значение из pass_or_num
  } else {
    // Обработка случая, когда установлен пароль
    char input_pass[256];
    printf("Введите пароль от архива: ");
    fgets(input_pass, sizeof(input_pass), stdin);

    // Убираем символ новой строки '\n' из ввода пользователя
    input_pass[strcspn(input_pass, "\n")] = '\0';

    if (strcmp(pass_or_num, input_pass) != 0) {
      printf("Неверный пароль!\n");
      fclose(archive_file);
      exit(EXIT_FAILURE);
    }

    // Считываем количество файлов после проверки пароля
    fscanf(archive_file, "%d\n", &num_files);
  }

  FileHeader* files = malloc(num_files * sizeof(FileHeader));
  for (int i = 0; i < num_files; i++) {
    fscanf(archive_file, "%s\n%ld\n", files[i].path, &files[i].size);
  }

  char archive_name[1024];
  snprintf(archive_name, sizeof(archive_name), "%s/%s", base_path_for_unarch, "archive_folder");
  create_dir(archive_name);

  for (int i = 0; i < num_files; i++) {
    char buf[4096];
    snprintf(buf, sizeof(buf), "%s/%s", archive_name, files[i].path);

    if (files[i].size == 0 && buf[strlen(buf) - 1] == '/') {
      create_dir(buf);
      continue;
    }

    char dir_path[4096];
    strncpy(dir_path, buf, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';
    char* last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL) {
      *last_slash = '\0';
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
