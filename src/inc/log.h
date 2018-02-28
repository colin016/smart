#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>       
#include <string.h>
#include <stdlib.h>
#include "sys.h"

#define LOG_PATH	"../sys"

struct log_inf
{
	char type; //0错误，1 local operation， 2 remote control, 3 alarm，4 sys;
	char username[20];
	char ret;  //0成功，1失败
	char data[58]; //操作或者错误信息
};

struct log_query
{
	char type;	//查询条件类型-1无效,1：类型，2：用户名，3，返回结果，4，信息，5日期
	char *username;	//查询的关键字，NULL无效
	int from;	//查询日期起始值，0无效
	int to;		//查询日期截止日，0无效
};

struct log_data
{
	FILE *pfile;
	char temp[20]; //查询日志结果的临时文件名
	int filesize;	//日志文件的当前大小
	int time;		//日志文件的创建时间
	char logfile[20]; //设置日志文件名，没有设置的话，按照标准安排自动生成
	int	maxsize;	//最大文件大小
	char policy;	//日志策略，0，表示最大时，备份重新生产新日志文件；1表示删除以前的部分
	float delscale;	//删除比例
};

//外部接口函数
int log_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret);
int log_query(int type,void *input,void *output);
int log_config(int type,void *cfg);
int log_control(int sendid, int type, void *input,void *output);
int log_close(void);

//内部接口函数
//按照规则生成一个日志文件，type=1表示正式日志文件，type=2表示临时日志文件
int log_getlogfile(int type, char *name);

//查询日志，将结果保存到备份文件中
int log_inquery(struct log_query *query, char *file);

//按照规则整理日志文件，删除前面部分	
int log_update(void);

//写入日志信息
int log_write(struct log_inf *loginf);

//获得模块配置信息	type=0,默认配置；type=1，当前配置		
int log_getcfg(int type, KeyValueArray *cfginf);

#endif
