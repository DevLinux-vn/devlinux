#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void signal_receive(int signum) {
  printf("Service shutting down...\n");
  exit(0);
}

int main() {
  setbuf(stdout, NULL);
  
  //Dang ky ham voi he thong khi kill
  signal(SIGTERM, signal_receive);
  
  while(1) {
    printf("Daily warning!\n");
    sleep(1);
  }
}