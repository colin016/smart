#include "proto.h"

extern struct proto_data *protodata;
static void to_json_specific(struct pkt_inf *pkt, void *data, cJSON *root);

//报文发送(isrepeat:1-->是否表示重发报文)
bool proto_sendmsg(int isrepeat, struct sockaddr_in *addr, struct pkt_inf *pkt,
		void *data) {
	char *pkt_basehead = NULL;
	char *pkt_jsondata = NULL;
	int length;
	int sockaddrlen;
	bool ret = true;
	//char buffer[DATA_MAXLEN]={0};

	if (pkt == NULL && pkt->buff == NULL) {
		return false;
	}

	sockaddrlen = sizeof(struct sockaddr_in);

	if (isrepeat == IS_REPEAT_TRUE) {
		if (sendto(protodata->sockfd, pkt->buff, pkt->len, 0,
				(struct sockaddr *) addr, sockaddrlen) > 0) {
			PLOG("报文重发成功  sendseq=%d\n", pkt->basehead.sendseq);
			ret = true;
		} else {
			PLOG("报文重发失败  sendseq=%d\n", pkt->basehead.sendseq);
			ret = false;
		}
	} else if (isrepeat == IS_REPEAT_FALSE) {
		pkt_basehead = proto_basehead_to_str(pkt);
		memcpy(pkt->buff, pkt_basehead, DATA_HEADLEN);
		free(pkt_basehead);

		if (data != NULL) {
			pkt_jsondata = proto_to_json(pkt, data);
			strcpy(pkt->buff + DATA_HEADLEN, pkt_jsondata);
			length = DATA_HEADLEN + strlen(pkt_jsondata) + 3;

			free(pkt_jsondata);
		} else {
			length = DATA_HEADLEN + 3;
		}

		strcpy(pkt->buff + length - 3, "\r\n");
		pkt->len = length - 1;
		if (sendto(protodata->sockfd, (unsigned char *)pkt->buff, length - 1, 0,
				(struct sockaddr *) addr, sockaddrlen) > 0)
			ret = true;
		else
			ret = false;
	}
	return ret;
}

char *proto_basehead_to_str(struct pkt_inf *pkt) {
	char *buffer;
	uint32_t sid;
	uint32_t seq;
	uint32_t ack;

	if (pkt == NULL) {
		return NULL;
	}
	buffer = (char *) calloc(1, DATA_HEADLEN);

	strcpy(buffer, PROTO_BASETAG);
	sid = htonl(pkt->basehead.sid);
	seq = htonl(pkt->basehead.sendseq);
	ack = htonl(pkt->basehead.ackseq);

	buffer[4] = pkt->basehead.cmd;
	buffer[5] = pkt->basehead.type;
	buffer[6] = pkt->basehead.opt;
	buffer[7] = pkt->basehead.sort;
	memcpy(buffer + 8, &sid, 4);
	memcpy(buffer + 12, &seq, 4);
	memcpy(buffer + 16, &ack, 4);

	int i;
	PLOG("send basehead Hex:");
	for (i = 0; i < 20; i++) {
		printf("%x ", (unsigned char)buffer[i]);
	}
	puts("");

	PLOG("send cmd:%d\n", pkt->basehead.cmd);
	PLOG("send type:%d\n", pkt->basehead.type);
	PLOG("send opt:%d\n", pkt->basehead.opt);
	PLOG("send sort:%d\n", pkt->basehead.sort);
	PLOG("send sid:%d\n", pkt->basehead.sid);
	PLOG("send seq:%d\n", pkt->basehead.sendseq);
	PLOG("send ack:%d\n", pkt->basehead.ackseq);
	return buffer;
}

char *proto_to_json(struct pkt_inf *pkt, void *data) {
	char *string = NULL;
	cJSON *root;
	cJSON *hopt;

	root = cJSON_CreateObject();
	hopt = cJSON_CreateObject();

	if ((pkt->basehead.opt & 7) > 0) {
		if ((pkt->basehead.opt & OPT_TRANSFER) > 0) {
			cJSON_AddNumberToObject(hopt, PROTO_HOPT_DID, pkt->did);
		}
		if ((pkt->basehead.opt & OPT_ENCRYPT) > 0) {
			cJSON_AddNumberToObject(hopt, PROTO_HOPT_KEYSEQ, pkt->keyseq);
		}
		cJSON_AddItemToObject(root, PROTO_HOPTTAG, hopt);
	}
	to_json_specific(pkt, data, root);

	string = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	PLOG("sned datajson:%s\n", string);
	return string;
}

static void to_json_specific(struct pkt_inf *pkt, void *data, cJSON *root) {
	switch (pkt->basehead.cmd) {
	case ZNAFCMD_LOGIN:
		to_json_login(pkt, data, root);
		break;

	case ZNAFCMD_ACCOUNT:
		to_json_accountman(pkt, data, root);
		break;

	case ZNAFCMD_CONTROL:
		to_json_remctl(pkt, data, root);
		break;

	case ZNAFCMD_QUERY:
		to_json_query(pkt, data, root);
		break;

	case ZNAFCMD_DELIVER:
		to_json_deliver(pkt, data, root);
		break;

	case ZNAFCMD_FIND:
		to_json_find(pkt, data, root);
		break;

	case ZNAFCMD_FILE:
		to_json_filetransmit(pkt, data, root);
		break;

	case ZNAFCMD_ENCODE:
		break;

	case ZNAFCMD_RELATE:
		break;

	case ZNAFCMD_UPDATE:
		to_json_upgrade(pkt, data, root);
		break;
	}
}
