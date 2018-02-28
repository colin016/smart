/*
*Filename:config_if.c
*
*Author:ZhuGuoquan
*
*Date:2018-1-16
*
*Func:
*       本文件为模块对外接口的实现源文件，即config模块接口的实现源文件
*/

#include "config.h"

struct config_data *pcfgdata;
inline static void cinfig_release(void);

/*
*func:初始化config模块
*param:
*   1、*cfg是keyvaluearray变量。keyvaluearray是一个自定义的键值对的结构体
*   2、callback 是通知其它模块的回调函数
*   3、*ret代表本模块对象
*return:
*   1、成功：0
*   2、失败：-1
*/
int config_init(KeyValueArray *cfg, moduleCallback callback,struct ModuleInfo *ret)
{
    pcfgdata = (struct config_data *)calloc(1,sizeof(struct config_data));
    if (pcfgdata == NULL)
    {
      goto error;
    }

    db_init(&pcfgdata->sqlite3dao);

    ret->close = config_close;
    ret->control = config_control;
    ret->query = config_query;
    ret->config = config_config;
    pcfgdata->callback=callback;

    if (cfg_init_keyvalues() == -1)
    {
      goto error;
    }
    return 0;

error:
    cinfig_release();
    return -1;
}

//配置模块查询，按下列要求执行
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
        2：查询打开文件信息			char name[10];
//线程名称，或者打开文件名，或者套接字类型端口字符串
        3：查询打开套接字信息		int waitnum;  //打开时间
                                        struct moduleinf *next；}；
查询具体信息*/
int config_query(int type, void *input, void *output) {
  switch (type) {
  case CFGQUERY_MODULE_INFO:
    switch (*(int *)input) {
    case CFGQUERY_MODULE_STATUS:
      if (pcfgdata == NULL)
        *(int *)output = 0;
      else
        *(int *)output = 1;
      break;
    case CFGQUERY_MEMORY_CONSUME:
      *(int *)output = sizeof(pcfgdata->cfginf.cfgbuf) +
                       sizeof(pcfgdata->cfginf.keyValueList) + sizeof(pcfgdata);
      break;
    case CFGQUERY_THREAD_NUMBER:
      *(int *)output = 0;
      break;
    case CFGQUERY_FILE_NUMBER:
      *(int *)output = 1;
      break;
    case CFGQUERY_SOCKET_NUMBER:
      *(int *)output = 0;
      break;
    default:
      return -1;
      break;
    }
    break;
  case CFGQUERY_SPECIFICITY_INFO:
    switch (*(int *)input) {
    case CFGQUERY_THREAD_SPECIFICITY:
      output = NULL;
      break;
    case CFGQUERY_FILE_SPECIFICITY:
      output = NULL;
      break;
    case CFGQUERY_SOCKET_SPECIFICITY:
      output = NULL;
      break;
    }
    break;
  default:
    return -1;
    break;
  }
  return 0;
}

//配置模块，按下列要求执行
/*
功能要求	type	cfg
设置修改多个配置项	1	KeyValueArray
设置修改配置项为默认值	2	NULL
查询配置项的当前值	3	KeyValueArray(查询模块提供空间）
查询配置项的默认值	4	KeyValueArray(查询模块提供空间）*/
int config_config(int type, void *cfg) { return 0; }

//控制本模块，按下列要求执行
/*
type	input	output	说明
1	keyvalue	无	写入修改配置
2	*key	*value	读取选择的配置项
3	无	keyvaluearray	读取配置文件内容
4   keyvaluearray 重新写配置文件内容
//5	keyvaluearray 附加配置内容*/
int config_control(int sendid, int type, void *input, void *output) {
  int ret=0;
  switch (type) {
  case CFGCTL_UPDATE_ONE:
    if (input == NULL)
      return -1;
    ret=cfg_update((KeyValue *)input);
    break;

  case CFGCTL_GET_ONE://主意input要给足内存
    if (input == NULL || output == NULL||sizeof((char*)output)<16)
    {
      PLOG("error:input==NULL or output==NULL or sizeof((char*)output)<16\n");
      return -1;
    }
    ret=cfg_getone((char *)input, (char *)output);
    break;

  case CFGCTL_READ_CFG:
    if (output == NULL)
      return -1;
    *(KeyValueArray *)output = pcfgdata->cfginf;
    break;

  case CFGCTL_NEW_CFG:
    if (input == NULL)
      return -1;
    ret=cfg_update_all((KeyValueArray *)input);
    break;

  case CFGCTL_ADD_ONE:
    ret=cfg_add_one((KeyValue *)input);
  break;

  }

  return ret;
}

//配置模块关闭，释放cfgdata内申请的空间
int config_close(void) {
  cinfig_release();
  return 0;
}

inline static void cinfig_release(void)
{
 if(pcfgdata!=NULL)
  {
    if(pcfgdata->sqlite3dao!=NULL)
    {
      free(pcfgdata->sqlite3dao);
    }
    if(pcfgdata->cfginf.cfgbuf!=NULL)
    {
      free(pcfgdata->cfginf.cfgbuf);
    }
    if(pcfgdata->cfginf.keyValueList!=NULL)
    {
      free(pcfgdata->cfginf.keyValueList);
    }
    free(pcfgdata);
  }
}
