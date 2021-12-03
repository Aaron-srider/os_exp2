#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

using std::cout;
using std::endl;

void log(const char* ch) {
    cout << ch << endl;
}

int main()
{  int fd,page_size;
    pid_t pid;
    page_size = getpagesize();
    cout << "当前系统页面大小：" << page_size << endl;

    //可读可写
    fd=open("./f1",O_RDWR);

    char * src = (char *) mmap(NULL,page_size,PROT_WRITE,MAP_SHARED,fd,0);

    if(src==MAP_FAILED)
    {
        printf("error\n");
        exit(1);
    }

    //分配内存空间
    void * dst = malloc(page_size);

    memmove(dst, src, page_size);

    cout << "dst 内容：" << (char*)dst << endl;

    pid=fork();
    if(pid==0)
    { /*  子进程空间  */
        char* temp = static_cast<char *>(memchr(dst, 'W', page_size));
        cout << "W后面的内容："<<temp << endl;
        cout << "temp起始地址："<<(void *)temp << endl;

        log("> 子进程修改共享区");
        * temp = '6';
        * (temp+1) = '1';
        * (temp+2) = '7';

        memmove(src, dst, page_size);
        cout << "src 内容：" << (char*)src << endl;
        cout << "pid：" << getpid() << endl;
        cout << "ppid：" << getppid()<< endl;
        close(fd);
        free(dst);
        dst = nullptr;


        cout << "src======" <<  (void*)src << endl;
    }
    else if(pid>0){  /* 父进程空间  */
        //wait(NULL);
        log("> 父进程唤醒");

        log("> 父进程覆盖写入5个'0'到共享区");
        memset(src, '0', 5);

        cout << "父进程修改后src的值：" << (char*)src << endl;

        log("> 读取f1文件内容");

        fd=open("./f1",O_RDWR);

        char* buffer = new char[255];
        int count;
        while((count =  read(fd, buffer, 255)) != 0) {
            cout << "f1文件的最终内容：" << buffer << endl;
        }

        log("> 关闭文件f1");
        close(fd);

        cout << "src======" <<   (void*)src << endl;
    }
    printf("PID is: %d,  PPID is: %d,  The src value of end is: %s\n", getpid(),
           getppid(), src);    /*显示src指向的共享存储区内容*/
    if(munmap(src,page_size)==0)     /*释放共享存储区*/
    { printf("PID is: %d,  PPID is: %d,munmap success\n\n",getpid(), getppid()); }
    else
    { printf("PID is: %d,  PPID is: %d,  munmap failed\n\n",getpid(), getppid()); }

    fd=open("./f1",O_RDWR);
    char* buffer = new char[255];
    int count;
    while((count =  read(fd, buffer, 255)) != 0) {
        cout << "f1文件的最终内容：" << buffer << endl;
    }
    while(1) {

    }
}

