
#include <sys/stat.h>


int get_access_right_string(char* string, unsigned long st_mode);
int get_date(char* date,struct stat* file);
char* get_name(char* path);
char* get_original_name(char* path);
void convert_to_time_number(int number, char* time_number);
void convert_to_date_number(int number, char* date_number);
char* get_path_from_root(char* root_path, char* file_path);
unsigned long get_number_length(unsigned long number);