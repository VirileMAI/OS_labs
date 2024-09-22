#include "arch.h"

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        int index = 0;
        char path_for_arch[1024] = "/mnt/a/LR_OS/lab_1";
        // char* path_for_arch = argv[1];
        char* path_to_arch = argv[1];
        strcat(path_to_arch, ".bin");
        int alloc_fileInfos = 30;
        FileHeader* FileHeaderList = malloc(sizeof(FileHeader) * alloc_fileInfos);
        readFiles(path_for_arch, &FileHeaderList, &alloc_fileInfos, &index);
        create_arch(index, path_for_arch, path_to_arch, FileHeaderList);
        free_fileHeader(FileHeaderList);
        FileHeaderList = NULL;
        printf("Архив создан успешно\n");
        exit(EXIT_SUCCESS);
    }
}