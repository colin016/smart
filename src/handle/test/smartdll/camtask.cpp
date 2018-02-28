#include <stdio.h>
#include <string.h>
#include "camtask.h"

camtask::camtask(int id,int pnum, int dnum):task(id, pnum, dnum)
{
	parainf = new para_cam[paranum];
	picbuf = new char[PICTURE_LEN];
}

camtask::~camtask( )
{
	delete []picbuf;
	delete []parainf;
}

int camtask::setparainf( )	//设置参数组
{
	para_cam *p;
	int i=0,ret = 0;
	
	p = (para_cam *)parainf;
	
	while(i<paranum)
	{
		if(i == SMART_DATA)
		{
			p->index = SMART_DATA;
			p->tip = (char *)SMART_TIP;
			p->valve = SMART_VALVE;
			p->file = NULL;
			p->next = p+1;
		}
		else if(i == DULL_DATA)
		{
			p->index = DULL_DATA;
			p->tip = (char *)DULL_TIP;
			p->valve = DULL_VALVE;
			p->file = NULL;	
			p->next = NULL;		
		}
		else
		{
			DBG("paranum error %d\n", paranum);
			ret = -1;
		}
		
		i++;
		p++;
	}
	return ret;
}

int camtask::setdef_ctl( )	//设置控制信息
{
	ctl_inf *p;
	int i=0,ret = 0;
	
	p = (ctl_inf *)ctlinf;
	
	while(i<ctlnum)
	{
		if(i == FILE_INDEX)
		{
			p->index = FILE_INDEX;
			p->tip = (char *)FILE_TIP;
			p->type = TYPE_FILE;
		}
		else if(i == CAMERA_INDEX)
		{
			p->index = CAMERA_INDEX;
			p->tip = (char *)CAMERA_TIP;
			p->type = TYPE_CAMERA;
		}
		else if(i == CAMSENSE_INDEX)
		{
			p->index = CAMSENSE_INDEX;
			p->tip = (char *)CAMSENSE_TIP;
			p->type = TYPE_SENSOR;			
		}
		else
		{
			DBG("ctlnum error %d\n", ctlnum);
			ret = -1;
		}
		
		i++;
		p++;
	}
	return ret;		
}

int camtask::reset( )				//重置任务
{
	memset(&lasttime, 0, sizeof(struct timeval));
	lastsense = 0;
	memset(picbuf, 0, PICTURE_LEN);
	
	return 0;
}

int camtask::getsize()
{
	return sizeof(camtask) + sizeof(ctl_inf)*ctlnum + sizeof(para_cam)*paranum;
}

void camtask::ctlexec(void) 		//定时执行的任务
{
	struct timeval curtime;
	
	gettimeofday(&curtime, NULL);
	if((curtime.tv_sec-lasttime.tv_sec)*1000000+(curtime.tv_usec-lasttime.tv_usec) < getval())
		return;
	
	//在这里加上你的处理
	cb(NOTICE_INFO, (void *)"camera task is running....");
}
/*
struct outctl
{
		char type;	//信息类别
		char sign;	//信息特征码
		char *buf;	//信息空间，由调用者分配空间
		int buflen;	//空间大小
		int	inflen;	//信息大小
};
*/
int camtask::ondevent(outctl *in)	//收到设备主动送来信息时进行处理
{
	ctl_inf *p;
	int i=0,ret = 0;
	
	p = (ctl_inf *)ctlinf;
	
	while(i < ctlnum)
	{
		if(p->type == in->type) //类型一致
		{
			if(p->info.devctl.sign == in->sign) //特征码一致
			{
				cb(NOTICE_INFO, (void *)"camera task receive a event....");
			}
		}
		
		i++;
		p++;
	}
	return ret;		
}
