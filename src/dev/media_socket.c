#include "dev.h"

extern struct dev_data *devdata;

static inline int open_socket();

int net_init(struct media_if *med_inf) //初始化函数指针
{
	med_inf->init=net_init;
	med_inf->control=net_control;
	med_inf->send=net_send;
	med_inf->recv=net_recv;
	med_inf->check=net_check;
	med_inf->close=net_close;
	med_inf->len=0;
    med_inf->fd=open_socket(med_inf);
    if(med_inf->fd<0)
    {
    	med_inf->status=CLOSE;
    	return -1;
    }
    med_inf->status=LIVE;			
	return 0;
}

int net_control(struct media_if *med_inf, int type, void *input, void *output)  //介质控制函数。
{

	return 0;
}

int net_close(struct media_if *med_inf) //关闭函数指针
{
	close(med_inf->fd);
	return 0;
}

int net_check(struct media_if *med_inf)//介质可用性检查函数指针
{

	return 0;
}

int net_send(struct media_if *med_inf, struct ext_dev *p_dev, struct reqctl *req)//介质发送信息函数
{
    struct sockaddr_in addr;
	struct scm_private scm;
	struct net_proto pkt;
	char buff[PKT_MAXSIZE];
	char head=0x7E;
	size_t pktlen=0;

	memset(&pkt,0,sizeof(pkt));
	memset(&addr,0,sizeof(addr));

	pkt.sign=req->sign;
	pkt.seq=0;//报文序号暂未使用
	pkt.len=req->len;
    memcpy(pkt.data,req->data,req->len);
    pktlen=sizeof(pkt)-DATA_SIZE+pkt.len;
    memcpy(buff,&head,1);
    memcpy(buff+2,&pkt,pktlen);

    fprintf(stdout,"net send:");
	for(int j=0;j<pktlen+2;j++)
		fprintf(stdout,"%02x ",(unsigned char)buff[j] );
	fprintf(stdout,"\n");

    scm=*(struct scm_private*)p_dev->data;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(scm.private.net.port);
    addr.sin_addr.s_addr=inet_addr(scm.private.net.ip);

	return sendto(med_inf->fd,buff,pktlen+2,0,(struct sockaddr*)&addr,sizeof(struct sockaddr));;
}

int net_recv(struct media_if *med_inf, char *buf, int len)//接收处理函数,实现有关处理协议及子接口控制
{
	struct device_proto pkt;
	int head=0;

    memset(&pkt,0,sizeof(pkt));
    memcpy(&head,buf,2);

	if (head!=0x7E)
	{
		fprintf(stderr, "Format of the packet is error\n");
		return -1;
	}
	
    pkt.type=PKT_TYPE_NET;
	memcpy(&pkt.proto.net,buf+2,len-2);
	proto_recv(&pkt);
	return 0;
}

static inline int open_socket(struct media_if *med)
{
    int fd=0;
    int port=0;
    struct sockaddr_in addr;

    if(med->para==NULL)
    {
    	return -1;
    }
    port=*(int*)med->para;
    addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

    fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0)
    {
    	return -1;
    }

	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		return -1;
	}
    return fd;
}