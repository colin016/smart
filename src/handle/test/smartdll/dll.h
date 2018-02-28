#ifndef __DLL_H
#define __DLL_H

#include "debug.h"
#include "task.h"
#include "timer.h"
#include "dllif.h"

#define TASK_SUM	2
#define DLL_VERSION	1
#define DLL_ID		"SZLY001"

//#define DLL_UNINIT	0
#define DLL_INITED	0
#define DLL_RUNNING	1
#define DLL_PAUSE	2
#define DLL_ERROR	-1

//动态库信息类定义
class dllib
{
	char 	status;		//库任务监控的状态
	int 	tasknum;		//用户采用的任务数
	int 	(*cb)(int type, void* msg); //回调函数
	
	class task	*dlltask[TASK_SUM];	//所有库信息
	class tktimer	*tasktimer;		//库的定时器
public:
	dllib( );		//构造函数
	~dllib();

	int start(bool force);	//启动监控操作,参数为true强迫重新启动
	int stop();		//停止所有监控任务
	
	void setstatus(char s){status = s;}
	char getstatus( ){return status;}
	class task *gettask(int i ){return dlltask[i];}
	int gettasknum() {return tasknum;}
	void setcb(int (*callback)(int type, void* msg)); //给各个任务设置回调函数
	int modtask(struct task_user *ptu); 	//修改一个任务
	int settask(struct task_user *head); 	//重新设置一个任务信息并启动
	int deltask(int taskid);				//删除一个任务
};	

//动态库接口函数，能够用于C语言调用
extern "C" int init(struct task_user *head, int (*callback)(int type, void* msg));
extern "C" int query(int type);
extern "C" int control(int type, void *in, void *out);
extern "C" int closedll(void);

#endif
