#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn)) void
new_proc(int p[2])
{
    int base_prime;
    int flag;
    int n;
    close(p[1]); /* only read */
    if (read(p[0], &base_prime, 4) != 4)
    {
        fprintf(2, "process failed to read from the pipe\n");
        exit(1);
    }
    printf("prime %d\n", base_prime);

    flag = read(p[0], &n, 4);
    if (flag)
    {
        /* read success */
        int new_pipe[2];
        pipe(new_pipe);
        if (fork() == 0)
        {
            /* grandson */
            new_proc(new_pipe);
        }
        else
        {
            close(new_pipe[0]);
            if (n % base_prime)
                write(new_pipe[1], &n, 4);
            while (read(p[0], &n, 4))
            {
                if (n % base_prime)
                    write(new_pipe[1], &n, 4);
            }
            close(p[0]);        /* close read pipe from parent*/
            close(new_pipe[1]); /* close write pipe for child */
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, const char *argv[])
{
    int p[2];
    pipe(p);
    if (fork() == 0)
    {
        /* child */
        new_proc(p);
    }
    else
    {
        /* parent */
        close(p[0]); /* only write */
        for (int i = 2; i <= 35; i++)
        {
            if (write(p[1], &i, 4) != 4)
            {
                fprintf(2, "first process failed to write %d into the pipe\n", i);
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    return 0;
}
