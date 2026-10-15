#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void add_product(int fd) {
	Product p;

	printf("ID: "); scanf("%d", &p.id);
	printf("Name: "); scanf("%s", p.name);
	printf("Quantity: "); scanf("%d", &p.quantity);
	printf("Price: "); scanf("%lf", &p.price);

	lseek(fd, 0, SEEK_END);
	ssize_t bytes_written = write(fd, &p, sizeof(Product));
	if (bytes_written == sizeof(Product)) {
		printf("Adding success!\n");
	}
}

void show_product(int fd) {
	int index;

	Product p;
	printf("Nhap index: \n"); scanf("%d", &index);

	off_t off_set = sizeof(Product) * (off_t)index;

	lseek(fd, off_set, SEEK_SET);

	if(read(fd, &p, sizeof(Product)) == sizeof(Product)){
		printf("[ID: %d | Name: %s | Quantity: %d | Price: %lf]\n", 
			p.id, p.name, p.quantity, p.price);
  
	}else {
		printf("Index don't exiest!");
	}
}

void  update_quantity_by_index(int fd) {
	int index, new_quantity;
	Product p;

	printf("Index: "); scanf("%d", &index);
	printf("New quantity: "); scanf("%d", &new_quantity);

	off_t off_set = sizeof(Product) * (off_t)index;

	lseek(fd, off_set, SEEK_SET);

	if(read(fd, &p, sizeof(Product)) != sizeof(Product)){
		printf("Khong ton tai Index!");
		return;
	}

	off_t field_offset = off_set + offsetof(Product, quantity);

	lseek(fd, field_offset, SEEK_SET);

	write(fd, &new_quantity, sizeof(int));

	printf("Success Replacing!\n");
}

void list_products(int fd) {
	Product p;

	lseek(fd, 0, SEEK_SET);

	while(read(fd, &p, sizeof(Product)) == sizeof(Product)) {
		printf("ID: %d | Name: %s | Quantity: %d | Price: %.2lf\n",
		p.id, p.name, p.quantity, p.price);
	}
}

int main() {
	Product p;

	int fd = open("products.bat", O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		perror("ERROR OPEN FILE!");
		return 1;
	}

	while(1) {
    system("clear");
		printf("===========DANH SACH LENH==========\n");
		printf("1.Add product\n");
		printf("2.Show product by index\n");
		printf("3.Update Quantity by index\n");
		printf("4.List all products\n");
		printf("5.Exit\n");

		int number;
		printf("Vui long nhap so: \n");
		scanf("%d", &number);
    
    while(getchar() != '\n');
    
		switch (number) {
			case 1:
				add_product(fd);
				break;
			case 2:
				show_product(fd);
				break;
			case 3:
				update_quantity_by_index(fd);
				break;
      case 4:
        list_products(fd);
        break;
			case 5:
				printf("Thoat chuong trinh!\n");
			        close(fd);
				return 0;
				break;
			default:
				printf("ERORR! Vui long nhap tu 1 -> 5\n");
		}
    printf("\nNhan Enter de tiep tuc...");
    while(getchar() != '\n');
    getchar();
	}
}
