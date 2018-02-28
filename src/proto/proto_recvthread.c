#include "proto.h"

extern struct proto_data *protodata;

//读取数据
static void doRead(void);
//接收的字符串转化为报文
static void str_to_pkt(struct sockaddr_in souraddr, char *msg, int length);

//启动网络数据接收线程
void *proto_nethandle(void *arg) {
	doRead();
	return NULL;
}

static void doRead(void) {
	int num = 0;
	socklen_t length = 0;
	char buffer[DATA_MAXLEN];
	struct sockaddr_in clientaddr;

	length = sizeof(struct sockaddr);
	while (1) {
		num = 0;
		memset(buffer, 0, sizeof(buffer));
		num = recvfrom(protodata->sockfd, buffer, DATA_MAXLEN, MSG_WAITALL,
				(struct sockaddr *) &clientaddr, &length);
		if (num == 0) {
			printf("client closed\n");
		} else if (num < 0) {
			perror("recvfrom");
			close(protodata->sockfd);
			return;
		} else {
			printf("received_len=%d\n",num);
			str_to_pkt(clientaddr, buffer, num);
			// printf("received:");
			// puts(buffer);
		}
	}
}

//接收的字符串转化为报文
static void str_to_pkt(struct sockaddr_in souraddr, char *msg, int length) {
	char err;
	char pkt_head[5];
	pkt_inf pkt;
	int sid;
	int seq;
	int ack;

	if (length < 20) {
		PLOG("packet too short!");
		return;
	}
	memset(pkt_head, 0, sizeof(pkt_head));
	memcpy(pkt_head, msg, 4);
	pkt.buff = msg;
	pkt.len = length;

	pkt.recvtime = time(NULL);
	/*
     *这里原地址和目的地址都是同一个，因为后面协代码的没有该过来
	*/
	pkt.souraddr = souraddr;
	pkt.peeraddr = souraddr;

	if (strcmp(pkt_head, PROTO_BASETAG) != 0) {
		PLOG("packet is incomplete!");
		return;
	}

	pkt.basehead.cmd = msg[4];
	pkt.basehead.type = msg[5];
	pkt.basehead.opt = msg[6];
	pkt.basehead.sort = msg[7];
	memcpy(&sid, msg + 8, 4);
	memcpy(&seq, msg + 12, 4);
	memcpy(&ack, msg + 16, 4);

	pkt.basehead.sid = ntohl(sid);
	pkt.basehead.sendseq = ntohl(seq);
	pkt.basehead.ackseq = ntohl(ack);
	int i;
	PLOG("recv basehead Hex:");
	for (i = 0; i < 20; i++) {
		printf("%x ", msg[i]);
	}
	puts("");

	PLOG("recv cmd:%d\n", pkt.basehead.cmd);
	PLOG("recv type:%d\n", pkt.basehead.type);
	PLOG("recv opt:%d\n", pkt.basehead.opt);
	PLOG("recv sort:%d\n", pkt.basehead.sort);
	PLOG("recv sid:%d\n", pkt.basehead.sid);
	PLOG("recv seq:%d\n", pkt.basehead.sendseq);
	PLOG("recv ack:%d\n", pkt.basehead.ackseq);

	//PLOG("recv datajson:%s\n", msg + DATA_HEADLEN);

	if ((err = proto_pkt_base_parse(&pkt)) == false) {
		return;
	} else if (err == abandon) {
		return;
	}
}
