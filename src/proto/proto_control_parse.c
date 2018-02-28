#include "proto.h"

extern struct proto_data *protodata;

bool proto_remctl_parse(pkt_inf *pkt) {
	bool ret;
	switch (pkt->basehead.type) {
	case TYPE_CONTROL_QUEST:
		proto_control_request_parse(pkt);
		break;

	default:
		PLOG("type error\n");
		ret = false;
		break;
	}
	return ret;
}

bool proto_control_request_parse(pkt_inf *pkt) {
	data_control_request data_request;
	relater_inf *user;
	char *username;
	char *para;
	int link;
	int inst;

	memset(&data_request, 0, sizeof(data_request));
	user = (relater_inf*) pkt->own_node;
	if (user == NULL) {
		pkt->own_node = relater_get_by_name(data_request.user);
		user = (relater_inf*) pkt->own_node;
	}

	username = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REMCTL_QUEST_USER);
	if (username == NULL) {
		return false;
	} else {
		strcpy(data_request.user, username);
		free(username);
	}
	para = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REMCTL_QUEST_RARA);
	if (para == NULL) {
		return false;
	} else {
		strcpy(data_request.para, para);
		free(para);
	}

	inst = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REMCTL_QUEST_INST);
	if (inst < 0) {
		return false;
	} else {
		data_request.inst = inst;
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REMCTL_QUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_request.link = link;
	}
	pkt->parsed_data = &data_request;

	protodata->recvnum++;
	if (user != NULL) {
		user->recvseq = pkt->basehead.sendseq;
		user->recvtime = pkt->recvtime;
	}
	proto_control_reply_start(pkt);
	return true;
}
