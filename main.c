#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

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

//C++ templates
extern const char _binary_templates_cpp_main_cpp_start;
extern const char _binary_templates_cpp_main_cpp_end;
extern const char _binary_templates_cpp_Makefile_start;
extern const char _binary_templates_cpp_Makefile_end;

bool new_dir(const char path[]);
bool write_new_file(const char path[], const char* data, size_t data_size);
bool write_c_project();
bool write_argo_project();
bool write_cpp_project();
bool write_header_file(const char* name);
void print_help();
void print_version();
enum language
{
    C,
    CPLUSPLUS,
    HEADER,
};
struct args
{
    const char* dir;
    enum language lang;
    bool help;
    bool version;
    bool ok;
};
struct args parse_args(const int argc, const char* const argv[]);

int main(int argc, char* argv[])
{
    const char* dir = "argo";
    bool (*write_project)() = write_argo_project;

    struct args args = parse_args(argc, (char const * const *)argv);
    if(args.help)
    {
        print_help();
        return 0;
    }
    else if(args.version)
    {
        print_version();
        return 0;
    }
    else if(!args.ok)
    {
        print_help();
        return EXIT_FAILURE;
    }
    else
    {
        dir = args.dir;
        switch(args.lang)
        {
            case C:
                if(strcmp("argo", dir)) write_project = write_c_project;
                break;
            case CPLUSPLUS:
                write_project = write_cpp_project;
                break;
            case HEADER:
                write_project = NULL;
                break;
            default:
                assert(false);
        }
    }

    if(write_project)
    {
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
    }
    else //Write new header file
    {
        write_header_file(dir);
    }
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
        write_new_file("main.c", &_binary_main_c_start, &_binary_main_c_end - &_binary_main_c_start) &&
        write_new_file("Makefile", &_binary_Makefile_start, &_binary_Makefile_end - &_binary_Makefile_start) &&
        new_dir("templates") &&
        new_dir("templates/c") &&
        write_new_file("templates/c/main.c", &_binary_templates_c_main_c_start, &_binary_templates_c_main_c_end - &_binary_templates_c_main_c_start) &&
        write_new_file("templates/c/Makefile", &_binary_templates_c_Makefile_start, &_binary_templates_c_Makefile_end - &_binary_templates_c_Makefile_start) &&
        new_dir("templates/cpp") &&
        write_new_file("templates/cpp/main.cpp", &_binary_templates_cpp_main_cpp_start, &_binary_templates_cpp_main_cpp_end - &_binary_templates_cpp_main_cpp_start) &&
        write_new_file("templates/cpp/Makefile", &_binary_templates_cpp_Makefile_start, &_binary_templates_cpp_Makefile_end - &_binary_templates_cpp_Makefile_start);

}

bool write_cpp_project()
{
    return
        write_new_file("main.cpp", &_binary_templates_cpp_main_cpp_start, &_binary_templates_cpp_main_cpp_end - &_binary_templates_cpp_main_cpp_start) &&
        write_new_file("Makefile", &_binary_templates_cpp_Makefile_start, &_binary_templates_cpp_Makefile_end - &_binary_templates_cpp_Makefile_start);
}

/* Mangle a character for use in header file include guard
Alphabetic character to upper case and non-alphanumeric to '_'
*/
char include_guard_character(char c)
{
    if(isalnum(c)) return toupper(c);
    return '_';
}

bool write_header_file(const char* name)
{
    const char* fmt = "%s";
    if(isdigit(name[0])) fmt = "_%s";
    char guard_id_buf[1 + snprintf(NULL, 0, fmt, name)];
    int written = snprintf(guard_id_buf, sizeof guard_id_buf, fmt, name);
    assert(written + 1 == sizeof guard_id_buf);
    assert('\0' == guard_id_buf[written]);
    //Mangle guard id
    for(char* p = guard_id_buf; *p; ++p)
    {
        *p = isalnum(*p) ? toupper(*p) : '_';
    }

    int res = fprintf(stdout,
        "#ifndef %s\n"
        "#define %s\n"
        "\n"
        "#endif //%s\n",
        guard_id_buf, guard_id_buf, guard_id_buf
    );
    //TODO: Write to file instead of stdout. fopen(<filename>, "wx");
    //TODO: Add .h suffix to filename if missing.
    //TODO: Error handling for fprintf
    return true;
}

void print_help()
{
    //TODO: document header "<language>"
    printf(
        "Utility for creating a new C/C++ project starting point.\n"
        "Usage: argo [option] <language> <name>\n"
        "\n"
        "<language>:\n"
        "   C, c   Create a new C project\n"
        "   C++, c++    Create a new C++ project\n"
        "<name> the new project. Creates a new directory with the project name.\n"
        "[option]:\n"
        "   --help      This help text\n"
        "   --version   Build date\n"
    );
}

void print_version()
{
    #ifdef BUILD_DATE
    printf("Build date: " BUILD_DATE "\n");
    #else
    #pragma GCC warning "BUILD_DATE macro not defined. Falling back on default build date string formatting."
    printf("Build date: "__DATE__" "__TIME__"\n");
    #endif
}

struct args parse_args(const int argc, const char* const argv[])
{
    struct args args = {.ok = true};
    enum {
        LANG,
        DIR,
    };
    int pos = LANG; //positional argument
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp("--help", argv[i]))
        {
            args.help = true;
        }
        else if(!strcmp("--version", argv[i]))
        {
            args.version = true;
        }
        else if(LANG == pos)
        {
            if(!strcmp("C", argv[i]) || !strcmp("c", argv[i]))
            {
                args.lang = C;
            }
            else if(!strcmp("C++", argv[i]) || !strcmp("c++", argv[i]))
            {
                args.lang = CPLUSPLUS;
            }
            else if(!strcmp("header", argv[i]))
            {
                args.lang = HEADER;
            }
            else
            {
                args.ok = false;
            }
            ++pos;
        }
        else if(DIR == pos)
        {
            args.dir = argv[i];
            ++pos;
        }
        else
        {
            args.ok = false;
            return args;
        }
    }
    args.ok = args.ok && (pos > DIR);
    return args;
}
