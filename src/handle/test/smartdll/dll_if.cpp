#include <stdio.h>
#include "dll.h"

class dllib *dllinf;

//动态库接口函数
int init(struct task_user *head, int (*callback)(int type, void* msg))
{
	int i;

	
	DBG("enter init( )......\n");
	//构造动态库的任务信息定义
	
	dllinf = new dllib; //本模块
	dllinf->setcb(callback);
	dllinf->setstatus(DLL_INITED);

	if(head)
		i = dllinf->settask(head); //如果携带有用户定义的任务设置，则进行设置
	else
		i = 1;
	
	return i;
}

int query(int type)
{
	int i, ret = 0;
	DBG("enter query( )......\n");
	DBG("type =%d\n", type);
	
	switch(type)
	{
		case 0:
			ret = DLL_VERSION;
			break;
		
		case 1:
			if(dllinf->getstatus() < DLL_RUNNING)
				ret = 0;
			else
				ret = 1;
			break;
			
		case 2:
			if(dllinf == NULL)
				ret = 0;
			else if(dllinf->getstatus() == DLL_INITED)
			{
				ret = sizeof(dllib);
				for(i=0; i<TASK_SUM; i++)
					ret += dllinf->gettask(i)->getsize();
			}
			break;
			
		case 3:
			if(dllinf==NULL || dllinf->getstatus() == DLL_ERROR)
				ret = 0;
			else
				ret = dllinf->gettasknum();
			break;
			
		case 4:
			if(dllinf->getstatus() >= DLL_INITED)
			{
				task *p;
				
				for(i=0; i<TASK_SUM; i++)
				{
					p = dllinf->gettask(i);
					ret += p->getnum();
				}
			}
			else
				ret = 0;
			
		case 5:
			ret = dllinf->getstatus();
			break;
			
		default:
			ret = -1;
			break;
	}
	
	return ret;
}

//最重要的控制接口
int control(int type, void *in, void *out)
{
	int i, ret = 0;
	DBG("enter control( )......\n");
	
	if(dllinf == NULL) //如果还没有初始化的话
		return -1;
	
	switch(type)
	{
	case 0:
		ret = TASK_SUM;
		break;
		
	case 1:
		for(i=0; i<TASK_SUM; i++)
		{
			dllinf->gettask(i)->gettaskinf((struct task_tip *)out+i);
		}
		break;
		
	case 2:
		i = *((int *)in);
		ret = dllinf->gettask(i)->getarraynum(0);		
		break;
		
	case 3:
	{
		int n = *((int *)in);
		dllinf->gettask(n)->getparainf((struct para_tip *)out);
	}
		break;
		
	case 4:
		i = *((int *)in);
		ret = dllinf->gettask(i)->getarraynum(1);		
		break;
		
	case 5:
	{
		int n = *((int *)in);
		dllinf->gettask(n)->getctlinf((struct ctl_inf *)out);
	}		
		break;

	case 6:
		if(dllinf->getstatus() == DLL_RUNNING)
		{
			ret = dllinf->stop();
			if(ret == 0)
				dllinf->setstatus(DLL_PAUSE);
			else
				dllinf->setstatus(DLL_ERROR);
		}
		break;
		
	case 7:
		ret = dllinf->settask((struct task_user *)in); //如果携带有用户定义的任务设置，则进行设置
		break;
		
	case 8:
		ret = dllinf->modtask((struct task_user *)in); //如果携带有用户定义的任务设置，则进行设置		
		break;

	case 9:
		ret = dllinf->deltask(*((int *)in));
		break;
		
	case 10:
	{
		int force;
		if(in == NULL)
			force = 0;
		else
			force = (*(int *)in);

		if(force == 0 && dllinf->getstatus() >= 0)
			dllinf->start(false);
		else
			dllinf->start(true);

	}
		break;
		
	case 11: //获取动态库id和版本
	{
		struct dll_ver *p = (struct dll_ver *)out;
		
		p->name = (char *)DLL_ID;
		p->version = DLL_VERSION;
	}
		break;
		
	default:
		ret = -1;
		break;		
	}
	return ret;
}

int closedll(void)
{
	DBG("enter close( )......\n");
	
	delete dllinf;
	
	return 0;
}
