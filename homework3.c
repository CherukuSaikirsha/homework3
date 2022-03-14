#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    int S_flag;           
    int s_flag;           
    int f_flag;           
    int t_flag;           
    int fileSize;         
    char filterTerm[300]; 
    char fileType[2];     
    char e_flag;	  
    char E_flag;	 
    char EType[300];	 
    char eType[300];	 
    char *e_split[10];
} FlagArgs;
short ECount;
char *E_split[1000];
// type definition of the function pointer. It's void because it won't return anything
typedef void FileHandler(char *filePath, char *dirfile, FlagArgs flagArgs, int nestingCount);
// the function that will be used for this assignment
void myPrinterFunction(char *filePath, char *dirfile, FlagArgs flagArgs, int nestingCount)
{
    struct stat buf;      
    lstat(filePath, &buf); 
    char line[100];        
    strcpy(line, "");      
    strcat(line, dirfile); 
    if (flagArgs.S_flag) 
    {
        char strsize[10];                          // allocate memory for the string 
        sprintf(strsize, " %d", (int)buf.st_size); // assign the size to the allocated string
        strcat(line, strsize);                     // concatenate the line and the size
    }
    if (flagArgs.s_flag) 
    {
        if (flagArgs.fileSize > (int)buf.st_size) 
        {
            strcpy(line, ""); 
        }
    }
    if (flagArgs.f_flag) 
    {
        if (strstr(dirfile, flagArgs.filterTerm) == NULL) 
        {
            strcpy(line, ""); 
        }
    }
    if (flagArgs.t_flag) 
    {
        if (strcmp(flagArgs.fileType, "f") == 0) 
        {
            if (S_ISDIR(buf.st_mode) != 0) 
            {
                strcpy(line, ""); 
            }
        }
        if (strcmp(flagArgs.fileType, "d") == 0) 
        {
            if (S_ISREG(buf.st_mode) != 0) // if the file is a regular file
            {
                strcpy(line, ""); 
            }
        }
    }
    if (strcmp(line, "") != 0) 
    {
        int i = 0;
        for (i = 0; i <= nestingCount; i++) 
        {
            printf("\t"); 
        }
        printf("%s\n", line); 
    }
    if ( (strcmp(line, "") != 0) && (flagArgs.E_flag ==1)) 
    {
	    E_split[ECount] = malloc(100);
	    strcpy(E_split[ECount],filePath); 
	    ECount +=1;
    }
    if ( (strcmp(line, "") != 0) && (flagArgs.e_flag ==1)) 
    {
	 char *split_ptr;
    	pid_t pid;
	int status;
	pid = fork();
	int count=0;
	char binary[100]="/bin/";
	if(pid == 0) 
	{
		split_ptr = strtok(flagArgs.eType," ");
		while(split_ptr != NULL)
		{
		     flagArgs.e_split[count] = split_ptr;
		     split_ptr = strtok(NULL," ");
		     count++;
		}
		flagArgs.e_split[count++] = filePath;
		flagArgs.e_split[count] = NULL;
		strcat(binary,flagArgs.e_split[0]);
		if(execv(binary,flagArgs.e_split) <0)
			printf("Error in child execution\n");
	 
	}
	else
	{
		wait(&status);
	}
    }
}
void readFileHierarchy(char *dirname, int nestingCount, FileHandler *fileHandlerFunction, FlagArgs flagArgs)
{
    struct dirent *dirent;
    DIR *parentDir = opendir(dirname); // open the dir
    if (parentDir == NULL)             // check if there is any problem opening the directory
    {
        printf("Error opening directory '%s'\n", dirname);
        exit(-1);
    }
    while ((dirent = readdir(parentDir)) != NULL)
    {
        if (strcmp((*dirent).d_name, "..") != 0 &&
            strcmp((*dirent).d_name, ".") != 0) 
        {
            char pathToFile[300];                                                  
            sprintf(pathToFile, "%s/%s", dirname, ((*dirent).d_name));             
            fileHandlerFunction(pathToFile, (*dirent).d_name, flagArgs, nestingCount); // function pointer call
            if ((*dirent).d_type == DT_DIR)                                        
            {
                nestingCount++;                                                    
                readFileHierarchy(pathToFile, nestingCount, fileHandlerFunction, flagArgs); 
                nestingCount--;                                                    
            }
        }
    }
    closedir(parentDir); // closeing the directory
}
int main(int argc, char **argv)
{
    int opt = 0;
    FlagArgs flagArgs = {
     .S_flag = 0,
     .s_flag = 0,
     .f_flag = 0,
     .t_flag = 0,
     .e_flag = 0,
     .E_flag = 0};
    while ((opt = getopt(argc, argv, "Ss:f:t:e:E:")) != -1)
    {
      switch (opt)
        {
        case 'S':
            flagArgs.S_flag = 1;
            break;
        case 's':
            flagArgs.s_flag = 1;             
            flagArgs.fileSize = atoi(optarg);
            break;
        case 'f':
            flagArgs.f_flag = 1;              
            strcpy(flagArgs.filterTerm, optarg); 
            break;
        case 't':
            flagArgs.t_flag = 1;               
            strcpy(flagArgs.fileType, optarg); 
      break;
        case 'e':
            flagArgs.e_flag = 1;               
            strcpy(flagArgs.eType, optarg); 
            break;
        case 'E':
            flagArgs.E_flag = 1;              
            strcpy(flagArgs.EType, optarg); 
            break;
        }
    }
    if(flagArgs.E_flag ==1)
    {
     char *split;
	split = strtok(flagArgs.EType," ");
	while(split !=NULL)
	{
	 E_split[ECount] = malloc(100);
	 strcpy(E_split[ECount],split); 
	 split = strtok(NULL," ");
	 ECount +=1;
	}
    }
    if (opendir(argv[argc - 1]) == NULL) 
    {
     char defaultdrive[300];
     getcwd(defaultdrive, 300);    
     printf("%s\n", defaultdrive); 
     readFileHierarchy(defaultdrive, 0, myPrinterFunction, flagArgs);
     //   return 0;
    }else
    {
    printf("%s\n", argv[argc - 1]); 
    readFileHierarchy(argv[argc - 1], 0, myPrinterFunction, flagArgs);
    }
    if(flagArgs.E_flag ==1)
    {
   	pid_t E_pid;
       E_pid = fork();
	if(E_pid) 
	  wait(NULL);
	else{ 
	    E_split[ECount] = NULL;
	    if(execvp(E_split[0],E_split)<0)
		    printf("Error in Execution\n");
	}	
    }

    return 0;
}
