#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t shutdown_flag=0;

void signal_receive(int signum) {
  shutdown_flag = 1;
}

int main() {
  setbuf(stdout, NULL);
  
  //'Register signal handler for SIGTERM'.
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_receive;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTERM, &sa, NULL);
  
  while(!shutdown_flag) {
    printf("Daily warning!\n");
    sleep(1);
  }
  
  printf("Service shutting down...\n");
  return 0;
}