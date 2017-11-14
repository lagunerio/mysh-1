#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <pwd.h>

#include <libgen.h>
#include <linux/limits.h>

#include "commands.h"
#include "built_in.h"
#include "signal_handlers.h"

#define MAX_LINE 256

#define BUFFERSIZE 128
#define BLANK_SYMBOL ' '
#define NULL_SYMBOL '\0'
#define NEWLINE_SYMBOL '\n'
#define COLON_SYMBOL ':'
#define SLASH_SYMBOL '/'
#define BLANK_STR " "
#define NULL_STR "\0"
#define NEWLINE_STR "\n"
#define COLON_STR ":"
#define SLASH_STR "/"
#define PARENT_DIR_STR ".."
#define PATH "PATH"

static char *paths[BUFFERSIZE];

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static pid_t pid; //set pid
static long pid_bg; // remember pid from bg
static int pid_flag=0 ; // toggle bg/fg

char *homedir;
char *envp_path;

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}


/*
 * This function finds the path where the input command is
 * stored in the system.
 */
void get_cmd_pth(char *cmd)
{
	int i;
	char *path = calloc(BUFFERSIZE, sizeof(char*));
	FILE *file;

	paths[0]="/usr/local/bin/"; paths[1]="/usr/bin/"; paths[2]="/bin/";
	paths[3]="/usr//sbin/"; paths[4]="/sbin/"; paths[5]= NULL;


	// iterate over all possible paths
	for(i = 0; paths[i]; i++) {
		strcpy(path, paths[i]);
		strncat(path, cmd, strlen(cmd));

		// get the right path by testing it existence
		if((file = fopen(path, "r"))) {
			strncpy(cmd, path, strlen(path));
			fclose(file);
		}
	}
	free(path);
}


/*
 * Description: Currently this function only handles single built_in commands.
 * You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  int child_status; // Check the status of the child process.
  pid_t temp_pid;

  if (n_commands > 0) {
    struct single_command* com = (*commands);

    //Create a child process
    pid = fork();

    //If there is no problem, then run child and wait.
    if(pid < 0){
      //There was a problem with fork
      fprintf(stderr, "Error: Cannot fork child.\n");
      return 1;
    }
    else if(pid==0) {
      // We are in a child process
      char *arg[] = {com->argv[0], com->argv[1], 0};

      // *******************************************
      // First, do Path Resolution for extra credits
      // *******************************************

      // Resolve arg[0]
      if  ((strncmp(arg[0],"cd",2)!=0) && (strncmp(arg[0],"pwd",3)!=0)){
	char *cmd = calloc(BUFFERSIZE, sizeof(char*));

	strcpy (cmd, arg[0]);
	get_cmd_pth(cmd);     // cmd with full path

	strcpy (arg[0], cmd); // restore into arg[0]
      }

      // Resolve arg[1]
      if (arg[1] != NULL) {
	char sPath[PATH_MAX];
	if (strncmp(arg[1], "~",1) == 0) {
	  // realpath does not handle "~".
	  homedir = getenv("HOME");
	  strcpy (arg[1], homedir);
	}
	else {
	  realpath(arg[1], sPath);
	  strcpy (arg[1], sPath);
	}
      }

      execv(arg[0], arg);

      if(strcmp(com->argv[1], "&")==0){

        // We only allow one bg for this assignment.
	if(pid_flag == 1){
           fprintf(stderr, "Error: Already forked a process.\n");
	   return 1;
	} else {

	  // Run the program in the background and prints the pid number.
	  pid_flag = 1;
	  execv(arg[0], arg);
	  pid_bg = (long)getpid();
	  printf("%ld\n", pid_bg);
	}
      }

      if(strcmp(arg[0], "fg")==0) {

	// Do nothing if there is no background process already running.
	// (Note to TA: I am displaying warning message for this assignment.)
	if(pid_flag == 0){
	  fprintf(stderr, "Warning: There is no background process to bring forward.\n");
	} else {

	  // Bring the background task forward.
	  pid_flag=0;
	  printf("%ld running \n", pid_bg);
	}
      }
    }
    else {
      // Wait for the child process to terminate.
      do {
        temp_pid = wait(&child_status);
      } while(temp_pid != pid);
    }

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    } else {
      //      fprintf(stderr, "%s: command not found\n", com->argv[0]);
      return -1;
    }
  }
  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
