#include "proto.h"

extern struct proto_data *protodata;

bool proto_query_parse(pkt_inf *pkt) {
	bool ret;

	switch (pkt->basehead.type) {
	case TYPE_QUERY_QUEST:
		ret = proto_query_request_parse(pkt);
		break;

	default:
		PLOG("type error\n");
		ret = false;
		break;
	}
	return ret;
}

//数据查询请求报文解析
bool proto_query_request_parse(pkt_inf *pkt) {
	data_query_request data_request;
	relater_inf *user;
	char *username = NULL;
	char *para = NULL;
	int code;
	int link;

	memset(&data_request, 0, sizeof(data_request));
	user = (relater_inf*) pkt->own_node;
	if (user == NULL) {
		user = relater_get_by_name(data_request.user);
		pkt->own_node = user;
	}

	username = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	QUERY_REQUEST_USER);
	if (username == NULL) {
		return false;
	} else {
		strcpy(data_request.user, username);
		free(username);
	}

	code = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	QUERY_REQUEST_CODE);
	if (code < 0) {
		return false;
	} else {
		data_request.code = code;
	}

	para = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	QUERY_REQUEST_PARA);
	if (para == NULL) {
		return false;
	} else {
		strcpy(data_request.para, para);
		free(para);
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	QUERY_REQUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_request.link = link;
	}

	protodata->recvnum++;
	if (user != NULL) {
		user->recvseq = pkt->basehead.sendseq;
		user->recvtime = pkt->recvtime;
	}

	proto_query_reply_start(pkt);
	return true;
}

