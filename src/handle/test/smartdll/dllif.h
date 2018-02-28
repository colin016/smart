#ifndef __DLLIF_H
#define __DLLIF_H

#include "devshare.h"

#define ALARM_INFO			1
#define MYLOG_INFO			2
#define NOTICE_INFO			3
#define ERROR_INFO			4

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
			char 	code[2];		//控制辅助指令
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
	char 	*tasktip;	//供用户查看的任务说明信息；
	char	*name; 		//监控任务名称
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

#endif
