#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

#define INDEX(_ARRAY_) (sizeof(_ARRAY_)/sizeof(_ARRAY_[0]))
typedef enum{
    ADD_PRODUCT = 1,
    SHOW_PRODUCT_INDEX,
    UPDATE_QUANTITY_INDEX,
    LIST_ALL_PRODUCT,
    EXIT_CHOICE,
} e_menu_t;

#define MIN_ID 1
#define MAX_ID 100

#define MIN_QUANTITY 1
#define MAX_QUANTITY 1000

#define MIN_PRICE 1.00
#define MAX_PRICE 1000.00

#define PRODUCT_DATA_FILE_NAME "products.dat"

int product_id_list[MAX_ID - MIN_ID] = {0};

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static ssize_t read_partial(int fd, const void *buf, size_t total_byte);
static ssize_t write_partial(int fd, const void *buf, size_t total_byte);

#ifdef PRELOAD_TEST_DATA
Product product_test_list[] = {
    {1,  "Laptop",                15,  899.99},
    {2,  "Smartphone",            30,  699.50},
    {3,  "Wireless Mouse",        50,   24.99},
    {4,  "Mechanical Keyboard",   20,   89.95},
    {5,  "Monitor 24-inch",       25,  159.99},
    {6,  "External Hard Drive",   40,   74.50},
    {7,  "USB-C Cable",           100,   9.99},
    {8,  "Bluetooth Speaker",     35,   49.95},
    {9,  "Gaming Chair",          10,  199.99},
    {10, "Desk Lamp",             60,   29.99},
    {11, "Power Bank",            45,   39.95},
    {12, "Smartwatch",            25,  149.99},
    {13, "Tablet",                18,  329.00},
    {14, "Printer",               12,  119.99},
    {15, "Router",                22,   89.99},
    {16, "Headphones",            55,   59.95},
    {17, "Webcam",                28,   39.99},
    {18, "Microphone",            16,   79.99},
    {19, "Portable SSD",          30,  129.99},
    {20, "Drone",                  8,  499.00}
};
#endif

int add_product(Product *product)
{
    int err;
    struct query_product_field {
        char *question;
        char *scanf_pattern;
        void *arg;
    };
    Product temp;
    struct query_product_field query_field[] = {
        {"What is the ID of the product?\n"     , "%d", &temp.id    },
        {"What is the name of the product?\n"   , "%s", &temp.name  },
        {"How many items of the product?\n"     , "%d", &temp.quantity   },
        {"How much does the product cost ?\n"   , "%f", &temp.price   },
    };
    memset(temp.name, 0, sizeof(temp.name));
    for(long unsigned int i = 0; i < INDEX(query_field); i++) {
        printf("%s", query_field[i].question);
        err = scanf(query_field[i].scanf_pattern, query_field[i].arg);
        if (err != 1) {
            printf("Error: Invalid input argument, err:%d\n", err);
            while (getchar() != '\n');
            i--;
            continue;
        }
        /* Remove any excessive arguments of input scanf */
        while (getchar() != '\n');
        /* Check input quantity */
        if(query_field[i].arg == &temp.quantity && (temp.quantity < MIN_QUANTITY || temp.quantity > MAX_QUANTITY)) {
            printf("Error: product quantity input is invalid, range from %d to %d\n", MIN_QUANTITY, MAX_QUANTITY);
            i--;
            continue;
        }
        /* Check input price */
        else if(query_field[i].arg == &temp.price && (temp.price < MIN_PRICE  || temp.price > MAX_PRICE))
        {
            printf("Error: product price input is invalid, range from %.2f to %.2f\n", MIN_PRICE, MAX_PRICE);
            i--;
            continue;
        }
        /* Check input product ID */
        else if(query_field[i].arg == &temp.id)
        {
            if(temp.id < MIN_ID  || temp.id > MAX_ID) {
                printf("Error: product ID input is invalid, range from %d to %d\n", MIN_ID, MAX_ID);
                i--;
                continue;
            }
            else if(product_id_list[temp.id] == 1)
            {
                printf("Error: product ID %d is already assigned, please select another\n", temp.id);
                i--;
                continue;
            }
            else {
                product_id_list[temp.id] = 1;
            }
        }

    }
    *product = temp;
    return 0;
}

