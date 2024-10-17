#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid;
    int p[2]; /* pipe io */
    pipe(p);

    if (fork() == 0)
    {
        /* child */
        int pid = getpid();
        char buf[2];
        if (read(p[0], buf, 1) != 1)
        {
            fprintf(2, "child: read failed\n");
            exit(1);
        }
        close(p[0]);
        printf("%d: received ping\n", pid);
        if (write(p[1], buf, 1) != 1)
        {
            fprintf(2, "child: write failed\n");
            exit(1);
        }
        close(p[1]);
        exit(0);
    }
    else
    {
        /* parent */
        pid = getpid();
        char message[2] = "a";
        char buf[2];
        buf[1] = 0;
        if (write(p[1], message, 1) != 1)
        {
            fprintf(2, "parent: write failed\n");
            exit(1);
        }
        // wait for child 
        close(p[1]);
        wait(0);
        if(read(p[0], buf, 1) != 1){
            fprintf(2, "parent: read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", pid);
        close(p[0]);
        exit(0);
    }
}