#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>     // offsetof
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define FILE_NAME "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static int open_rw(void) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd < 0) perror("open");
    return fd;
}

static void add_product(void) {
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) { perror("open"); return; }

    Product p;
    printf("ID: ");        scanf("%d", &p.id);
    printf("Name: ");      scanf(" %63[^\n]", p.name);
    printf("Quantity: ");  scanf("%d", &p.quantity);
    printf("Price: ");     scanf("%lf", &p.price);

    if (write(fd, &p, sizeof(p)) != sizeof(p))
        perror("write");
    close(fd);
}

static void show_by_index(void) {
    int idx;
    printf("Index: ");
    scanf("%d", &idx);

    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    off_t off = (off_t)idx * sizeof(Product);
    if (lseek(fd, off, SEEK_SET) == (off_t)-1) {
        perror("lseek"); close(fd); return;
    }

    Product p;
    ssize_t n = read(fd, &p, sizeof(p));
    if (n == sizeof(p))
        printf("ID=%d Name=%s Qty=%d Price=%.2f\n",
               p.id, p.name, p.quantity, p.price);
    else
        printf("Index out of range.\n");
    close(fd);
}

static void update_quantity(void) {
    int idx, new_qty;
    printf("Index: ");        scanf("%d", &idx);
    printf("New quantity: "); scanf("%d", &new_qty);

    int fd = open_rw();
    if (fd < 0) return;

    off_t off = (off_t)idx * sizeof(Product) + offsetof(Product, quantity);
    if (lseek(fd, off, SEEK_SET) == (off_t)-1) {
        perror("lseek"); close(fd); return;
    }

    if (write(fd, &new_qty, sizeof(new_qty)) != sizeof(new_qty))
        perror("write");
    else
        printf("Updated.\n");
    close(fd);
}

static void list_all(void) {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    Product p;
    int idx = 0;
    while (read(fd, &p, sizeof(p)) == sizeof(p)) {
        printf("[%d] ID=%d  %-20s Qty=%d  Price=%.2f\n",
               idx++, p.id, p.name, p.quantity, p.price);
    }
    close(fd);
}

int main(void) {
    int choice;
    while (1) {
        printf("\n1. Add  2. Show  3. Update qty  4. List  5. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) break;
        switch (choice) {
            case 1: add_product();    break;
            case 2: show_by_index();  break;
            case 3: update_quantity();break;
            case 4: list_all();       break;
            case 5: return 0;
            default: printf("Invalid.\n");
        }
    }
    return 0;
}
