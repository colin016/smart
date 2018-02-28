#include <stdio.h>
#include "fantask.h"

fantask::fantask(int id,int pnum, int dnum):task(id, pnum, dnum)
{
	parainf = new para_fan[paranum];
}

fantask::~fantask( )
{
	delete []parainf;
}

int fantask::setparainf( )	//设置参数组
{
	para_fan *p;
	int i=0,ret = 0;
	
	p = (para_fan *)parainf;
	
	while(i<paranum)
	{
		if(i == SENSITIVE_DATA)
		{
			p->index = SENSITIVE_DATA;
			p->tip = (char *)SENSITIVE_TIP;
			p->senseth = SENSITIVE_SVALVE;
			p->tempth = SENSITIVE_TVALVE;
			p->next = p+1;
		}
		else if(i == SLOWLY_DATA)
		{
			p->index = SLOWLY_DATA;
			p->tip = (char *)SLOWLY_TIP;
			p->senseth = SLOWLY_SVALVE;
			p->tempth = SLOWLY_TVALVE;
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

int fantask::setdef_ctl( )	//设置控制信息
{
	ctl_inf *p;
	int i=0,ret = 0;
	
	p = (ctl_inf *)ctlinf;
	
	while(i<ctlnum)
	{
		if(i == TEMP_INDEX)
		{
			p->index = TEMP_INDEX;
			p->tip = (char *)TEMP_TIP;
			p->type = TYPE_SENSOR;
		}
		else if(i == SENSE_INDEX)
		{
			p->index = SENSE_INDEX;
			p->tip = (char *)SENSE_TIP;
			p->type = TYPE_SENSOR;
		}
		else if(i == FANCTL_INDEX)
		{
			p->index = FANCTL_INDEX;
			p->tip = (char *)FANCTL_TIP;
			p->type = TYPE_RELAY;			
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

int fantask::reset( )				//重置任务
{
	lasttime.tv_sec = 0;
	lastsense = 0;
	lasttemp = 0;
	
	return 0;
}

int fantask::getsize()
{
	return sizeof(fantask) + sizeof(ctl_inf)*ctlnum + sizeof(para_fan)*paranum;
}

void fantask::ctlexec(void) 		//定时执行的任务
{
	int curtemp, cursense;
	int speed, isturn; //转速和是否摆动
	int ret;
	class para_fan *pf = (class para_fan *)&(parainf[index]);
	struct timeval curtime;
	
	gettimeofday(&curtime, NULL);
	if((curtime.tv_sec-lasttime.tv_sec)*1000000+(curtime.tv_usec-lasttime.tv_usec) <  getval() )
		return;
	
	ret = getinfofdev(TEMP_INDEX, &curtemp, sizeof(int));
//	DBG("Get temperature info: ret=%d, temp=%d\n", ret, curtemp);

	ret = getinfofdev(SENSE_INDEX,&cursense, sizeof(int));
//	DBG("Get sensor info: ret=%d, sense=%d\n", ret, cursense);
	
	if(ret == -1)
	{
		cb(ERROR_INFO, (void *)"get sense information error");
		return;
	}
	
	if(curtemp > pf->senseth && cursense > pf->senseth)
	{
		speed = 10 + (curtemp - lasttemp);
		isturn = cursense > lastsense ? 1 : 0;
	}
	else
	{
		speed = 0;
		isturn = 0;
	}
	
	if(speed != ctlinf[FANCTL_INDEX].info.devctl.code[0] 
	|| isturn !=  ctlinf[FANCTL_INDEX].info.devctl.code[1])
	{
		ctlinf[FANCTL_INDEX].info.devctl.code[0] = speed;
		ctlinf[FANCTL_INDEX].info.devctl.code[1] = isturn;
		ret = dev_control(FANCTL_INDEX);
		if(ret != 0)
		{
			cb(ERROR_INFO, (void *)"Control device fail....");
			return;
		}
	}
	
	gettimeofday(&lasttime, NULL);
	lastsense = cursense;
	lasttemp = curtemp;	
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
int fantask::ondevent(outctl *in)	//收到设备主动送来信息时进行处理
{
	ctl_inf *p;
	class para_fan *pf = (class para_fan *)&(parainf[index]);
	int i=0,ret = 0, data;
	
	p = (ctl_inf *)ctlinf;
	
	while(i < ctlnum)
	{
		if(p->type == in->type) //类型一致
		{
			if(p->info.devctl.sign == in->sign) //特征码一致
			{
				int speed;
				int isturn;
				
				data = *((int *)in->buf);
				
				if(i == SENSE_INDEX)  //如果是人体感应信息
				{
					if(data > pf->senseth)
					{
						speed = 10 + (data - lastsense);
						if(lastsense > pf->senseth)
							isturn = 1;
						else 
							isturn = 0;
					}
					else if(data < 0)
					{
						speed = 0;
						isturn = 0;
					}
				}
				else if(i == TEMP_INDEX)
				{
					if(data > pf->tempth)
					{
						speed = 10 + (data - lasttemp);
						if(lasttemp > pf->tempth)
							isturn = 1;
						else 
							isturn = 0;
					}
					else if(data < 0)
					{
						speed = 0;
						isturn = 0;
					}

				}			
				
				if(speed != ctlinf[FANCTL_INDEX].info.devctl.code[0] 
				|| isturn !=  ctlinf[FANCTL_INDEX].info.devctl.code[1])
				{
					ctlinf[FANCTL_INDEX].info.devctl.code[0] = speed;
					ctlinf[FANCTL_INDEX].info.devctl.code[1] = isturn;
					ret = dev_control(FANCTL_INDEX);
					if(ret != 0)
					{
						cb(ERROR_INFO, (void *)"Control device fail....");
						return ret;
					}
				}
			}
		}
		
		i++;
		p++;
	}
	return ret;		
}
