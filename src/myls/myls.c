#include "myls.h"
#include "myls_sorter.h"
#include "myls_utils.h"

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#include "../utils/macro.h"
#include "../utils/Error.h"
#define is_not_point_directory(i) (strcmp(i,".")!=0 && strcmp(i,"..")!=0)

char* current_root_path="/";

int main(int argc, char* argv[]){
		int res = exec_my_ls(argc-1,argv+1);
		if(res){
			Error_PrintErrorMsg("MYLS:");
		}
		exit(res);
}

int exec_my_ls(int argc, char* argv[]){
	int return_value=0;
	int masque_option=0;
	char** args = treat_arg(argc, argv, &masque_option); //a désalouer à la fin
	
	if(args == NULL){
		//return_value=1;
		return 1;
	}
	if(args[0]==NULL){
		char* current_path= getcwd(NULL,0);
		if(current_path == NULL){
			//return_value=1;
			Error_SetError(ERROR_GETCWD);
			free(args);
			return 1;
		}
		current_root_path=current_path;
		int res = explore_dir(current_path,masque_option,0);
		free(current_path);
		if(res){
			//return_value = 1;
			free(args);
			return 1;
		}
	}else{
		int size = 0;
		for(int i=0;args[i]!=NULL;i++) size+=1;
		sort_dir_path_tab(args,size);
		return_value = explore_files_alones(args,size,masque_option);
		for(int i=0;i<size && !return_value;i++){
			current_root_path=args[i];
			if(explore_dir(args[i],masque_option,size>1?1:0)){
				return_value=1;
			}
			
			}
		for(int i=0;i<size;i++){
			free(args[i]);
		}
	}
	free(args);

	return return_value;
}

char** treat_arg(int argc, char* argv[], int* masque_option){
    char** dir_tab=NULL;
    int cpt=0;
	*masque_option=0;
	char* current_path = getcwd(NULL,0);
	if(!current_path){
		Error_SetError(ERROR_GETCWD);
        goto cleanup;
	}
    size_t current_path_length=strlen(current_path);
    dir_tab = malloc(sizeof(char*)*(argc+1));
    if(!dir_tab){
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        goto cleanup;
    }
    for(int i=0;i<argc; i++){
        if(argv[i][0]=='-'){
            int res=treat_option(masque_option,argv[i]);
            if (res) goto cleanup;
        }else{
            if(argv[i][0]=='/'){
                if(!(dir_tab[cpt]=malloc(sizeof(char)*(strlen(argv[i])+1)))){
                    Error_SetError(ERROR_MEMORY_ALLOCATION);
                    goto cleanup;
                }
                strcpy(dir_tab[cpt],argv[i]);
                cpt++;
            }else{
                size_t length_path = strlen(argv[i])+3+current_path_length;
                dir_tab[cpt]= malloc(sizeof(char)*length_path);
                if(!dir_tab[cpt]){
                    Error_SetError(ERROR_MEMORY_ALLOCATION);
                    goto cleanup;
                }else{
                    sprintf(dir_tab[cpt],"%s//%s",current_path,argv[i]);
                    cpt++;
                }
            }
        }

    }
    dir_tab[cpt]=NULL;
    free(current_path);
    return dir_tab;
cleanup:
    if (dir_tab) for(int i=0;i<cpt;i++) free(dir_tab[i]);
    free(dir_tab);
    free(current_path);
	return NULL;
}

