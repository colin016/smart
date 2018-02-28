#include "proto.h"

extern struct proto_data *protodata;

bool proto_upgrade_parse(pkt_inf *pkt) {
	bool ret = false;

	if (pkt->basehead.type == TYPE_UPGRADE_REPLY) {
		ret = proto_upgrade_reply_parse(pkt);
	}

	return ret;
}

//升级查询应答报文解析
bool proto_upgrade_reply_parse(pkt_inf *pkt) {
	data_upgrade_reply data_reply;
	char *info = NULL;
	int ret;

	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	UPGRADE_REPLY_RET);
	if (ret < -1) {
		return false;
	} else {
		data_reply.ret = ret;
	}

	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	UPGRADE_REPLY_INFO);
	if (info == NULL) {
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	protodata->sendnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	PLOG("ret=%d\n", data_reply.ret);
	PLOG("info=%s\n", data_reply.info);
	return true;
}
