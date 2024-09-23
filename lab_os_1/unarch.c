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
        printf("%s\n%ln\n", files[i].path, &files[i].size);
    }
}

int read_files(char* path_arch) {
  int unpacked = FALSE;
  DIR* dir = opendir(path_arch);
  if (!dir) {
    perror("Ошибка открытия директории");
    exit(EXIT_FAILURE);
  }
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG && strstr(entry->d_name, ".bin") != NULL)
    {
        FileHeader file_header;
        FileHeader fileHeader;
        strncpy(fileHeader.filename, entry->d_name,
            sizeof(fileHeader.filename) - 1);
        fileHeader.filename[sizeof(fileHeader.filename) - 1] = '\0';
        snprintf(fileHeader.path, strlen(path_arch) + strlen(entry->d_name) + 2,
            "%s/%s", path_arch, entry->d_name);
        unarch(file_header.path, path_arch);
        // delete_file(file_header.path);
        unpacked = TRUE;
    }
  }
  closedir(dir);
  return unpacked;
}