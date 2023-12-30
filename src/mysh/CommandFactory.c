#include <malloc.h>
#include <string.h>
#include "CommandFactory.h"
#include "../utils/Error.h"

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
    unsigned int nb_args=0, start_args=0;
    char* tmp;
    int idx_arg=0;
    this->nb_command=0;
    Token last_token = TOKEN_NONE;
    for (unsigned i=0; tokens[i]; i++) {
        if (!CommandFactory_resizeIfFull(this)) {
            this->nb_command=0;
            return NULL;
        }
        //printf("%d,",tokens[i]);
        switch (tab_token[i]) {
            case TOKEN_ERROR:
                printf("error, ");
                this->nb_command=0;
                return NULL;
            case TOKEN_STR:
                //printf("str(%s), ",args[idx_arg++]);

                nb_args++;

                args_length++;
                token_length++;
                break;
            case TOKEN_COMMAND:

                subcommand(args+args_start_idx,args_length,tab_token+token_start, token_length);
                printf("command(), ");
                if (nb_args==0) {
                    this->nb_command=0;
                    return NULL;
                }
                tmp = args[start_args+nb_args];
                args[start_args+nb_args]=NULL;
                Command* command = Command_create(args[start_args], (const char**) (args+start_args));
                args[start_args+nb_args]=tmp;
                if (!command) {
                    this->nb_command=0;
                    return NULL;
                }
                Command_destroy(this->commands[this->nb_command]);
                //this->commands[this->nb_command++]=command;
                //start_args+=nb_args;
                nb_args=0;
                break;
            case TOKEN_PIPE:
                //    printf("pipe, ");
                token_length++;
                break;
            case TOKEN_OR:

                subcommand(args+args_start_idx,args_length,tab_token+token_start, token_length);
                printf("or, ");
                token_start+=token_length;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_BACKGROUND:
                //    printf("background, ");    
                
                
                break;
            case TOKEN_AND:
                subcommand(args+args_start_idx,args_length,tab_token+token_start, token_length);
                printf("and, ");
                token_start+=token_length;
                args_start_idx+= args_length;
                token_length = 0;
                args_length = 0;
                break;
            case TOKEN_REDIRECT_OUTPUT:
                //    printf("redirect output, ");

                token_length++;
                
                break;
            case TOKEN_REDIRECT_OUTPUT_APPEND:
                //printf("redirect output append, ");

                token_length++;
                break;

            case TOKEN_REDIRECT_INPUT:
                //printf("redirect input, ");
                
                token_length++;
                break;
            default:
                break;
        }
        last_token = tab_token[i];
    }

    printf("\n");
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

void subcommand(char** args, int args_length, Token* tokens, int token_length){
    int args_idx=0;
    printf("(");
    Token last_token = TOKEN_NONE;
    for(int i=0;i<=token_length;i++){
        switch (tokens[i]) {
            
            case TOKEN_STR:
                printf("str(%s)",args[args_idx++]);
                printf(", ");
                
                break;

            case TOKEN_PIPE:
                   printf("pipe, ");

                break;
            
            case TOKEN_BACKGROUND:
                    printf("background, ");
                
                break;
            case TOKEN_REDIRECT_OUTPUT:
                    printf("redirect output, ");
                break;
            case TOKEN_REDIRECT_OUTPUT_APPEND:
                printf("redirect output append, ");
                break;

            case TOKEN_REDIRECT_INPUT:
                printf("redirect input, ");
                break;
            default:
                break;
        }
        last_token = tokens[i];
    }
    printf("),");
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