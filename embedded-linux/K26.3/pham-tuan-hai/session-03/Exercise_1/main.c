#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
	int id;
	char name[64];
	int age;
	float gpa;
} Student;


void add_student(int fd) {
    Student s;
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
    scanf("%s", s.name);

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

    lseek(fd, 0, SEEK_END);
    ssize_t bytes_written = write(fd, &s, sizeof(Student));
    if (bytes_written == sizeof(Student)) {
        printf("Adding success!\n");
    }
}

void list_students(int fd) {
	Student s;

	lseek(fd, 0, SEEK_SET);

	printf("=====DANH SACH SINH VIEN=====");

	while(read(fd, &s, sizeof(Student)) == sizeof(Student)){ 
		printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n",
			s.id, s.name, s.age, s.gpa);
	}
}

void find_student(int fd) {
	int target_id;
	printf("Nhap ID: ");
	scanf("%d", &target_id);

	Student s;
	int found = 0;

	lseek(fd, 0, SEEK_SET);

	while(read(fd, &s, sizeof(Student)) == sizeof(Student)){
		if (s.id == target_id) {
			found = 1;
			printf("FOUND: ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
		}
	}
	if(found != 1) { printf("Khong tim thay"); }

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
			        close(fd);
				return 0;
				break;
			default:
				printf("ERORR! Vui long nhap tu 1 -> 4");
		}
	}
}
