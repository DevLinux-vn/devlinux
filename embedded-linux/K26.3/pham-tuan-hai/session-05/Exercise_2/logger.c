#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  
  int cycle = 1;
  for(int i = 0; i < 15; i++){
    fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
    fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
    fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);
    cycle++;
    
    sleep(2);
  }
  
  abort();
  return 0;
}