#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
void copy(char **p1, char *p2)
{
    *p1 = malloc(strlen(p2) + 1);
    strcpy(*p1, p2);
}

// i - 起始下标
int readLine(char **pars, int i)
{
    int d = 1024;
    char buf[d];
    int j = 0;
    // 读取一行
    while (read(0, buf + j, 1))
    {
        /* 遇到换行符 */
        if (buf[j] == '\n')
        {
            buf[j] = 0;
            break;
        }
        j++;
        if (j == d)
        {
            fprintf(2, "Parameters too long in one line\n");
            exit(1);
        }
    }

    if (j == 0)
    {
        /* 没有读取到内容 */
        return -1;
    }

    /* 以空格划分 */
    int k = 0;
    while (k < j)
    {
        if (i > MAXARG)
        {
            fprintf(2, "Too many parameters\n");
            exit(1);
        }
        while ((k < j) && (buf[k] == ' '))
        {
            /* 忽略 如'  abc   abc'中的多个空格 */
            k++;
        }
        int l = k; /* 起始位置 */
        while ((k < j) && (buf[k] != ' '))
        {
            /* 保留字符串 */
            k++;
        }
        buf[k++] = 0;
        copy(&pars[i], buf + l);
        i++;
    }

    return i;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: xargs command [args]\n");
        exit(1);
    }
    else
    {
        int i;
        char *pars[MAXARG]; /* 参数字符串数组 */
        for (i = 1; i < argc; i++)
        {
            copy(&pars[i - 1], argv[i]);
        }
        int end;
        while ((end = readLine(pars, argc - 1)) != -1)
        {
            pars[end] = 0;
            if (fork() == 0)
            {
                exec(pars[0], pars);
                exit(1);
            }
            else
            {
                wait(0);
            }
        }
    }
    exit(0);
}