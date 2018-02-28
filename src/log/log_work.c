#include "log.h"

extern struct log_data *plogdata;

//按照规则生成一个日志文件，type=1表示正式日志文件，type=2表示临时日志文件
int log_getlogfile(int type, char *name)
{
	return 0;
}

//查询日志，将结果保存到备份文件中
int log_inquery(struct log_query *query, char *file)
{
	return 0;
}

//按照规则整理日志文件，删除前面部分	
int log_update(void)
{
	return 0;
}

//写入日志信息
int log_write(struct log_inf *loginf)
{
	return 0;
}

//获得模块配置信息	type=0,默认配置；type=1，当前配置		
int log_getcfg(int type, KeyValueArray *cfginf)
{
	return 0;
}
