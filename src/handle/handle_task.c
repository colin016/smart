//handle_task.c
//实现监控要求的管理
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle.h"
#include "mxml.h"
extern struct handle_data *phdata;

//解析监控任务文件（task.xml）
static int parse_task_file(mxml_node_t *root);
//解析xml中的控制信息部分
static int parse_ctlinf(mxml_node_t *root,mxml_node_t *node,struct task_user *taskuser);
//监控任务链表初始化
static int tasklist_init(struct task_user *node);
static int ctl_write(struct task_user *ptask,mxml_node_t *task_node);

//检查是否有监控任务文件，有则读出文件内容，对handledata.task链表赋值
int handle_taskinit(void)
{
    FILE *fp=NULL;
    mxml_node_t *root;

    fp=fopen(TASK_FILE,"r");
    if (fp==NULL)
    {
    	perror("open task file");
    	return -1;
    }
    root=mxmlLoadFile(NULL, fp,MXML_TEXT_CALLBACK);
    fclose(fp);
    if (root==NULL)
    {
    	PLOG("task file load fail.\n");
    	return-1;
    }
    if (parse_task_file(root))
    {
    	mxmlDelete(root);
    	return -1;
    }
    mxmlDelete(root);
	return 0;
}

//根据handledata.task链表内容，重新写监控任务文件
int handle_taskwrite(void)
{
    struct task_user *p=NULL;
    int i=0;
    size_t tnn_len=0;
    size_t id_len=0;
    char number[5];
    char id[5];
    char task_node_name[10];

    p=phdata->task;
    tnn_len=sizeof(task_node_name);
    id_len=sizeof(id);
    memset(number,0,sizeof(number));
    mxml_node_t *xml=mxmlNewXML("1.0");
    mxml_node_t *xml_root=mxmlNewElement(xml,TASKXML_TASK);
    sprintf(number,"%d",phdata->tasknum);
    mxmlElementSetAttr(xml_root,TASKXML_NUMBER,number);
    while(p!=NULL)
    {
        memset(task_node_name,0,tnn_len);
        memset(id,0,id_len);

        sprintf(task_node_name,"task%d",i++);
        mxml_node_t *task_node=mxmlNewElement(xml_root,task_node_name);
        sprintf(id,"%d",p->id);
        mxmlElementSetAttr(task_node,TASKXML_ID,id);
        mxmlElementSetAttr(task_node,TASKXML_NAME,p->name);
        
        mxml_node_t *tasktip=mxmlNewElement(task_node,TASKXML_TASKTIP);
        mxmlNewText(tasktip,0,p->tasktip);
        mxml_node_t *level=mxmlNewElement(task_node,TASKXML_LEVEL);
        mxmlNewInteger(level,p->level);
        mxml_node_t *type=mxmlNewElement(task_node,TASKXML_TYPE);
        mxmlNewInteger(type,p->type);
        mxml_node_t *intelval=mxmlNewElement(task_node,TASKXML_INTELVAL);
        mxmlNewInteger(intelval,p->intelval);
        mxml_node_t *index=mxmlNewElement(task_node,TASKXML_INDEX);
        mxmlNewInteger(index,p->index);
        ctl_write(p,task_node);
        p=p->next;
    }
	return 0;	
}

//查找任务编号为id的监控任务内容，当id=-1时，返回所有任务链表
struct task_user *handle_taskquery(int id)
{
    mutex_lock(&phdata->mutex);
    struct task_user *p=phdata->task;

    while(p!=NULL)
    {
        if (p->id==id)
        {
            break;
        }
        p=p->next;
    }
    mutex_unlock(&phdata->mutex);
	return p;	
}

//删除一个监控任务
int handle_taskdel(int id)
{
    mutex_lock(&phdata->mutex);
    struct task_user *p=phdata->task;
    struct task_user *q=NULL;
    int flag=0;
    
    while(p!=NULL)
    {
        if (p->id==id)
        {
            break;
        }
        q=p;
        p=p->next;
    }
    if (p==NULL)
    {
        flag=-1;
    }else
    {
        if (q==NULL)
        {
            phdata->task=NULL;
        }else
        {
            q->next=p->next;
        }
        free(p);
    }
    mutex_unlock(&phdata->mutex);
	return flag;
}

//更新监控任务
int handle_taskmod(struct task_user *task)
{
    mutex_lock(&phdata->mutex);
    struct task_user *p=phdata->task;
    int flag=-1;

    while(p!=NULL)
    {
        if (task->id==p->id)
        {
            *p=*task;
            flag=0;
        }
        p=p->next;
    }
    mutex_unlock(&phdata->mutex);
	return flag;
}

