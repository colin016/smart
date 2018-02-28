#ifndef __DLLTASK_H
#define __DLLTASK_H

#include <sys/time.h>
#include "dll.h"

#define TASK_INVALID	0
#define TASK_WORKING	1
#define TASK_PAUSE	2	 
//动态库本身的系列定义
//参数组基类定义
class para_inf
{
public:
	int index;	//任务支持参数组编号——类型
	char *tip;	//参数组提示信息
	class para_inf *next;
};

//监控任务基类定义
class task
{
	int	id;			//监控任务编号
	char 	*tasktip;	//供用户查看的任务说明信息；
	char	*taskname; 	//监控任务名称
	
	char	level;		//报警级别，人工自定义数据
	short	type;		//报警类别，人工自定义数据
	
	int	num;	//自启动以来，本监控任务产生符合条件的监控信息数量。	
	int	intelval;	//监控任务执行时间间隔 微秒
	char	status; //任务状态 0 invalid；1 working 2 pause
	static int tasksum; //统计生产了多少个派生对象	
protected:
	struct timeval lasttime; //数据采集时间
	//任务本身的信息定义
	int	(*cb)(int type, void* msg);	
	int 	index;	//监控分析参数集编号****	
	const int paranum;
	class para_inf *parainf;
	const int ctlnum;
	class ctl_inf *ctlinf;	
	
public: //任务处理接口定义

	task(int id, int pnum, int cnum);
	virtual ~task();
	
	static int getsum() {return task::tasksum;}
	int getval() {return intelval;}
	int getnum() {return num;}
	int getstatus() {return status;}
	int getid() {return id;}
	void setstatus(char s) {status=s;}
	
	int gettaskinf(struct task_tip *taskinf); //获得任务信息
	int getctlinf(struct ctl_inf *data); 	//获得所有控制信息
	int getparainf(struct para_tip *pinf); 	//获得所有参数组提示信息
	
	int setname(char *name, char *tip);
	int setbase(int intval, char t, char l);
	int setpara(int ind); 					//设置选择的参数组
	int setcb(int (*callback)(int type, void* msg));
	int setctlinf(struct ctl_inf *data); 	//设置控制信息,如果不存在则添加，如果存在则修改。
	
	int getinfofdev(int index, void *buffer, int blen);		//访问设备的接口
	int dev_control(int index);

	int getarraynum(int type);	//获得控制信息数量或者参数组数量，0参数组，1控制信息	
	
	virtual int setparainf() = 0;	//设置参数组
	virtual int setdef_ctl() = 0;
	virtual int reset( ) = 0;				//重置任务
	virtual int getsize( ) = 0;		//获取任务	
	
	virtual void ctlexec(void) = 0; 	//定时执行的任务
	virtual int ondevent(struct outctl *in)=0;	//收到设备主动送来信息时进行处理
};

#endif
