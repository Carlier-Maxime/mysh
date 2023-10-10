# MYSH

> mysh is a mini-shell

## Functionality

- [ ] Run commands from a command prompt
- [ ] Show current directory
- [ ] Wait for a command to finish executing before re-displaying the command prompt
- [ ] Unconditional sequence of commands with ';'
- [ ] Conditional sequence of command with '&&' and '||'
- [ ] Support wildcards : '*', '?', '[ens]'
- [ ] Internal command 'cd' for change directory
- [ ] Internal command 'exit' for quit mysh
- [ ] Ctrl-C for exit by killing all background processes (with confirmation request)
- [ ] Ctrl-C for kill current process (not exit mysh)
- [ ] Internal command 'status' (show status for last process executed in foreground)
- [ ] External command 'myls' : 'ls -l' with parameter '-a' and '-R' (colored by file type)
- [ ] External command 'myps' : 'ps aux' with color by process state
- [ ] Pipeline redirection '|'
- [ ] File Redirection : '>', '>>', '2>', '2>>', '>&', '>>&', '<'
- [ ] Support background command with '&'
- [ ] Internal command 'myjob' : display all process in background
- [ ] Ctrl-Z for stop current process
- [ ] Internal command 'myfg' : pass job stopped or job background to foreground
- [ ] Internal command 'mybg' : pass job stopped to background
- [ ] Support variable environment with internal command 'setenv' and 'unsetenv' (use '$var' for access to variable 'var')
- [ ] Support variable local with internal command 'set' and 'unset' (use '$var' for access to variable 'var')