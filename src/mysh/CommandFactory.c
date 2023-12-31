#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "CommandFactory.h"
#include "../utils/Error.h"
#include "../utils/macro.h"

bool CommandFactory_resizeIfFull(CommandFactory* this) {
    if (!this || !this->commands) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (this->nb_command==this->max_commands) {
        Command** tmp = realloc(this->commands, sizeof(Command*)*(this->max_commands<<=1));
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            this->max_commands>>=1;
            return false;
        }
        this->commands=tmp;
        for (unsigned int i=this->nb_command; i<this->max_commands; i++) this->commands[i]=NULL;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

const Command** CommandFactory_buildCommands(CommandFactory* this, const Token* tokens, char** args) {
    if (!this || !this->commands || !tokens || !args) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    Token* tab_token = simplify_token_tab(tokens);
    int args_start_idx=0;
    int args_length = 0;
    int token_start =0;
    int token_length = 0;
    //unsigned int nb_args=0, start_args=0;
    char* tmp;
    //int idx_arg=0;
    //this->nb_command=0;

    //

    int length = 0;
    while(args[length]!=NULL) length++;
    subcommand* subcommand_list = malloc(sizeof(subcommand)*length);
    int idx_subcommand_list = 0;
    Token* token_list = malloc(sizeof(Token)*length);
    int idx_token_list=0;
    //


    

    for (unsigned i=0; tokens[i]; i++) {
        if (!CommandFactory_resizeIfFull(this)) {
            this->nb_command=0;
            free(tab_token);
            //free(command_list);
            free(token_list);
            return NULL;
        }
        //printf("%d,",tokens[i]);
        switch (tab_token[i]) {
            case TOKEN_ERROR:
                //printf("error, ");
                free(tab_token);
                //free(command_list);
                free(token_list);
                this->nb_command=0;
                return NULL;
            case TOKEN_STR:
                //printf("str(%s), ",args[idx_arg++]);

                //nb_args++;

                args_length++;
                token_length++;
                break;
            case TOKEN_SEMICOLON:
                create_subcommand((subcommand_list+idx_subcommand_list++ ),args+args_start_idx,args_length,tab_token+token_start, token_length);
                token_list[idx_token_list++]=tab_token[i];
                //printf("semicolon, ");
                token_start+=token_length+1;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_COMMAND:

                create_subcommand((subcommand_list+idx_subcommand_list++ ),args+args_start_idx,args_length,tab_token+token_start, token_length);
                token_list[idx_token_list++]=tab_token[i];
                //printf("command(), ");
                /*if (nb_args==0) {
                    this->nb_command=0;
                    return NULL;
                }*/
                //tmp = args[start_args+nb_args];
                //args[start_args+nb_args]=NULL;
                //Command* command = Command_create(args[start_args], (const char**) (args+start_args));
                //args[start_args+nb_args]=tmp;
                /*if (!command) {
                    this->nb_command=0;
                    return NULL;
                }
                Command_destroy(this->commands[this->nb_command]);
                *///this->commands[this->nb_command++]=command;
                //start_args+=nb_args;
                //nb_args=0;
                break;
            case TOKEN_PIPE:
                 create_subcommand((subcommand_list+idx_subcommand_list++ ),args+args_start_idx,args_length,tab_token+token_start, token_length);
                 token_list[idx_token_list++]=tab_token[i];
                //printf("pipe, ");
                token_start+=token_length+1;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_OR:

                create_subcommand((subcommand_list+idx_subcommand_list++ ),args+args_start_idx,args_length,tab_token+token_start, token_length);
                token_list[idx_token_list++]=tab_token[i];
                //printf("or, ");
                token_start+=token_length+1;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_BACKGROUND:
                 //   printf("background, ");    
                token_length++;
                
                break;
            case TOKEN_AND:
                create_subcommand((subcommand_list+idx_subcommand_list++ ),args+args_start_idx,args_length,tab_token+token_start, token_length);
                token_list[idx_token_list++]=tab_token[i];
                //printf("and, ");
                token_start+=token_length+1;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_REDIRECT_OUTPUT:
               //     printf("redirect output, ");

                token_length++;
                
                break;
            case TOKEN_REDIRECT_OUTPUT_APPEND:
             //   printf("redirect output append, ");

                token_length++;
                break;

            case TOKEN_REDIRECT_INPUT:
            //    printf("redirect input, ");
                
                token_length++;
                break;
            default:
                break;
        }
    }
    execute_subcommand_list(subcommand_list, idx_subcommand_list, token_list, idx_token_list);
    //printf("\n");
    for(int i=0;i<idx_subcommand_list;i++){
        free(subcommand_list[i].args);
        free(subcommand_list[i].tokens);
        //free(subcommand_list[i]);
    }
    
    free(token_list);
    free(subcommand_list);

    free(tab_token);
    return (const Command**) this->commands;
}

CommandFactory *CommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandFactory *this = malloc(sizeof(CommandFactory));
    if (!this) return NULL;
    this->max_commands=4;
    this->nb_command=0;
    if (!(this->commands=malloc(sizeof(Command*)*this->max_commands))) goto cleanup;
    for (unsigned int i=0; i<this->max_commands; i++) this->commands[i]=NULL;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandFactory_destroy(this);
    return NULL;
}

