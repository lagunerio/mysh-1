#include "signal_handlers.h"

sigset_t set;

void catch_sigint(int)
{
  printf("sigint\n");

}

void catch_sigtstp(int)
{
  //signal(SIGSTP, SIG_IGN); //ignore ctrl+z signal
  printf("sigtstp\n");
}
