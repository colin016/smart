#include "dev.h"

extern struct dev_data *devdata;

//添加一个新的IO到事件处理线程中
int dev_addevent(struct media_if *medinf)
{
	struct epoll_event event;
	event.data.fd=medinf->fd;
	event.events=EPOLLIN|EPOLLET;

	if(epoll_ctl(devdata->epoll_fd, EPOLL_CTL_ADD,medinf->fd, &event)==-1)
		return -1;

	return 0;
}

//从事件处理线程中删除一个接口
int dev_delevent(struct media_if *medinf)
{
	struct epoll_event event;
	event.data.fd=medinf->fd;
	event.events=EPOLLIN|EPOLLET;

	if(epoll_ctl(devdata->epoll_fd, EPOLL_CTL_DEL,medinf->fd, &event)==-1)
		return -1;

	return 0;
}
