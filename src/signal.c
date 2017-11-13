#include "signal_handlers.h"

sigset_t set;

void catch_sigint(int signalNo)
{
  signal(SIGINT, SIG_IGN); //ignore ctrl+z signal

}

void catch_sigtstp(int signalNo);
{
  signal(SIGSTP, SIG_IGN); //ignore ctrl+z signal
}
