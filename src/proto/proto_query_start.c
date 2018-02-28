#include "proto.h"

extern struct proto_data *protodata;

//执行查询请求
static bool execute_query_request(pkt_inf *pkt, data_query_reply *data_reply);

//数据查询请求报文应答
bool proto_query_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_query_reply data_reply;
	relater_inf *user;

	user = (relater_inf*) pkt->own_node;
	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));

	pkt_reply.basehead.cmd = ZNAFCMD_QUERY;
	pkt_reply.basehead.type = TYPE_QUERY_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;

	if (user != NULL) {
		execute_query_request(pkt, &data_reply);
	} else {
		data_reply.ret = QUERY_RET_ERROR;
		strcpy(data_reply.info, ERROR_NO_USER);
	}
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply)==true) {
		protodata->sequence += protodata->sequence + protodata->sendnum;
		protodata->sendnum++;
		if (user != NULL) {
			user->sendseq = protodata->sequence;
			user->sendtime = time(NULL);
		}
	}
	return true;
}

static bool execute_query_request(pkt_inf *pkt, data_query_reply *data_reply) {
	data_query_request *data_request;
	int code;
	int info;

	data_request = (data_query_request*) pkt->parsed_data;
	code = data_request->code;

	switch (code) {
	case QUERY_ID_NUM: //用户id
		data_reply->ret = 0;
		data_reply->id = pkt->basehead.sid;
		break;

	case QUERY_MSG_NUM: //所有未读信息数量数
		data_reply->ret = 10;
		info = QUERY_MSG_NUM;
		memcpy(data_reply->info, &info, 4);
		break;

	case QUERY_ALARMMSG_NUM: //未读报警消息数
		data_reply->ret = 10;
		info = QUERY_ALARMMSG_NUM;
		memcpy(data_reply->info, &info, 4);
		break;

	case QUERY_RELATERMSG_NUM: //未读关联通知数
		data_reply->ret = 10;
		info = QUERY_RELATERMSG_NUM;
		memcpy(data_reply->info, &info, 4);
		break;

	default:
		PLOG("query code error\n");
		data_reply->ret = QUERY_RET_ERROR;
		memcpy(data_reply->info, ERROR_QUERYCODE_INVAILD, 4);
		break;
	}
	return true;
}
