#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static int write_full(int fd, const void *buf, size_t count) {
  size_t total_written = 0;
  const char *ptr = (const char *)buf;
  
  while (total_written < count) {
    ssize_t written = write(fd, ptr + total_written,
                              count - total_written);
    if (written < 0) {
      if(errno == EINTR) continue;
      
      return -1;
    }
    total_written += (size_t)written;
  }
  return 0;
}

//Read success: 1
//Read fail (error): -1
//End of file (success): 0;
static int read_full(int fd, void *buf, size_t count) {
  size_t total_read = 0;
  char *ptr = (char *)buf;
  
  while(total_read < count) {
    ssize_t n = read(fd, ptr + total_read, count - total_read);
    
    if(n < 0) {
      if(errno == EINTR) continue;
      
      return -1;
    }
    if (n == 0) {
      if (total_read == 0) return 0;
      return -1;
    }
    total_read += (size_t)n;
  }
  return 1;
}

void add_product(int fd) {
	Product p;
  memset(&p, 0, sizeof(Product));
  
	printf("ID: "); scanf("%d", &p.id);
	printf("Name: ");
  
  if (scanf("%63s", p.name) == 1) {
    while(getchar() != '\n');
  }
	printf("Quantity: "); scanf("%d", &p.quantity);
	printf("Price: "); scanf("%lf", &p.price);

	if(lseek(fd, 0, SEEK_END) == (off_t)-1) {
    perror("lseek failed");
    return;    
  }
	if(write_full(fd, &p, sizeof(Product)) == -1) {
		perror("Write failed\n");
    return;
	}
  
  if(fsync(fd) == -1) {
    perror("fsync failed");
  }
  
  printf("Adding success!\n");
}

void show_product(int fd) {
	int index;

	Product p;
  memset(&p, 0, sizeof(Product));
	printf("Nhap index: \n"); scanf("%d", &index);
  if(index < 0) {
    printf("Error negative number");
    return;
  }

	off_t offset = sizeof(Product) * (off_t)index;

	if(lseek(fd, offset, SEEK_SET) == (off_t)-1) {
    printf("lseek failed\n");
    return;
  }
  
  int status;
	if ((status = read_full(fd, &p, sizeof(Product))) == 1) {
		printf("[ID: %d | Name: %s | Quantity: %d | Price: %lf]\n", 
			p.id, p.name, p.quantity, p.price);
  
	}
  
  if (status == -1){
		printf("Read failed\n");
    return;
	}
}

void  update_quantity_by_index(int fd) {
	int index, new_quantity;
	Product p;
  memset(&p, 0, sizeof(Product));

	printf("Index: "); scanf("%d", &index);
  if(index < 0) {
    printf("Index can not be negative\n");
    return;
  }
	printf("New quantity: "); scanf("%d", &new_quantity);

	off_t offset = sizeof(Product) * (off_t)index;

	if(lseek(fd, offset, SEEK_SET) == (off_t)-1) {
    perror("lseek failed");
    return;
  }

	if (read_full(fd, &p, sizeof(Product)) != 1) {
		printf("Khong ton tai Index!\n");
		return;
	}

	off_t field_offset = offset + offsetof(Product, quantity);

	if(lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
    printf("lseek failed\n");
    return;
  }

//Update sau khi tim thay
	if(write_full(fd, &new_quantity, sizeof(int)) == -1) {
    perror("Write failed");
    return;
  }

  if(fsync(fd) == -1) {
    perror("fsync failed");
  }
  
	printf("Success Replacing!\n");
}

void list_products(int fd) {
	Product p;
  memset(&p, 0, sizeof(Product));

	if(lseek(fd, 0, SEEK_SET) == (off_t)-1) {
    printf("lseek failed\n");
    return;
  }
  
  int status;
	while((status = read_full(fd, &p, sizeof(Product))) == 1) {
		printf("ID: %d | Name: %s | Quantity: %d | Price: %.2lf\n",
		p.id, p.name, p.quantity, p.price);
	}
  
  if(status == -1) {
    printf("Read failed\n");
    return;
  }
}

int main() {
	Product p;
  memset(&p, 0, sizeof(Product));

	int fd = open("products.dat", O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		perror("ERROR OPEN FILE!");
		return 1;
	}

	while(1) {
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
          if (close(fd) == -1) {
          perror("close failed");
          return 1;
        }
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
