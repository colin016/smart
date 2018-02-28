#include "dev.h"
#include "devshare.h"

extern struct dev_data *devdata;	//引用模块变量

int devctl_inf_create(int sendid,void *input,struct devctl_inf *node)
{
  if(node==NULL||input==NULL)
    return -1;

  struct devctl_in *p=(struct devctl_in*)input;
  if(sem_init(&node->sem,0,0)!=0)
    return -1;

  time(&node->time);
  node->devid=p->devid;
  node->requestid=sendid;

  return 0;
}

int devctl_inf_add(struct devctl_inf *node)
{
  mutex_ctlinflist_lock(&devdata->ctltable);
  struct devctl_inf *p=devdata->ctltable.ctl_list;

	node->next=NULL;
	if (p==NULL)
	{
			devdata->ctltable.ctl_list=node;
	}
  else
  {
  	while (p->next!=NULL)
  	{
  		p=p->next;
  	}
  	p->next=node;
  }
  devdata->ctltable.size+=1;

  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

int devctl_inf_delete(int seq)
{
  mutex_ctlinflist_lock(&devdata->ctltable);
  struct devctl_inf *cur=devdata->ctltable.ctl_list;
	struct devctl_inf *pre=NULL;
  int flag=0;

	if(devdata->ctltable.ctl_list==NULL)
		return -1;

	while (cur!=NULL)
	{
		if(cur->seq==seq)
			break;
		pre=cur;
		cur=cur->next;
	}

	if(cur==NULL)
	{
		flag=-1;
	}
	else
	{
		if(pre==NULL)
				devdata->ctltable.ctl_list=cur->next;
		else
				pre->next=cur->next;
    PLOG("Delete sendseq=%d packet!\n",cur->seq);
		free(cur);
    devdata->ctltable.size-=1;
	}
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return flag;
}

struct devctl_inf *devctl_inf_get(unsigned short seq)
{
  struct devctl_inf *p=NULL;
  p=devdata->ctltable.ctl_list;

  while(p!=NULL)
  {
    if(p->seq==seq)
      break;
    p=p->next;
  }
  return p;
}

int devctl_inf_clear(void)
{
  mutex_ctlinflist_lock(&devdata->ctltable);
  struct devctl_inf *p=NULL;
  p=devdata->ctltable.ctl_list;

  while(devdata->ctltable.ctl_list!=NULL)
  {
    p=devdata->ctltable.ctl_list;
    devdata->ctltable.ctl_list=devdata->ctltable.ctl_list->next;
    free(p);
  }
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}