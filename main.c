#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

extern const char _binary_main_c_start;
extern const char _binary_main_c_end;
extern const char _binary_Makefile_start;
extern const char _binary_Makefile_end;

bool new_dir(const char path[]);
bool write_new_file(const char path[], const char* data, size_t data_size);

int main(int argc, char* argv[])
{
    const char dir[] = "./argo";
    if(!new_dir(dir))
    {
        return EXIT_FAILURE;
    }

    if(!write_new_file("./argo/main.c", &_binary_main_c_start, &_binary_main_c_end - &_binary_main_c_start))
    {
        return EXIT_FAILURE;
    }
    if(!write_new_file("./argo/Makefile", &_binary_Makefile_start, &_binary_Makefile_end - &_binary_Makefile_start))
    {
        return EXIT_FAILURE;
    }

    printf("Created project %s\n", dir);
    return 0;
}

bool new_dir(const char path[])
{
    //Check if directory already exists. If so, abort.
    struct stat res = {0};
    if(0 == stat(path, &res)) { //Destination exists
        fprintf(stderr, "error: destination %s already exists\n", path);
        return false;
    } else if(ENOENT != errno) //Unexpected error. Path _probably_ exists
    {
        fprintf(stderr, "error: destination %s error\n", path);
        fprintf(stderr, "errno: %d; ", errno); perror("stat()");
        return false;
    }

    //Make directory
    if(mkdir(path, 0777)) //Error making directory
    {
        fprintf(stderr, "error: error creating destination %s\n", path);
        fprintf(stderr, "errno: %d; ", errno); perror("mkdir()");
        return false;
    }

    return true;
}

bool write_new_file(const char path[], const char* data, size_t data_size)
{
    FILE* file = fopen(path, "w");
    if(!file)
    {
        fprintf(stderr, "error: opening file %s\n", path);
        fprintf(stderr, "errno: %d; ", errno); perror("fopen()");
        return false;
    }

    if(fwrite(data, 1, data_size, file) != data_size)
    {
        fprintf(stderr, "error: writing file %s\n", path);
        fprintf(stderr, "errno: %d; ", errno); perror("fwrite()");
        return false;
    }

    fclose(file);
    return true;
}
