
#include "config.h"

struct config_data *pcfgdata;

//配置模块初始化
//主要工作：将配置文件内容读到内存，并解析，填写cfgdata数据结构
//配置文件格式
/*
#第一行总是表示本配置文件具有多少个配置项。固定长度。
#每行以回车换行结尾，#开头的表示注释,比如：
CONFIGITEM=0001
IPADDR=192.168.1.1
*/
//对ret指针赋值
int config_init(KeyValueArray *cfg, moduleCallback callback,
                struct ModuleInfo *ret) {
  // dictionary *ini;

  ret->close = config_close;
  ret->control = config_control;
  ret->query = config_query;
  ret->config = config_config;

  pcfgdata = (struct config_data *)calloc(1,sizeof(struct config_data));
  if (pcfgdata == NULL)
    return -1;
  memset(pcfgdata, 0, sizeof(struct config_data));

  if (cfg_get_keyvalue() == -1)
    return -1;

  return 0;
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

  case CFGCTL_GET_ONE:
    if (input == NULL || output == NULL)
      return -1;
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
  free(pcfgdata->cfginf.cfgbuf);
  free(pcfgdata->cfginf.keyValueList);
  free(pcfgdata);

  return 0;
}
