//实现对报警消息的访问管理

#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "handle.h"
extern struct handle_data *phdata;

static char* read_file(void);
static int init_alarmlist(void);
static int alarm_parse(struct alarm_inf *alarm,cJSON *item);


//初始化报警消息文件
//如果文件存在，则不变；如果文件不存在，则创建文件。
int handle_alarminit( void )
{
    FILE *fp=NULL;

    fp=fopen(ALARM_FILE,"a");
    if (fp==NULL)
    {
    	return -1;
    }
    fclose(fp);
	return 0;
}

//将报警消息备份到指定的文件中去
int handle_alarmbak(char *file)
{
	FILE *falam=NULL;
	FILE *fbak=NULL;
	char buf[1024];
	size_t len=sizeof(buf);
	size_t recvlen;

    fbak=fopen(file,"w");
    if (fbak==NULL)
    {
    	return -1;
    }
	falam=fopen(ALARM_FILE,"r");
	if (falam==NULL)
	{
		fclose(fbak);
		return -1;
	}
    
    memset(buf,0,len);
    while((recvlen=fread(buf,len,1,falam))!=EOF)
    {
        fwrite(buf,recvlen,1,fbak);
        memset(buf,0,len);
    }
   
    fclose(fbak);
    fclose(falam);
	return 0;
} 


//查询报警消息，将结果保存到备份文件中
int handle_alarmquery(struct alarm_query *query, char *file)
{
	if (phdata->alarmtable.alarm==NULL)
	{
		if(init_alarmlist()==-1)
			return -1;
	}else
    {
        if(alarmlist_query(query,file)==-1)
        {
            return -1;
        }
    }

	return 0;
}

//写入一条报警信息，当报警消息条数超过最大值时，备份前面的，重新生成新文件。
int handle_alarmwrite(struct alarm_inf *alarminf)
{
    if(alarmlist_add(alarminf)==-1)
    {
        return -1;
    }
    if (alarmlist_save()==-1)
    {
        return -1;
    }
	return 0;
}

//查询报警消息，将结果读入内存中,buf是存放的数组指针，
//由调用者申请内存，需要读的条数在query中。如果buf为NULL，则仅返回的int是实际符合要求的条数。
int handle_alarmload(struct alarm_query *query, struct alarm_inf *buf)
{
    if (alarmlist_load(query,buf)==-1)
    {
        return -1;
    }
	return 0;
}

static int init_alarmlist(void)
{
 	char *str=NULL;
 	int alarm_num=0;
 	int i=0;
 	size_t alarlen=0;
 	cJSON *root=NULL;
 	cJSON *alarm_item=NULL;
 	cJSON *json_alarms=NULL;
 	struct alarm_inf *alarm_node=NULL;
    
    str=read_file();
    if (str==NULL)
    {
    	return -1;
    }
 	root=cJSON_Parse(str);
 	json_alarms=cJSON_GetObjectItem(root,JSON_ALARMS);
    alarm_num=cJSON_GetArraySize(json_alarms);

    alarlen=sizeof(struct alarm_inf);
    for (i = 0; i < alarm_num; ++i)
    {
    	alarm_node=calloc(1,alarlen);
    	alarm_item=cJSON_GetArrayItem(json_alarms,i);
        alarm_parse(alarm_node,alarm_item);
        alarmlist_add(alarm_node);
    }
	return 0;
}

static int alarm_parse(struct alarm_inf *alarm,cJSON *item)
{
	cJSON *json_type=cJSON_GetObjectItem(item,JSON_ALARM_TYPE);
	cJSON *json_level=cJSON_GetObjectItem(item,JSON_ALARM_LEVEL);
	cJSON *json_time=cJSON_GetObjectItem(item,JSON_ALARM_TIME);
	cJSON *json_attach=cJSON_GetObjectItem(item,JSON_ALARM_ATTACH);

    if (json_type==NULL||json_level==NULL||json_time==NULL||json_attach==NULL)
    {
    	return -1;
    }

    alarm->type=json_type->valueint;
    alarm->level=json_level->valueint;
    alarm->time=json_time->valueint;
    strcpy(alarm->attach,json_attach->valuestring);

    return 0;
}

static char* read_file(void)
{
	FILE *fp=NULL;
	char *str=NULL;
	int len;
	int err;

	fp=fopen(ALARM_FILE,"r");
	if (fp==NULL)
	{
		return NULL;
	}
    fseek(fp,0,SEEK_END);
    len=ftell(fp);
    fseek(fp,0,SEEK_SET);
    str=calloc(1,len);
    err=fread(str,1,len,fp);
    if(len!=err)
    {
       fclose(fp);
       return NULL;
    }
	fclose(fp);

	return str;
}