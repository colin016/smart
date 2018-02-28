#include "dev.h"

//模块全局变量声明
struct dev_data *devdata;

static int ctl_media_add(struct media_if *media);

//设置默认配置
int dev_defaultcfg( void)
{
    devdata->max_media_num=VALUE_MAX_MEDIA_NUM;
    devdata->max_device_num=VALUE_MAX_DEVICE_NUM;
    devdata->timeout=VALUE_TIMEOUT;
    return 0;
}


//对本模块进行初始化处理，由主框架模块调用
//0 构造epoll接收处理线程
//1 打开设备配置文档，构造介质和附件链表，并对介质和设备初始化
//2 对ret赋值并返回
int dev_init(struct KeyValueArray *cfg, moduleCallback callback, struct ModuleInfo *ret)
{
  int err=-1;

  devdata=(struct dev_data*)calloc(1,sizeof(struct dev_data));
  if(NULL==devdata)
  {
    return -1;
  }

  init_ret(ret);
  if(NULL==ret)
  {
    return -1;
  }
  pthread_mutex_init(&devdata->ctltable.ctl_lock,NULL);
  pthread_mutex_init(&devdata->devtable.dev_lock,NULL);
  pthread_mutex_init(&devdata->medtable.med_lock,NULL);
  devdata->callback=callback;
  devdata->epoll_fd=epoll_create(10);
  
  srand(time(NULL));
  devdata->sendseq=rand()%100+1;

  if (-1==devdata->epoll_fd)
  {
    return -1;
  }
  err=init_conf(cfg);
  if (-1==err)
  {
    return -1;
  }

  if(init_medias_devices()==-1)
    return -1;
  else
    PLOG("init medias and devices sucessfully!\n");
  
  if (init_devctlcmd()==-1)
  {
    return -1;
  }

  err=pthread_create(&devdata->recv_thread_id,NULL,dev_recvhandle,NULL);
  if (0!=err)
  {
    return -1;
  }
  err=pthread_create(&devdata->timer_thread_id,NULL,dev_timer,NULL);
  if (0!=err)
  {
    return -1;
  }

	return 0;
}

//模块控制，由其他模块调用本模块进行工作的接口
int  dev_control(int sendid, int type, void *input, void *output)
{
  //struct devctl_inf *ctl_node=NULL;
  struct media_if *med_list=NULL;
  struct ext_dev *dev_list=NULL;

	switch(type)
	{
	case EXECUTE_CMD://执行需求指令
    if(proto_send(input,output)==-1)
    {
      printf("send message failled\n");
      return -1;
    }
		break;

	case MODIFY_MEDIA://修改介质配置参数
    if(media_update((struct media_if*)input))
      return -1;
    if(control_cjson_control(JSON_MEDIA_UPDATE,input)==-1)
      return -1;
		break;

	case MODIFY_DEVICE://修改设备参数
    if(device_update((struct ext_dev*)input)==-1)
      return -1;
    else if(control_cjson_control(JSON_DEVICE_UPDATE,input)==-1)
      return -1;
		break;

	case ADD_DEVICE://添加设备
    
    if(device_add((struct ext_dev*)input)==-1)
      return -1;
    else if(control_cjson_control(JSON_DEVICE_ADD,input)==-1)
      return -1;
		break;

	case DELETE_DEVICE://删除设备
    if(device_delete(*(int*)input)==-1)
      return -1;
    else if(control_cjson_control(JSON_DEVICE_DELETE,input)==-1)
      return -1;
		break;

	case GET_DEVICE_FILE://获取设备描述文件
    output=(void*)DEV_CONF_FILE;
		break;

	case SET_DEVICE_FILE://设置新设备描述文件

		break;

	case GET_MEDIAS_STATUS://获取介质状态
    med_list=(struct media_if*)output;
    //memcpy(output,devdata->medtable.med_list,sizeof(struct ext_dev));
    med_list->next=(void*)devdata->medtable.med_list;
		break;

	case GET_DEVICES_STATUS://获取设备当前状态
    dev_list=(struct ext_dev*)output;
    dev_list->next=(void*)devdata->devtable.dev_list;
		break;

  case ADD_MEDIA:
  if(input==NULL)
    return -1;
  if(ctl_media_add((struct media_if*)input)==-1)
    return -1;
  else 
  {
     control_cjson_control(JSON_MEDIA_ADD,input);
     fprintf(stdout, "media add\n");
  }
   break;
	}

	return 0;
}

//模块关闭处理，由主框架main模块调用
int  dev_close(void)
{
  //关闭线程
  pthread_cancel(devdata->timer_thread_id);
  pthread_cancel(devdata->recv_thread_id);
  device_clear();
  media_clear();
  devctl_inf_clear();
  close(devdata->epoll_fd);
  free(devdata);
	return 0;
}

//对模块的配置进行操作，具体定义如下
//功能要求			type	cfg
//设置修改多个配置项	1	KeyValueArray
//修改配置项为默认值	2	NULL
//查询配置项的当前值	3	KeyValueArray(模块重新申请内存？）
//查询配置项的默认值	4	NULL(模块申请内存？）
int dev_config(int type, void*cfg)
{
  switch (type)
  {
    case MODIRY_MULTI_CONF:
    break;
    case MODIRY_DEFAULT_CONF:
    break;
    case QUERY_ALL_CONF:
    break;
    case QUERY_DEFAULT_CONF:
    break;
  }
	return 0;
}

//提供给主模块的接口
int dev_query(int type, void *input, void *output)
{
  switch (type)
  {
    case QUERY_MODULE_INFO:
      if(query_module_info(input,output))
        return -1;
    break;
    case QUERY_DETAILED_INFO:
      if(query_detailed_info(input,output))
        return -1;
    break;
  }
	return 0;
}


static int ctl_media_add(struct media_if *media)
{
  int ret=0;

  if(media==NULL)
    return -1;

   switch(media->type)
   {
    case MEDIA_SERIAL:
    serial_init(media);
    media_add(media);
    break;

    case MEDIA_USB:
    break;

    case MEDIA_ETHERNET:
    net_init(media);
    media_add(media);
    break;

    case MEDIA_WIFI:
    break;

    case MEDIA_ZIGBEE:
    break;

    case MEDIA_BLUETOOTH:
    break;
   }

   return ret;
}