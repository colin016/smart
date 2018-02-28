#include "dev.h"

int same_media_device_add(struct media_if *med,struct ext_dev *device_node)
{
  struct ext_dev *p;
  
  if(med==NULL)
  {
    fprintf(stderr, "dev_same_media_man.c line %d:med==NULL\n",__LINE__);
    return -1;
  }

  p=med->list;
  if(p==NULL)
  {
    med->list=device_node;
  }
  else
  {
    while(p->ifnext!=NULL)
    {
      p=p->ifnext;
    }
    p->ifnext=device_node;
  }
  return 0;
}

int same_media_device_delete(struct media_if *med,int devid)
{
  struct ext_dev *p=NULL;
  struct ext_dev *pre=NULL;
  int flag=0;

  if(med==NULL)
  {
    fprintf(stderr, "Line %d:med==NULL\n",__LINE__ );
    return 0;
  }
  p=med->list;
  if(p==NULL)
  {
    flag=-1;
  }
  else
  {
    while(p!=NULL)
    {
      if(p->devid==devid)
        break;
      pre=p;
      p=p->ifnext;
    }

    if(p==NULL)
    {
      flag=-1;
    }
    else
    {
      if(pre==NULL)
        med->list=p->ifnext;
      else
        pre->ifnext=p->ifnext;
    }
  }

  return flag;
}

int same_media_device_clear(struct media_if *med)
{
  struct ext_dev *p=med->list;
  struct ext_dev *temp=NULL;

  while(p!=NULL)
  {
    temp=p->ifnext;
    free(temp);
    p=temp;
  }

  return 0;
}
