#include "dev.h"

extern struct dev_data *devdata;	//引用模块变量

int device_list_init(struct ext_dev *node)
{
	struct ext_dev *temp=devdata->devtable.dev_list;
	node->next=NULL;

	if(devdata->max_device_num>devdata->devtable.size)
	{
		if(temp==NULL)
		{
			devdata->devtable.dev_list=node;
			temp=node;
			devdata->devtable.size+=1;
		}
		else
		{
			//找到链表的尾节点
			while (temp->next!=NULL)
			{
				temp=temp->next;
			}

			temp->next=node;
			temp=node;
			devdata->devtable.size+=1;
		}

		if(same_media_device_add(node->media,node)==-1)
			return -1;
	}
	else
	{
		return -1;
	}
	return 0;
}
//添加一个设备
int device_add(struct ext_dev *node)
{
	mutex_devicelist_lock(&devdata->devtable);
	struct ext_dev *temp=devdata->devtable.dev_list;
	int flag=0;

	node->next=NULL;

	if(devdata->max_device_num>devdata->devtable.size)
	{
		if (temp==NULL)
		{
				devdata->devtable.dev_list=node;
				devdata->devtable.size+=1;
		}
		else
		{
			while (temp->next!=NULL)
			{
				temp=temp->next;
			}
			temp->next=node;
			devdata->devtable.size+=1;
		}
		if(same_media_device_add(node->media,node)==-1)
			flag= -1;
	}
	else
	{
		flag= -1;
	}
	mutex_devicelist_unlock(&devdata->devtable);
	return flag;
}

//删除一个设备
int device_delete(int devid)
{
	mutex_devicelist_lock(&devdata->devtable);
	struct ext_dev *temp=devdata->devtable.dev_list;
	struct ext_dev *pre=NULL;
	int flag=0;

	if(devdata->devtable.dev_list==NULL&&devdata->devtable.size<=0)
	{
		flag= -1;
	}
	else
	{
		while (temp!=NULL)
		{
			if(temp->devid==devid)
				break;
			pre=temp;
			temp=temp->next;
		}

		if(temp==NULL)
		{
			flag= -1;
		}
		else
		{
			if(pre==NULL)
					devdata->devtable.dev_list=temp->next;
			else
					pre->next=temp->next;
			if(same_media_device_delete(temp->media,temp->devid)==-1)
				flag= -1;
			
			temp->close(temp);
			free(temp);
			devdata->devtable.size-=1;
		}
	}
	mutex_devicelist_unlock(&devdata->devtable);
	return flag;
}

//更新一个设备表内容
int device_update(struct ext_dev *node)
{
	mutex_devicelist_lock(&devdata->devtable);
	struct ext_dev *p=devdata->devtable.dev_list;
	struct ext_dev temp;
	int flag=-1;
	while (p!=NULL)
	{
		if(p->devid==node->devid)
		{
			temp=*p;
			*p=*node;
			p->next=temp.next;
			p->ifnext=temp.ifnext;
			flag= 0;
		}
		p=p->next;
	}
	mutex_devicelist_unlock(&devdata->devtable);
	return flag;
}

//释放所有的设备表信息
int device_clear(void )
{
	mutex_devicelist_lock(&devdata->devtable);
	struct ext_dev *q=NULL;

	while (devdata->devtable.dev_list!=NULL)
	{
		
		q=devdata->devtable.dev_list;
		devdata->devtable.dev_list=devdata->devtable.dev_list->next;
		q->close(q);
		free(q);
	}

	mutex_devicelist_unlock(&devdata->devtable);
	return 0;
}

struct ext_dev *device_get(int id)
{
	mutex_devicelist_lock(&devdata->devtable);
	struct ext_dev *dev_p=devdata->devtable.dev_list;

    if (devdata->devtable.size<=0)
    {
    	return NULL;
    }

	while(dev_p!=NULL)
	{
		if(dev_p->devid==id)
			break;
		dev_p=dev_p->next;
	}
	mutex_devicelist_unlock(&devdata->devtable);
	return dev_p;
}
