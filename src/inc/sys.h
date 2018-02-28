#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "debug.h"

#define DB_CONFIG	"../sys/smart.db"

#define MODULE_NAME_LEN 20
#define SUCCCESS	0
#define FAIL		-1
#define TRUE		1
#define FALSE		0

#define CONFIG 0
#define LOG 1
#define DEVICE 2
#define HANDLE 3
#define PROTO 4
#define FRAME 5

typedef struct ModuleInfo ModuleInfo;

typedef int (*moduleClose)(void);
typedef int (*moduleQuery)(int type, void *input, void *output);
typedef int (*moduleConfig)(int type, void*cfg);
typedef int (*moduleControl)(int sendid, int type, void * input, void * output);
typedef int (*moduleCallback)(int sendid, int recvid, int type, void *input, void *output);

typedef struct Key_Value
{
	char *key;//关键字
	char *value;//取值
	short valuelen;//值长度
	short keylen;//关键字长度
}KeyValue;

//键值对数组
typedef struct KeyValueArray
{
	int 		count;//键值对总个数（配置项数目）
	char 		*cfgbuf; //所有配置项所在缓冲区。
	KeyValue 	*keyValueList; //配置键值对链表队列
}KeyValueArray;

typedef int (*moduleInit)( KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret);

//模块信息结构定义
struct ModuleInfo
{
	char name[MODULE_NAME_LEN];//模块名称
	char type;//模块类型，0表示模块是静态的，1还是可动态升级的。静态的模块只能与主模块一起升级。动态模块可独立升级。
	char maincode;//主功能号，当前对应模块id
	char subcode;//次功能号
	int version;//模块版本号
	int id;  //模块的id

	//模块的接口函数注册
	moduleInit init; //初始化输入配置参数，注册回调函数。
	moduleClose close;//模块关闭接口
	moduleQuery query; //查询模块的信息，包括状态、线程数、使用内存、打开文件数、打开套接字数等信息
	moduleConfig config; //对本模块的配置进行操作，包括查询或者设置
	moduleControl control;//对本模块进行控制管理
};

#endif
