#ifndef __CAMTASK_H
#define __CAMTASK_H

#include "task.h"

#define CAMNAME		"摄像头陌生人检测"
#define CAMTIP		"利用摄像头检测指定区域是否有陌生人"
#define CAMTYPE		1	//报警类型
#define CAMLEVEL	0	//报警级别，0-10可变，级别越高，表示可能性越大
#define CAMINTELVAL 1000000	//1秒

#define CAMPARA_NUM	2	//参数组数量 2个

#define SMART_DATA	0
#define SMART_TIP	"smart parameter group"
#define SMART_VALVE		5

#define DULL_DATA 	1
#define DULL_TIP		"dullness parameter group"
#define DULL_VALVE		8

#define CAMCTL_NUM	3	//需要信息数量 2个

#define FILE_INDEX		0
#define FILE_TIP		"select a module file"

#define CAMERA_INDEX		1
#define CAMERA_TIP		"select camera dev"

#define CAMSENSE_INDEX		2
#define CAMSENSE_TIP		"select the sense dev"

#define PICTURE_LEN		10240
//定义一个具体的智能控制风扇的任务，从基类派生

//参数组派生类定义
class para_cam: public para_inf
{
public:
	char *file;	//深度学习文件
	int valve;	//阀值
};
/*
struct timeval {
    time_t      tv_sec;     seconds 
    suseconds_t tv_usec;     microseconds 
};*/

//监控任务派生类定义
class camtask:public task
{
	int lastsense;	//上次采集的人体感应值
	char *picbuf;	//上次采集的图片值
	
public:
	camtask(int id, int pnum, int dnum);
	~camtask();


	int setparainf( );	//设置参数组信息
	int reset( );				//重置任务
	int getsize();
	void ctlexec(void); 	//定时执行的任务
	int ondevent(outctl *in);	//收到设备主动送来信息时进行处理
	int setdef_ctl();
};

#endif
