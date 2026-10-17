#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static int fd = -1;

static void open_data_file(void)
{
    fd = open(FILENAME, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        perror("open products.dat");
        exit(EXIT_FAILURE);
    }
}

static void flush_stdin(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static long record_count(void)
{
    off_t size = lseek(fd, 0, SEEK_END);

    if (size < 0) {
        perror("lseek");
        exit(EXIT_FAILURE);
    }

    return (long)(size / sizeof(Product));
}

static void add_product(void)
{
    Product p;
    memset(&p, 0, sizeof(p));

    printf("Nhap ID: ");
    scanf("%d", &p.id);
    flush_stdin();

    printf("Nhap ten: ");
    fgets(p.name, sizeof(p.name), stdin);

    size_t len = strlen(p.name);
    if (len > 0 && p.name[len - 1] == '\n') {
        p.name[len - 1] = '\0';
    }

    printf("Nhap so luong: ");
    scanf("%d", &p.quantity);

    printf("Nhap gia: ");
    scanf("%lf", &p.price);
    flush_stdin();

    off_t end = lseek(fd, 0, SEEK_END);

    if (end < 0) {
        perror("lseek");
        return;
    }

    if (write(fd, &p, sizeof(p)) != (ssize_t)sizeof(p)) {
        perror("write");
        return;
    }

    printf("Da them san pham ID=%d (index=%ld).\n",
           p.id,
           (long)(end / sizeof(Product)));
}

static void print_product(int index, const Product *p)
{
    printf("[%d] ID=%-6d Ten=%-20s SL=%-6d Gia=%.2f\n",
           index,
           p->id,
           p->name,
           p->quantity,
           p->price);
}

static void show_by_index(void)
{
    long idx;

    printf("Nhap index (0-based): ");
    scanf("%ld", &idx);
    flush_stdin();

    long total = record_count();

    if (idx < 0 || idx >= total) {
        printf("Index khong hop le (file dang co %ld record).\n", total);
        return;
    }

    off_t offset = (off_t)idx * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) < 0) {
        perror("lseek");
        return;
    }

    Product p;

    if (read(fd, &p, sizeof(p)) != (ssize_t)sizeof(p)) {
        perror("read");
        return;
    }

    print_product((int)idx, &p);
}

static void update_quantity(void)
{
    long idx;
    int new_qty;

    printf("Nhap index can cap nhat: ");
    scanf("%ld", &idx);
    flush_stdin();

    long total = record_count();

    if (idx < 0 || idx >= total) {
        printf("Index khong hop le (file dang co %ld record).\n", total);
        return;
    }

    printf("Nhap so luong moi: ");
    scanf("%d", &new_qty);
    flush_stdin();

    off_t rec_offset = (off_t)idx * sizeof(Product);
    off_t field_offset =
        rec_offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) < 0) {
        perror("lseek");
        return;
    }

    if (write(fd, &new_qty, sizeof(new_qty))
        != (ssize_t)sizeof(new_qty)) {
        perror("write");
        return;
    }

    printf("Da cap nhat so luong index=%ld thanh %d.\n",
           idx,
           new_qty);
}

static void list_products(void)
{
    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        return;
    }

    Product p;
    int idx = 0;
    ssize_t n;

    while ((n = read(fd, &p, sizeof(p))) == (ssize_t)sizeof(p)) {
        print_product(idx, &p);
        idx++;
    }

    if (n < 0) {
        perror("read");
    }

    if (idx == 0) {
        printf("(Danh sach rong)\n");
    }
}

static void print_menu(void)
{
    printf("\n===== QUAN LY SAN PHAM =====\n");
    printf("1. Them san pham\n");
    printf("2. Xem san pham theo index\n");
    printf("3. Cap nhat so luong theo index\n");
    printf("4. Liet ke tat ca\n");
    printf("5. Thoat\n");
    printf("Chon: ");
}

int main(void)
{
    open_data_file();

    int choice;

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            flush_stdin();
            continue;
        }

        flush_stdin();

        switch (choice) {
            case 1:
                add_product();
                break;

            case 2:
                show_by_index();
                break;

            case 3:
                update_quantity();
                break;

            case 4:
                list_products();
                break;

            case 5:
                close(fd);
                printf("Tam biet!\n");
                return 0;

            default:
                printf("Lua chon khong hop le.\n");
        }
    }
}