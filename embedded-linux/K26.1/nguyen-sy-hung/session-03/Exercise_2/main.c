#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>

#define FILE_NAME        "products.dat"
#define NAME_LENGTH      64


typedef struct {
	int id;
	char name[NAME_LENGTH];
	int quantity;
	double price;
} Product;


static int write_full(int fd, const void *buf, size_t size)
{
	const char *ptr = buf;
	size_t total = 0;

	while (total < size) {
		ssize_t bytes;

		bytes = write(fd, ptr + total, size - total);

		if (bytes == -1)
			return -1;

		total += bytes;
	}

	return 0;
}


static ssize_t read_full(int fd, void *buf, size_t size)
{
	char *ptr = buf;
	size_t total = 0;

	while (total < size) {
		ssize_t bytes;

		bytes = read(fd, ptr + total, size - total);

		if (bytes == 0)
			return total;

		if (bytes == -1)
			return -1;

		total += bytes;
	}

	return total;
}


static int is_valid_index(int fd, int index)
{
	struct stat st;
	off_t records;

	if (index < 0)
		return 0;

	if (fstat(fd, &st) == -1) {
		perror("fstat");
		return 0;
	}

	records = st.st_size / sizeof(Product);

	return index < records;
}


static void print_product(const Product *product)
{
	printf("ID       : %d\n", product->id);
	printf("Name     : %s\n", product->name);
	printf("Quantity : %d\n", product->quantity);
	printf("Price    : %.2lf\n", product->price);
}


static void add_product(int fd)
{
	Product product;

	printf("Enter ID: ");
	if (scanf("%d", &product.id) != 1) {
		fprintf(stderr, "Invalid ID\n");
		return;
	}

	getchar();

	printf("Enter name: ");
	fgets(product.name,
	      sizeof(product.name),
	      stdin);

	product.name[strcspn(product.name, "\n")] = '\0';

	printf("Enter quantity: ");
	if (scanf("%d", &product.quantity) != 1) {
		fprintf(stderr, "Invalid quantity\n");
		return;
	}

	printf("Enter price: ");
	if (scanf("%lf", &product.price) != 1) {
		fprintf(stderr, "Invalid price\n");
		return;
	}

	if (lseek(fd, 0, SEEK_END) == -1) {
		perror("lseek");
		return;
	}

	if (write_full(fd, &product,
		       sizeof(Product)) == -1) {
		perror("write");
		return;
	}

	printf("Product added successfully\n");
}


static void show_product_by_index(int fd)
{
	int index;
	Product product;
	ssize_t ret;
	off_t offset;


	printf("Enter index: ");

	if (scanf("%d", &index) != 1) {
		fprintf(stderr, "Invalid index\n");
		return;
	}


	if (!is_valid_index(fd, index)) {
		printf("Product not found\n");
		return;
	}


	offset = (off_t)index * sizeof(Product);

	if (lseek(fd, offset, SEEK_SET) == -1) {
		perror("lseek");
		return;
	}


	ret = read_full(fd, &product,
			sizeof(Product));


	if (ret != sizeof(Product)) {
		fprintf(stderr,
			"Cannot read product record\n");
		return;
	}


	printf("\nProduct information\n");
	printf("-------------------\n");

	print_product(&product);
}


static void update_quantity(int fd)
{
	int index;
	int quantity;
	off_t offset;


	printf("Enter index: ");

	if (scanf("%d", &index) != 1) {
		fprintf(stderr, "Invalid index\n");
		return;
	}


	if (!is_valid_index(fd, index)) {
		printf("Product not found\n");
		return;
	}


	printf("Enter new quantity: ");

	if (scanf("%d", &quantity) != 1) {
		fprintf(stderr, "Invalid quantity\n");
		return;
	}


	offset = (off_t)index * sizeof(Product);
	offset += offsetof(Product, quantity);


	if (lseek(fd, offset, SEEK_SET) == -1) {
		perror("lseek");
		return;
	}


	if (write_full(fd,
		       &quantity,
		       sizeof(quantity)) == -1) {
		perror("write");
		return;
	}


	printf("Quantity updated successfully\n");
}


static void list_products(int fd)
{
	Product product;
	ssize_t ret;


	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		return;
	}


	printf("\n======= PRODUCT LIST =======\n");


	while (1) {
		ret = read_full(fd,
				&product,
				sizeof(Product));


		if (ret == 0)
			break;


		if (ret != sizeof(Product)) {
			fprintf(stderr,
				"Corrupted product record\n");
			return;
		}


		print_product(&product);

		printf("----------------------------\n");
	}
}


static void print_menu(void)
{
	printf("\n========== MENU ==========\n");
	printf("1. Add product\n");
	printf("2. Show product by index\n");
	printf("3. Update quantity by index\n");
	printf("4. List all products\n");
	printf("5. Exit\n");
	printf("==========================\n");
	printf("Choose: ");
}


int main(void)
{
	int fd;
	int choice;


	fd = open(FILE_NAME,
		  O_RDWR | O_CREAT,
		  0644);


	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}


	while (1) {
		print_menu();

		if (scanf("%d", &choice) != 1) {
			fprintf(stderr,
				"Invalid input\n");

			if (close(fd) == -1)
				perror("close");

			return EXIT_FAILURE;
		}


		switch (choice) {
		case 1:
			add_product(fd);
			break;

		case 2:
			show_product_by_index(fd);
			break;

		case 3:
			update_quantity(fd);
			break;

		case 4:
			list_products(fd);
			break;

		case 5:
			if (close(fd) == -1) {
				perror("close");
				return EXIT_FAILURE;
			}

			printf("Exit program\n");
			return EXIT_SUCCESS;

		default:
			printf("Invalid option\n");
			break;
		}
	}
}