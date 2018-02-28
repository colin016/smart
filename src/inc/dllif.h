#ifndef __DLLIF_H
#define __DLLIF_H

#include "devshare.h"
#include "pthread.h"

#define ALARM_INFO			1
#define MYLOG_INFO			2
#define NOTICE_INFO			3
#define ERROR_INFO			4

//监控任务XML文件的标签定义
#define TASKXML_TASK "task"
#define TASKXML_NUMBER "number"
#define TASKXML_ID "id"
#define TASKXML_NAME "name"
#define TASKXML_TASKTIP "tasktip"
#define TASKXML_LEVEL "level"
#define TASKXML_TYPE "type"
#define TASKXML_INTELVAL "intelval"
#define TASKXML_INDEX "index"
#define TASKXML_CTLINF "ctlinf"

#define CTLINFXML_NUMBER "number"
#define CTLINFXML_FILENAME "filename"
#define CTLINFXML_INDEX "index"
#define CTLINFXML_TYPE "type"
#define CTLINFXML_DEVNO "devno"
#define CTLINFXML_SIGN "sign"
#define CTLINFXML_CODE "code"
#define CTLINFXML_LEN "len"

#define TASKNAME_MAXLEN 50
#define TASKTIP_MAXLEN 256

struct para_tip
{
	int index;	//任务支持参数组编号
	char *tip;	//参数组提示信息
};

struct task_tip
{
	int		id;			//监控任务编号
	char 	*tasktip;	//供用户查看的任务说明信息；
	char	*name; 	//监控任务名称，人工自定义
	char	level;		//报警级别，人工自定义数据
	short	type;		//报警类别，人工自定义数据
};

//信息采集，文件，设备控制数据结构
struct ctl_inf
{
	union
	{
		struct
		{
			struct	ext_dev *dev;	//控制设备指针
			short 	devno; 			//设备编号
			char 	sign; 			//信息特别码，用于定义具体的信息或者控制类别。	
			char 	code[2];		//控制辅助指令(新改为char，原为int)
			char	len;			//控制辅助指令长度
		} devctl;
		char	filename[16];		//文件名
	}info;

	char 	type;			//信息类别，包括0-5文件，6-10控制，11以上采集，需要针对类型进行详细定义
	
	char	index;			//需要的信息编号，一般与数组下标一致。具体含义由库中任务自己定义	
	char	*tip;			//要求信息的界面提示信息
};

struct task_user
{
	int		id;			//监控任务编号
	char 	tasktip[TASKTIP_MAXLEN];	//供用户查看的任务说明信息；
	char	name[TASKNAME_MAXLEN]; 	//监控任务名称
	char	level;		//报警级别，人工自定义数据
	short	type;		//报警类别，人工自定义数据
	int	intelval;	//任务执行时间间隔

	int ctlnum; //需要的控制信息数量
	struct ctl_inf	*ctlinf;	//控制信息
	
	int index;	//选定的参数组编号
	struct task_user *next;
};

struct dll_ver
{
	char *name;
	short version;
};

int mutex_lock(pthread_mutex_t *mutex);
int mutex_unlock(pthread_mutex_t *mutex);

#endif
