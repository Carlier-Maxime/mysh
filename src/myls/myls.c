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

#include "../utils/macro.h"
#include "../utils/Error.h"
#define is_not_point_directory(i) (strcmp(i,".") && strcmp(i,".."))
#define exit_error(condition, message) if(condition){perror(message);exit(1);}
#define exit_error_malloc(pointer) exit_error(pointer==NULL, "Erreur malloc:")

int main(int argc, char* argv[]){
		int res = exec_my_ls(argc-1,argv+1);
		if(res){
			Error_PrintErrorMsg("MYLS:");
		}
		exit(res);
}

int exec_my_ls(int argc, char* argv[]){
	int return_value=0;
	int masque_option;
	char** args = treat_arg(argc, argv, &masque_option); //a désalouer à la fin
	if(args == NULL){
		return_value=1;
	}else{

		if(args[0]==NULL){
			char* current_path= getcwd(NULL,0);
			//exit_error_malloc(current_path)
			if(current_path == NULL){
				return_value=1;
				Error_SetError(ERROR_GETCWD);
			}else{

				if(explore_dir(current_path,masque_option)){
					return_value = 1;
				}
				free(current_path);
			}
		}else{
			int size = 0;
			for(int i=0;args[i]!=NULL;i++) size+=1;
			sort_dir_path_tab(args,size);
			for(int i=0;i<size && !return_value;i++){
				if(explore_dir(args[i],masque_option)){
					return_value=1;
				}
				

			}
			for(int i=0;i<size;i++){
				free(args[i]);
			}
		}
		free(args);
	}

	return return_value;
}

char** treat_arg(int argc, char* argv[], int* masque_option){
	char** return_value=NULL;
	int erreur=0;
	*masque_option=0;
	char* current_path= getcwd(NULL,0);
	if(current_path == NULL){
		erreur=1;
		Error_SetError(ERROR_GETCWD);
	}else{
		int current_path_length=strlen(current_path);

		char** dir_tab = malloc(sizeof(char*)*(argc+1));
		if(dir_tab == NULL){
			erreur=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
		}else{
			int cpt=0;
			for(int i=0;i<argc && !erreur;i++){
				if(argv[i][0]=='-'){
					int res=treat_option(masque_option,argv[i]);
					if(res){
						erreur=1;
					}
				}else{
					//printf("%s\n",argv[i]);
					if(argv[i][0]=='/'){

						dir_tab[cpt]=malloc(sizeof(char)*(strlen(argv[i])+1));
						if(dir_tab[cpt]==NULL){
							erreur=1;
							Error_SetError(ERROR_MEMORY_ALLOCATION);
						}else{
							strcpy(dir_tab[cpt],argv[i]);
							cpt++;
						}
						//exit_error_malloc(dir_tab[cpt])
						
					}else{

						int length_path = strlen(argv[i])+2+current_path_length;
						dir_tab[cpt]= malloc(sizeof(char)*length_path);
						if(dir_tab[cpt]==NULL){
							erreur=1;
							Error_SetError(ERROR_MEMORY_ALLOCATION);
						}else{

							sprintf(dir_tab[cpt],"%s/%s",current_path,argv[i]);

							cpt++;
						}
					}
				}

			}
			if(erreur){
				for(int i=0;i<cpt;i++){
					free(dir_tab[i]);
				}
				free(dir_tab);
			}else{
				dir_tab[cpt]=NULL;
				return_value = dir_tab;
			}
		}
		free(current_path);
		//exit_error_malloc(dir_tab)
		
	}
	//exit_error(current_path == NULL, "Erreur getcwd: ")
	
	return return_value;
}
int treat_option(int* masque_option, char* option){
	int return_value = 0;
	if(*option!='-'){
		Error_SetError(ERROR_INVALID_OPTION);
		return_value=1;
	}
	int i=1;
	while(option[i]!='\0' && !return_value){
		if(option[i]=='a'){
			*masque_option=*masque_option|1;
		}else if(option[i]=='R'){
			*masque_option=*masque_option|2;
		}else{
			Error_SetError(ERROR_INVALID_OPTION);
			return_value=1;
		}
		i++;

	}
	if(i<1){
		Error_SetError(ERROR_INVALID_OPTION);
		return_value=1;
	}
	return return_value;
}

