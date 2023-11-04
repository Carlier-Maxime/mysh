#ifndef MYLS
#define MYLS


#define NO_COLOR_OPTION "--no-color"

#include <dirent.h>
#include <sys/stat.h>

typedef struct {
	char* name;
	char* path;
	struct stat* file;
} full_file;

typedef struct{
	char* path;
	char* absolute_path;
}precise_path;



int exec_my_ls(int argc, char* argv[]);
char** treat_arg(int argc, char* argv[], int* masque_option);
int treat_option(int* masque_option, char* option);
int explore_file(char* path, int masque_option, full_file* file_tab, int idx);
int explore_dir(char * dir_path, int masque_option, int display_dir_name);

int print_file(char* name,struct stat* file, int masque_option, int size_length);
void sort_dir_path_tab(char** dir_path, int size);
void sort_file_tab(full_file* file_tab, int size);


int push_file(struct stat*,int* file_size, int* file_max_size, full_file** file_tab, char* name, char* path);
int push_directory(char* path,int* directory_size, int* directory_max_size, char*** directory_tab);

int explore_files_alones(char** args,int size, int masque_option);
/*#define STACK_SIZE 10
typedef struct _s*/
#endif
