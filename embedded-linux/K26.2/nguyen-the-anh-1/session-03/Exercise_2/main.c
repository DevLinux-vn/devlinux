#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#define DATA_FILE "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static int read_int(const char *prompt)
{
    char buf[64];
    int value;
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &value);
    return value;
}

static double read_double(const char *prompt)
{
    char buf[64];
    double value;
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%lf", &value);
    return value;
}

static void read_line(const char *prompt, char *out, size_t max_len)
{
    printf("%s", prompt);
    fgets(out, (int)max_len, stdin);
    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n') {
        out[len - 1] = '\0';
    }
}

/* Tra ve so record hien co trong file, dua vao kich thuoc file / sizeof(Product) */
static long get_record_count(int fd)
{
    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek");
        return -1;
    }
    return (long)(size / (off_t)sizeof(Product));
}

static void add_product(void)
{
    Product p;
    memset(&p, 0, sizeof(p));

    p.id = read_int("Nhap ID: ");
    read_line("Nhap ten: ", p.name, sizeof(p.name));
    p.quantity = read_int("Nhap so luong: ");
    p.price = read_double("Nhap gia: ");

    /* O_APPEND dam bao them vao cuoi file, khong ghi de record cu */
    int fd = open(DATA_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }

    ssize_t n = write(fd, &p, sizeof(Product));
    if (n != (ssize_t)sizeof(Product)) {
        perror("write");
    } else {
        printf("Da them san pham ID=%d\n", p.id);
    }

    close(fd);
}

static void show_by_index(void)
{
    int fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1) {
        printf("Chua co du lieu san pham.\n");
        return;
    }

    long count = get_record_count(fd);
    int index = read_int("Nhap index: ");

    if (index < 0 || index >= count) {
        printf("Index khong hop le (co %ld record).\n", count);
        close(fd);
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return;
    }

    Product p;
    ssize_t n = read(fd, &p, sizeof(Product));
    if (n != (ssize_t)sizeof(Product)) {
        perror("read");
        close(fd);
        return;
    }

    printf("ID=%d Ten=%s SoLuong=%d Gia=%.2f\n",
           p.id, p.name, p.quantity, p.price);

    close(fd);
}

static void update_quantity(void)
{
    int fd = open(DATA_FILE, O_RDWR);
    if (fd == -1) {
        printf("Chua co du lieu san pham.\n");
        return;
    }

    long count = get_record_count(fd);
    int index = read_int("Nhap index can cap nhat: ");

    if (index < 0 || index >= count) {
        printf("Index khong hop le (co %ld record).\n", count);
        close(fd);
        return;
    }

    int new_quantity = read_int("Nhap so luong moi: ");

    /* Nhay thang toi dung byte cua field quantity ben trong record,
       khong doc/ghi lai toan bo record */
    off_t offset = (off_t)index * sizeof(Product);
    off_t field_offset = offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return;
    }

    ssize_t n = write(fd, &new_quantity, sizeof(int));
    if (n != (ssize_t)sizeof(int)) {
        perror("write");
    } else {
        printf("Da cap nhat so luong record index=%d thanh %d\n",
               index, new_quantity);
    }

    close(fd);
}

static void list_products(void)
{
    int fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1) {
        printf("Chua co du lieu san pham.\n");
        return;
    }

    Product p;
    int index = 0;
    printf("%-5s %-6s %-20s %-8s %-10s\n",
           "Idx", "ID", "Ten", "SoLuong", "Gia");

    while (1) {
        ssize_t n = read(fd, &p, sizeof(Product));
        if (n == 0) break;
        if (n != (ssize_t)sizeof(Product)) {
            perror("read");
            break;
        }
        printf("%-5d %-6d %-20s %-8d %-10.2f\n",
               index, p.id, p.name, p.quantity, p.price);
        index++;
    }

    if (index == 0) {
        printf("Danh sach rong.\n");
    }

    close(fd);
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
    int choice;
    int running = 1;

    while (running) {
        print_menu();
        choice = read_int("");

        switch (choice) {
            case 1: add_product();      break;
            case 2: show_by_index();    break;
            case 3: update_quantity();  break;
            case 4: list_products();    break;
            case 5:
                printf("Tam biet!\n");
                running = 0;
                break;
            default:
                printf("Lua chon khong hop le.\n");
        }
    }

    return 0;
}
