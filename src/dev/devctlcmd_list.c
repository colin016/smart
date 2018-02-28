#include "dev.h"


static void print_cmd(struct devctl_cmd *pnode,char isshow);

struct devctl_cmd* devctlcmd_add(struct devctl_cmd *head,struct devctl_cmd *pnode)
{
	struct devctl_cmd *phead=NULL;

    phead=head;
    pnode->next=NULL;

    if (phead==NULL)
    {
    	head=pnode;
    }else
    {
    	while(phead->next!=NULL)
    	{
    		phead=phead->next;
    	}
    	phead->next=pnode;
    }
	print_cmd(pnode,0);
    return head;
}

int devctlcmd_clear(struct ext_dev *pdev)
{
    struct devctl_cmd *p=NULL;

    while(pdev->devctlcmd!=NULL)
    {
    	p=pdev->devctlcmd;
    	pdev->devctlcmd=pdev->devctlcmd->next;
    	free(p);
    }

	return 0;
}

static void print_cmd(struct devctl_cmd *pnode,char isshow)
{
	int i=0;
    if (isshow==0)
    {
    	return;
    }
	if (pnode->type==1)
	{	
        printf("attachment's name:%s\n",pnode->ctl.attachment.attach_name);
        printf("attachment's bigtype:%d\n",pnode->ctl.attachment.bigtype );
        printf("attachment's smalltype:%d\n",pnode->ctl.attachment.smalltype);
        printf("attachment's number:%d\n",pnode->ctl.attachment.no);
	}

	for (i = 0; i < pnode->ctl.attachment.cmd_num; ++i)
	{
		printf("%s命令%d:%s\n",pnode->ctl.attachment.attach_name,pnode->ctl.attachment.cmd[i].ctl_no,pnode->ctl.attachment.cmd[i].tip );
	}
}