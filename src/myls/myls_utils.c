#include "myls_utils.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

#include "../utils/Error.h"


int get_access_right_string(char* string, size_t st_mode){
	string[0]= S_ISDIR(st_mode)? 'd':'-';
	size_t tmp = st_mode;
	int executable = 0;
	for(int i=0;i<9;i++){
		int modulo = i%3;
		int idx = 8-(i/3)*3-modulo+1;
		if(tmp & 1){
			string[idx] = (char) (modulo==2?'r':modulo==1?'w':'x');
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
	const char* months[12]={"Jan","Feb","Mars","Apr","May","Jun","Jul","Aug", "Sep","Oct","Nov","Dec"};

	struct tm* date = localtime(&(file->st_mtime));
	if(date == NULL){
		return_value = 1;
		Error_SetError(ERROR_LOCALTIME);
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
			struct tm* current_date = localtime(&current_time);
			if(current_date==NULL){
				return_value=1;
				Error_SetError(ERROR_LOCALTIME);

			}else{
				char date_day[3];
				convert_to_date_number(day, date_day);
				//exit_error(current_date == NULL, "Error gmtime")
				if(/*current_date->tm_mon == month &&*/ current_date->tm_year == year){
					char time_hour[3];
					char time_minute[3];
					convert_to_time_number(hour,time_hour);
					convert_to_time_number(min,time_minute);
					sprintf(string,"%s %s %s:%s",months[month], date_day, time_hour, time_minute);
				}else{
					sprintf(string,"%s %s  %d",months[month], date_day,year+1900);
				}
			}	
		}
		//exit_error((int)current_time == -1, "Error time:")

		
	}
	return return_value;
	//exit_error(date == NULL, "Error gmtime")
	

}
char* get_name(char* path){
	char* res=path;
	for(int i=0;path[i]!='\0';i++){
		if(path[i]=='/') res=path+i+1;
	}
	return res;
}
char* get_original_name(char* path){
	char* res=path;
	size_t length = strlen(path);
	for(size_t i=0;i<length-3;i++){
		if(path[i]=='/' && path[i+1]=='/') res = path+i+2;
	}
	return res;
}
void convert_to_time_number(int number, char* time_number){
	if(number<10){
		sprintf(time_number,"0%d",number);
	}else{
		sprintf(time_number,"%d",number);
	}
}
void convert_to_date_number(int number, char* date_number){
	if(number<10){
		sprintf(date_number," %d",number);
	}else{
		sprintf(date_number,"%d",number);
	}
}
char* get_path_from_root(char* root_path, char* file_path){
	int idx=0;
	for(int i=0;*(root_path+i)!='\0';i++){
		if(*(root_path+i)=='/') idx = i;
	}
	return file_path+idx+1;
}
size_t get_number_length(size_t number){
	size_t length=0;
	size_t tmp=number;
	do{
		tmp/=10;
		length++;
	}while(tmp>0);
	return length;
}