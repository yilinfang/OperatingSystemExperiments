#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <math.h>
using namespace std;
void printmode(int mode)
{
    char str[11];
    strcpy(str, "----------");
    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';
    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';
    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
    cout << str;
} //打印文件权限信息

void printime(time_t s)
{
    struct tm *p = gmtime(&s);
    char str[80];
    strftime(str, 80, "%Y-%m-%d %H:%M:%S", p);
    cout << str;
} //打印时间

void printuid(uid_t uid)
{
    struct passwd *pw;
    if ((pw = getpwuid(uid)) != NULL)
    {
        cout << pw->pw_name;
    }
} //打印uid

void printgid(gid_t gid)
{
    struct group *g;
    if ((g = getgrgid(gid)) != NULL)
    {
        cout << g->gr_name;
    }
} //打印gid

void printlen(size_t s)
{
    cout.flags(ios::right);
    cout << dec << setw(9) << s;
} //打印size

int judgename(struct dirent entry)
{
    int length = strlen(entry.d_name);
    if (length == 1)
    {
        if (entry.d_name[0] == '.')
        {
            return 1; //当前目录
        }
    }
    if (length == 2)
    {
        if (entry.d_name[0] == '.' && entry.d_name[1] == '.')
        {
            return 1; //父目录
        }
    }
    else if (entry.d_type == DT_DIR)
    {
        return 2; //子目录
    }
    return 0;     //文件
}
void printstat(struct stat s)
{
    printmode(s.st_mode); //打印权限信息
    cout << " ";
    cout << s.st_nlink; //硬连接数
    cout << " ";
    printuid(s.st_uid); //打印uid
    cout << " ";
    printgid(s.st_gid); //打印gid
    cout << " ";
    printlen(s.st_size); //打印size
    cout << " ";
    printime(s.st_mtime); //打印最后修改时间
};
void printdir(const char *dir, int depth)
{
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    chdir(dir);
    if ((dp = opendir(".")) == NULL)
    {
        cout << "打开目录失败" << endl;
        return;
    }
    while ((entry = readdir(dp)) != NULL) //遍历子文件和子目录
    {
        lstat(entry->d_name, &statbuf);
        if (judgename(*entry) == 1)
        {
            continue;
        }
        else if (judgename(*entry) == 0)
        {
            for (int i = 0; i < depth / 2; i++)
            {
                cout << " ";
            }
            cout << depth << " ";
            printstat(statbuf);
            cout << " " << entry->d_name;
            if (entry->d_name[0] == '.')
            {
                cout << "(隐藏)";
            }
            cout << endl;
        }
        else if (judgename(*entry) == 2)
        {
            for (int i = 0; i < depth / 2; i++)
            {
                cout << " ";
            }
            cout << depth << " ";
            printstat(statbuf);
            cout << " " << entry->d_name;
            if (entry->d_name[0] == '.')
            {
                cout << "(隐藏)";
            }
            cout << ":" << endl;
            printdir(entry->d_name, depth + 4);
        }
    };
    chdir("..");
    closedir(dp);
}

int main(int argv, char *argc[])
{
    if (argv == 1)
    {
        printdir(".", 0);
    }
    else
    {
        printdir(argc[1], 0);
    }
    return 0;
}