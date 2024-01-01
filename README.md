# MYSH

> mysh is a mini-shell

## Functionality

- [X] Run commands from a command prompt
- [X] Show current directory
- [X] Wait for a command to finish executing before re-displaying the command prompt
- [X] Unconditional sequence of commands with ';'
- [X] Conditional sequence of command with '&&' and '||'
- [ ] Support wildcards : '*', '?', '[ens]'
- [ ] Internal command 'cd' for change directory
- [X] Internal command 'exit' for quit mysh
- [ ] Ctrl-C for exit by killing all background processes (with confirmation request)
- [ ] Ctrl-C for kill current process (not exit mysh)
- [ ] Internal command 'status' (show status for last process executed in foreground)
- [X] External command 'myls' : 'ls -l' with parameter '-a' and '-R' (colored by file type)
- [ ] External command 'myps' : 'ps aux' with color by process state
- [X] Pipeline redirection '|'
- [ ] File Redirection : '>', '>>', '2>', '2>>', '>&', '>>&', '<'
- [ ] Support background command with '&'
- [ ] Internal command 'myjob' : display all process in background
- [ ] Ctrl-Z for stop current process
- [ ] Internal command 'myfg' : pass job stopped or job background to foreground
- [ ] Internal command 'mybg' : pass job stopped to background
- [ ] Support variable environment with internal command 'setenv' and 'unsetenv' (use '$var' for access to variable 'var')
- [ ] Support variable local with internal command 'set' and 'unset' (use '$var' for access to variable 'var')

## Installation

### requirements

the commands given for install requirements, work in debian (Ubuntu)  

before install requirements execute the following commands for update package :
```shell
sudo apt update
sudo apt upgrade
```

- git for git clone repositories : ```sudo apt install git```  
- gcc compiler for compile : ```sudo apt install gcc```  
- make for run Makefile : ```sudo apt install make```  

### commands for install

```shell
git clone https://gitlab.univ-artois.fr/maxime_carlier/mysh
cd mysh
make
```