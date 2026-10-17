#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#define DB_FILE "products.dat"
#define MAX_NAME_LEN 64
#define SUCCESS 0
#define ERR_IO -1
#define ERR_INPUT -2
#define ERR_NOT_FOUND -3

typedef struct {
    int    id;
    char   name[MAX_NAME_LEN];
    int    quantity;
    double price;
} Product;

int clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return SUCCESS;
}

int get_int_input(const char *prompt, int *out_val) {
    while (1) {
        printf("%s", prompt);
        int res = scanf("%d", out_val);
        if (res == EOF) return ERR_IO;
        if (res == 1) {
            clear_input_buffer();
            return SUCCESS;
        } else {
            clear_input_buffer();
            printf("Invalid input. Please enter an integer.\n");
        }
    }
}

int get_double_input(const char *prompt, double *out_val) {
    while (1) {
        printf("%s", prompt);
        int res = scanf("%lf", out_val);
        if (res == EOF) return ERR_IO;
        if (res == 1) {
            clear_input_buffer();
            return SUCCESS;
        } else {
            clear_input_buffer();
            printf("Invalid input. Please enter a number.\n");
        }
    }
}

int get_string_input(const char *prompt, char *out_str, int max_len) {
    printf("%s", prompt);
    if (fgets(out_str, max_len, stdin) == NULL) {
        return ERR_IO;
    }
    size_t len = strlen(out_str);
    if (len > 0 && out_str[len - 1] == '\n') {
        out_str[len - 1] = '\0';
    } else {
        clear_input_buffer();
    }
    return SUCCESS;
}

int add_product(void) {
    Product p;
    if (get_int_input("Enter product ID: ", &p.id) != SUCCESS) return ERR_INPUT;
    if (get_string_input("Enter product name: ", p.name, MAX_NAME_LEN) != SUCCESS) return ERR_INPUT;
    if (get_int_input("Enter quantity: ", &p.quantity) != SUCCESS) return ERR_INPUT;
    if (get_double_input("Enter price: ", &p.price) != SUCCESS) return ERR_INPUT;

    int fd;
    do {
        fd = open(DB_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        perror("open failed");
        return ERR_IO;
    }

    ssize_t bytes_written;
    do {
        bytes_written = write(fd, &p, sizeof(Product));
    } while (bytes_written == -1 && errno == EINTR);

    if (bytes_written != sizeof(Product)) {
        perror("write failed");
        close(fd);
        return ERR_IO;
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    printf("Product added successfully.\n");
    return SUCCESS;
}

int get_total_records(int fd, off_t *total) {
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        perror("lseek failed");
        return ERR_IO;
    }
    *total = file_size / sizeof(Product);
    return SUCCESS;
}

int show_product_by_index(int index) {
    if (index < 0) {
        printf("Invalid index.\n");
        return ERR_INPUT;
    }

    int fd;
    do {
        fd = open(DB_FILE, O_RDONLY);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        if (errno == ENOENT) {
            printf("No records found.\n");
            return SUCCESS;
        }
        perror("open failed");
        return ERR_IO;
    }

    off_t total_records = 0;
    if (get_total_records(fd, &total_records) != SUCCESS) {
        close(fd);
        return ERR_IO;
    }

    if (index >= total_records) {
        printf("Index out of bounds (Total records: %ld).\n", (long)total_records);
        close(fd);
        return ERR_NOT_FOUND;
    }

    off_t offset = (off_t)index * sizeof(Product);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        close(fd);
        return ERR_IO;
    }

    Product p;
    ssize_t bytes_read;
    do {
        bytes_read = read(fd, &p, sizeof(Product));
    } while (bytes_read == -1 && errno == EINTR);

    if (bytes_read == sizeof(Product)) {
        printf("Product at index %d -> ID: %d, Name: %s, Quantity: %d, Price: %.2f\n",
               index, p.id, p.name, p.quantity, p.price);
    } else {
        perror("read failed");
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    return SUCCESS;
}

int update_quantity_by_index(int index) {
    if (index < 0) {
        printf("Invalid index.\n");
        return ERR_INPUT;
    }

    int fd;
    do {
        fd = open(DB_FILE, O_RDWR);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        if (errno == ENOENT) {
            printf("No records found.\n");
            return SUCCESS;
        }
        perror("open failed");
        return ERR_IO;
    }

    off_t total_records = 0;
    if (get_total_records(fd, &total_records) != SUCCESS) {
        close(fd);
        return ERR_IO;
    }

    if (index >= total_records) {
        printf("Index out of bounds (Total records: %ld).\n", (long)total_records);
        close(fd);
        return ERR_NOT_FOUND;
    }

    int new_quantity;
    if (get_int_input("Enter new quantity: ", &new_quantity) != SUCCESS) {
        close(fd);
        return ERR_INPUT;
    }

    /* calculate offset using offsetof */
    off_t offset = (off_t)index * sizeof(Product) + offsetof(Product, quantity);
    
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        close(fd);
        return ERR_IO;
    }

    ssize_t bytes_written;
    do {
        bytes_written = write(fd, &new_quantity, sizeof(int));
    } while (bytes_written == -1 && errno == EINTR);

    if (bytes_written != sizeof(int)) {
        perror("write failed");
        close(fd);
        return ERR_IO;
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    
    printf("Quantity updated successfully.\n");
    return SUCCESS;
}

int list_all_products(void) {
    int fd;
    do {
        fd = open(DB_FILE, O_RDONLY);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        if (errno == ENOENT) {
            printf("No records found.\n");
            return SUCCESS;
        }
        perror("open failed");
        return ERR_IO;
    }

    Product p;
    ssize_t bytes_read;
    int idx = 0;
    printf("--- Product List ---\n");
    while (1) {
        do {
            bytes_read = read(fd, &p, sizeof(Product));
        } while (bytes_read == -1 && errno == EINTR);

        if (bytes_read != sizeof(Product)) {
            break;
        }
        printf("[%d] ID: %d, Name: %s, Qty: %d, Price: %.2f\n", idx++, p.id, p.name, p.quantity, p.price);
    }

    if (bytes_read == -1) {
        perror("read failed");
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    printf("--------------------\n");
    return SUCCESS;
}

int print_menu(void) {
    printf("\nMenu:\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    return SUCCESS;
}

int main(void) {
    int choice;
    int running = 1;

    while (running) {
        print_menu();
        if (get_int_input("Enter choice: ", &choice) != SUCCESS) {
            if (feof(stdin)) {
                printf("\nEOF detected. Exiting...\n");
                break;
            }
            continue;
        }

        switch (choice) {
            case 1:
                add_product();
                break;
            case 2: {
                int idx;
                if (get_int_input("Enter product index: ", &idx) == SUCCESS) {
                    show_product_by_index(idx);
                } else if (feof(stdin)) running = 0;
                break;
            }
            case 3: {
                int idx;
                if (get_int_input("Enter product index to update: ", &idx) == SUCCESS) {
                    update_quantity_by_index(idx);
                } else if (feof(stdin)) running = 0;
                break;
            }
            case 4:
                list_all_products();
                break;
            case 5:
                running = 0;
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
