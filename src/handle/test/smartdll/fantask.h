#ifndef __FANTASK_H
#define __FANTASK_H

#include "task.h"

#define FANNAME		"智能电扇控制"
#define FANTIP		"利用温度和人体传感器智能控制电扇运转"
#define FANTYPE		0	//固定值，没有报警内容
#define FANLEVEL	0	//固定值，没有报警内容
#define FANINTELVAL 2000000	//2秒

#define FANPARA_NUM	2	//参数组数量 2个

#define SENSITIVE_DATA	0
#define SENSITIVE_TIP	"sensitive parameter group"
#define SENSITIVE_SVALVE		5
#define SENSITIVE_TVALVE		10

#define SLOWLY_DATA 	1
#define SLOWLY_TIP		"slowly parameter group"
#define SLOWLY_SVALVE		8
#define SLOWLY_TVALVE		18

#define FANCTL_NUM	3	//需要信息数量 3个

#define TEMP_INDEX		0
#define TEMP_TIP		"select temperature dev"

#define SENSE_INDEX		1
#define SENSE_TIP		"select human sense dev"

#define FANCTL_INDEX	2
#define FANCTL_TIP		"select the fan control dev"
//定义一个具体的智能控制风扇的任务，从基类派生

//参数组派生类定义
class para_fan: public para_inf
{
public:
	int senseth;	//人体感应阀值
	int tempth;	//温度阀值
};
/*
struct timeval {
    time_t      tv_sec;     seconds 
    suseconds_t tv_usec;     microseconds 
};*/

//监控任务派生类定义
class fantask:public task
{
	int lastsense;	//上次采集的人体感应值
	int lasttemp;	//上次采集的温度值
	
public:
	fantask(int id, int pnum, int dnum);
	~fantask();

	int setdef_ctl();	
	int setparainf( );	//设置参数组信息
	int reset( );				//重置任务
	void ctlexec(void) ; 	//定时执行的任务
	int ondevent(outctl *in);	//收到设备主动送来信息时进行处理
	int getsize( );		//获取任务
};

#endif
