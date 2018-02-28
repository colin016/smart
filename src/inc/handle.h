#ifndef HANDLE_H
#define HANDLE_H

#include "sys.h"
#include "dllinf.h"

//资源位置
#define DLL_FILE	"./smartdll.so"
#define TASK_FILE	"../task/task.xml"
#define ALARM_FILE	"../data/alarm.txt"

//handle_control函数参数type宏
#define HANHLE_GET_TNUM 0           //获得动态库支持的监控任务数量
#define HANDLE_GET_ATDETAIL 1        //获得所有的监控任务具体值及提示信息，调用者给出内存空间
#define HANDLE_GET_TPNUM 2           //获得动态库支持的某监控任务可选参数组数量
#define HANDLE_GET_TPDETAIL 3        //获得一个监控任务所有支持可选参数组编号及其提示信息
#define HANDLE_GET_TANUM 4           //获得动态库支持的某监控任务需要的附件信息数量
#define HANDLE_GET_AINFO 5           //获得动态库需要的附件信息
#define HANDLE_STOP_ATASK 6          //停止所有监控任务
#define HANDLE_RESET_ALLTASK 7       //重新所有监控任务
#define HANDLE_ADDUPDATE_TASK 8      //添加或修改某个监控任务
#define HANDLE_DEL_TASK 9            //删除某个监控任务
#define HANDLE_START_ALLTASK 10      //启动所有监控任务
#define HANDLE_UPDATE_TASKFILE 20    //根据动态库更新监控文件
#define HANDLE_GET_TASK 21           //获取监控任务，直接赋值任务结构指针(某个或全部监控任务)
#define HANDLE_GET_ALARMINFO 22      //获取报警消息在内存，最大条数在query结构中
#define HANDLE_BACKUPS_ALARMINFO 23  //备份满足条件的报警消息到文件
#define HANDLE_UPDATE_DLL 24         //更新动态库
#define HANDLE_PAUSE_MONITOR 25      //暂停监控处理
#define HANDLE_RECOVER_MONITOR 26    //恢复监控处理
//模块数据结构,演示程序不用
struct handle_data
{
	struct	dllinfo	dinfo;

	pthread_mutex_t mutex;//监控任务链表互斥锁

	int tasknum;
	struct task_user *task;//监控任务链表
    struct alarm_table alarmtable;//监控任务表
	int status;
	
	KeyValue 	*cfgList;	
	moduleCallback		callback;		//访问其他模块功能的回
};


//外部接口函数定义，演示程序不用
int handle_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret);
int handle_close(void);
int handle_query(int type, void *input, void *output); 
int handle_config(int type, void*cfg); 
int handle_control(int sendid, int type, void * input, void * output);

int handle_taskinit(void);
int handle_taskwrite(void);
struct task_user *handle_taskquery(int id);
int handle_taskdel(int id);
int handle_taskmod(struct task_user *task);
int handle_taskadd(struct task_user *task);
int handle_taskupdate(void);
#endif
