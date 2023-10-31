#ifndef MYLS
#define MYLS

#define NOT_AN_OPTION 3
#define NO_OPTION_IN_STRING 2
#define OPTION_DONT_EXISTS 1

#include <dirent.h>
#include <sys/stat.h>

typedef struct {
	char* name;
	char* path;
	struct stat* file;
} full_file;

int exec_my_ls(int argc, char* argv[]);
char** treat_arg(int argc, char* argv[], int* masque_option);
int treat_option(int* masque_option, char* option);

int explore_dir(char * dir_path, int masque_option);

int print_file(char* name,struct stat* file);
int get_access_right_string(char* string, int st_mode);
int get_date(char* date,struct stat* file);
char* get_dir_name(char* path);
void sort_dir_path_tab(char** dir_path, int size);
void sort_file_tab(full_file* file_tab, int size);


int push_file(struct stat*,int* file_size, int* file_max_size, full_file** file_tab, char* name, char* path);
int push_directory(char* path,int* directory_size, int* directory_max_size, char*** directory_tab);
/*#define STACK_SIZE 10
typedef struct _s*/
#endif
