#ifndef SMART_H_
#define SMART_H_

#include "work.h"

//用户界面访问同步处理
struct man_data
{
	pthread_mutex_t lock;	//互斥锁
	int time;
	
	//work模块的接口指针
	int (*workinit)(int num, smartCallback callback);
	int (*workclose)(void);
	int (*workcontrol)(int funcid, int type, void * input, void * output);
};	

//回调函数定义，用于work通知smart
int man_callback(int funcid, int type, void *input, void *output);

//外部接口函数定义	
int man_init(void);
int man_close(void);
int man_control(int funcid, int type, void * input, void * output);

//内部接口函数定义
int man_initsys(void); //对操作系统进行初始化，如设置网络参数
int man_getsyscfg(int type); //获取系统配置信息
void man_timer(void *arg);	//用于work模块发送异步错误时的解锁操作
#endif
