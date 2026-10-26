#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
	int   id;
	char  name[50];
	int   quantity;
	float unit_price;
} Order;

// Child process order processing function[cite: 2]
void process_order(Order o) {
	float total = o.quantity * o.unit_price;
	printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
	printf("[CHILD-%d] %s x%d — Total: %.0f VND\n", o.id, o.name, o.quantity, total);
	printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
	sleep(2);
}

int main() {
	// Hardcoded order data as required[cite: 2]
	Order orders[3] = {
		{1, "Backpack", 2, 350000},
		{2, "Shoes",    1, 500000},
		{3, "Hat",      3, 120000}
	};
	
	int num_orders = 3;
	pid_t pids[3];
	
	printf("===================================================\n");
	printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
	printf("===================================================\n");
	printf("[MANAGER] PID: %d — spawning %d child processes...\n\n", getpid(), num_orders);
	
	// LOOP 1: Spawn child processes[cite: 2]
	for (int i = 0; i < num_orders; i++) {
		// Flush stdout to prevent duplicate output across forks[cite: 2]
		fflush(stdout); 
		
		pid_t pid = fork();
		
		if (pid < 0) {
			perror("fork failed");
			exit(1);
		} else if (pid == 0) {
			// [Child] Process the order and exit immediately[cite: 2]
			process_order(orders[i]);
			exit(0);
		} else {
			// [Parent] Store the child's PID[cite: 2]
			pids[i] = pid;
			printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
		}
	}
	
	printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n", num_orders);
	
	int successful = 0;
	int failed = 0;
	float total_revenue = 0.0;
	
	// LOOP 2: Wait for specific child processes and collect results[cite: 2]
	for (int i = 0; i < num_orders; i++) {
		int status;
		// Wait for the exact PID recorded in loop 1[cite: 2]
		pid_t child_pid = waitpid(pids[i], &status, 0);
		
		if (child_pid > 0) {
			// Check if the child exited normally via exit()[cite: 2]
			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				if (exit_code == 0) {
					printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n", child_pid, orders[i].id, exit_code);
					successful++;
					total_revenue += (orders[i].quantity * orders[i].unit_price);
				} else {
					printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → FAILED\n", child_pid, orders[i].id, exit_code);
					failed++;
				}
			} else {
				printf("[MANAGER] waitpid(%d) — order #%d: terminated abnormally\n", child_pid, orders[i].id);
				failed++;
			}
		} else {
			perror("waitpid failed");
		}
	}
	
	// Print the final summary table[cite: 2]
	printf("\n================= SUMMARY =================\n");
	printf("  Total orders    : %d\n", num_orders);
	printf("  Successful      : %d\n", successful);
	printf("  Failed          : %d\n", failed);
	printf("  Total revenue   : %'.0f VND\n", total_revenue); 
	printf("===========================================\n");
	
	return 0;
}