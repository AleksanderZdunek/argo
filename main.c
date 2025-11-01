#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Self-referential source
extern const char _binary_main_c_start;
extern const char _binary_main_c_end;
extern const char _binary_Makefile_start;
extern const char _binary_Makefile_end;

//C templates
extern const char _binary_templates_c_main_c_start;
extern const char _binary_templates_c_main_c_end;
extern const char _binary_templates_c_Makefile_start;
extern const char _binary_templates_c_Makefile_end;

bool new_dir(const char path[]);
bool write_new_file(const char path[], const char* data, size_t data_size);
bool write_c_project();
bool write_argo_project();

int main(int argc, char* argv[])
{
    const char* dir = "argo";
    bool (*write_project)() = write_argo_project;
    if(argc > 1)
    {
        dir = argv[1];
        if(strcmp(dir, "argo")) write_project = write_c_project;
    }
    //TODO: Print use help

    //Create new project directory
    if(!new_dir(dir)) return EXIT_FAILURE;

    //Important to change to new empty directory before writing files
    if(chdir(dir))
    {
        fprintf(stderr, "error: opening new project dir ./%s\n", dir);
        fprintf(stderr, "errno: %d; ", errno); perror("chdir()");
        return EXIT_FAILURE;
    }

    //Write files
    if(!write_project()) return EXIT_FAILURE;

    printf("Created project ./%s\n", dir);
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

bool write_c_project()
{
    return
        write_new_file("main.c", &_binary_templates_c_main_c_start, &_binary_templates_c_main_c_end - &_binary_templates_c_main_c_start) &&
        write_new_file("Makefile", &_binary_templates_c_Makefile_start, &_binary_templates_c_Makefile_end - &_binary_templates_c_Makefile_start);
}

bool write_argo_project()
{
    return
        new_dir("templates") &&
        new_dir("templates/c") &&
        write_new_file("main.c", &_binary_main_c_start, &_binary_main_c_end - &_binary_main_c_start) &&
        write_new_file("Makefile", &_binary_Makefile_start, &_binary_Makefile_end - &_binary_Makefile_start) &&
        write_new_file("templates/c/main.c", &_binary_templates_c_main_c_start, &_binary_templates_c_main_c_end - &_binary_templates_c_main_c_start) &&
        write_new_file("templates/c/Makefile", &_binary_templates_c_Makefile_start, &_binary_templates_c_Makefile_end - &_binary_templates_c_Makefile_start);

}