int explore_dir(char * dir_path, int masque_option){
	int return_value=0;
	int file_max_size=10;
	int file_size=0;
	int directory_max_size=10;
	int directory_size=0;
	int block_total=0;
	struct dirent* directory;



	DIR * dir = opendir(dir_path);
	if(dir == NULL){
		return_value = 1;
		Error_SetError(ERROR_OPEN_DIR);
	}else{
		full_file * file_tab =malloc(sizeof(full_file)*file_max_size);
		if(file_tab == NULL){
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			return_value=1;
		}else{
			char** directory_tab = malloc(sizeof(char*)*directory_max_size);
			//exit_error_malloc(directory_tab)
			if(directory_tab==NULL){
				Error_SetError(ERROR_MEMORY_ALLOCATION);
				return_value=1;
			}else{
				printf("%s:\n",get_dir_name(dir_path));
				errno=0;
				//int cpt=0;
				while((directory= readdir(dir))!=NULL && !return_value){

					
					char* path = malloc(sizeof(char)*(strlen(dir_path)+strlen(directory->d_name)+2));
					if(path==NULL){
						return_value=1;
						Error_SetError(ERROR_MEMORY_ALLOCATION);
					}else{
						//exit_error_malloc(directory_tab)
						sprintf(path,"%s/%s",dir_path,directory->d_name);
						struct stat * file = malloc(sizeof(struct stat));
						if(file==NULL){
							return_value=1;
							Error_SetError(ERROR_MEMORY_ALLOCATION);
							free(path);
						}else{

							int stat_res = stat(path,file);
							if(stat_res==-1){
								return_value=1;
								Error_SetError(ERROR_STAT);
								free(path);
								free(file);
							}else{
								if((masque_option & 1 || directory->d_name[0]!='.')){
									int push_file_res = push_file(file, &file_size, &file_max_size, &file_tab, directory->d_name, path);
									if(push_file_res){
										return_value = 1;
										free(path);
										free(file);
									}else{
										block_total+=file->st_blocks*512/1024;


										if(S_ISDIR(file->st_mode) && masque_option & 2 && is_not_point_directory(directory->d_name) && (masque_option & 1 || directory->d_name[0]!='.')){
											int push_dir_res = push_directory(path,&directory_size,&directory_max_size,&directory_tab);
											if(push_dir_res == 1){
												return_value = 1;
											}
										}

									}
								}else{
									free(path);

									free(file);
								}
							}
							
						}
						//exit_error_malloc(file)
		
						
					}
					
				}
				if(!return_value){
					printf("total %d\n",block_total);
					sort_file_tab(file_tab,file_size);
					for(int i=0;i<file_size && !return_value; i++){
		
						return_value=print_file((file_tab+i)->name,(file_tab+i)->file);

					}
					printf("\n");
					if(!return_value){
						sort_dir_path_tab(directory_tab,directory_size);

						for(int i=0;i<directory_size && !return_value;i++){
							//printf("%s\n",*(directory_tab+i));
							return_value=explore_dir(*(directory_tab+i),masque_option);
						}
					}

					
				}
				//exit_error(errno, "Erreur readdir:")

				
				for(int i=0;i<file_size; i++){	
					free((file_tab+i)->name);
					free((file_tab+i)->path);
					free((file_tab+i)->file);

				}
				free(directory_tab);
			}
			
			free(file_tab);
		}
		closedir(dir);
	}
	//exit_error(dir==NULL,"Erreur opendir: ")

	
	
	return return_value;
}

