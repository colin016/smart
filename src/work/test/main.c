#include <stdio.h>
#include "work.h"

extern struct work_data *pworkdata;
int config_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret);
int config_control(int sendid, int type, void *input,void *output);

int dev_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret);
int dev_control(int sendid, int type, void * input, void * output);

int handle_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret);
int handle_control(int sendid, int type, void * input, void * output);

int log_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret);
int log_control(int sendid, int type, void *input,void *output);

int proto_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret);
int proto_control(int sendid, int type, void * input, void * output);

int smart_callback(int funcid, int type, void *input, void *output)
{

	return 0;
}

void ac_exit_system(int num)
{
	pworkdata->ifexit = 1;
}

void pid_init()
{
	signal(SIGINT, ac_exit_system);/*register signal handler*/
	signal(SIGTERM, ac_exit_system);/*register signal handler*/
	signal(SIGKILL, ac_exit_system);/*register signal handler*/
}

int main() {
	pid_init();

	work_init(5,smart_callback);
	while (pworkdata->ifexit)
	{
		printf("hello word!\n");
		sleep(1);
	}

	work_close();
	return 0;
}




int config_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret)
{
	return 0;
}

int config_control(int sendid, int type, void *input,void *output)
{
	return 0;
}

int dev_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret)
{
	return 0;
}
int dev_control(int sendid, int type, void * input, void * output)
{
	return 0;
}

int handle_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret)
{
	return 0;
}

int handle_control(int sendid, int type, void * input, void * output)
{
	return 0;
}

int log_init(KeyValueArray  *cfg,moduleCallback callback,struct ModuleInfo* ret)
{
	return 0;
}

int log_control(int sendid, int type, void *input,void *output)
{
	return 0;
}

int proto_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo* ret)
{
	return 0;
}
int proto_control(int sendid, int type, void * input, void * output)
{
	return 0;
}
