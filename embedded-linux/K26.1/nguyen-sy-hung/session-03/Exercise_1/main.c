#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILE_NAME        "students.dat"
#define NAME_LENGTH      64


typedef struct {
	int id;
	char name[NAME_LENGTH];
	int age;
	float gpa;
} Student;


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


static void add_student(int fd)
{
	Student student;

	printf("Enter ID: ");
	scanf("%d", &student.id);

	getchar();

	printf("Enter name: ");
	fgets(student.name, sizeof(student.name), stdin);
	student.name[strcspn(student.name, "\n")] = '\0';

	printf("Enter age: ");
	scanf("%d", &student.age);

	printf("Enter GPA: ");
	scanf("%f", &student.gpa);


	if (lseek(fd, 0, SEEK_END) == -1) {
		perror("lseek");
		return;
	}


	if (write_full(fd, &student, sizeof(Student)) == -1) {
		perror("write");
		return;
	}

	printf("Student added successfully\n");
}


static void print_student(const Student *student)
{
	printf("ID   : %d\n", student->id);
	printf("Name : %s\n", student->name);
	printf("Age  : %d\n", student->age);
	printf("GPA  : %.2f\n", student->gpa);
}


static void list_students(int fd)
{
	Student student;
	ssize_t ret;


	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		return;
	}


	printf("\n===== STUDENT LIST =====\n");


	while (1) {
		ret = read_full(fd, &student, sizeof(Student));

		if (ret == 0)
			break;

		if (ret == -1) {
			perror("read");
			return;
		}

		if (ret != sizeof(Student)) {
			fprintf(stderr,
				"Error: corrupted student record\n");
			return;
		}

		print_student(&student);
		printf("------------------------\n");
	}
}


static void find_student(int fd)
{
	int id;
	int found = 0;
	Student student;
	ssize_t ret;


	printf("Enter student ID: ");
	scanf("%d", &id);


	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		return;
	}


	while (1) {
		ret = read_full(fd, &student, sizeof(Student));

		if (ret == 0)
			break;

		if (ret == -1) {
			perror("read");
			return;
		}

		if (ret != sizeof(Student)) {
			fprintf(stderr,
				"Error: corrupted student record\n");
			return;
		}

		if (student.id == id) {
			printf("\nStudent found\n");
			print_student(&student);
			found = 1;
			break;
		}
	}


	if (!found)
		printf("Student not found\n");
}


static void print_menu(void)
{
	printf("\n========== MENU ==========\n");
	printf("1. Add student\n");
	printf("2. List all students\n");
	printf("3. Find student by ID\n");
	printf("4. Exit\n");
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
			add_student(fd);
			break;

		case 2:
			list_students(fd);
			break;

		case 3:
			find_student(fd);
			break;

		case 4:
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