void CommandFactory_destroy(CommandFactory *this) {
    if (!this) return;
    if (this->commands) {
        for (unsigned int i=0; i<this->max_commands; i++) Command_destroy(this->commands[i]);
        free(this->commands);
    }
    free(this);
}

void create_subcommand(subcommand* res, char** args, int args_length, Token* tokens, int token_length){
    //subcommand* res= malloc(sizeof(subcommand));
    res->args = malloc(sizeof(char*)*(args_length+1));
    res->tokens = malloc(sizeof(Token)*(token_length+1));
    for(int i=0;i<args_length;i++){
        res->args[i] = args[i];
    }
    for(int i=0;i<token_length;i++){
        res->tokens[i] = tokens[i];
    }
    res->tokens[token_length]=TOKEN_NONE;
    res->args[args_length]=NULL;
    //return res;

    int args_idx=0;
    //printf("(");
    //Token last_token = TOKEN_NONE;
    for(int i=0;i<token_length;i++){
        switch (tokens[i]) {
            
            case TOKEN_STR:
                /*printf("str(%s)",args[args_idx++]);
                printf(", ");*/
                
                break;
            
            case TOKEN_BACKGROUND:
                 //   printf("background, ");
                
                break;
            case TOKEN_REDIRECT_OUTPUT:
                 //   printf("redirect output, ");
                break;
            case TOKEN_REDIRECT_OUTPUT_APPEND:
                //printf("redirect output append, ");
                break;

            case TOKEN_REDIRECT_INPUT:
                //printf("redirect input, ");
                break;
            default:
                //printf("TRUC");
                break;
        }
        //last_token = tokens[i];
    }
    //printf("),");
    //return NULL;
}

Token* simplify_token_tab(const Token* tokens){
    int cpt=0;
    for(int i=0;tokens[i];i++){
        cpt++;
    }
    Token* res = malloc(sizeof(Token)*cpt);
    int idx=0;
    Token last_token = TOKEN_NONE;
    for (unsigned i=0; tokens[i]; i++) {
        //printf("%d,",tokens[i]);
        switch (tokens[i]) {
            case TOKEN_BACKGROUND:
                if(last_token != TOKEN_AND){
                
                    res[idx++]=tokens[i];   
                }
                
                break;
            case TOKEN_REDIRECT_OUTPUT:
                if(last_token != TOKEN_REDIRECT_OUTPUT_APPEND){

                    res[idx++]=tokens[i];
                }
                break;

            case TOKEN_PIPE:
                if(last_token != TOKEN_OR){
                   
                    res[idx++]=tokens[i];
                }
                break;
            default:
                res[idx++]=tokens[i];
                break;
        }
        last_token = tokens[i];
    }
    return res;
}


int execute_subcommand_list(subcommand* subcommand_list,int idx_subcommand_list,Token* token_list,int idx_token_list){
    int subcommand_res=0;
    int last_command_was_exec=0;
    int fd[2];
    int input;

    for(int i = 0;i<idx_subcommand_list;i++){
        //printf("%d\n", subcommand_res);
        if(     (i==0)
            ||  (token_list[i-1] == TOKEN_OR && subcommand_res)
            ||  (token_list[i-1] == TOKEN_AND && !subcommand_res)
            ||  (token_list[i-1] == TOKEN_SEMICOLON)
            ||  (token_list[i-1] == TOKEN_PIPE && last_command_was_exec)){
            //printf("ok\n");
            last_command_was_exec = true;
            if(i>0 && token_list[i-1]==TOKEN_PIPE){
                input = fd[0];
            }
            if(token_list[i]==TOKEN_PIPE){
                
                if(pipe(fd)==-1){
                    perror("pipe failed");
                }

            }
            int pid = fork();

            if(!pid){
                int tmp1;
                int tmp2;
                int out;
                int in;
                if(i>0 && token_list[i-1]==TOKEN_PIPE){
                    in=dup(0);
                    close(0);
                    tmp1=dup(input);
                    close(input);
                }
                if(token_list[i] == TOKEN_PIPE){
                    out=dup(1);
                    close(1);
                    tmp2=dup(fd[1]);
                    close(fd[1]);
                    close(fd[0]);
                }
                execv(subcommand_list[i].args[0], subcommand_list[i].args);
                execvp(subcommand_list[i].args[0], subcommand_list[i].args);
                perror(RED("exec failed"));

                if(i>0 && token_list[i-1]==TOKEN_PIPE){
                    close(tmp1);
                    close(input);
                    dup(in);
                    close(in);
                }

                if(token_list[i] == TOKEN_PIPE){
                    close(tmp2);
                    dup(out);
                    close(out);
                    
                }
                exit(1);
            }else if(pid < 0){
                perror("fork failed");
                exit(1);
            }
            if(token_list[i]==TOKEN_PIPE){
                close(fd[1]);
            }
            if(i>0 && token_list[i-1]==TOKEN_PIPE){
                close(input);
                //if()
            }
            wait(&subcommand_res);
        }else{
            last_command_was_exec=0;
        }
    }
    return 0;
}