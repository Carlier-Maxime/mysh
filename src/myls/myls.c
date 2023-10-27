#include "myls.h"
#include "myls_sorter.h"

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#include "macro.h"
#define is_not_point_directory(i) (strcmp(i,".") && strcmp(i,".."))
#define exit_error(condition, message) if(condition){perror(message);exit(1);}
#define exit_error_malloc(pointer) exit_error(pointer==NULL, "Erreur malloc:")

int main(int argc, char* argv[]){
		exec_my_ls(argc-1,argv+1);
}

int exec_my_ls(int argc, char* argv[]){
	int masque_option;
	char** args = treat_arg(argc, argv, &masque_option);
	if(args[0]==NULL){
		char* current_path= getcwd(NULL,0);
		exit_error_malloc(current_path)
		explore_dir(current_path,masque_option);
		free(current_path);
	}else{
		int size = 0;
		for(int i=0;args[i]!=NULL;i++) size+=1;
		sort_dir_path_tab(args,size);
		for(int i=0;i<size;i++){
			explore_dir(args[i],masque_option);
			free(args[i]);

		}
	}
	free(args);

	return 0;
}

char** treat_arg(int argc, char* argv[], int* masque_option){
	*masque_option=0;
	char* current_path= getcwd(NULL,0);
	exit_error(current_path == NULL, "Erreur getcwd: ")
	int current_path_length=strlen(current_path);

	char** dir_tab = malloc(sizeof(char*)*(argc+1));
	exit_error_malloc(dir_tab)
	int cpt=0;
	for(int i=0;i<argc;i++){
		if(argv[i][0]=='-'){
			int res=treat_option(masque_option,argv[i]);
			if(res){
				fprintf(stderr, "Erreur: %d\n", res);
				exit(1);
			}
		}else{
			if(argv[i][0]=='/'){

				dir_tab[cpt]=malloc(sizeof(char)*(strlen(argv[i])+1));
				exit_error_malloc(dir_tab[cpt])
				strcpy(dir_tab[cpt],argv[i]);
				cpt++;
			}else{
				int length_path = strlen(argv[i])+2+current_path_length;
				dir_tab[cpt]=malloc(sizeof(char)*length_path);
				exit_error_malloc(dir_tab[cpt])
				sprintf(dir_tab[i],"%s/%s",current_path,argv[i]);
				cpt++;
			}
		}

	}
	dir_tab[cpt]=NULL;
	free(current_path);
	return dir_tab;
}
int treat_option(int* masque_option, char* option){
	if(*option!='-'){
		//erreur
		return NOT_AN_OPTION;
	}
	int i=1;
	while(option[i]!='\0'){
		if(option[i]=='a'){
			*masque_option=*masque_option|1;
		}else if(option[i]=='R'){
			*masque_option=*masque_option|2;
		}else{
			return NO_OPTION_IN_STRING;
		}
		i++;

	}
	return i>1?0:OPTION_DONT_EXISTS;
}

void explore_dir(char * dir_path, int masque_option){
	DIR * dir = opendir(dir_path);
	exit_error(dir==NULL,"Erreur opendir: ")
	struct dirent* directory;

	int file_max_size=10;
	int file_size=0;
	full_file * file_tab = malloc(sizeof(full_file)*file_max_size);
	exit_error_malloc(file_tab)
	int directory_max_size=10;
	int directory_size=0;
	char** directory_tab = malloc(sizeof(char*)*directory_max_size);
	exit_error_malloc(directory_tab)
	printf("\n%s:\n",get_dir_name(dir_path));
	int block_total=0;
	errno=0;
	while((directory= readdir(dir))!=NULL){

		
		char* path = malloc(sizeof(char)*(strlen(dir_path)+strlen(directory->d_name)+2));
		
		exit_error_malloc(directory_tab)
		sprintf(path,"%s/%s",dir_path,directory->d_name);
		struct stat * file = malloc(sizeof(struct stat));
		exit_error_malloc(file)
		
		exit_error(stat(path,file)==-1, "Erreur stat: ")
		if(
			(!S_ISDIR(file->st_mode) || !(masque_option & 2) || !is_not_point_directory(directory->d_name))&&
			(masque_option & 1 || directory->d_name[0]!='.')
		){
			if(file_size>=file_max_size){
				file_max_size*=2;
				file_tab=realloc(file_tab, sizeof(full_file)*file_max_size);
				exit_error(file_tab==NULL,"Erreur realloc")
			}
			(file_tab+file_size)->name=malloc((strlen(directory->d_name)+1)*sizeof(char));
			exit_error_malloc((file_tab+file_size)->name)
			strcpy((file_tab+file_size)->name, directory->d_name);
			block_total+=file->st_blocks*512/1024;
			(file_tab+file_size)->path=path;
			(file_tab+file_size)->file=file;
			file_size++;
		}else if(masque_option & 2 && is_not_point_directory(directory->d_name) && (masque_option & 1 || directory->d_name[0]!='.')){
			if(directory_size>=directory_max_size){
				directory_max_size*=2;
				directory_tab=realloc(directory_tab, sizeof(char*)*directory_max_size);
				exit_error(directory_tab==NULL,"Erreur realloc")
			}
			*(directory_tab+directory_size)=path;

			directory_size++;
			//("%d\n",directory_size );
			free(file);
		}else{
			free(path);

			free(file);
		}
	}
	exit_error(errno, "Erreur readdir:")

	printf("total %d\n",block_total);
	sort_file_tab(file_tab,file_size);
	for(int i=0;i<file_size; i++){
		
		print_file((file_tab+i)->name,(file_tab+i)->file);
		free((file_tab+i)->name);
		free((file_tab+i)->path);
		free((file_tab+i)->file);

	}
	free(file_tab);


	sort_dir_path_tab(directory_tab,directory_size);

	for(int i=0;i<directory_size;i++){

		explore_dir(/**(dir_path_tab+i)*/ *(directory_tab+i),masque_option);
		
		free(*(directory_tab+i));
	}
	free(directory_tab);

	//free(dir_path_tab);
	closedir(dir);

}

