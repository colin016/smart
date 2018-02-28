/*
*Filename:config.h
*
*Author:ZhuGuoquan
*
*Date:2018-1-16
*
*Func:
*       本文件为config模块的头文件
*/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3dao.h"
#include "sys.h"
#include "debug.h"

#define TABLE_CONFIG "tb_config"
#define TABLE_CONFIG_KEY "cfg_key"
#define TABLE_CONFIG_VALUE "cfg_value"
#define MAX_KEYVALUELEN 100            /*配置文件中一行最大的字符数量*/

/*控制类型定义*/
#define CFGCTL_UPDATE_ONE 1            /*更新一个配置*/
#define CFGCTL_GET_ONE 2               /*获取指定的配置*/
#define CFGCTL_READ_CFG 3              /*获得所有的配置*/
#define CFGCTL_NEW_CFG 4               /*重新写入配置*/
#define CFGCTL_ADD_ONE 5							 /*向配置文件中添加一个配置*/

/*查询类型定义*/
#define CFGQUERY_MODULE_INFO 1         /*查询模块信息*/
#define CFGQUERY_SPECIFICITY_INFO 2    /*查询具体信息*/
#define CFGQUERY_MODULE_STATUS 0       /*查询模块状态*/
#define CFGQUERY_MEMORY_CONSUME 1      /*查询消耗内存数*/
#define CFGQUERY_THREAD_NUMBER 2       /*查询线程数*/
#define CFGQUERY_FILE_NUMBER 3         /*查询打开文件数*/
#define CFGQUERY_SOCKET_NUMBER 4       /*查询打开套接字数*/
#define CFGQUERY_THREAD_SPECIFICITY 1  /*查询所有线程具体信息*/
#define CFGQUERY_FILE_SPECIFICITY 2    /*查询所有打开文件信息*/
#define CFGQUERY_SOCKET_SPECIFICITY 3  /*查询所有打开套接字信息*/

struct config_data
{
	KeyValueArray	cfginf;
	int filesize;                       //配置文件大小，如果有异常，大小为0;
	struct sqlite3_DAO *sqlite3dao;     //sqlie3 database access object
	moduleCallback	callback;           //访问其他模块功能的回调函数
};

struct moduleinf{
char name[10];                          //线程名称，或者打开文件名，或者套接字类型端口字符串
int waitnum;                            //等待队列数量，或者打开时间
struct moduleinf *next;
};

//外部接口函数
int config_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret);
int config_query(int type,void *input,void *output);
int config_config(int type,void *cfg);
int config_control(int sendid, int type, void *input,void *output);
int config_close(void);

//内部接口函数
int cfg_init_keyvalues(void);
int cfg_update(KeyValue *cfg);               //更新一个配置项
int cfg_getone(const char *key,char *cfg);   //获得一个配置的结果
int cfg_update_all(KeyValueArray *cfg);      //更新所有的配置
int cfg_add_one(KeyValue *cfg);              //添加一个配置

#endif
