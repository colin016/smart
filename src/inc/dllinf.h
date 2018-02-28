#ifndef DLLINF_H
#define DLLINF_H

#include "dllif.h"

//动态库状态
#define DLL_UNLOAD		0
#define DLL_RUNNING		1
#define DLL_PAUSE		2
#define DLL_ERROR		-1
#define MAX_DEMAND		10

#define DLL_GET_MTNUM 0     //获得动态库支持的监控任务数量
#define DLL_GET_ATDETAIL 1  //获得所有的监控任务具体值及提示信息，调用者给出内存空间
#define DLL_GET_TPNUM 2     //获得动态库支持的某监控任务可选参数组数量
#define DLL_GET_TDETAIL 3   //获得一个监控任务所有支持可选参数组编号及其提示信息
#define DLL_GET_TCINUM 4    //获得动态库支持的某监控任务需要的控制信息数量
#define DLL_GET_TCINFO 5    //获得动态库需要控制信息
#define DLL_STOP_ATASK 6    //停止所有监控任务
#define DLL_RESET_ATASK 7   //重置所有监控任务
#define DLL_MOD_TASK 8      //修改某个监控任务
#define DLL_DEL_TASK 9      //删除某个监控任务
#define DLL_START_ATASK 10  //启动/重启 所有监控任务

#define JSON_ALARMS "alarms"
#define JSON_ALARM_ID "id"
#define JSON_ALARM_TYPE "type"
#define JSON_ALARM_LEVEL "level"
#define JSON_ALARM_TIME "time"
#define JSON_ALARM_ATTACH "attach"

//查询条件定义
#define QUERY_SORT_TYPE 1 //按类型查找
#define QUERY_SORT_LEVEL 2 //按级别查找
#define QUERY_SORT_TIME 3 //按日期查找

//动态库信息
struct dllinfo 
{
	char dllname[20];
	void *handle;
	char status;
	
	int (*init)(struct task_user *head, int (*callback)(int type, void* msg));
	int (*control)(int type, void *in, void *out);
	int (*query)(int type);
	int (*close)(void);
	
	int (*callback)(int type, void* msg);
};

//报警消息数据结构
struct alarm_inf
{
	int id;//编号
	char type;	//报警类型
	char level;	//报警级别
	int  time;	//报警时间
	char attach[64]; //附件文件名,以分号分开
	struct alarm_inf *next;
};

struct alarm_inf node;

struct alarm_table
{
    int num;//报警消息数量
	struct alarm_inf *alarm;//报警消息链表
};

struct alarm_query
{
	char sort;	//查询条件类型：1：类型，2：级别，4，日期
	char type;	//查询的关键字，
	char level;	//查询的级别
	unsigned char num; //最大信息条数
	int from;	//查询日期起始值，
	int to;		//查询日期截止日，
	
};

int dll_close();
int dll_init(char *dllfile);
int dll_change(char *newfile);
int dll_settask(int id);

//初始化报警消息文件
//如果文件存在，则不变；如果文件不存在，则创建文件。
int handle_alarminit( void );
//将报警消息备份到指定的文件中去
int handle_alarmbak(char *file);
//查询报警消息，将结果保存到备份文件中
int handle_alarmquery(struct alarm_query *query, char *file);
//写入一条报警信息，当报警消息条数超过最大值时，备份前面的，重新生成新文件。
int handle_alarmwrite(struct alarm_inf *alarminf);
//查询报警消息，将结果读入内存中,buf是存放的数组指针，
//由调用者申请内存，需要读的条数在query中。如果buf为NULL，则仅返回的int是实际符合要求的条数。
int handle_alarmload(struct alarm_query *query, struct alarm_inf *buf);

//报警信息链表添加节点
int alarmlist_add(struct alarm_inf *pnode);
//报警消息链表删除节点
int alarmlist_del(int id);
//根据查询要求，把符合要求的报警消息备份到文件
int alarmlist_query(struct alarm_query *query,char *file);
//清楚报警消息链表
int alarmlist_clear(void);
//保存报警消息链表到文件
int alarmlist_save(void);
////根据查询要求，把符合要求的报警消息加载到内存
int alarmlist_load(struct alarm_query *query,struct alarm_inf *buf);

#endif
