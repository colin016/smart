#include "dev.h"
extern struct dev_data *devdata;


int zigbee_control(struct media_if *med_inf, int type, void *input, void *output)  //介质控制函数。
{
	switch (type)
	{
		case 1:
		break;
	}
	return 0;
}

int zigbee_init(struct media_if *med_inf) //初始化函数指针
{
	med_inf->init=zigbee_init;
	med_inf->control=zigbee_control;
	med_inf->send=zigbee_send;
	med_inf->recv=zigbee_recv;
	med_inf->check=zigbee_check;
	med_inf->close=zigbee_close;

	if (serial_open(med_inf)==-1)
	{
		return -1;
	}
	med_inf->status=LIVE;
	return 0;
}

int zigbee_close(struct media_if *med_inf) //关闭函数指针
{
	close(med_inf->fd);
	if (med_inf->buf!=NULL)
		free(med_inf->buf);
	if(med_inf->para!=NULL)
		free(med_inf->para);
	return 0;
}

int zigbee_check(struct media_if *med_inf)//介质可用性检查函数指针
{
    if(med_inf==NULL)
	{
		return -1;
	}
	if(med_inf->status==CLOSE)
	{
		return -1;
	}
	return 0;
}

int zigbee_send(struct media_if *med_inf,struct ext_dev *p_dev,struct reqctl *req)//介质发送信息函数
{
	struct scm_private *para=NULL;
	struct zigbee_proto pkt;
	char buff[PKT_MAXSIZE];
	size_t pktlen=0;

	memset(&pkt,0,sizeof(pkt));
    memset(buff,0,sizeof(buff));

    para=(struct scm_private *)p_dev->data;
	pkt.sign=req->sign;//控制码
	pkt.seq=req->seq;//报文序号
	memcpy(pkt.data,req->data,req->len);//控制参数

    pktlen=sizeof(struct zigbee_proto)-DATA_SIZE+req->len-1;
	buff[0]=0xfe;//包头
	buff[1]=pktlen+4;//数据长度
	buff[2]=0x90;
	buff[3]=0x90;
	memcpy(buff+4,&para->private.zig.addr,2);
	memcpy(buff+6,&pkt.sign,1);
	memcpy(buff+6+1,&pkt.seq,2);
    memcpy(buff+6+3,pkt.data,req->len);
	buff[pktlen+6]=0xff;

    fprintf(stdout,"zigbee send:");
	for(int j=0;j<=pktlen+6;j++)
		fprintf(stdout,"%02x ",(unsigned char)buff[j] );
	fprintf(stdout,"\n");

	return write(med_inf->fd,buff,pktlen+7);
}

int zigbee_recv(struct media_if *med_inf, char *buf, int len)//接收处理函数,实现有关处理协议及子接口控制
{
	static struct device_proto pkt;
	static int recvlen=0;
	static int rellen=0;
	static char pkttag=0;
	static char esctag=0;
	static char data[100];
    int i=0;

    for (i = 0; i <len; ++i)
    {
     	switch((unsigned char)buf[i])
	    {
			case 0xff://收到结束符
				if(recvlen ==rellen) //收到一个完整的zigbee帧
				{
					pkt.type=PKT_TYPE_ZIGBEE;
					pkt.pktlen=recvlen-4;
					pkt.proto.zigbee.sign=data[4];
                    PLOG("recvice a packet\n");
					memcpy(&pkt.proto.zigbee.seq,data+5,2);
					//pkt.proto.zigbee.seq=ntohs(pkt.proto.zigbee.seq);
					memcpy(&pkt.proto.zigbee.data,data+7,pkt.pktlen);
					proto_recv(&pkt);
				}
				pkttag = 0; //一切重新开始
				recvlen = 0;
				esctag = 0;
				rellen=0;
				return 0;
			break;
			
			case 0xfe://收到转义字符
				if(pkttag == 0) //初始状态
				{
					memset(data,0,sizeof(data));
					memset(&pkt,0,sizeof(pkt));
					pkttag = 1; //开始接收
				}
				else
					esctag = 1; 	//是转意字符
			break;
			
			default://收到一般数据
				if(pkttag == 1)//如果是长度字节
				{
					rellen = buf[i];
					pkttag = 2;
				}
				else //已经收到长度字节
				{
					if(esctag)
					{
						data[(int)recvlen]=buf[i]+2;
						esctag = 0;
					}
					else
						data[(int)recvlen] = buf[i];
					
					recvlen++;
				}
			break;
	   }
    }
	
	return 0;
}
