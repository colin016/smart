#ifndef __DLLIF_H
#define __DLLIF_H

#include "devshare.h"

#define ALARM_INFO			1
#define MYLOG_INFO			2
#define NOTICE_INFO			3
#define ERROR_INFO			4

struct para_tip
{
	int index;	//����֧�ֲ�������
	char *tip;	//��������ʾ��Ϣ
};

struct task_tip
{
	int		id;			//���������
	char 	*tasktip;	//���û��鿴������˵����Ϣ��
	char	*name; 	//����������ƣ��˹��Զ���
	char	level;		//���������˹��Զ�������
	short	type;		//��������˹��Զ�������
};

//��Ϣ�ɼ����ļ����豸�������ݽṹ
struct ctl_inf
{
	union
	{
		struct
		{
			struct	ext_dev *dev;	//�����豸ָ��
			short 	devno; 			//�豸���
			char 	sign; 			//��Ϣ�ر��룬���ڶ���������Ϣ���߿������	
			char 	code[2];		//���Ƹ���ָ��
			char	len;			//���Ƹ���ָ���
		} devctl;
		char	filename[16];		//�ļ���
	}info;

	char 	type;			//��Ϣ��𣬰���0-5�ļ���6-10���ƣ�11���ϲɼ�����Ҫ������ͽ�����ϸ����
	
	char	index;			//��Ҫ����Ϣ��ţ�һ���������±�һ�¡����庬���ɿ��������Լ�����	
	char	*tip;			//Ҫ����Ϣ�Ľ�����ʾ��Ϣ
};

struct task_user
{
	int		id;			//���������
	char 	*tasktip;	//���û��鿴������˵����Ϣ��
	char	*name; 		//�����������
	char	level;		//���������˹��Զ�������
	short	type;		//��������˹��Զ�������
	int	intelval;	//����ִ��ʱ����

	int ctlnum; //��Ҫ�Ŀ�����Ϣ����
	struct ctl_inf	*ctlinf;	//������Ϣ
	
	int index;	//ѡ���Ĳ�������
	struct task_user *next;
};

struct dll_ver
{
	char *name;
	short version;
};

#endif
