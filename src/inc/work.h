#ifndef WORK_H_
#define WORK_H_

#include "sys.h"
#include "config.h"
#include "dev.h"
#include "log.h"
#include "handle.h"
#include "proto.h"

//模块用到的常量定义
#define WORK_MODULE_NUM		5
#define TOTAL_NUM			5
#define WORKING 1
#define EXIT 0

//库外部回调函数定义，用于通知smart
typedef int (*smartCallback)(int funcid, int type, void *input, void *output);

struct work_data
{
	struct ModuleInfo modinf[TOTAL_NUM];
	smartCallback callback;
	int ifexit;
};

//外部接口函数定义
int work_init(int num, smartCallback call);

int work_close(void);
int work_control(int funcid, int type, void * input, void * output);

//内部接口函数
int work_callback(int sendid, int recvid, int type, void *input, void *output);
#endif
