#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef struct {
	int id;
	char name[64];
	int age;
	float gpa;
} Student;

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

void add_student(int fd) {
    Student s;
    memset(&s, 0, sizeof(Student));
    int status;

    while (1) {
        printf("ID (> 0): ");
        status = scanf("%d", &s.id);
        if (status == 1 && s.id > 0) {
            break;
        }
        printf("Loi: ID phai la so nguyen duong.\n");
        while (getchar() != '\n'); 
    }

    printf("Name: "); 
    if (scanf("%63s", s.name) == 1) {
      while(getchar() != '\n');
    }

    while (1) {
        printf("Age (1 - 80): ");
        status = scanf("%d", &s.age);
        if (status == 1 && s.age > 0 && s.age <= 80) {
            break;
        }
        printf("Loi: Tuoi phai la so nguyen tu 1 den 80.\n");
        while (getchar() != '\n'); 
    }

    while (1) {
        printf("GPA (0.0 - 10.0): ");
        status = scanf("%f", &s.gpa);
        if (status == 1 && s.gpa >= 0.0 && s.gpa <= 10.0) {
            break;
        }
        printf("Loi: GPA phai la so thuc tu 0.0 den 10.0.\n");
        while (getchar() != '\n'); 
    }

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
    perror("lseek failed");
    return;
}
    if(write_full(fd, &s, sizeof(Student)) == -1) {
      perror("Write failed");
      return;
    }
    
    printf("Adding success!\n");
    
}

void list_students(int fd) {
	Student s;
  memset(&s, 0, sizeof(Student));

	if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
    perror("lseek failed");
    return;
  }

	printf("=====DANH SACH SINH VIEN=====");

	int status;
  while ((status = read_full(fd, &s, sizeof(Student))) == 1) {
      printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
  }
  if (status == -1) {
      printf("Read failed\n");
  }
}

void find_student(int fd) {
	int target_id;
	printf("Nhap ID: ");
	scanf("%d", &target_id);

	Student s;
  memset(&s, 0, sizeof(Student));
	int found = 0;

	if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
    perror("lseek failed");
    return;
}
  
  int status;
	while((status = read_full(fd, &s, sizeof(Student))) == 1){
		if (s.id == target_id) {
			found = 1;
			printf("FOUND: ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, 
                                                                              s.gpa);
      break;
		}
    if (status == -1) {
      printf("Read failed\n");
    }
	}
	if(!found) { printf("Khong tim thay"); }

	return;
}


int main() {
  int fd = open("students.dat", O_RDWR | O_CREAT, 0666);
  
  if (fd < 0) {
  	perror("ERROR OPEN FILE!");
  	return 1;
  }
	while(1) {
		printf("===========DANH SACH LENH==========\n");
		printf("1.Them hoc sinh\n");
		printf("2.Danh sach hoc sinh\n");
		printf("3.Tim hoc sinh theo ID\n");
		printf("4.Thoat ra\n");

		int number;
		printf("Vui long nhap so: \n");
		scanf("%d", &number);
		switch (number) {
			case 1:
				add_student(fd);
				break;
			case 2:
				list_students(fd);
				break;
			case 3:
				find_student(fd);
				break;
			case 4:
				printf("Thoat chuong trinh!");
        if (close(fd) == -1) {
          perror("close failed");
          return 1;
        }
				return 0;
				break;
			default:
				printf("ERORR! Vui long nhap tu 1 -> 4");
		}
	}
}
