//dman.c
//实现动态库管理和初始化
//
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "handle.h"

extern struct handle_data *phdata;

//动态库回送信息的函数
int dll_sendback(int type, void* msg)
{
	printf("receive dll msg: type=%d\n",type);
	return 0;
}

//初始化一个动态库
int dll_init(char *dllfile)
{
	struct	dllinfo	*di;
	int len;

	di = &phdata->dinfo;
	if(di->status != 0) //不是初始状态
		return -1;

	len = strlen(dllfile);
	if(len>0 && len < 20)
		strcpy(di->dllname,dllfile);
	else
		return -1; //文件名长度检查不合适的话

	di->handle = dlopen(di->dllname, RTLD_LAZY);
	if(di->handle)
	{
		di->init = dlsym(di->handle, "init");
		di->control = dlsym(di->handle, "control");
		di->close = dlsym(di->handle, "close");
		di->query = dlsym(di->handle, "query");

		di->callback = dll_sendback;

		if(di->init(phdata->task, di->callback) == 0)
			di->status = DLL_RUNNING;
		else if(phdata->tasknum == 0)
			di->status = DLL_PAUSE;
		else
			di->status = DLL_ERROR;
	}
	else
	{
		printf("open dll=%s ERROR\n", dllfile);
		return -1;
	}

	return 0;
}

int dll_close()
{
	struct	dllinfo	*di;

	di = &phdata->dinfo;
	if(di->status == 0) //是初始状态,直接 返回。
		return 0;

	if(di->status == DLL_RUNNING) //正在运行
		di->control(10, NULL, NULL); 			//送入空监控任务，让动态库停下来。

	dlclose(di->handle);
	di->status = 0;

	return 0;
}

//更换一个动态库文件
int dll_change(char *newfile)
{
	struct	dllinfo	*di;

	di = &phdata->dinfo;
	if(di->status == 0) //是初始状态,直接 返回。
		return dll_init(newfile);
	else if(strcmp(di->dllname,newfile) != 0) //新文件名和以前的不一样
	{
		dll_close();
		return dll_init(newfile);
	}
	else
		printf("dll filename not changed\n");

	return 0;
}
