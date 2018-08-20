
# Contents of the Directory
* guish.cpp - main fuction is present in this file, entire code with doccumentation is available in this file
* guish.l - This file helps to tokenize the command entered by the user
* makefile - guish.cpp, guish.l is directly compiled using this make file, just run 'make' in current directory.
* Readme.txt

# Islander Shell 
Utilization of fork and exec is the main aim of this project. 'hist' is the shell inbuilt command and it is implemented by using linked list provided by the STL library. For I/O redirection 'dup2' a system call to duplicate the file discriptors is used. Finally Islander shell is simple replecation of original shell with two new inbuilt commands [hist, r n]
					
# Commands
Apart from the normal shell commands two new commands hist and r n are implements. hist list last 10 commands used and command r follwed by the priviously used commsnd number will execute that command. For example if your last command is 'ls -a' typing the command 'r 10' will execute 'ls -a'. All the control signals 'ctrl+c', 'ctrl+d', 'ctrl+x' are captures and only 'exit' command can quit the terminal 
