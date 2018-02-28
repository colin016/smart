#include "dev.h"

extern struct proto_data *protodata;	//引用模块变量
extern struct dev_data *devdata;

void *dev_timer(void *arg)
{
	char *ret = arg;

	while (1)
	{
		pthread_testcancel();
		check_medias();
	    //check_device();
		check_ctltable();
		//sleep(10);
		sleep(devdata->timeout);
	}
	return ret;
}

int  check_medias(void)
{
	mutex_medialist_lock(&devdata->medtable);
	struct media_if *med=devdata->medtable.med_list;

	while(med!=NULL)
	{
		if(med->check(med)==-1)
		{
				med->status=CLOSE;
		}
		else
		{
			med->status=LIVE;
		}
		med=med->next;
	}
	mutex_medialist_unlock(&devdata->medtable);
	return 0;
}

// int  check_device(void)
// {
// 	mutex_devicelist_lock(&devdata->devtable);
// 	struct ext_dev *dev=devdata->devtable.dev_list;

// 	while (dev!=NULL)
// 	{
// 		if(dev->keepalive(dev)==-1)
// 		{
// 			dev->status=CLOSE;
// 		}
// 		else
// 		{
// 			dev->status=LIVE;
// 		}
// 		dev=dev->next;
// 	}
// 	mutex_devicelist_unlock(&devdata->devtable);
// 	return 0;
// }

int  check_ctltable(void)
{
	mutex_ctlinflist_lock(&devdata->ctltable);
	struct devctl_inf *ctl=devdata->ctltable.ctl_list;
	time_t pre_time=0;

	if(devdata->ctltable.size!=0)
	{
		while(ctl!=NULL)
		{
			time(&pre_time);
			if((pre_time-ctl->time)>3)
			{
				strcpy(ctl->output->msg,"超时无响应！");
				ctl->output->len=strlen(ctl->output->msg);
	            sem_post(&ctl->sem);
			}
			ctl=ctl->next;
		}
	}
	mutex_ctlinflist_unlock(&devdata->ctltable);
	return 0;
}
