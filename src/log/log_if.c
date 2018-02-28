#include "log.h"
 
struct log_data *plogdata;

//配置模块初始化
//主要工作：获取本模块配置，查找或者构造日志文件并打开，填写logdata数据结构，填写ret
//日志文件由宏LOG_PATH和配置规则定义
//日志文件格式
//时间 日志类型 操作用户名 执行结果 操作信息
/*日志类型：
sys：开关机，升级等
alarm：产生了报警信息
remote：远程控制操作记录
local：本地控制操作记录
error：内存不足，接口断开等*/
//对ret指针赋值
int log_init(KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret)
{
	return 0;
}

//日志模块查询，按下列要求执行
/*
type	input		output
1	请求号含义：
	0：查询模块状态	0(暂停)/1(工作)
	1：查询消耗内存数	消耗内存字节（int）
	2：查询线程数	线程数
	3：查询打开文件数	打开文件数
	4：查询打开套接字数	打开套接字数
2	请求号：
	1：查询线程具体信息		struct moduleinf{
	2：查询打开文件信息			char name[10]; //线程名称，或者打开文件名，或者套接字类型端口字符串
	3：查询打开套接字信息		int waitnum;  //打开时间
					struct moduleinf *next；}；	查询具体信息*/
int log_query(int type,void *input,void *output)
{
	return 0;
}

//配置本模块，按下列要求执行
/*
序号	配置名			说明
1	char logfile[20]	设置日志文件名
2	int maxsize			设置配置文件最大大小
3	char policy			设置最大时策略，1、按时间生成新日志；2 删除旧日志
4	float delscale		设置最大时删除比例
5	keyvaluearray   	设置所有上述配置
6	keyvaluearray		获取本模块所有配置	*/
int log_config(int type,void *cfg)
{
	return 0;
}


//控制本模块，按下列要求执行
/*
type input			output	说明
1	struct loginf	无	写入日志
2	无				无	清空日志
3	struct log_query	char *url	查询日志
4	无				char *url	获取当前日志文件url
5	无				keyvaluearray	获取默认配置*/
int log_control(int sendid, int type, void *input,void *output)
{
	return 0;
}


//配置模块关闭，关闭日志文件，释放loggdata内申请的空间
int log_close(void)
{
	return 0;
}


