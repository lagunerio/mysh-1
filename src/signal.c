#include "signal_handlers.h"

sigset_t set;

void catch_sigint(int signalNo)
{
  printf("sigint\n");

}

void catch_sigtstp(int signalNo);
{
  //signal(SIGSTP, SIG_IGN); //ignore ctrl+z signal
  printf("sigtstp\n");
}
