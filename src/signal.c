//#include "signal_handlers.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

//sigset_t set;

void catch_sigint(int)
{
  printf("sigint\n");

}

void catch_sigtstp(int)
{
  //signal(SIGSTP, SIG_IGN); //ignore ctrl+z signal
  printf("sigtstp\n");
}