int list_all_product()
{
    Product product;

    int fd = open(PRODUCT_DATA_FILE_NAME, O_RDONLY);
    if(fd == -1) {
        perror("Error of opening file\n");
        return 1;
    }

    // Reset file offset to the beginning
    off_t result = lseek(fd, 0, SEEK_SET);
    if (result == (off_t)-1) {
        perror("lseek failed");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }

    while(1) {
        ssize_t count = read_partial(fd, (void*)&product, sizeof(product));
        if(count == 0) {
            printf("EOF\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 0;
        }
        else if (count < 0) {
            printf("Error: Fail to read data\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 1;
        }
        else {
            printf("---------------\n");
            printf("product:\n");
            printf("id: %d,\t name: %s,\t quantity: %d,\t price: %.2f\n", product.id, product.name, product.quantity, product.price);
        }
    }

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    return 0;
}

int show_product_index()
{
    int err;
    int product_index;
    Product product = {0};

    printf("Input product index:\n");

    err = scanf("%d", &product_index);
    while (err != 1) {
        printf("Error: Invalid input argument, err:%d\n", err);
        while (getchar() != '\n');
        err = scanf("%d", &product_index);
    }

    int fd = open(PRODUCT_DATA_FILE_NAME, O_RDONLY);
    if(fd == -1) {
        perror("Error of opening file\n");
        return 1;
    }

    off_t seek_result = lseek(fd, product_index * sizeof(Product), SEEK_SET);
    if(seek_result == (off_t)-1) {
        perror("lseek failed to find product_index");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;

    }

    ssize_t count = read_partial(fd, (void*)&product, sizeof(Product));
    if(count == 0) {
        printf("Not found product\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    else if (count < 0) {
        printf("Error: Fail to read data\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    else {
        printf("---------------\n");
        printf("Product has been found:");
        printf("id: %d,\t name: %s,\t quantity: %d,\t price: %.2f\n", product.id, product.name, product.quantity, product.price);
    }

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    return 0;
}

int update_quantity_index()
{
    int err;
    int product_index;
    int    quantity = 0;
    int    read_quantity = 0;

INPUT:
    printf("Input product index and quantity:\n");
    err = scanf("%d%d", &product_index, &quantity);
    if (err != 2) {
        printf("Error: Invalid input argument, err:%d\n", err);
        goto INPUT;
    }
    /* Remove any excessive arguments of input scanf */
    while (getchar() != '\n');
    if(quantity < MIN_QUANTITY || quantity > MAX_QUANTITY) {
        printf("Error: product quantity input is invalid, range from %d to %d\n", MIN_QUANTITY, MAX_QUANTITY);
        goto INPUT;
    }

    // Byte offset of the record at position [index]
    off_t offset = (off_t)product_index * sizeof(Product);

    // Byte offset of a specific field within that record
    off_t quantity_offset = offset + offsetof(Product, quantity);

    int fd = open(PRODUCT_DATA_FILE_NAME, O_RDWR);
    if(fd == -1) {
        perror("Error of opening file\n");
        return 1;
    }

    off_t seek_result = lseek(fd, quantity_offset, SEEK_SET);
    if(seek_result == (off_t)-1) {
        perror("lseek failed to find quantity_offset before write");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;

    }

    ssize_t result = write_partial(fd, (void*)&quantity, sizeof(quantity));
    if(result < 0) {
        perror("Error: fail to write data\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }

    lseek(fd, quantity_offset, SEEK_SET);
    if(seek_result == (off_t)-1) {
        perror("lseek failed to find quantity_offset before read");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;

    }

    result = read_partial(fd, (void*)&read_quantity, sizeof(read_quantity));
    if(result < 0) {
        perror("Error: fail to read data\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    if(read_quantity != quantity) { 
        printf("Error: read and write data mismatch, actual:%d, expected:%d \n", read_quantity, quantity); 
    }

    return 0;
}

static ssize_t read_partial(int fd, const void *buf, size_t total_byte)
{
    size_t byte_read = 0;
    while (byte_read < total_byte) {
        ssize_t read_chunk = read(fd, (void*)buf + byte_read, total_byte - byte_read);
        if(read_chunk < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("Error: Fail to read chunk data");
            return -1;
        }
        else if(read_chunk == 0) {
            return 0;
        }
        byte_read += read_chunk;
    };
    return byte_read;
}

static ssize_t write_partial(int fd, const void *buf, size_t total_byte)
{
    size_t byte_written = 0;
    while (byte_written < total_byte) {
        ssize_t written_chunk = write(fd, (void*)buf + byte_written, total_byte - byte_written);
        if(written_chunk < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("Error: Fail to write chunk data");
            return -1;
        }
        else if(written_chunk == 0) {
            return 0;
        }
        byte_written += written_chunk;
    };
    return byte_written;
}

e_menu_t print_menu()
{
    e_menu_t menu;
    int temp;
    printf("Menu\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    if(scanf("%d", (int*)&temp) != 1) {
        printf("Error: Invalid input argument\n");
        return 1;
    }
    menu = (e_menu_t)temp;
    return menu;
}

static int write_to_file(Product product)
{
    int fd = open(PRODUCT_DATA_FILE_NAME, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if(fd == -1) {
        perror("Error of opening file\n");
        return 1;
    }

    ssize_t count = write(fd, (void*)&product, sizeof(product));
    if(count != sizeof(product)) {
        printf("Error: write data mismatch, byte written:%lu\n", count);
        return 1;
    }

    printf("Write product info to %s successfully\n", PRODUCT_DATA_FILE_NAME);
    printf("id: %d,\t name: %s,\t quantity: %d,\t price: %.2f\n", product.id, product.name, product.quantity, product.price);

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    return 0;
}

#ifdef PRELOAD_TEST_DATA
static int load_product_list()
{
    int err = 0;
    for(long unsigned int i = 0; i < INDEX(product_test_list); i++) {
        Product product = product_test_list[i];
        err = write_to_file(product);
        product_id_list[product.id] = 1;
        if(err) {
            printf("Error: fail to write to %s\n", PRODUCT_DATA_FILE_NAME);
            return 1;
        }
    }
    return 0;
}
#endif

int main()
{
    int err = 0;
    e_menu_t menu;
    Product product;
#ifdef PRELOAD_TEST_DATA
    err = load_product_list();
#endif
    while(1)
    {
        if(err == 0) {
            menu = print_menu();
        }
        else {
            printf("Error: Fail to process menu operation\n");
            return 1;
        }
        switch(menu) {
            case ADD_PRODUCT:
                err = add_product(&product);
                if(err == 0) {
                   err = write_to_file(product);
                }
                break;
            case LIST_ALL_PRODUCT:
                err = list_all_product();
                break;
            case SHOW_PRODUCT_INDEX:
                err = show_product_index();
                break;
            case UPDATE_QUANTITY_INDEX:
                err = update_quantity_index();
                break;
            case EXIT_CHOICE:
                printf("Exit\n");
                return 0;
            default:
                printf("Error: Invalid choice\n");
                continue;
        }

    }
    return 0;
}
