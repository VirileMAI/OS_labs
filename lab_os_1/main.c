#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  char filename[4097] ;
  long size;
} FileInfo;

void createDir(const char* path) {
  char command[4096];
  sprintf(command, "mkdir -p %s", path);
  system(command);
}
void deleteFile(const char* path) {
  char command[4096];
  sprintf(command, "rm %s", path);
  system(command);
}

void createTreeOfDirs(FileInfo* files, int numFiles, char* dirPath2) {
  for (int i = 0; i < numFiles; ++i) {
    char buff[4097];
    strcpy(buff, files[i].filename);
    for (int j = strlen(files[i].filename) - 1; files[i].filename[j] != '/';
         j--) {
      buff[j] = '\0';
    }
    char buf3[4097] = "";
    char* buf2 = strtok(buff, "/");
    while (buf2 != NULL) {
      strcat(buf3, buf2);
      char buf4[4097] = "";
      strcpy(buf4, dirPath2);
      strcat(buf4, "/");
      strcat(buf4, buf3);
      createDir(buf4);
      strcat(buf3, "/");  // добавляем разделитель "/"
      buf2 = strtok(NULL, "/");
    }
  }
}

int unarchive(char* dirPath, char* dirPath2) {
  FILE* archiveFile = fopen(dirPath, "rb");
  if (!archiveFile) {
    perror("Ошибка при открытии архивного файла");
    return EXIT_FAILURE;
  }
  char str[256];
  fgets(str, sizeof(str), archiveFile);
  int a = strcmp(str, "#arch.bin\n");
  if (a != 0) {
    printf("%s", "Данный файл не является архивом\n");
    return -1;
  }
  int numFiles;
  fscanf(archiveFile, "%d\n", &numFiles);

  FileInfo* files = malloc(numFiles * sizeof(FileInfo));

  for (int i = 0; i < numFiles; ++i) {
    fscanf(archiveFile, "%s\n%ld\n", files[i].filename, &files[i].size);
    printf("%s\n%ln\n", files[i].filename, &files[i].size);
  }
  // createTreeOfDirs(files, numFiles, dirPath2);
  for (int i = 0; i < numFiles; ++i) {
    char buf[4097];
    strcpy(buf, dirPath2);
    strcat(buf, "/");
    strcat(buf, files[i].filename);
    FILE* outputFile = fopen(buf, "wb");
    if (!outputFile) {
      perror("Ошибка при создании файла");
      return EXIT_FAILURE;
    }
    char* buffer = malloc(files[i].size);
    fread(buffer, 1, files[i].size, archiveFile);
    fwrite(buffer, 1, files[i].size, outputFile);

    fclose(outputFile);
    free(buffer);
  }
  fclose(archiveFile);
  free(files);
  return 1;
}

int readFiles(char* dirPath) {
  int unpacked = 0;
  // Откроем директорию
  DIR* dir = opendir(dirPath);
  if (!dir) {
    perror("Ошибка при открытии директории");
    exit(EXIT_FAILURE);
  }
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG && strstr(entry->d_name, ".bin") != NULL) {
      FileInfo fileInfo;
      snprintf(fileInfo.filename, sizeof(dirPath) + sizeof(entry->d_name),
               "%s/%s", dirPath, entry->d_name);
      unarchive(fileInfo.filename, dirPath);
      deleteFile(fileInfo.filename);
      unpacked = 123;
    } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
               strcmp(entry->d_name, "..") != 0) {
      FileInfo fileInfo;
      char subDirPath[4097];
      snprintf(subDirPath, sizeof(dirPath) + sizeof(entry->d_name), "%s/%s",
               dirPath, entry->d_name);
      int yourMotherIsSlut = readFiles(subDirPath);
      if (yourMotherIsSlut == 123) unpacked = yourMotherIsSlut;
    }
  }
  closedir(dir);
  return unpacked;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(
        stderr,
        "Использование: %s <путь_к_директории> <путь_к_выходной_директории>\n",
        argv[0]);
    return EXIT_FAILURE;
  }
  char* dirPath = argv[1];
  char* dirPath2 = argv[2];
  unarchive(dirPath, dirPath2);
  int unpacked = 123;
  while (unpacked == 123) {
    unpacked = readFiles(dirPath2);
  }

  return EXIT_SUCCESS;
}