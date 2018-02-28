#ifndef __TASKTIMER_H
#define __TASKTIMER_H

#include <pthread.h>

class tktimer
{
	pthread_t tid; //定时器线程id 
	bool isrun;		//是否线程退出
	bool status;	//状态0,stop; 1 working
	int interval;		//睡眠时间
	
public:
	tktimer();
	~tktimer();
  
    	friend void * thread_func(void * args);    //获取执行方法的指针 
	void beginwork(){status = true;}
	void stopwork() {status = false;}
	
	int calval(int a, int b);			//计算两个数的最大公约数
	int computval( ); //计算执行间隔时间
	int start(); //初始化线程
	int stop();	//关闭定时器线程	
};	

#endif
