#include <stdio.h>
#include <string.h>
#include "dll.h"

//对task类的成员函数进行实现
int task::tasksum = 0;

task::task(int id, int pnum, int cnum):id(id), paranum(pnum), ctlnum(cnum)
{
	task::tasksum++;
	ctlinf = new ctl_inf[ctlnum];
	memset(&lasttime, 0, sizeof(struct timeval));
}

task::~task()
{
	delete []ctlinf;	
}

int task::getctlinf(struct ctl_inf *data)	//获得所有控制信息
{
	int ret = 0;
	
	if(data)
		memcpy(data, ctlinf, ctlnum * sizeof(struct ctl_inf));
	else
		ret = -1;
	
	return ret;
}
/*
	int		id;			//监控任务编号
	char 	*tasktip;	//供用户查看的任务说明信息；
	char	*name; 	//监控任务名称，人工自定义
	char	level;		//报警级别，人工自定义数据
	short	type;		//报警类别，人工自定义数据*/
int task::gettaskinf(struct task_tip *taskinf)	//获得任务信息
{
	int ret = 0;
	
	if(taskinf)
	{
		taskinf->id = id;
		taskinf->tasktip = tasktip;
		taskinf->name = taskname;
		taskinf->level = level;
		taskinf->type = type;
	}
	else
		ret = -1;
	
	return ret;
}

int task::getparainf(struct para_tip *pinf)	//获得所有参数组提示信息
{
	int i;
	para_inf *p = parainf;
	
	if(pinf==NULL)
		return -1;
	
	for(i=0; i<paranum; i++)
	{
		
		(pinf+i)->index = p->index;
		(pinf+i)->tip = p->tip;
		p = p->next;
	}
	return 0;
}

int task::setname(char *name,char *tip)
{
	taskname = name;
	tasktip = tip;
	return 0;
}

int task::setbase(int intval, char t, char l)
{
	if(intval > 0)
		intelval = intval;
	else
		DBG("error parameter: intval=%d\n", intval);

	type = t;
	level = l;
	
	return 0;
}

int task::setpara(int ind)	//设置选择的参数组
{
	index = ind;
	
	return 0;
}

int task::setcb(int (*callback)(int type, void* msg))
{
	cb = callback;
	
	return 0;
}

int task::setctlinf(struct ctl_inf *data)	//设置控制信息,如果不存在则添加，如果存在则修改。
{
	int ret = 0;
	
	if(data !=NULL)
		memcpy(ctlinf, data,  ctlnum * sizeof(struct ctl_inf));
	else
		ret = -1;
	
	return ret;
}

int task::getinfofdev(int index, void *buffer, int blen)		//访问设备的接口
{
	int ret = 0;
	
	if(index >= ctlnum)
		ret = -1;
	else
	{
		ctl_inf *p = ctlinf + index;
	
		if(p->type != TYPE_FILE && p->info.devctl.dev != NULL)
		{
			reqctl req;
			outctl out;
			
			req.sign = p->info.devctl.sign;
			req.code[0] = p->info.devctl.code[0];
			if((req.len=p->info.devctl.len)>1)
			{
				req.code[1] = p->info.devctl.code[1];
				req.len = 2;
			}
			else
				req.len = 1;
			
			out.buf = (char *)buffer;
			out.buflen = blen;
			ret = p->info.devctl.dev->control(p->info.devctl.dev, &req, &out);
			if(ret == 0)
				ret = out.inflen;
		}
	}
	
	return ret;	
}

int task::dev_control(int index)
{
	int ret = 0;
	
	if(index >= ctlnum)
		ret = -1;
	else
	{
		ctl_inf *p = ctlinf + index;
	
		if(p->type != TYPE_FILE && p->info.devctl.dev != NULL)
		{
			reqctl req;
		
			req.sign = p->info.devctl.sign;
			req.code[0] = p->info.devctl.code[0];
			if((req.len=p->info.devctl.len)>1)
			{
				req.code[1] = p->info.devctl.code[1];
				req.len = 2;
			}
			else
				req.len = 1;
			
			ret = p->info.devctl.dev->control(p->info.devctl.dev,&req, NULL);
		}
		else
			ret = -1;
	}
	
	return ret;	
}
	
int task::getarraynum(int type)		//获得控制信息数量或者参数组数量，0参数组，1控制信息
{
	int ret = -1;
	
	if(type == 0)
		ret = paranum;
	else if(type == 1)
		ret = ctlnum;
	else
	{
		DBG("parameter:type error %d\n", type);
	}	
	return ret;
}
