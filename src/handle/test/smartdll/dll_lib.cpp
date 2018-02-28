#include <stdio.h>
#include "dll.h"
#include "fantask.h"
#include "camtask.h"

//对dllib类的成员函数进行实现
dllib::dllib()		//构造函数,参数为本动态库支持的任务数
{
	class task *ptask;
	int i;
	
	status = false;
	tasknum = 0;
	cb = NULL;
	tasktimer = NULL;
	
	for(i = 0; i<TASK_SUM; i++)
	{
		switch(i)
		{
		case 0:
			ptask = new fantask(0, FANPARA_NUM, FANCTL_NUM);//电扇任务id为0, 2个参数组，3个控制信息
			ptask->setname((char *)FANNAME, (char *)FANTIP);
			ptask->setbase(FANINTELVAL, FANTYPE, FANLEVEL);

			break;
			
		case 1:
			ptask = new camtask(1,CAMPARA_NUM, CAMCTL_NUM);//任务id为1, 2个参数组，3个控制信息
			ptask->setname((char *)CAMNAME, (char *)CAMTIP);
			ptask->setbase(CAMINTELVAL,CAMTYPE, CAMLEVEL);
			
			break;
			
		default:
			ptask = NULL;
			break;
		}
		ptask->setdef_ctl();
		ptask->setparainf();
		dlltask[i] = ptask;	
	}
	if(task::getsum() < TASK_SUM)
		printf("Tasks defined too few，you should define %d tasks...\n", TASK_SUM - task::getsum());
	else
		printf("Tasks have been defined completely!!!\n");
}

dllib::~dllib()		//构造函数,参数为本动态库支持的任务数
{
	int i;
	for(i=0; i<TASK_SUM; i++)
	{
		if(dlltask[i])
		{
			delete dlltask[i];
			dlltask[i] = NULL;
		}
	}
	
	if(tasktimer)
		delete tasktimer;	
}

 //给各个任务设置回调函数
void dllib::setcb(int (*callback)(int type, void* msg))
{
	int i;

	cb = callback;
	for(i=0; i<TASK_SUM; i++)
	{
		if(dlltask[i])
		{
			dlltask[i]->setcb(callback);
		}
	}
}

int dllib::start(bool force)	//启动监控操作,参数为强迫重新启动
{
	int i;
	
	if(tasknum == 0)
	{
		status = DLL_INITED;
		if(tasktimer)
			tasktimer->stop();
		return -1;
	}
	
	if(force)
	{
		tasktimer->stopwork();
		for(i=0; i<TASK_SUM; i++)
		{
			if(dlltask[i])
			{
				dlltask[i]->reset();
			}
		}
	}
	
	if(tasknum > 0)
	{
		tasktimer->beginwork();		//仅仅改变状态，让定时器可以继续执行监控任务
		status = DLL_RUNNING;
	}

	return 0;
}
int dllib::stop()		//停止所有监控任务
{
	if( tasknum > 0 &&  status == DLL_RUNNING)	//如果没有用户定义的任务直接返回为真
	{		
		tasktimer->stopwork();
		status = DLL_PAUSE;
	}
	
	return 0;
}

int dllib::settask(struct task_user *head) //重新设置库的监控任务并启动
{
	int i;
	struct task_user *ptu = head;
	
	if(status == DLL_RUNNING)
	{
		tasktimer->stopwork();
		status = DLL_PAUSE;
	}
	
	tasknum = 0;
	for(i = 0; i<TASK_SUM; i++)
	{
		if(dlltask[i]->getstatus() == TASK_WORKING) //如果处于运行状态的，置为暂停状态
			dlltask[i]->setstatus(TASK_PAUSE);
	}

	while(ptu)
	{
		if(modtask(ptu) == 0)
		tasknum++;
		ptu = ptu->next;
	}
	
	if(tasknum > 0 && status < DLL_RUNNING)
	{
		if(tasktimer)
			tasktimer->start();
		else
			tasktimer = new tktimer;
		status = DLL_RUNNING;		
	}
	else if(tasknum > 0 && status == DLL_PAUSE)
	{
		tasktimer->beginwork();
		status = DLL_RUNNING;
	}
	else
	{
		if(tasktimer)
			tasktimer->stop();
		status = DLL_INITED;
	}
	
	return 0;
}

int dllib::deltask(int taskid)		//删除一个任务,实际上只是状态改变为invalid。
{
	int i,ret = 0;
	
	for(i=0; i<TASK_SUM; i++)
		if(dlltask[i]->getid() == taskid)
			break;
	
	if(i < TASK_SUM)
	{
		dlltask[i]->setstatus(TASK_INVALID);
		tasknum--;
		if(tasknum == 0)
		{
			tasktimer->stop();
			status = DLL_INITED;
		}
	}
	else
		ret = -1;
	return ret;
}

int dllib::modtask(struct task_user *ptu) //修改一个任务信息
{
	int i,ret = 0;
	
	for(i=0; i<TASK_SUM; i++)
	{
		if(ptu->id == dlltask[i]->getid())
			break;
	}
	
	if(i < TASK_SUM)
	{
		if(dlltask[i]->getstatus() == TASK_WORKING)
			dlltask[i]->setstatus(TASK_PAUSE);
		else if(dlltask[i]->getstatus() == TASK_INVALID)
			tasknum++;
		
		dlltask[i]->setbase(ptu->intelval, ptu->type, ptu->level);
		dlltask[i]->setpara(ptu->index); 		//设置选择的参数组
		dlltask[i]->setctlinf(ptu->ctlinf); 	//设置控制信息
		dlltask[i]->setstatus(TASK_WORKING); 	//成功设置之后，置为工作状态
	}
	else
		ret = -1;
	
	return ret;
}
