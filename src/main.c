#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "signal.h"
#include "commands.h"
#include "built_in.h"
#include "utils.h"

/*
//structure to handle signals
int sigaction(int signum, const struct sigaction *act);
struct sigaction
{
  //void(*sa_handler)int
  sigset_t sa_mask;
  int sa_flags;
}

struct sigaction act;
*/
int main()
{
  char buf[8096];
  struct sigaction act_int;
  //struct sigaction act_tstp;
  act_int.sa_handler = catch_sigint();
  //act_tstp.sa_handler = catch_sigtstp();


  while (1) {

    sigemptyset(&act_int.sa_mask);
    //sigemptyset(&act_tstp.sa_mask);
    act_int.sa_flags = 0;
    //act_tstp.sa_flags = 0;
/*
    if(sigaction(SIGINT, &act) == SIG_ERR){
      catch_sigint(1);
      continue;
    }
    if(sigaction(SIGTSTP, &act) == SIG_ERR){
      catch_sigtstp(2);
      continue;
    }
*/
    if(sigaction(SIGINT, &act_int, 0)== SIG_ERR){
      printf("sigaction() error; act_int\n");
      exit(1);
    }
    //else if(sigaction(SIGTSTP, &act_tstp, 0)== SIG_ERR){
    //  printf("sigaciton() error; act_tstp\n");
    //  exit(1);
    //}

    fgets(buf, 8096, stdin);

    struct single_command commands[512];
    int n_commands = 0;
    mysh_parse_command(buf, &n_commands, &commands);

    int ret = evaluate_command(n_commands, &commands);

    free_commands(n_commands, &commands);
    n_commands = 0;

    if (ret == 1) {
      break;
    }
  }

  return 0;
}