int treat_option(int* masque_option, char* option){
	int return_value = 0;
	if(*option!='-'){
		Error_SetError(ERROR_INVALID_OPTION);
		//return_value=1;
		return 1;
	}
	int i=1;
	while(option[i]!='\0' && !return_value){
		if(option[i]=='a'){
			*masque_option=*masque_option|1;
		}else if(option[i]=='R'){
			*masque_option=*masque_option|2;
		}else if(!strcmp(option,NO_COLOR_OPTION)){
			*masque_option=*masque_option|4;
		}else{
			Error_SetError(ERROR_INVALID_OPTION);
			return 1;
			//return_value=1;
		}
		i++;

	}
	if(i<1){
		Error_SetError(ERROR_INVALID_OPTION);
		//return_value=1;
		return 1;
	}
	return return_value;
}
int explore_file(char* path, full_file* file_tab, int idx){
	struct stat * file = malloc(sizeof(struct stat));
	if(file==NULL){
		Error_SetError(ERROR_MEMORY_ALLOCATION);
		return 1;
		//free(path);
	}

	int stat_res = stat(path,file);
	if(stat_res==-1){
		free(file);
		Error_SetError(ERROR_STAT);
		return 1;				
	}
	if(!S_ISDIR(file->st_mode)){
		(file_tab+idx)->file=file;
		(file_tab+idx)->name=get_original_name(path);
		return 0;
		//return_value=print_file(get_name(path),file,masque_option,0);
	}else{
		free(file);
		return -1;
	}
		
}
int explore_dir(char * dir_path, int masque_option, int display_dir_name){
	int return_value=0;
	int file_max_size=10;
	int file_size=0;
	int directory_max_size=10;
	int directory_size=0;
	size_t block_total=0;
	struct dirent* directory;



	DIR * dir = opendir(dir_path);
	if(dir == NULL){
		struct stat * file = malloc(sizeof(struct stat));
		if(file==NULL){
			//return_value=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			//free(path);
			return 1;
		}else{
			int stat_res = stat(dir_path,file);

			free(file);
			if(stat_res==-1){
				//return_value=1;
				Error_SetError(ERROR_STAT);		
				return 1;			
			}
			//free(file);
			return 0;
		}
		/*return_value = 1;
		Error_SetError(ERROR_OPEN_DIR);*/
	}
	full_file * file_tab =malloc(sizeof(full_file)*file_max_size);
	if(file_tab == NULL){
		closedir(dir);
		Error_SetError(ERROR_MEMORY_ALLOCATION);
		//return_value=1;
		return 1;
	}
	char** directory_tab = malloc(sizeof(char*)*directory_max_size);
		//exit_error_malloc(directory_tab)
	if(directory_tab==NULL){
		Error_SetError(ERROR_MEMORY_ALLOCATION);
		closedir(dir);
		free(file_tab);
		//return_value=1;
		return 1;
	}
	if(display_dir_name || masque_option & 2) printf("%s:\n",get_path_from_root(current_root_path,dir_path));
	errno=0;
	//int cpt=0;
	while((directory= readdir(dir))!=NULL && !return_value){
				
		char* path = malloc(sizeof(char)*(strlen(dir_path)+strlen(directory->d_name)+2));
		if(path==NULL){
			return_value=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			break;
		}
		//exit_error_malloc(directory_tab)
		sprintf(path,"%s/%s",dir_path,directory->d_name);
		struct stat * file =malloc(sizeof(struct stat));
		if(file==NULL){
			return_value=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			free(path);
			break;
		}


		int stat_res =  stat(path,file);
		if(stat_res==-1){
			return_value=1;
			Error_SetError(ERROR_STAT);
			free(path);
			free(file);
			break;
		}
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
	if(!return_value){
		printf("total %zu\n",block_total);
		sort_file_tab(file_tab,file_size);
		int max_length=0;
		for(int i=0;i<file_size;i++){
				
			int length = get_number_length((file_tab+i)->file->st_size);
			max_length=max_length>length?max_length:length;
		}
		for(int i=0;i<file_size && !return_value; i++){
	
			return_value=print_file((file_tab+i)->name,(file_tab+i)->file,masque_option,max_length);
		}
		if(!return_value){
			sort_dir_path_tab(directory_tab,directory_size);

			for(int i=0;i<directory_size && !return_value;i++){
				printf("\n");
				//printf("%s\n",*(directory_tab+i));
				return_value=explore_dir(*(directory_tab+i),masque_option,1);
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
	
		
	free(file_tab);
	
	closedir(dir);
	
	return return_value;
}

// permissions, number of linked hard-link, owner, group, size, last modify time, nom (bleu=dossier, vert=executable)
int print_file(char* name,struct stat* file,int masque_option, size_t size_length){
	int return_value;
	char droits[12];
	char date[16];

	int is_executable = get_access_right_string(droits, file->st_mode);
	return_value = get_date(date,file);
	if(return_value){
		return return_value;
	}
	errno=0;
	char * owner_name = getpwuid(file->st_uid)->pw_name;
	if(owner_name == NULL && !errno){
		return_value = 1;
		Error_SetError(ERROR_PWUID);
		return return_value;
	}

	//exit_error(owner_name == NULL && !errno, "Erreur getpwuid: ")

	char * group_name = getgrgid(file->st_gid)->gr_name;
	if(group_name == NULL && !errno){
		return_value = 1;
		Error_SetError(ERROR_GRGID);
		return return_value;
	}
	if(owner_name == NULL) owner_name = " ";
	if(group_name == NULL) group_name = " ";
	printf("%s %ld %s %s ",droits,file->st_nlink,owner_name,group_name);
	int length=get_number_length(file->st_size);
	int length_diff=size_length-length;
	for(int i=0;i<length_diff;i++){
		printf(" ");
	}
	printf("%ld %s ",file->st_size,date);
	if(S_ISDIR(file->st_mode) && !(masque_option & 4)){
		printf("%s%s%s\n",BLUE_BEGIN,name,COLOR_RESET);
	}else if(is_executable && !(masque_option & 4)){
		printf("%s%s%s\n",GREEN_BEGIN,name,COLOR_RESET);
	}else{
		printf("%s\n",name);
	}
	

	
	return return_value;
	
}




int push_file(struct stat* file, int* file_size, int* file_max_size, full_file** file_tab, char* name, char* path){
	if(*file_size>=*file_max_size){
		full_file* tmp = *file_tab;
		(*file_max_size)*=2;
		*file_tab=realloc(*file_tab, sizeof(full_file)*(*file_max_size));
		if(*file_tab == NULL){
			//return_value=1;
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			*file_tab=tmp;
			return 1;
		}

			//exit_error(file_tab==NULL,"Error realloc")
	}
	(*file_tab+*file_size)->name= malloc((strlen(name)+1)*sizeof(char));
	if((*file_tab+*file_size)->name == NULL){
		//return_value = 1;
		Error_SetError(ERROR_MEMORY_ALLOCATION);
		return 1;
	}
	strcpy((*file_tab+*file_size)->name, name);
	
	(*file_tab+*file_size)->path=path;
	(*file_tab+*file_size)->file=file;
	(*file_size)++;

		//exit_error_malloc((file_tab+file_size)->name)
		
	
	return 0;
}

int push_directory(char* path,int* directory_size, int* directory_max_size, char*** directory_tab){
	if(*directory_size>=*directory_max_size){
		(*directory_max_size)*=2;
		char** tmp = *directory_tab;
		*directory_tab=realloc(*directory_tab, sizeof(char*)*(*directory_max_size));
		if(*directory_tab == NULL){
			Error_SetError(ERROR_MEMORY_ALLOCATION);
			*directory_tab=tmp;
			return 1;
		}
		//exit_error(directory_tab==NULL,"Error realloc")
	}
	*(*directory_tab+*directory_size)=path;

	(*directory_size)++;
	return 0;
}

int explore_files_alones(char** args,int max_size, int masque_option){
	int return_value=0;
	int is_there_file=0;
	int is_there_dir=0;
	full_file* file_tab=malloc(sizeof(full_file)*max_size);
	int file_size=0;
	if(file_tab==NULL){
		return_value=1;
		Error_SetError(ERROR_MEMORY_ALLOCATION);
		return 1;
	}
	
	for(int i=0;i<max_size && !return_value;i++){
		int res=explore_file(args[i],file_tab,file_size);
		if(res==1){
			return_value=1;
			break;
		}
			
		if(res == 0) {
			is_there_file=1;
			file_size++;
		}
		if(res == -1) is_there_dir = 1;
			//if(i==size-1 && is_there_dir && is_there_file) printf("\n");
		
	}
	int max_length=0;
	for(int i=0;i<file_size;i++){
					
		int length = get_number_length((file_tab+i)->file->st_size);
		max_length=max_length>length?max_length:length;
	}
	for(int i=0;i<file_size && !return_value; i++){
		return_value=print_file((file_tab+i)->name,(file_tab+i)->file,masque_option,max_length);
	}
	for(int i=0;i<file_size;i++){
		free((file_tab+i)->file);
	}
	free(file_tab);
	if(is_there_dir && is_there_file) printf("\n");
	return return_value;
}