//添加一个任务
int handle_taskadd(struct task_user *task)
{
    mutex_lock(&phdata->mutex);
    struct task_user *p=phdata->task;
    
    if (p==NULL)
    {
        phdata->task=task;
    }
    else
    {
        while(p->next!=NULL)
        {
            p=p->next;
        }
        p->next=task;
    }
    mutex_unlock(&phdata->mutex);
	return 0;
}

//根据动态库支持，更新任务文件，去掉不支持的监控任务项
int handle_taskupdate(void)
{

	return 0;
}

//解析监控任务文件（task.xml）
static int parse_task_file(mxml_node_t *root)
{
	struct task_user *taskuser=NULL;
	mxml_node_t *taskxml=NULL;
    mxml_node_t *node=NULL;
	mxml_node_t *tasktip=NULL;
	mxml_node_t *level=NULL;
	mxml_node_t *type=NULL;
	mxml_node_t *intelval=NULL;
	mxml_node_t *index=NULL;
	mxml_node_t *ctlinf=NULL;
	const char *task_num=NULL;
    const char *id=NULL;
    const char *name=NULL;
    char task_node_name[20];
    int i=0;

    memset(task_node_name,0,sizeof(task_node_name));
	taskxml = mxmlFindElement(root, root, TASKXML_TASK,NULL, NULL,MXML_DESCEND);
	if (taskxml==NULL)
	{
		return -1;
	}
	task_num=mxmlElementGetAttr(taskxml,TASKXML_NUMBER);
	if (task_num==NULL)
	{
		return -1;
	}
	phdata->tasknum=atoi(task_num);

	for (i = 0; i <phdata->tasknum; ++i)
	{
		sprintf(task_node_name,"task%d",i+1);
		node=mxmlFindElement(taskxml, root, task_node_name,NULL, NULL,MXML_DESCEND);
		tasktip=mxmlFindElement(node,root,TASKXML_TASKTIP,NULL,NULL,MXML_DESCEND);
		level=mxmlFindElement(node,root,TASKXML_LEVEL,NULL,NULL,MXML_DESCEND);
		type=mxmlFindElement(node,root,TASKXML_TYPE,NULL,NULL,MXML_DESCEND);
		intelval=mxmlFindElement(node,root,TASKXML_INTELVAL,NULL,NULL,MXML_DESCEND);
		index=mxmlFindElement(node,root,TASKXML_INDEX,NULL,NULL,MXML_DESCEND);
		ctlinf=mxmlFindElement(node,root,TASKXML_CTLINF,NULL,NULL,MXML_DESCEND);
        id=mxmlElementGetAttr(node,TASKXML_ID);
        name=mxmlElementGetAttr(node,TASKXML_NAME);

        if (node==NULL||tasktip==NULL||level==NULL||type==NULL||intelval==NULL
        	||index==NULL||ctlinf==NULL||id==NULL||name==NULL)
        {
        	return -1;
        }

        taskuser=(struct task_user*)calloc(1,sizeof(struct task_user));
        taskuser->id=atoi(id);
        taskuser->level=atoi(level->child->value.text.string);
        taskuser->type=atoi(type->child->value.text.string);
        taskuser->intelval=atoi(intelval->child->value.text.string);
        taskuser->index=atoi(index->child->value.text.string);
        strcpy(taskuser->tasktip,tasktip->child->value.text.string);
        strcpy(taskuser->name,name);
        if (parse_ctlinf(root,ctlinf,taskuser)==-1)
        {
        	free(taskuser);
        	return -1;
        }
        tasklist_init(taskuser);
	}
    return 0;
}

