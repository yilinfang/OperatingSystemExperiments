#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    dp = opendir(dir);
    if (dp == NULL)
    {
        printf("can not open %s.\n", dir);
        return;
    }
    chdir(dir);
    while(entry = readdir(dp))
    {
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        exit(-1);
    }

    return 0;
}
