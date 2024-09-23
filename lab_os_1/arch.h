#ifndef ARCH
#define ARCH
#define BLOCK_SIZE 10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
typedef struct {
    char filename[1024];
    char path[1024];
    char otn_path[1024];
    long size;
}FileHeader;
int readFiles(const char* dirPath, FileHeader** files, int* alloc_FileHeader, int* index, const char* base_name, const char* relative_path);
void free_fileHeader(FileHeader *fileHeader);
void create_arch(int index, char* path_for_arch, char* path_to_arch, FileHeader *files);
#endif