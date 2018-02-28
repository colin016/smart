//智能监控框架管理模块
#include "work.h"

struct work_data *pworkdata;

//处理模块的回调
int work_callback(int sendid, int recvid, int type, void *input, void *output)
{
	int ret=0;
	switch (recvid)
	{
		case CONFIG:
		ret=config_control(sendid,type,input,output);
		break;
		case LOG:
		ret=log_control(sendid,type,input,output);
		break;
		case HANDLE:
		ret=handle_control(sendid,type,input,output);
		break;
		case DEVICE:
		ret=dev_control(sendid,type,input,output);
		break;
		case PROTO:
		ret=proto_control(sendid,type,input,output);
		break;
	}
	return ret;
}

//功能模块关闭处理
int work_close()
{
	free(pworkdata);
	return 0;
}

//功能模块的控制处理
int work_control(int funcid, int type, void *input, void *output)
{
	int ret=0;
	switch (funcid)
	{
		case CONFIG:
		ret=config_control(FRAME,type,input,output);
		break;
		case LOG:
		ret=log_control(FRAME,type,input,output);
		break;
		case HANDLE:
		ret=handle_control(FRAME,type,input,output);
		break;
		case DEVICE:
		ret=dev_control(FRAME,type,input,output);
		break;
		case PROTO:
		ret=proto_control(FRAME,type,input,output);
		break;
	}
	return ret;
}

//功能模块初始化处理
int work_init(int num, smartCallback callback)
{
	int err=0;
	struct KeyValueArray *cfg=NULL;
	pworkdata = (struct work_data *)calloc(1,sizeof(struct work_data));
        cfg=(struct KeyValueArray*)calloc(1,sizeof(struct KeyValueArray));
	//检查必要的环境
	//初始化数据结构workdata
	//依次调用各个模块的初始化
	pworkdata->ifexit=WORKING;
	pworkdata->callback=callback;
	err=config_init(NULL,work_callback,&pworkdata->modinf[CONFIG]);
	if(err==-1)
		return -1;
	err=config_control(FRAME,CFGCTL_READ_CFG,NULL,cfg);
	if(err==-1)
		return -1;
	err=dev_init(cfg,work_callback,&pworkdata->modinf[DEVICE]);
	if(err==-1)
		return -1;
	err=log_init(cfg,work_callback,&pworkdata->modinf[LOG]);
	if(err==-1)
		return -1;
	err=proto_init(cfg,work_callback,&pworkdata->modinf[PROTO]);
	if(err==-1)
		return -1;
	err=handle_init(cfg,work_callback,&pworkdata->modinf[HANDLE]);
	if(err==-1)
		return -1;
	return 0;
}

