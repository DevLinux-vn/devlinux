#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    for (int i = 1; i <= 15; i++)
    {
        fprintf(stderr,
                "<3> ERROR log %d\n",
                i);

        fprintf(stderr,
                "<4> WARNING log %d\n",
                i);

        fprintf(stderr,
                "<6> INFO log %d\n",
                i);

        sleep(2);
    }

    fprintf(stderr,
            "<3> Fatal error. Abort...\n");

    abort();

    return 0;
}
