#include <stdio.h>
#include "log.h"

int main_callback(int sendid, int recvid, int type, void *input, void *output);

int main()
{
	int ret = 0,i;
	struct ModuleInfo modinf;
	char Buffer[40];

	memset(&modinf,0,sizeof(struct ModuleInfo));

	ret = log_init(NULL, main_callback, &modinf);
	
	if(ret <= -1)
	{
		printf("初始化失败，程序退出\n");
		return -1;
	}
	
	while(1)
	{
		//提示信息
		puts("\n选择要测试的功能:");
		puts("1: 获取当前日志文件");		
		puts("2: 查询日志\n");
		puts("quit: 退出程序\n");

		//等待用户输入
		fgets(Buffer,sizeof(Buffer),stdin);
		if(!strlen(Buffer)) //如果没有输入信息，继续循环
			continue;
			
		if(Buffer[0] == '1')
			i = 1;
		else if(Buffer[0] == '2')
			i = 2;
		else
		{
			//程序的一个退出条件
			if(!strncmp(Buffer, "quit", 4)) 
			{
				break;
			}
			else
			{
				printf("输入错误：%s", Buffer);
				continue;
			}			
		}
		
		//执行用户选择
		switch(i)
		{
		case 1:
		{
			char logfile[20];
			modinf.control(0,4, NULL, logfile);
			printf("The log file is %s\n", logfile);
		}
			break;
		case 2:
		{
			struct log_query qlog;
			char buf[20];

			qlog.type = 5;
			qlog.from = 0;
			qlog.to = 0;
			
			modinf.control(0, 11, &qlog, buf);
		}
			break;
		default:
			printf("ERROR!\n");
		}
		
	}	
	
	//程序退出处理
	modinf.close( );

    return 0;
}


int main_callback(int sendid, int recvid, int type, void *input, void *output)
{
    int ret = 0;

    printf("callback is called!\n");
	printf("sendid=%d recvid=%d type=%d\n",sendid, recvid, type);
	
    return ret;
}
