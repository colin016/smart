#include "dev.h"
extern struct dev_data *devdata;

static void pkg_print(struct device_proto *pkg,int length);

int serial_control(struct media_if *med_inf, int type, void *input, void *output)  //介质控制函数。
{
	switch (type)
	{
		case 1:
		break;
	}
	return 0;
}

int serial_init(struct media_if *med_inf) //初始化函数指针
{
	med_inf->init=serial_init;
	med_inf->control=serial_control;
	med_inf->send=serial_send;
	med_inf->recv=serial_recv;
	med_inf->check=serial_check;
	med_inf->close=serial_close;
	med_inf->len=0;
	
	med_inf->buf=(char*)malloc(1024);
	memset(med_inf->buf,0,1024);
	if (serial_open(med_inf)==-1)
	{
		return -1;
	}
	med_inf->status=LIVE;
	return 0;
}

int serial_close(struct media_if *med_inf) //关闭函数指针
{
	close(med_inf->fd);
	if (med_inf->buf!=NULL)
		free(med_inf->buf);
	if(med_inf->para!=NULL)
		free(med_inf->para);
	return 0;
}

int serial_check(struct media_if *med_inf)//介质可用性检查函数指针
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

int serial_send(struct media_if *med_inf, struct ext_dev *p_dev,struct reqctl *req)//介质发送信息函数
{
	struct serial_proto pkt;
	char buff[PKT_MAXSIZE];
	size_t pktlen=0;
	int i=0;
	short checksum=0;
	short head=0x7E;

	memset(&pkt,0,sizeof(pkt));
    pkt.dst=p_dev->devid;
    pkt.src=0;//0表示上位机
    pkt.next=0;//0表示不制定下一站编号（上位机不用制定下一站）
    pkt.sign=req->sign;//控制编号
    pkt.seq=0;//暂不使用报文序号
    pkt.len=req->len;//数据长度
    memcpy(pkt.data,req->data,req->len);

    pktlen=sizeof(pkt)-DATA_SIZE+pkt.len;
    memcpy(buff,&head,2);
    memcpy(buff+2,&pkt,pktlen);

	for (i = 0; i <pktlen; i++)
	{
		checksum+=buff[i+2];
	}
    memcpy(buff+2+pktlen, &checksum, 2);

	fprintf(stdout,"serial send:");
	for(int j=0;j<pktlen+4;j++)
		fprintf(stdout,"%02x ",(unsigned char)buff[j] );
	fprintf(stdout,"\n");
	return write(med_inf->fd,buff,pktlen+4);
}

int serial_recv(struct media_if *med_inf, char *buf, int len)//接收处理函数,实现有关处理协议及子接口控制
{
	static struct device_proto pkt;
	static int recvlen=0;
	static char esctag=0;
	static char pkttag=0;
	static char pkt_buff[1024];
    int i=0;

    for (i = 0; i < len; ++i)
    {
    	if (pkttag==0)
	    {
	    	memset(&pkt,0,sizeof(pkt));
	    	memset(pkt_buff,0,sizeof(pkt_buff));
	    }
    	if (recvlen==pkt.proto.serial.len+10)//数据长度加上固定长度再加校验和长度等于报文长度（除协议头和校验和）
    	{
    		//计算校验和
    		int cal_sum=0;
    		int rel_sum=0;
    		int j=0;
            PLOG("recvice a packet...\n");
    		memcpy(&rel_sum,pkt_buff+recvlen-2,2);
    		for (j=0;j<recvlen-2;j++)
			{
				cal_sum+=*(buf+j);
			}
			if (cal_sum==rel_sum)
			{
				pkt.type=PKT_TYPE_SERIAL;
				memcpy(&pkt.proto.serial,pkt_buff,recvlen-2);
				pkg_print(&pkt,recvlen-2);
				proto_recv(&pkt);
			}
    		recvlen=0;
	        esctag=0;
	        pkttag=0;
    	}

    	switch(buf[i])
    	{
    		case 0x7E:
	    		if (pkttag==0)
	    		{
	    			pkttag=1;
	    		}else
	    		{
	    			esctag=1;
	    		}
    		    break;
    		
    		default:
    		    if (pkttag!=0)    		    {
    			    if (esctag==1)
		            {
		    	        pkt_buff[recvlen]=buf[i]+2;
		    	        esctag=0;
		            }else
		            {
		               pkt_buff[recvlen]=buf[i];
		               recvlen+=1;
		               if (recvlen==8)
		               { 
		                   memcpy(&pkt.proto.serial.len,pkt_buff+7,2);
		                   pkttag=2;
		               }
		            }
    		    }
    		    break;
    	}
    }

	return 0;
}

static void pkg_print(struct device_proto *pkg,int length)
{
	int j=0;
	char buf[20];
	memcpy(buf,&pkg->proto.serial,length);
	printf("dev recv:");
	for(j=0;j<length;j++)
		printf("%02x ",buf[j] );
	puts("");
}
