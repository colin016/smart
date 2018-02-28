#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle.h"

struct ModuleInfo modinf;
int main_callback(int sendid, int recvid, int type, void *input, void *output);

static int menu();//选择菜单
static int get_tasknum();//获得动态库支持的监控任务数量
static int get_task();//获取动态库支持的监控任务
static int get_taskparanum();//获得动态库支持的某监控任务可选参数组数量
static int get_taskpara();//获得一个监控任务所有支持可选参数组编号及其提示信息
static int get_taskattunum();//获得动态库支持的某监控任务需要的附件信息数量
static int get_taskattinfo();//获得动态库需要的附件信息
static int reset_alltask();//重置所有监控任务
static int add_task();//添加或修改某个监控任务

int main()
{
	int ret = 0,i;

	memset(&modinf,0,sizeof(struct ModuleInfo));

	ret = handle_init(NULL, main_callback, &modinf);
	
	if(ret <= -1)
	{
		printf("初始化失败，程序退出\n");
		return -1;
	}
	
	while(1)
	{
        i=menu();
		printf("%d\n",i);
		//执行用户选择
		switch(i)
		{
		case 0:
		    get_tasknum();
		    break;
		case 1:
		    get_task();
		    break;
		case 2:
            get_taskparanum();
			break;
		case 3:
		    get_taskpara();
		    break;
		case 4:
		    get_taskattunum();
		    break;
		case 5:
		    get_taskattinfo();
		    break;
		case 6:
		    modinf.control(0, HANDLE_STOP_ATASK, NULL, NULL);
		    break;
		case 7:
		    reset_alltask();
		    break;
		case 8:
		    add_task();
		    break;
		case 9:
		    {
             int num;
             printf("请输入任务编号\n");
             scanf("%d",&num);
             getchar();
		     modinf.control(0, HANDLE_DEL_TASK, &num, NULL);
		    }
		    break;
		case 10:
		    {
                int num=1;
                modinf.control(0, HANDLE_START_ALLTASK, &num, NULL);
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




static int menu()
{
	int i=0;
	char Buffer[6];
	int blen;

	//提示信息
	puts("\n选择要测试的功能:");
	puts("0:获得动态库支持的监控任务数量");
	puts("1:查询动态库支持的监控任务");		
	puts("2:获得动态库支持的某监控任务可选参数组数量");
	puts("3:获得一个监控任务所有支持可选参数组编号及其提示信息");
	puts("4:获得动态库支持的某监控任务需要的附件信息数量");
	puts("5:获得动态库需要的附件信息");
	puts("6:停止所有监控任务");
	puts("7:重置所有监控任务");
	puts("8:添加或修改某个监控任务");
	puts("9:删除某个监控任务");
	puts("10:启动所有监控任务");
	puts("quit: 退出程序\n");

	//等待用户输入
	A:fgets(Buffer,sizeof(Buffer),stdin);
	if(!strlen(Buffer)) //如果没有输入信息，继续循环
		goto A;
    
    blen=strlen(Buffer)-1;

	if (Buffer[0]=='0'&&blen==1)
		i=0;
	else if(Buffer[0] == '1'&&blen==1)
		i = 1;
	else if(Buffer[0] == '2'&&blen==1)
		i = 2;
	else if (Buffer[0]=='3'&&blen==1)
	    i=3;
	else if(Buffer[0]=='4'&&blen==1)
		i=4;
	else if(Buffer[0]=='5'&&blen==1)
		i=5;
	else if(Buffer[0]=='6'&&blen==1)
		i=6;
	else if(Buffer[0]=='7'&&blen==1)
		i=7;
	else if(Buffer[0]=='8'&&blen==1)
		i=8;
	else if(Buffer[0]=='9'&&blen==1)
		i=9;
	else if(strncmp(Buffer,"10",2)==0&&blen>1)
		i=10;
	else
	{
		//程序的一个退出条件
		if(!strncmp(Buffer, "quit", 4)) 
		{
			exit(0);
		}
		else
		{
			printf("输入错误：%s", Buffer);
		}			
	}
	return i;
}

static int get_tasknum()
{
	printf(">>动态库支持监控任务数量:%d\n", modinf.control(0, HANHLE_GET_TNUM, NULL, NULL));
	return 0;
}

static int get_task()
{
	int i;
	int num = modinf.control(0,HANHLE_GET_TNUM,NULL,NULL);
	if(num > 0)
	{
		struct task_tip *buf = (struct task_tip *)malloc(num * sizeof(struct task_tip));
	
		modinf.control(0, HANDLE_GET_ATDETAIL, NULL, buf);
		for(i = 0; i<num; i++)
		{
			printf(">>Task NO:%d, %s\n", buf[i].id, buf[i].tasktip);
		}
		
		free(buf);
	}
	return 0;
}

static int get_taskparanum()
{
	int i=0;
    int n=modinf.control(0, HANHLE_GET_TNUM, NULL, NULL);
   for (i = 0; i < n; ++i)
   {
   	int num= modinf.control(0, HANDLE_GET_TPNUM, &i, NULL);
    printf(">>动态库支持%d号监控任务可选参数组数量:%d\n",i,num);
   }
   return 0;
}

static int get_taskpara()
{
	int i;
	int j;
	int n=modinf.control(0, HANHLE_GET_TNUM, NULL, NULL);
	for (i = 0; i < n; ++i)
	{
		int num=modinf.control(0, HANDLE_GET_TPNUM, &i, NULL);
		struct para_tip* paratip=calloc(num,sizeof(struct para_tip));
		modinf.control(0, HANDLE_GET_TPDETAIL, &i, paratip);
		for ( j = 0; j <num; ++j)
		{
			printf(">>index=%d\n",paratip[j].index );
			printf(">>tip=%s\n",paratip[j].tip );
		}
	}
	return 0;
}

static int get_taskattunum()
{
	int n=modinf.control(0, HANHLE_GET_TNUM, NULL, NULL);
	int i;

	for (i = 0; i < n; ++i)
	{
		int num=modinf.control(0, HANDLE_GET_TANUM, &i, NULL);
		printf(">>%d号监控任务附件信息数量：%d\n",i,num);
	}
	return 0;
}

static int get_taskattinfo()
{
	int n=modinf.control(0, HANHLE_GET_TNUM, NULL, NULL);
	int i=0;
	int j;
    struct ctl_inf *ctlinf;
	for (i = 0; i < n; ++i)
	{
		int num=modinf.control(0, HANDLE_GET_TANUM, &i, NULL);

		ctlinf=calloc(num,sizeof(struct ctl_inf));
		modinf.control(0, HANDLE_GET_AINFO, &i, ctlinf);
		for (j = 0; j < num; ++j)
		{
			printf("the %d ctlinf: type=%02x, index=%d,tip=%s\n", i, ctlinf[j].type, ctlinf[j].index, ctlinf[j].tip);
			printf("the control info: dev=%p, devid=%d, instruct=%d, code=%d %d, len=%d\n",
				ctlinf[j].info.devctl.dev, ctlinf[j].info.devctl.devno, 
				ctlinf[j].info.devctl.sign, ctlinf[j].info.devctl.code[0], 
				ctlinf[j].info.devctl.code[1],ctlinf[j].info.devctl.len);
		}
	}
	return 0;
}

static int reset_alltask()
{
	struct task_user *task=NULL;
    int ret;

	ret=modinf.control(0, HANDLE_RESET_ALLTASK, task, NULL);
	if (ret<0)
	{
		printf("设置失败\n");
	}
	return 0;
}

static int add_task()
{
	struct task_user *task=NULL;

	int ret;

	ret=modinf.control(0, HANDLE_ADDUPDATE_TASK, task, NULL);
	if (ret<0)
	{
		printf("设置失败\n");
	}
	return 0;
}