#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <fcntl.h> 

int getToksSize(char**); 
char* stripString(char*); 

int
main() 
{
	char *toks[512];
	char *inFiles[512];
	char *outFiles[512]; 
	char path[50];
	char buf[1024];
	char command[20]; 
	int i; 
	int pid, status; 
	int commandsLength; 
	int filein, fileout;
	int isAppend; 

	printf(">>>--->");
	while(fgets(buf, 1024, stdin)){
		
		filein = 0; //STDIN 
		fileout = 1; // STDOUT 

		// Edge case: the user inputs just enter 
		if(strcmp(buf, "\n") == 0) {
			printf(">>>--->");
			continue; 
		}
		
		i=0;
		// Tokenize inputs 
		inFiles[i] = strtok(buf, "<");
		while(inFiles[i]!=NULL){
			i++; 
			inFiles[i] = strtok(NULL, "<"); 
		}
		if(i>1){
			printf("%d: %s\n", i, stripString(inFiles[1]));
			filein = open(stripString(inFiles[1]), O_RDONLY); 
		}

		i=0;
		isAppend=0; 
		// Tokenize Redirect Outputs
		if(strstr(buf, ">>")){
			isAppend=1; 
		}
		outFiles[i] = strtok(buf, ">");
		while(outFiles[i]!=NULL){
			i++; 
			outFiles[i] = strtok(NULL, ">"); 
		}
		if(i>1){
			if(isAppend==0) {
				fileout = open(stripString(outFiles[1]), O_WRONLY | O_CREAT, 0640); 
			}else{
				printf("THIS IS APPENDING\n"); 
				printf("%s\n", stripString(outFiles[1]));
				fileout = open(stripString(outFiles[1]), O_WRONLY | O_APPEND | O_CREAT, 0640);   
			}
			
		} 

		i=0; 
		// Tokenize the input for usage with exec 
		toks[i] = strtok(buf, " \t\r\n"); 
		while(toks[i]!=NULL){
			i++;
			toks[i] = strtok(NULL, " \t\r\n");
		}
		commandsLength = getToksSize(toks);

		
		
		// --> built-ins
		if(strcmp(toks[0], "cd")==0 ){
			strncpy(path, getenv("HOME"), 49);
			// Edge case: no arguments for cd 
			if (commandsLength == 1) {
				chdir(path);
			}
			else{
				// Edge case: ~ for path means HOME 
				if(strcmp(toks[1], "~")==0)
				{
					chdir(path);
				}
				else{
				chdir(toks[1]); 
				}
			}
		}
		else if (strcmp(toks[0], "quit")==0) {
			break; 
		}
		
		// otherwise exec functions  
		else {
			strncpy(command, toks[0], 20); 
			pid = fork();
			if (pid==0) {
				//file io setup 
				if(filein>0){
					dup2(filein, 0);
					close(filein);
				}
				if(fileout>1){
					dup2(fileout, 1); 
					close(fileout);
				}
				// exec 
				execvp(command, toks); 
				perror("The error message is... "); 
			}
			else {
				waitpid(pid, &status, 0); 
			}
		}
		printf(">>>--->"); 
	}
}

int getToksSize(char** toks){
	int i;
	i=0;
	while(toks[i]!=NULL) {
		i++;
	}
	return i; 
}

char* stripString(char* string){
	char tail; 
	while(*string==' '){
		string++;
	}
	tail = *(string + strlen(string)-1);
	if(tail=='\n' || tail=='\r'){
		*(string+strlen(string)-1) = '\0';
	}
	return string; 
}

