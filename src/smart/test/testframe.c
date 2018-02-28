//智能监控框架管理模块
#include "sys.h"
#include "smart.h"

//处理模块的回调
int work_callback(int sendid, int recvid, int type, void *input, void *output)
{
	return man_callback(recvid, type, input, output);
}

//功能模块关闭处理
int work_close()
{
	printf("Work_close()");
	
	return 0;
}

//功能模块的控制处理
int work_control(int funcid, int type, void *input, void *output)
{
	printf("Work_control()");
	return 0;
}

//功能模块初始化处理
int work_init(int num, smartCallback callback)
{
	printf("Work_init()");
	return 0;
}

