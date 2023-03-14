#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
	int pid;

	// 系统调用pipe()，创建两个管道
	int p[2];
	pipe(p);  

	// 系统调用fork()，创建子进程，并分别对父进程和子进程进行操作
	if (fork() == 0) // child (receive -> send)
	{
		pid = getpid();  // 获取子进程pid
		char buf[2];  // 用来read后保存数据

		// 【接收】
		// 返回值表示读到的字节数。若!= 1，说明读完了
		// 读文件p[0]数据，写入buf
		if (read(p[0], buf, 1) != 1)  // p[0]：文件描述符  buf：保存读到的数据  1：读1个字节
		{
			fprintf(2, "failed to read in child\n");
			exit(1);
		}
		close(p[0]);  // 关闭不需要的文件描述符
		printf("%d: received ping\n", pid);

		// 【发送】
		if(write(p[1], buf, 1) != 1)
		{
			fprintf(2, "failed to write in child\n");
			exit(1);
		}
		close(p[1]);
		exit(0);
	} else {	     // parent (send -> receive)
		pid = getpid();

		char info[2] = "a";
		char buf[2];
		buf[1] = 0;

		// 【发送】
		// 读info数据，写入文件p[1]
		if (write(p[1], info, 1) != 1)
		{
			fprintf(2, "failed to write in parent\n");
			exit(1);
		}

		// 【等待子进程发送】
		close(p[1]);
		wait(0);

		// 【接收】
		if(read(p[0], buf, 1) != 1){
			fprintf(2, "failed to read in parent\n");
			exit(1);
		}
		printf("%d: received pong\n", pid);
		close(p[0]);
		exit(0);
	}
}