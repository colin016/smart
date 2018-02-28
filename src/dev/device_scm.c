#include "dev.h"

//设备控制函数，针对已有设备类型，定义一个通用操作函数。input是输入指令；output是输出信息；在实现过程中，每个基本设备定义有type和input。
int scm_control(struct ext_dev *p, struct reqctl *req, struct devctl_out *output)
{
	struct devctl_inf *ctl_node=NULL;
	int ret;

	if (p==NULL||req==NULL||output==NULL)
	{
		return -1;
	}

    ret=p->media->send(p->media,p,req);
    
    if(ret>=0)
    {
	    ctl_node=calloc(1,sizeof(struct devctl_inf));
	    if (ctl_node==NULL)
	    {
	    	return -1;
	    }
	    ctl_node->devid=p->devid;
	    ctl_node->time=time(NULL);
	    ctl_node->seq=req->seq;
	    if(sem_init(&ctl_node->sem,0,0)!=0)
	    {
	       free(ctl_node);
	       return -1;
	    }
	    ctl_node->output=output;
	    if(devctl_inf_add(ctl_node)==-1)
	    {
	      PLOG("add \"struct devctl_inf\" node failed!\n");
	      free(ctl_node);
	      return -1;
	    }
	    if(sem_wait(&ctl_node->sem)!=0)
        {
    	    return -1;
        }
        devctl_inf_delete(req->seq);
        return 0;
    }

	return -1;
}

int scm_init(struct ext_dev *extdev)//初始化函数指针
{
	extdev->init=scm_init;
	extdev->control=scm_control;
	extdev->close=scm_close;
	extdev->keepalive=scm_keepalive;
	extdev->recvtime=0;
	if(extdev->media!=NULL&&extdev->media->status==LIVE)
	{
	    extdev->status=LIVE;
    }else
    {
    	extdev->status=CLOSE;
    }
	return 0;
}

int scm_close(struct ext_dev *p) //关闭函数指针
{
	devctlcmd_clear(p);
	if(p->data!=NULL)
        free(p->data);
	return 0;
}

int scm_keepalive(struct ext_dev *p)//保活函数指针
{
   return 0;
}
