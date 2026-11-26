#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define EXIT_CODE 0

int main()
{
  sigset_t block_set, old_set;
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGINT);

  for (int i = 0; i < 5; i++)
  {
    //Block SIGINT signal to prevent interruption during critical section
    if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1)
    {
      perror("sigprocmask(SIG_BLOCK)");
      exit(EXIT_FAILURE);
    }

    //Critical section: writing transaction
    printf("[SAFE] Writing transaction #%d...\n", i + 1);
    sleep(3); // Simulate writing log
    printf("[SAFE] Transaction #%d committed.\n", i + 1);

    //Unblock SIGINT signal after critical section
    if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1)
    {
      perror("sigprocmask(SIG_SETMASK)");
      exit(EXIT_FAILURE);
    }

    printf("[IDLE] Waiting for next transaction...\n");
    sleep(3); // Simulate idle time
  }

  exit(EXIT_CODE);
}
