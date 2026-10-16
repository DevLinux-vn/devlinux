#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
  int id;
  char name[50];
  int quantity;
  float unit_price;
} Order;

Order orders[3] = {
  {1, "Backpack", 2, 350000},
  {2, "Shoes",    1, 500000},
  {3, "Hat",      3, 120000}
};

void process_order(Order o) {
  float total = o.quantity * o.unit_price;
  printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
  printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
         o.id, o.name, o.quantity, total);
  printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
  sleep(2);
}



int main() {
  system("clear");
  pid_t pids[3];
  int status;
  float total_revenue = 0;
  
  printf("===================================================\n");
  printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
  printf("===================================================\n");
  
  printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());
  
  //LOOP 1
  for(int i = 0; i < 3; i++) {
    fflush(stdout);
    
    pid_t pid = fork();
    
    if(pid == 0) {
      
      process_order(orders[i]);
      exit(0);
      
    } else if (pid > 0){
      pids[i] = pid;
      printf("[MANAGER] fork() order #%d → child PID: %d\n", i + 1, pid);
    }
  }
  
  printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");
  
  //LOOP 2
  int success_count = 0;
  
  for(int i = 0; i < 3; i++) {
    waitpid(pids[i], &status, 0);
    
    if(WIFEXITED(status)){
      int exit_code = WEXITSTATUS(status);
      
      printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n", 
              pids[i], i + 1, exit_code);
    
      if(exit_code == 0){
        success_count++;
        total_revenue += orders[i].quantity * orders[i].unit_price;
      }
      
    }
  }
  
  printf("================= SUMMARY =================\n");
  printf("Total order: 3\n");
  printf("Success: %d\n", success_count);
  printf("Total revenue: %.f\n", total_revenue);
  printf("===========================================\n");
  
  return 0;
}