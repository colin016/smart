#include "handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//模块全局变量声明
struct handle_data *phdata;

static int get_task(void *input,void *output);

//设置默认配置
int handle_defaultcfg( void)
{
    struct	task_user	*task = (struct	task_user *)calloc(1,sizeof(struct task_user));
    if (task == NULL) {
        return -1;
    }
    memset(task, 0, sizeof(struct task_user));
    task->id = 0;
    strcpy(task->tasktip,"test");
    strcpy(task->name,"test");
    task->type = 1;
    task->ctlnum = 1;
	phdata->task = task;
	phdata->tasknum = 1;
    return 0;
}


//对本模块进行初始化处理，由主框架模块调用
int handle_init(struct KeyValueArray *cfg, moduleCallback callback, struct ModuleInfo *ret)
{
#if 1
	ret->close = handle_close;
	ret->control = handle_control;
	ret->query = handle_query;
	ret->config = handle_config;
	ret->maincode = ret->id =  HANDLE;
	phdata = (struct handle_data *) malloc(sizeof(struct handle_data));
	if (phdata == NULL) {
		return -1;
	}
	memset(phdata, 0, sizeof(struct handle_data));
	if (handle_taskinit() < 0 ){
    //没有任务文件的话，初始化一个默认任务到task中
		handle_defaultcfg();
	}
	if (dll_init(DLL_FILE) < 0) {
		return  -1;
	}
    if (handle_alarminit()<0)
    {
        return -1;
    }
	phdata->callback = callback;
#endif
	return 0;
}

//模块控制，由其他模块调用本模块进行工作的接口，详细要求见设计说明书38页前后
int  handle_control(int sendid, int type, void *input, void *output)
{
    if (NULL == phdata) {//没有初始化，不能进行控制
        return -1;
    }
	switch(type) {
    case HANHLE_GET_TNUM: //获得动态库支持的监控任务数量
        return phdata->dinfo.control(DLL_GET_MTNUM,input,output);
        break;
    case HANDLE_GET_ATDETAIL://获得所有的监控任务具体值及提示信息，调用者给出内存空间
        return phdata->dinfo.control(DLL_GET_ATDETAIL,input,output);
        break;
    case HANDLE_GET_TPNUM://获得动态库支持的某监控任务可选参数组数量
        return phdata->dinfo.control(DLL_GET_TPNUM,input,output);
        break;
    case HANDLE_GET_TPDETAIL://获得一个监控任务所有支持可选参数组编号及其提示信息
        return phdata->dinfo.control(DLL_GET_TDETAIL,input,output);
        break;
    case HANDLE_GET_TANUM://获得动态库支持的某监控任务需要的控制信息数量
        return phdata->dinfo.control(DLL_GET_TCINUM,input,output);
        break;
    case HANDLE_GET_AINFO://获得动态库需要的附件信息
        return phdata->dinfo.control(DLL_GET_TCINFO,input,output);
        break;
    case HANDLE_STOP_ATASK://停止所有监控任务
        return phdata->dinfo.control(DLL_STOP_ATASK,input,output);
        break;
    case HANDLE_RESET_ALLTASK://重置所有监控任务
        return phdata->dinfo.control(DLL_RESET_ATASK,input,output);
        break;
    case HANDLE_ADDUPDATE_TASK://修改某个监控任务,先添加，后删除
        return phdata->dinfo.control(DLL_MOD_TASK,input,output);
        break;
    case HANDLE_DEL_TASK://删除某个监控任务
        return phdata->dinfo.control(DLL_DEL_TASK,input,output);
        break;
    case HANDLE_START_ALLTASK://重启 所有监控任务
        return phdata->dinfo.control(DLL_START_ATASK,input,output);
        break;
    case HANDLE_UPDATE_TASKFILE://根据动态库更新监控文件
            // TODO:不理解怎么操作
        break;
    case HANDLE_GET_TASK://获取监控任务，直接赋值任务结构指针
        return get_task(input,output);
        break;
    case HANDLE_GET_ALARMINFO:
        return handle_alarmload((struct alarm_query *)input,(struct alarm_inf *)output);
        break;
    case HANDLE_BACKUPS_ALARMINFO:
        return handle_alarmquery((struct alarm_query *)input,(char*)output);
        break;
    case HANDLE_UPDATE_DLL:
        return dll_change((char*)input);
        break;
    case HANDLE_PAUSE_MONITOR:
        phdata->dinfo.status = DLL_PAUSE;
        break;
    case HANDLE_RECOVER_MONITOR:
        phdata->dinfo.status = DLL_RUNNING;
        break;
    default:
        break;
    }
	return 0;
}

//模块关闭处理，由主框架main模块调用
int  handle_close(void)
{
    struct	task_user	*task =phdata->task;
    dll_close();//关闭动态库
    //free task
    while (phdata->task) {

        task = phdata->task->next;
        if (phdata->task->ctlinf) {
            free(phdata->task->ctlinf);
        }
        free(phdata->task);
        phdata->task = task;
    }
	return 0;
}

//对模块的配置进行操作，具体定义如下
//功能要求			type	cfg
//设置修改多个配置项	1	KeyValueArray
//修改配置项为默认值	2	NULL
//查询配置项的当前值	3	KeyValueArray(模块重新申请内存？）
//查询配置项的默认值	4	NULL(模块申请内存？）
int handle_config(int type, void*cfg)
{

	return 0;
}

//提供给主模块的接口
int handle_query(int type, void *input, void *output)
{

	return 0;
}

static int get_task(void *input,void *output)
{
    struct task_user   *task_head =phdata->task;
    int tasknum = *(int *)(input);
    struct  task_user   *task_current = (struct task_user   *)(input);

    if (NULL == task_current) {
        return -1;
    }
    if (tasknum==-1)
    {
        task_current=task_head;
    }if (tasknum>=0)
    {
        task_current=handle_taskquery(tasknum);
    }
    return 0;
}
