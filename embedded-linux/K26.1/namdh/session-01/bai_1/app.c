#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *buffer = malloc(100);

    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    printf("Nhap chuoi: ");
    fgets(buffer, 100, stdin);

    printf("Ban vua nhap: %s", buffer);

    // Cố tình không free(buffer);

    return 0;
}