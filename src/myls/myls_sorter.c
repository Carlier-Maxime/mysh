
#include "myls_sorter.h"

#include <string.h>
void sort_dir_path_tab(char** dir_path_tab, int size){
	for(int i=0;i<size;i++){
		int min_idx=i;
		for(int j=i+1;j<size;j++){
			if(strcasecmp(dir_path_tab[min_idx],dir_path_tab[j])>0) min_idx=j;
		}
		if(i!=min_idx){
			char* tmp = dir_path_tab[i];
			dir_path_tab[i]=dir_path_tab[min_idx];
			dir_path_tab[min_idx]=tmp;
		}
	}
}


void sort_file_tab(full_file* file_tab, int size){
	for(int i=0;i<size;i++){
		int min_idx=i;
		for(int j=i+1;j<size;j++){
			if(strcasecmp(file_tab[min_idx].name,file_tab[j].name)>0) min_idx=j;
		}
		if(i!=min_idx){
			full_file tmp = file_tab[i];
			file_tab[i]=file_tab[min_idx];
			file_tab[min_idx]=tmp;
		}
	}
}