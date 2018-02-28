//实现系统初始化和控制处理，调用work接口对work初始化，获取配置，网络参数初始化。
#include "smart.h"

struct man_data mdata;

//回调函数定义，用于work通知smart
int man_callback(int funcid, int type, void *input, void *output)
{
	return 0;
}


//整个系统的初始化，根据情况初始化work模块
int man_init(void)
{
	return work_init(1, man_callback);
}

//关闭整个系统
int man_close(void)
{
	//return work_close();
}

//实现对系统整个信息的访问和控制，大多数情况下，调用work_control()函数实现
//当work_control是异步实现的时候，自己挂起等待manage_callback解除挂起。
int man_control(int funcid, int type, void * input, void * output)
{
	return work_control(funcid, type, input, output);
}

//内部接口函数定义
//系统初始化
int man_initsys(void)
{
	return 0;
}

//从work模块获取系统配置信息
int man_getsyscfg(int type)
{
	return 0;
}

void man_timer(void *arg)
{


}
