/* Copyright (C) 2025 XYZ Corp. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stddef.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void print_menu() {
    printf("\n=== Product Management System ===\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

void add_product(int fd) {
    Product p;
    printf("\nEnter product ID: ");
    scanf("%d", &p.id);
    printf("Enter product name: ");
    scanf("%63s", p.name);
    printf("Enter product quantity: ");
    scanf("%d", &p.quantity);
    printf("Enter product price: ");
    scanf("%lf", &p.price);

    lseek(fd, 0, SEEK_END);
    if (write(fd, &p, sizeof(Product)) == sizeof(Product)) {
        printf("Product added successfully!\n");
    } else {
        printf("Error writing to file!\n");
    }
}

void show_product_by_index(int fd) {
    int index;
    Product p;
    off_t offset;

    printf("\nEnter product index: ");
    scanf("%d", &index);

    offset = (off_t)index * sizeof(Product);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf("Error seeking in file!\n");
        return;
    }

    if (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        printf("\n=== Product at index %d ===\n", index);
        printf("ID: %d\n", p.id);
        printf("Name: %s\n", p.name);
        printf("Quantity: %d\n", p.quantity);
        printf("Price: %.2f\n", p.price);
    } else {
        printf("Product not found at index %d!\n", index);
    }
}

void update_quantity_by_index(int fd) {
    int index, new_quantity;
    Product p;
    off_t offset, field_offset;

    printf("\nEnter product index: ");
    scanf("%d", &index);

    offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf("Error seeking in file!\n");
        return;
    }

    if (read(fd, &p, sizeof(Product)) != sizeof(Product)) {
        printf("Product not found at index %d!\n", index);
        return;
    }

    printf("Current quantity: %d\n", p.quantity);
    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);

    field_offset = offset + offsetof(Product, quantity);
    if (lseek(fd, field_offset, SEEK_SET) == -1) {
        printf("Error seeking to quantity field!\n");
        return;
    }

    if (write(fd, &new_quantity, sizeof(int)) == sizeof(int)) {
        printf("Quantity updated successfully!\n");
    } else {
        printf("Error writing to file!\n");
    }
}

void list_all_products(int fd) {
    Product p;
    ssize_t bytes_read;
    int count = 0;

    lseek(fd, 0, SEEK_SET);

    printf("\n=== All Products ===\n");
    while ((bytes_read = read(fd, &p, sizeof(Product))) == sizeof(Product)) {
        printf("\nProduct %d:\n", count);
        printf("  ID: %d\n", p.id);
        printf("  Name: %s\n", p.name);
        printf("  Quantity: %d\n", p.quantity);
        printf("  Price: %.2f\n", p.price);
        count++;
    }

    if (count == 0) {
        printf("No products found!\n");
    }
}

int main() {
    int fd = open("products.dat", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    int choice;
    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_product(fd);
                break;
            case 2:
                show_product_by_index(fd);
                break;
            case 3:
                update_quantity_by_index(fd);
                break;
            case 4:
                list_all_products(fd);
                break;
            case 5:
                close(fd);
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid option! Please try again.\n");
        }
    }

    return 0;
}
