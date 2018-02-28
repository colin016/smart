#include "proto.h"

extern struct proto_data *protodata;

//远程控制请求应答
bool proto_control_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_control_request *data_request;
	data_control_reply data_reply;
	relater_inf *user;
	bool ret;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));
	user = (relater_inf*) pkt->own_node;
	data_request = (data_control_request*) pkt->parsed_data;
	if (data_request == NULL) {
		PLOG("data_request==NULL\n");
		return false;
	}
	if (user == NULL) {
		data_reply.info = calloc(1, INFO_LEN);
		data_reply.ret = REMCTL_RET_ERROR;
		strcpy(data_reply.info, ERROR_NO_USER);
	} else {
		if (user->loginstatus != WORK_STATE) {
			data_reply.info = calloc(1, INFO_LEN);
			data_reply.ret = REMCTL_RET_ERROR;
			strcpy(data_reply.info, ERROR_USER_LONGINOUT);
		} else {
			//执行远程指令并得到执行结果
			proto_remote_control(pkt, &data_reply);
		}
	}
	pkt_reply.basehead.cmd = ZNAFCMD_CONTROL;
	pkt_reply.basehead.type = TYPE_CONTROL_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sendseq = protodata->sendnum + protodata->sequence;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply)==true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;

		if (user != NULL) {
			user->sendseq = protodata->sequence;
			user->sendtime = time(NULL);
		}
		ret = true;
	} else {
		ret = false;
	}

	return ret;
}