// droit, number of linked hard-link, owner, group, size, last modif time, nom (bleu=dossier, vert=executable)
void print_file(char* name,struct stat* file){
	char droits[12];
	char date[16];

	int is_executable = get_access_right_string(droits, file->st_mode);
	get_date(date,file);
	errno=0;
	char * owner_name = getpwuid(file->st_uid)->pw_name;
	exit_error(owner_name == NULL && !errno, "Erreur getpwuid: ")

	char * group_name = getgrgid(file->st_gid)->gr_name;
	exit_error(group_name == NULL && !errno, "Erreur getgrid: ")
	if(owner_name == NULL) owner_name = " ";
	if(group_name == NULL) group_name = " ";
	if(S_ISDIR(file->st_mode)){
		printf("%s %ld %s %s %ld %s %s%s%s\n",droits,file->st_nlink,owner_name,group_name, file->st_size,date,BLUE_BEGIN,name,COLOR_RESET);
	}else if(is_executable){
		printf("%s %ld %s %s %ld %s %s%s%s\n",droits,file->st_nlink,owner_name,group_name, file->st_size,date,GREEN_BEGIN,name,COLOR_RESET);
	}else{
		printf("%s %ld %s %s %ld %s %s\n",droits,file->st_nlink,owner_name,group_name, file->st_size,date,name);
	}
	
}


int get_access_right_string(char* string, int st_mode){
	string[0]= S_ISDIR(st_mode)? 'd':'-';
	int tmp = st_mode;
	int executable = 0;
	for(int i=0;i<9;i++){
		int modulo = i%3;
		int idx = 8-(i/3)*3-modulo+1;
		if(tmp & 1){
			string[idx] = modulo==2?'r':modulo==1?'w':'x';
			if(modulo==0) {
				executable =1;
			}
		}else{
			string[idx]='-';
		}
		tmp = tmp>>1;
	}
	string[10]='\0';
	return executable;

}

void get_date(char* string,struct stat* file){
	const char* months[12]={"janv.","févr.","mars","avril","mai","juin","juil.","août", "sept.","oct.","nov.","déc."};

	struct tm* date = gmtime(&(file->st_mtime));
	exit_error(date == NULL, "Erreur gmtime")
	int min=date->tm_min;
	int hour=date->tm_hour;
	int day = date->tm_mday;
	int month=date->tm_mon;
	int year=date->tm_year;
	time_t current_time = time(NULL);
	exit_error((int)current_time == -1, "Erreur time:")

	struct tm* current_date = gmtime(&current_time);
	exit_error(current_date == NULL, "Erreur gmtime")
	if(current_date->tm_mon == month && current_date->tm_year == year){
		sprintf(string,"%s  %d %d:%d",months[month], day, hour, min);
	}else{
		sprintf(string,"%s  %d  %d",months[month], day,year+1900);
	}

}
char* get_dir_name(char* path){
	char* res=path;
	for(int i=0;path[i]!='\0';i++){
		if(path[i]=='/') res=path+i+1;
	}
	return res;
}




