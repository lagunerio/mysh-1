#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "signal.h"
#include "commands.h"
#include "built_in.h"
#include "utils.h"


//structure to handle signals
int sigaction(int signum, const struct sigaction *act);
struct sigaction
{
  //void(*sa_handler)int
  sigset_t sa_mask;
  int sa_flags;
}
struct sigaction act;

int main()
{
  char buf[8096];

  while (1) {

    int i=0;
    struct sigaction act;
    //act.sa_handler = my_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGINT, &act) == SIG_ERR){
      catch_sigint(1);
      continue;
    }
    if(sigaction(SIGTSTP, &act) == SIG_ERR){
      catch_sigtstp(2);
      continue;
    }
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