// droit, number of linked hard-link, owner, group, size, last modif time, nom (bleu=dossier, vert=executable)
int print_file(char* name,struct stat* file){

	int return_value=0;
	char droits[12];
	char date[16];

	int is_executable = get_access_right_string(droits, file->st_mode);
	return_value = get_date(date,file);
	if(!return_value){
		errno=0;
		char * owner_name = getpwuid(file->st_uid)->pw_name;
		if(owner_name == NULL && !errno){
			return_value = 1;
			Error_SetError(ERROR_PWUID);
		}

		//exit_error(owner_name == NULL && !errno, "Erreur getpwuid: ")

		char * group_name = getgrgid(file->st_gid)->gr_name;
		if(group_name == NULL && !errno){
			return_value = 1;
			Error_SetError(ERROR_GRGID);
		}
		if(!return_value){
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
	}
	
	return return_value;
	
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

int get_date(char* string,struct stat* file){
	int return_value = 0;
	const char* months[12]={"janv.","févr.","mars","avril","mai","juin","juil.","août", "sept.","oct.","nov.","déc."};

	struct tm* date = gmtime(&(file->st_mtime));
	if(date == NULL){
		return_value = 1;
		Error_SetError(ERROR_GMTIME);
	}else{
		int min=date->tm_min;
		int hour=date->tm_hour;
		int day = date->tm_mday;
		int month=date->tm_mon;
		int year=date->tm_year;
		time_t current_time = time(NULL);
		if((int)current_time == -1){
			return_value=1;
			Error_SetError(ERROR_TIME);
		}else{
			struct tm* current_date = gmtime(&current_time);
			if(current_date==NULL){
				return_value=1;
				Error_SetError(ERROR_GMTIME);

			}else{

				//exit_error(current_date == NULL, "Erreur gmtime")
				if(current_date->tm_mon == month && current_date->tm_year == year){
					sprintf(string,"%s  %d %d:%d",months[month], day, hour, min);
				}else{
					sprintf(string,"%s  %d  %d",months[month], day,year+1900);
				}
			}	
		}
		//exit_error((int)current_time == -1, "Erreur time:")

		
	}
	return return_value;
	//exit_error(date == NULL, "Erreur gmtime")
	

}
char* get_dir_name(char* path){
	char* res=path;
	for(int i=0;path[i]!='\0';i++){
		if(path[i]=='/') res=path+i+1;
	}
	return res;
}

int push_file(struct stat* file, int* file_size, int* file_max_size, full_file** file_tab, char* name, char* path){
	int return_value=0;
	if(*file_size>=*file_max_size){
		full_file* tmp = *file_tab;
		(*file_max_size)*=2;
		*file_tab=realloc(*file_tab, sizeof(full_file)*(*file_max_size));
		if(*file_tab == NULL){
			return_value=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			*file_tab=tmp;
		}

			//exit_error(file_tab==NULL,"Erreur realloc")
	}
	if(!return_value){
		(*file_tab+*file_size)->name= malloc((strlen(name)+1)*sizeof(char));
		if((*file_tab+*file_size)->name == NULL){
			return_value = 1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
		}else{
			strcpy((*file_tab+*file_size)->name, name);
	
			(*file_tab+*file_size)->path=path;
			(*file_tab+*file_size)->file=file;
			(*file_size)++;
		}
		//exit_error_malloc((file_tab+file_size)->name)
		
	}
	return return_value;
}

int push_directory(char* path,int* directory_size, int* directory_max_size, char*** directory_tab){
	int return_value=0;
	if(*directory_size>=*directory_max_size){
		(*directory_max_size)*=2;
		char** tmp = *directory_tab;
		*directory_tab=realloc(*directory_tab, sizeof(char*)*(*directory_max_size));
		if(*directory_tab == NULL){
			return_value = 1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			*directory_tab=tmp;
		}
		//exit_error(directory_tab==NULL,"Erreur realloc")
	}
	if(!return_value){
		*(*directory_tab+*directory_size)=path;

		(*directory_size)++;

	}
	return return_value;
}




