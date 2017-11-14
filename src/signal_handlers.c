#include <stdio.h>
#include <signal.h>
#include "signal_handlers.h"

void catch_sigint(int signalNo)
{
  // Implemetation of ctrl-C
  signal(SIGINT, SIG_IGN);

  // printf ("This is control-C \n");

  fflush(stdout);
}

void catch_sigtstp(int signalNo)
{
  // Implementatio of ctrol-Z
   signal(SIGTSTP, SIG_IGN);

   //  printf ("This is control-Z \n");

   fflush(stdout);
}
