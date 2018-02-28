#include <stdio.h>
#include <stdlib.h>
#include "handle.h"
#include "cJSON.h"

extern struct handle_data *phdata;

static int change_to_json(struct alarm_inf *pnode,cJSON *alarms);

int alarmlist_add(struct alarm_inf *pnode)
{
   struct alarm_inf *p=phdata->alarmtable.alarm;

   if (p==NULL)
   {
   	   phdata->alarmtable.num+=1;
   	   phdata->alarmtable.alarm=pnode;
   }else
   {
       while(p->next!=NULL)
       {
       	  p=p->next;
       }
       phdata->alarmtable.num+=1;
       p->next=pnode;
   }
   return 0;
}

int alarmlist_del(int id)
{
   return 0;
}

int alarmlist_clear(void)
{
	struct alarm_inf *p=phdata->alarmtable.alarm;
    struct alarm_inf *q=NULL;

    while(p!=NULL)
    {
    	q=p;
    	p=p->next;
    	phdata->alarmtable.num-=1;
    	free(q);
    }
	return 0;
}

int alarmlist_save(void)
{
	struct alarm_inf *p=phdata->alarmtable.alarm;
    cJSON *jroot=NULL;
    cJSON *jarray=NULL;
    FILE *fp=NULL;
    char *str=NULL;

    fp=fopen(ALARM_FILE,"w");
    if (fp==NULL)
    {
   	  return -1;
    }

    jroot=cJSON_CreateObject();
    jarray=cJSON_CreateArray();
	while(p!=NULL)
	{
        change_to_json(p,jarray);
		p=p->next;
	}
    cJSON_AddItemToObject(jroot,JSON_ALARMS,jarray);
    str=cJSON_Print(jroot);
    if (str!=NULL)
    {
    	fprintf(fp, "%s\n",str);
    	free(str);
    }else
    {
    	cJSON_Delete(jroot);
    	fclose(fp);
    	return -1;
    }
    cJSON_Delete(jroot);
    fclose(fp);
  return 0;
}

int alarmlist_load(struct alarm_query *query,struct alarm_inf *buf)
{
	struct alarm_inf *p=phdata->alarmtable.alarm;
	int num=0;
    
    while(p!=NULL&&num<=query->num)
    {
    	if (query->sort==QUERY_SORT_TYPE)
   	   {
   	   	 if (query->type==p->type)
   	  	 {
   	  	 	if (buf==NULL)
   	  	 	{
   	  	 		buf[num]=*p;
   	  		    buf[num].next=NULL;//防止函数外通过next指针影响报警链表
   	  	 	}
   	  		num++;
   	  	 }
   	   }
   	   else if (query->sort==QUERY_SORT_LEVEL)
   	   {
   	  	 if (query->level==p->level)
   	  	 {
   	  		if (buf==NULL)
   	  	 	{
   	  	 		buf[num]=*p;
   	  		    buf[num].next=NULL;//防止函数外通过next指针影响报警链表
   	  	 	}
   	  		num++;
   	  	 }
   	   }else if (query->sort==QUERY_SORT_TIME)
   	   {
   	  	 if (query->from<p->time&&query->to>p->time)
   	  	 {
   	  		if (buf==NULL)
   	  	 	{
   	  	 		buf[num]=*p;
   	  		    buf[num].next=NULL;//防止函数外通过next指针影响报警链表
   	  	 	}
   	  		num++;
   	  	 }
   	   }
    	p=p->next;
    }
    
	return num;
}

int alarmlist_query(struct alarm_query *query,char *file)
{
    struct alarm_inf *p=phdata->alarmtable.alarm;
    FILE *fp=NULL;
    cJSON *jroot=NULL;
    cJSON *jarray=NULL;
    char *str=NULL;
    int num=0;

    jroot=cJSON_CreateObject();
    jarray=cJSON_CreateArray();

    fp=fopen(file,"w");
    if (fp==NULL)
    {
   	  return -1;
    }

    while(p!=NULL&&num<=query->num)
    {
   	   if (query->sort==QUERY_SORT_TYPE)
   	   {
   	   	 if (query->type==p->type)
   	  	 {
   	  		change_to_json(p,jarray);
   	  		num++;
   	  	 }
   	   }
   	   else if (query->sort==QUERY_SORT_LEVEL)
   	   {
   	  	 if (query->level==p->level)
   	  	 {
   	  		change_to_json(p,jarray);
   	  		num++;
   	  	 }
   	   }else if (query->sort==QUERY_SORT_TIME)
   	   {
   	  	 if (query->from<p->time&&query->to>p->time)
   	  	 {
   	  		change_to_json(p,jarray);
   	  		num++;
   	  	 }
   	   }
   	   p=p->next;
    }
    cJSON_AddItemToObject(jroot,JSON_ALARMS,jarray);
    str=cJSON_Print(jroot);
    if (str!=NULL)
    {
    	fprintf(fp, "%s\n",str);
    	free(str);
    }else
    {
    	cJSON_Delete(jroot);
        fclose(fp);
    	return -1;
    }
    cJSON_Delete(jroot);
    fclose(fp);
    return 0;
}

static int change_to_json(struct alarm_inf *pnode,cJSON *alarms)
{
    cJSON *jitem=NULL;

    jitem=cJSON_CreateObject();
    if (jitem==NULL)
    {
    	return -1;
    }
    cJSON_AddNumberToObject(jitem,JSON_ALARM_ID,pnode->id);
    cJSON_AddNumberToObject(jitem,JSON_ALARM_TYPE,pnode->type);
    cJSON_AddNumberToObject(jitem,JSON_ALARM_LEVEL,pnode->level);
    cJSON_AddNumberToObject(jitem,JSON_ALARM_TIME,pnode->time);
    cJSON_AddStringToObject(jitem,JSON_ALARM_ATTACH,pnode->attach);
    cJSON_AddItemToArray(alarms,jitem);
	return 0;
}

