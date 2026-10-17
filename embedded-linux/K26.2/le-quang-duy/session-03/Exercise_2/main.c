#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int fd = open("products.dat", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Cannot open file");
        return 1;
    }

    int choice;
    do {
        printf("\n--- PRODUCT MANAGEMENT ---\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_stdin();

        if (choice == 1) {
            Product p;
            printf("Enter ID: ");
            scanf("%d", &p.id);
            clear_stdin();
            
            printf("Enter Name: ");
            fgets(p.name, 64, stdin);
            p.name[strcspn(p.name, "\n")] = 0;
            
            printf("Enter Quantity: ");
            scanf("%d", &p.quantity);
            
            printf("Enter Price: ");
            scanf("%lf", &p.price);
            
            lseek(fd, 0, SEEK_END);
            write(fd, &p, sizeof(Product));
            printf("Product added.\n");
        } 
        else if (choice == 2) {
            int index;
            printf("Enter index (starting from 0): ");
            scanf("%d", &index);
            
            Product p;
            off_t offset = (off_t)index * sizeof(Product);
            if (lseek(fd, offset, SEEK_SET) == -1 || read(fd, &p, sizeof(Product)) != sizeof(Product)) {
                printf("Error: Product at index %d not found.\n", index);
            } else {
                printf("Index %d -> ID: %d | Name: %s | Qty: %d | Price: %.2f\n", 
                       index, p.id, p.name, p.quantity, p.price);
            }
        } 
        else if (choice == 3) {
            int index, new_qty;
            printf("Enter index to update (starting from 0): ");
            scanf("%d", &index);
            printf("Enter new quantity: ");
            scanf("%d", &new_qty);
            
            off_t offset = (off_t)index * sizeof(Product);
            off_t field_offset = offset + offsetof(Product, quantity);
            
            // Check if record exists before writing blindly
            off_t eof_pos = lseek(fd, 0, SEEK_END);
            if (offset >= eof_pos) {
                printf("Error: Index %d is out of bounds.\n", index);
            } else {
                lseek(fd, field_offset, SEEK_SET);
                write(fd, &new_qty, sizeof(int));
                printf("Quantity updated for product at index %d.\n", index);
            }
        }
        else if (choice == 4) {
            Product p;
            lseek(fd, 0, SEEK_SET);
            int index = 0;
            printf("\n--- Product List ---\n");
            while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
                printf("[%d] ID: %d | Name: %s | Qty: %d | Price: %.2f\n", 
                       index++, p.id, p.name, p.quantity, p.price);
            }
            if (index == 0) printf("No products available.\n");
        }
    } while (choice != 5);

    close(fd);
    printf("Exiting program.\n");
    return 0;
}