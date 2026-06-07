#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static void print_product(int index, const Product *p)
{
    printf("  [%d] ID       : %d\n",     index, p->id);
    printf("       Name     : %s\n",     p->name);
    printf("       Quantity : %d\n",     p->quantity);
    printf("       Price    : %.2f\n",   p->price);
}

/* Returns number of records currently in the file */
static int get_count(int fd)
{
    off_t size = lseek(fd, 0, SEEK_END);
    return (int)(size / sizeof(Product));
}

static void add_product(int fd)
{
    Product p;
    memset(&p, 0, sizeof(p));

    printf("Enter ID       : "); scanf("%d",    &p.id);
    printf("Enter Name     : "); scanf(" %63[^\n]", p.name);
    printf("Enter Quantity : "); scanf("%d",    &p.quantity);
    printf("Enter Price    : "); scanf("%lf",   &p.price);

    lseek(fd, 0, SEEK_END);
    write(fd, &p, sizeof(Product));
    printf("Product added.\n");
}

static void show_by_index(int fd)
{
    int count = get_count(fd);
    if (count == 0) { printf("No products.\n"); return; }

    int idx;
    printf("Enter index (0-%d): ", count - 1); scanf("%d", &idx);
    if (idx < 0 || idx >= count) { printf("Invalid index.\n"); return; }

    Product p;
    lseek(fd, (off_t)idx * sizeof(Product), SEEK_SET);
    read(fd, &p, sizeof(Product));
    print_product(idx, &p);
}

static void update_quantity(int fd)
{
    int count = get_count(fd);
    if (count == 0) { printf("No products.\n"); return; }

    int idx;
    printf("Enter index (0-%d): ", count - 1); scanf("%d", &idx);
    if (idx < 0 || idx >= count) { printf("Invalid index.\n"); return; }

    int new_qty;
    printf("Enter new quantity: "); scanf("%d", &new_qty);

    /* Jump directly to the quantity field — do not rewrite the entire record */
    off_t field_offset = (off_t)idx * sizeof(Product) + offsetof(Product, quantity);
    lseek(fd, field_offset, SEEK_SET);
    write(fd, &new_qty, sizeof(int));
    printf("Quantity updated.\n");
}

static void list_all(int fd)
{
    Product p;
    int count = 0;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        print_product(count++, &p);
    }

    if (count == 0)
        printf("No products.\n");
}

int main(void)
{
    int fd = open("products.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int choice;
    do {
        printf("\n=== Product Manager ===\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: add_product(fd);      break;
        case 2: show_by_index(fd);    break;
        case 3: update_quantity(fd);  break;
        case 4: list_all(fd);         break;
        case 5: printf("Goodbye.\n"); break;
        default: printf("Invalid option.\n"); break;
        }
    } while (choice != 5);

    close(fd);
    return 0;
}
