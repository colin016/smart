#include "dev.h"

extern struct dev_data *devdata;	//引用模块变量
//介质链表的初始化
int media_list_init(struct media_if *node)
{
	struct media_if *temp=devdata->medtable.med_list;
	node->next=NULL;
    
    if(devdata->medtable.size>=devdata->max_media_num)
    {
    	fprintf(stdout,"介质链表已满...\n");
    	return -1;
    }
	if(node!=NULL)
	{
		dev_addevent(node);
	}

	if(temp==NULL)
	{
		devdata->medtable.med_list=node;
		temp=node;
		devdata->medtable.size+=1;
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
		devdata->medtable.size+=1;
	}

	return 0;
}

//添加一个介质
int media_add(struct media_if *node)
{
	mutex_medialist_lock(&devdata->medtable);
	struct media_if *temp=devdata->medtable.med_list;
	int flag=0;

    if(devdata->medtable.size>=devdata->max_media_num)
    {
    	fprintf(stdout,"介质链表已满...\n");
    	return -1;
    }
	if(node==NULL)
	{
		return -1;
	}

	if (temp==NULL)
	{
			devdata->medtable.med_list=node;
			devdata->medtable.size+=1;
	}
	else
	{
			while (temp->next!=NULL)
			{
				temp=temp->next;
			}
			temp->next=node;
			devdata->medtable.size+=1;
	}
	mutex_medialist_unlock(&devdata->medtable);
	return flag;
}

//删除一个介质>>>>>>>>>>>>>>>>>>>>>>>
int media_delete(char id)
{
	mutex_medialist_lock(&devdata->medtable);
	struct media_if *temp=devdata->medtable.med_list;
	struct media_if *pre=NULL;
	int flag=0;

	if(devdata->medtable.med_list==NULL)
		flag=-1;
	else
	{
		while (temp!=NULL)
		{
			if(temp->id==id)
				break;
			pre=temp;
			temp=temp->next;
		}

		if(temp==NULL)
		{
			flag=-1;
		}
		else
		{
			if(pre==NULL)
					devdata->medtable.med_list=temp->next;
			else
					pre->next=temp->next;
			if(dev_delevent(temp)==-1)
				flag=-1;
			if(same_media_device_clear(temp)==-1)
				return -1;
			temp->close(temp);
			free(temp);
			devdata->medtable.size-=1;
		}
	}
	mutex_medialist_unlock(&devdata->medtable);
	return flag;
}

//更新一个介质表内容
int media_update(struct media_if *node)
{
	mutex_medialist_lock(&devdata->medtable);
	struct media_if *temp=devdata->medtable.med_list;
	struct ext_dev *dev_list=NULL;
	struct media_if  *med_next=NULL;
	int flag=-1;

	while (temp!=NULL)
	{
		if(temp->id==node->id)
		{
			dev_list=temp->list;
			med_next=temp->next;
			*temp=*node;
			temp->list=dev_list;
			temp->next=med_next;
			flag=0;
			break;
		}
		temp=temp->next;
	}
	mutex_medialist_unlock(&devdata->medtable);
	return flag;
}

//释放所有的介质表信息
int media_clear(void)
{
	mutex_medialist_lock(&devdata->medtable);
	struct media_if *q=NULL;

	while (devdata->medtable.med_list!=NULL)
	{
		q=devdata->medtable.med_list;
		devdata->medtable.med_list=devdata->medtable.med_list->next;
		devdata->medtable.size-=1;
		q->close(q);
		free(q);
		
	}
	mutex_medialist_unlock(&devdata->medtable);
	return 0;
}

struct media_if *media_get(char id)
{
	mutex_medialist_lock(&devdata->medtable);
    struct media_if *head=devdata->medtable.med_list;

    while (head!=NULL)
    {
      if(head->id==id)
        break;
      head=head->next;
    }
	mutex_medialist_unlock(&devdata->medtable);
    return head;
}
