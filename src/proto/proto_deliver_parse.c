#include "proto.h"

extern struct proto_data *protodata;

bool proto_deliver_parse(pkt_inf *pkt) {
	if (pkt->basehead.cmd == ZNAFCMD_DELIVER
			&& pkt->basehead.type == TYPE_DELIVER_REPLY) {
		proto_deliver_reply_parse(pkt);
		return true;
	}
	return false;
}

bool proto_deliver_reply_parse(pkt_inf *pkt) {
	data_deliver_reply data_reply;
	char *info = NULL;
	int ret;

	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	DELIVER_REPLY_RET);
	if (ret < -1) {
		return false;
	} else {
		data_reply.ret = ret;
	}
	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	DELIVER_REPLY_INFO);
	if (info == NULL) {
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);

	PLOG("%s\n", data_reply.info);
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	return true;
}
