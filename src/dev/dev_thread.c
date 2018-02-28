#include "dev.h"

extern struct dev_data *devdata;

static int net_event(struct media_if *p);

void *dev_recvhandle(void *arg)
{
	struct media_if *p=NULL;
	char buf[512];
	size_t buf_len;
	int each_recvlen=0;
	int len=0;
	int n=0;
	int i=0;

	buf_len=sizeof(buf);
	while(1)
	{
		
		pthread_testcancel();

		n=epoll_wait(devdata->epoll_fd,devdata->events,MAXEVENTS,-1);
		mutex_medialist_lock(&devdata->medtable);

		for (i = 0; i < n; i++)
		{
		    p=devdata->medtable.med_list;

			while (p!=NULL)
			{
				if(p->fd==devdata->events[i].data.fd)
				{
					bzero(buf,sizeof(buf));

					switch(p->type)
	                {
	                	case MEDIA_ETHERNET:
	                	case MEDIA_WIFI:
                        net_event(p);
	                	break;

                        case MEDIA_ZIGBEE:
	                	case MEDIA_SERIAL:
		                while((each_recvlen=read(p->fd,buf,buf_len))>0)
						{
							len+=each_recvlen;
						}

						if(len==0||len<0)
						{
							break;
						}
						else
						{
							PLOG("recv:");
							for (int i = 0; i < len; ++i)
							{
								printf("%02x ",(unsigned char)buf[i] );
							}
							puts("");
							p->recv(p,buf,len);
						}

						len=0;
	                	break;
	                }
	               break;
				}
				p=p->next;
			}
		}
		mutex_medialist_unlock(&devdata->medtable);
	}
	return 0;
}


static int net_event(struct media_if *p)
{
	socklen_t addr_len;
	struct sockaddr_in addr;
	char buf[512];
	size_t buf_len;
	int len=0;

    memset(&addr,0,sizeof(addr));
    buf_len=sizeof(buf);
	addr_len=sizeof(struct sockaddr);

	len=recvfrom(p->fd,buf,buf_len,0,(struct sockaddr*)&addr,&addr_len);
	if(len<=0)
	{
		return -1;
	}else
	{
		p->addr=&addr;
		p->recv(p,buf,len);
	}
	return 0;
}