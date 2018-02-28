#include <stdio.h>
#include "dll.h"
#include <unistd.h>

extern class dllib *dllinf;


void * thread_func(void * args)    //获取执行方法的指针 
{
	tktimer *pt = (tktimer *)args;
	task *p;
	int i;
	
	while(pt->isrun && pt->status)
	{
		DBG("timer running ...\n");
		for(i=0; i<TASK_SUM; i++)
		{
			p = dllinf->gettask(i);
			if(p)
			{
				if(p->getstatus() == TASK_WORKING)
					p->ctlexec();
			}
		}
		usleep(pt->interval);
	}
		
	return NULL;
}

tktimer::tktimer()
{
	tid = 0;
	interval = computval();

	DBG("construct timer, interval = %d\n", interval);
	if(interval > 0)
	{
		status = true;
		start();
	}
	else
	{
		status = false;
		isrun = false;
	}
}

tktimer::~tktimer()
{
	if(tid)
	{
		DBG("releasing timer...\n");
		isrun = false;
		pthread_join(tid, NULL);
	}
	DBG("~tktimer finish\n");
}

int tktimer::calval(int a, int b)
{
	int i, ret = 0;
	int max, min;
	
	if(a<=0 || b<=0)
		return 0;
	
	if(a > b)
	{
		max = a;
		min = b;
	}
	else if(b > a)
	{
		max = b;
		min = a;
	}
	else
		return a;
	
	while(1)
	{
		i = max % min;
		if(i == 0)
		{
			ret = min;
			break;
		}
		else
		{
			max = min;
			min = i;
		}
	}
		
	return ret;	
}

int tktimer::computval( ) //计算执行间隔时间
{
	int a=0,ret = 0,b,i;
	task *p;
	
	for(i=0; i<TASK_SUM; i++)
	{
		p = dllinf->gettask(i);
		if(p)
		{
			if(p->getstatus() != TASK_INVALID)
			{
				if(a == 0)
					a = p->getval();
				else
				{
					b = p->getval();
					a = calval(a,b); //计算两个数的最大公约数
					if(a <= 0)
						break;
				}
			}
		}
	}
	
	if(a > 0)
		ret = a;
	else
	{
		ret = 1000000; //一秒钟
		DBG("******abnormal result******\n");
	}

	return ret;
}

int tktimer::start()  //初始化线程
{
	int ret = 0;

	DBG("start timer...\n");	

	if(tid == 0)
	{	
		DBG("First run, creating timer thread....\n");
		if(pthread_create(&tid, NULL, thread_func, this)!=0) //启动不成功
		{
			DBG("set timer thread fail....\n");
			isrun = false;
			ret = -1;
		}
	}
	isrun = true;
	DBG("create timer success, tid=%ul\n",(int)tid);
			
	return ret;
}

int tktimer::stop()	//关闭定时器线程	
{
	int ret = 0;
	if(tid)
	{
		isrun = false;
		pthread_join(tid, NULL);
		tid = 0;
	}
	else
		ret = -1;
	return ret;
}


