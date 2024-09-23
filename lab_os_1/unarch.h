#ifndef UNARCH
#define UNARCH
#define FALSE 0
#define TRUE 1


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
  char filename[1024];
  char path[1024];
  long size;
} FileHeader;

void create_dir(const char* path);

void delete_file(const char* path);

int unarch(char* path_to_arch,char* path_for_unarch);

#endif