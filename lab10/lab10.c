/* Author(s): Anthony Chavez.
 *
 * This is lab10.c the csc60mshell
 * This program serves as a skeleton for doing labs 9, 10, 11.
 * Student is required to use this program to build a mini shell
 * using the specification as documented in direction.
 * Date: Fall 2019
 */

/* The include files section */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Define Section */
#define MAXLINE 80
#define MAXARGS 20
#define MAX_PATH_LENGTH 50
#define TRUE 1

/* function prototypes */
int parseline(char *cmdline, char **argv);
void process_input(int argc, char **argv);
void handle_redir(int argc, char *argv[]); 

/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
    char cmdline[MAXLINE];
    char *argv[MAXARGS];
    char * dir;
    char pathway[MAX_PATH_LENGTH];
    int argc;
    int count;
    int status;
    pid_t pid;

    /* Loop forever to wait and process commands */
    while (TRUE)
    {
	/* Print your shell name: csc60msh (m for mini shell) */
	printf("csc60msh> ");

	/* Read the command line */
	fgets(cmdline, MAXLINE, stdin);

	/* Call parseline to build argc/argv */
	argc = parseline(cmdline, argv);

        /* Print number of args passed in */
        printf("ARGC = %i\n", argc);

        /* Print argument(s) entered */
        for(count = 0; count < argc; count++)
        {
            printf("ARGV %i = %s \n", count, argv[count]);
        }

        /* If user hits enter without a command */
        if(argc == 0)
            continue;

        /* Exit call */    
        if(strcmp(argv[0], "exit") == 0)
            exit(EXIT_SUCCESS);
        
        /* pwd call */
        else if(strcmp(argv[0], "pwd") == 0)
        {
            getcwd(pathway, MAX_PATH_LENGTH);

            if(pathway != NULL)
                printf("Current Directory: %s\n", pathway);
            else
                printf("Error Printing Path");

            continue;
        }

        /* cd call */
        else if(strcmp(argv[0], "cd") == 0)
        {
            if(argc == 1)
                dir = getenv("HOME");
            else
                dir = argv[1];

            count = chdir(dir);

            if(count == -1)
                perror("Error changing directory\n");

            continue;
        }
 	
        /* Else, fork off a process */
        else {
            pid = fork();
            switch(pid)
    	    {
	    	case -1:
		    perror("Shell Program fork error");
	            exit(EXIT_FAILURE);
	   	case 0:
		    /* I am child process. I will execute the command, */
		    /* and call: execvp */
		    process_input(argc, argv);
		    break;
	   	default:
		    /* I am parent process */
		    if (wait(&status) == -1)
		    	perror("Parent Process error");
		    else
		   	printf("Child returned status: %d\n",status);
		    break;
	    } 	/* end of the switch */
	}	/* end of the if-else-if */
    }		/* end of the while */
} 		/* end of main */

/* ----------------------------------------------------------------- */
/*                  parseline                                        */
/* ----------------------------------------------------------------- */
/* parse input line into argc/argv format */

int parseline(char *cmdline, char **argv)
{
    int argc = 0;
    char *separator = " \n\t"; /* Includes space, Enter, Tab */
 
    /* strtok searches for the characters listed in separator */
    argv[argc] = strtok(cmdline, separator);

    while ((argv[argc] != NULL) && (argc+1 < MAXARGS)) 
    	argv[++argc] = strtok((char *) 0, separator);
     		
    return argc;  
}
/* ----------------------------------------------------------------- */
/*                  process_input                                    */
/* ----------------------------------------------------------------- */

void process_input(int argc, char **argv) // Child Process
{                        
    int temp;

    /* Step 1: Call handle_redir to deal with operators:             */
    /* < , or  >, or both                                            */

    handle_redir(argc, argv);

    /* Step 2: perform system call execvp to execute command         */
    /* Hint: Please be sure to review execvp.c sample program        */
    
    temp = execvp(argv[0], argv);
    if (temp  == -1)
    {                                         
        fprintf(stderr, "Error on the exec call\n");              
        _exit(EXIT_FAILURE);                                      
    }                                                             
 
}
/* ----------------------------------------------------------------- */
void handle_redir(int argc, char *argv[]) // Child Process
/* ----------------------------------------------------------------- */
{
    int loc_out = 0, loc_in = 0;
    int count, fileD;

    /* Start of Loop  */
    for(count = 0; count < argc; count++)
    {
        /* '>' == 0 */
        if(strcmp(argv[count], ">") == 0)
        {
            /* Error: cannot output to more than one file */
            if(loc_out != 0)
            {
                fprintf(stderr, "No Command Error.\n");
                _exit(EXIT_FAILURE);
            }

            /* Error: no command entered */
            else if(count == 0)
            {
                fprintf(stderr, "No command entered error.\n");
                _exit(EXIT_FAILURE);
            }
            loc_out = count;  // Set loc_out to current loop count
        }
        
        /* /</ == 0  */
        else if(strcmp(argv[count], "<") == 0)
        {
            /* Error: cannot input from more than one file */
            if(loc_in != 0)
            {
                fprintf(stderr, "Cannot input from more than one file error.\n");
                _exit(EXIT_FAILURE);
            }

            /* Error: no comman entered */
            else if(count == 0)
            {
                fprintf(stderr, "No command entered error.\n");
                _exit(EXIT_FAILURE);
            }
            loc_in = count;  // Set loc_in to current loop count
        }
    } /* End of Loop  */

    /* File Actions  */
    if(loc_out != 0)
    {
        /* Check if agrv[indexed by loc_out + 1] is NULL */
        if(argv[loc_out + 1] == NULL)
        {
            fprintf(stderr, "Absent file error.\n");
            _exit(EXIT_FAILURE);
        }

        /* Open file using name from argv */
        fileD = open(argv[loc_out + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        /* File opening error check */
        if(fileD == -1)
        {
            perror("File open error.\n");
            _exit(EXIT_FAILURE);
        }

        /* Switch standard-out to value of FD */
        dup2(fileD, 1);

        /* Close file */
        if(close(fileD) == -1)
        {
            perror("File Close Error.\n");
            _exit(EXIT_FAILURE);
        }

        argv[loc_out] = NULL;
    }
    if(loc_in != 0) 
    {
        /* Check if index + 1 is NULL  */
        if(argv[loc_in + 1] == NULL)
        {
            fprintf(stderr, "No File Found Error.\n");
            _exit(EXIT_FAILURE);
        }

        fileD = open(argv[loc_in + 1], O_RDONLY);

        if(fileD == -1)
        {
            perror("Open File Error.\n");
            _exit(EXIT_FAILURE);
        }

        dup2(fileD, 0);

        if(close(fileD) == -1)
        {
            perror("File Close Error.\n");
            _exit(EXIT_FAILURE);
        }

        argv[loc_in] = NULL;
    } /* End of File Actions */
}
/* ----------------------------------------------------------------- */