//解析xml中的控制信息部分
static int parse_ctlinf(mxml_node_t *root,mxml_node_t *node,struct task_user *taskuser)
{
	int i=0;
	const char *number=NULL;
	char ctlinf_node_name[20];
	mxml_node_t *ctlinfxml_node=NULL;
	mxml_node_t *index=NULL;
	mxml_node_t *type=NULL;
	mxml_node_t *devno=NULL;
	mxml_node_t *sign=NULL;
	mxml_node_t *code=NULL;
	mxml_node_t *len=NULL;
	mxml_node_t *filename=NULL;

    memset(ctlinf_node_name,0,sizeof(ctlinf_node_name));
    number=mxmlElementGetAttr(node,CTLINFXML_NUMBER);
    if (number==NULL)
    {
    	return -1;
    }
    taskuser->ctlnum=atoi(number);

    taskuser->ctlinf=(struct ctl_inf*)calloc(taskuser->ctlnum,sizeof(struct ctl_inf));
    for (i = 0; i < taskuser->ctlnum; ++i)
    {
    	int t;

    	sprintf(ctlinf_node_name,"ctlinf%d",i+1);
        ctlinfxml_node=mxmlFindElement(node,root,ctlinf_node_name,NULL,NULL,MXML_DESCEND);
        type=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_TYPE,NULL,NULL,MXML_DESCEND);
        if (type==NULL)
        {
        	return -1;
        }
        index=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_INDEX,NULL,NULL,MXML_DESCEND);
        if (index==NULL)
        {
        	return -1; 
        }
        t=atoi(type->child->value.text.string);
        
        taskuser->ctlinf[i].index=atoi(index->child->value.text.string);
        taskuser->ctlinf[i].type=t;
        if (t>5)
        {
            devno=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_DEVNO,NULL,NULL,MXML_DESCEND);
            sign=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_SIGN,NULL,NULL,MXML_DESCEND);
            code=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_CODE,NULL,NULL,MXML_DESCEND);
            len=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_LEN,NULL,NULL,MXML_DESCEND);

            if (ctlinfxml_node==NULL||index==NULL||type==NULL||devno==NULL||sign==NULL
	    	||code==NULL||len==NULL)
	        {
	        	free(taskuser->ctlinf);
	            return -1;
	        }
	        taskuser->ctlinf[i].info.devctl.devno=atoi(devno->child->value.text.string);
	        taskuser->ctlinf[i].info.devctl.sign=atoi(sign->child->value.text.string);
	        taskuser->ctlinf[i].info.devctl.len=atoi(len->child->value.text.string);
	        strcpy(taskuser->ctlinf[i].info.devctl.code,code->child->value.text.string);
        }else if (t<=5&&t>=0)
        {
        	filename=mxmlFindElement(ctlinfxml_node,root,CTLINFXML_FILENAME,NULL,NULL,MXML_DESCEND);
        	if (filename==NULL)
        	{
        		free(taskuser->ctlinf);
        		return -1;
        	}
        	strcpy(taskuser->ctlinf[i].info.filename,filename->child->value.text.string);
        } 
    }
	return 0;
}

//监控任务链表初始化
static int tasklist_init(struct task_user *node)
{
	struct task_user *p=NULL;
    
    p=phdata->task;

    if (p==NULL)
    {
    	phdata->task=node;
    }else
    {
    	while(p->next!=NULL)
    	{
    		p=p->next;
    	}
    	p->next=node;
    }

	return 0;
}

static int ctl_write(struct task_user *ptask,mxml_node_t *task_node)
{
    char number[10];
    char ctl_node_name[20];
    size_t cnn_len=0;
    int i=0;

    cnn_len=sizeof(ctl_node_name);
    memset(number,0,sizeof(number));

    sprintf(number,"%d",ptask->ctlnum);
    mxml_node_t *ctl_inf=mxmlNewElement(task_node,TASKXML_CTLINF);
    mxmlElementSetAttr(ctl_inf,CTLINFXML_NUMBER,number);

    for (i = 0; i < ptask->ctlnum; ++i)
    {
        memset(ctl_node_name,0,cnn_len);
        sprintf(ctl_node_name,"ctlinf%d",i+1);
        mxml_node_t *ctl_node=mxmlNewElement(ctl_inf,ctl_node_name);

        mxml_node_t *index=mxmlNewElement(ctl_node,CTLINFXML_INDEX);
        mxmlNewInteger(index,ptask->ctlinf->index);
        mxml_node_t *type=mxmlNewElement(ctl_node,CTLINFXML_TYPE);
        mxmlNewInteger(type,ptask->ctlinf->type);
        if (ptask->ctlinf->type>=0&&ptask->ctlinf->type<=5)
        {
            mxml_node_t *filename=mxmlNewElement(ctl_node,CTLINFXML_FILENAME);
            mxmlNewText(filename,0,ptask->ctlinf->info.filename);
        }
        if (ptask->type>=6&&ptask->type<=10)
        {
            mxml_node_t *devno=mxmlNewElement(ctl_node,CTLINFXML_DEVNO);
            mxmlNewInteger(devno,ptask->ctlinf->info.devctl.devno);
            mxml_node_t *sign=mxmlNewElement(ctl_node,CTLINFXML_SIGN);
            mxmlNewInteger(sign,ptask->ctlinf->info.devctl.sign);
            mxml_node_t *len=mxmlNewElement(ctl_node,CTLINFXML_LEN);
            mxmlNewInteger(len,ptask->ctlinf->info.devctl.len);
            mxml_node_t *code=mxmlNewElement(ctl_node,CTLINFXML_CODE);
            mxmlNewText(code,0,ptask->ctlinf->info.devctl.code);
        }
    }
    return 0;
}