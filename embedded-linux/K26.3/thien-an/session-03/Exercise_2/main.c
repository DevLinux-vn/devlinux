#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

#define FILE_NAME "products.dat"
#define PRODUCT_NAME_MAX 64

typedef struct {
	int    id;
	char   name[PRODUCT_NAME_MAX];
	int    quantity;
	double price;
} Product;

void add_product(void)
{
	Product p;
	printf("Enter product's id: ");
	scanf("%d", &p.id);
	printf("Enter name: ");
	scanf(" %[^\n]", p.name);
	printf("Enter quantity: ");
	scanf("%d", &p.quantity);
	printf("Enter price: ");
	scanf("%lf", &p.price);

	int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		perror("Error opening file");
		return;
	}

	ssize_t written = write(fd, &p, sizeof(Product));
	if (written != sizeof(Product))
	{
		perror("Error writing to file");
	}
	else
	{
		printf("Product added successfully.\n");
	}

	close(fd);
}

void list_all_product(void)
{
	int fd = open(FILE_NAME, O_RDONLY);
	if (fd < 0)
	{
		perror("Error opening file");
		return;
	}
	
	Product p;
	int count = 0;
	ssize_t bytes_read;

	printf("\n--- List of All Products ---\n");
	while ((bytes_read = read(fd, &p, sizeof(Product))) > 0)
	{
		if (bytes_read != sizeof(Product)) break;
		printf("[Index: %d] ID: %d | Name: %s | Quantity: %d | Price: %.2f\n", count, p.id, p.name, p.quantity, p.price);
		count++;
	}
	
	if (bytes_read < 0) {
		perror("read error");
	}
	
	if (count == 0) {
		printf("The list is empty.\n");
	}
	close(fd);
}

void show_product_by_index()
{
	int target_index;
	printf("Enter Target index to find: ");
	scanf("%d", &target_index);
	
	if (target_index < 0) {
		printf("Index must be >= 0\n");
		return;
	}
	
	int fd = open(FILE_NAME, O_RDONLY);
	if (fd < 0)
	{
		perror("Error opening file");
		return;
	}

	Product p;
	off_t offset = (off_t)target_index * sizeof(Product);
	if (lseek(fd, offset, SEEK_SET) == -1)
	{
		perror("Error lseek");
	}
	else
	{
		ssize_t bytes = read(fd, &p, sizeof(Product));
		if (bytes < 0)
		{
			perror("read error");
		}
		else if (bytes == 0)
		{
			printf("Index %d out of bounds\n", target_index);
		}
		else if (bytes != sizeof(Product)) 
		{
			printf("Partial read error\n");
		}
		else
		{
			printf("ID: %d | Name: %s | Quantity: %d | Price: %.2f\n", p.id, p.name, p.quantity, p.price);
		}
	}
	
	close(fd);
}

void update_by_index()
{
	int target_index;
	int updated_quantity;
	printf("Enter Target index to find: ");
	scanf("%d", &target_index);
	
	if (target_index < 0) {
		printf("Index must be >= 0\n");
		return;
	}
	
	int fd = open(FILE_NAME, O_WRONLY);
	if (fd < 0)
	{
		perror("Error opening file");
		return;
	}

	printf("Updated quantity: ");
	scanf("%d", &updated_quantity);
	
	off_t offset = (off_t)target_index * sizeof(Product);
	off_t field_offset = offset + offsetof(Product, quantity);
	
	if (lseek(fd, field_offset, SEEK_SET) == -1)
	{
		perror("Error lseek");
	}
	else
	{
		ssize_t written = write(fd, &updated_quantity, sizeof(int));
		if (written != sizeof(int))
		{
			perror("Error writing to file");
		}
		else
		{
			printf("Quantity of index %d is updated successfully to %d\n", target_index, updated_quantity);
		}
	}
	
	close(fd);
}

int main()
{
	int choice;
	while (1)
	{
		printf("\n1. Add product\n");
		printf("2. Show product by index\n");
		printf("3. Update quantity by index\n");
		printf("4. List all products\n");
		printf("5. Exit\n");
		printf("Choice: ");

		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			printf("Invalid input. Please enter a number\n");
			continue;
		}

		switch (choice)
		{
			case 1:
				add_product();
				break;
			case 2:
				show_product_by_index();
				break;
			case 3:
				update_by_index();
				break;
			case 4:
				list_all_product();
				break;
			case 5:
				printf("Exiting...\n");
				return 0;
			default:
				printf("Invalid choice. Please enter from 1 - 5\n");
		}
	}
	return 0;
}