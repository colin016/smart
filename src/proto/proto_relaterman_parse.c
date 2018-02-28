#include "proto.h"

extern struct proto_data *protodata;

static bool synch_start(data_synch_reply *data_reply);

bool proto_synch_parse(pkt_inf *pkt) {
	bool ret;
	switch (pkt->basehead.type) {
	case TYPE_RELATE_QUEST:
		ret = proto_relateacc_request_parse(pkt);
		break;

	case TYPE_RELOUT_QUEST:
		ret = proto_separate_request_parse(pkt);
		break;

	case TYPE_RELGET_QUEST:
		ret = proto_getaccount_request_parse(pkt);
		break;

	case TYPE_RELSYN_REPLY:
		ret = proto_synch_reply_parse(pkt);
		break;

	case TYPE_RELSYN_QUEST:
		ret = proto_synch_request_parse(pkt);
		break;

	case TYPE_RELMOD_REQUEST:
		ret = proto_relmod_request_parse(pkt);
		break;
	}
	return ret;
}

//关联帐号请求解析
bool proto_relateacc_request_parse(pkt_inf *pkt) {
	data_relateacc_request data_request;
	char *user = NULL;
	char *puser = NULL;
	char *muser = NULL;
	char *alias = NULL;
	char *password = NULL;
	int link;
	char ref;
	int right;

	memset(&data_request, 0, sizeof(data_request));
	ref = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELATER_QUEST_REF);
	if (ref < 0) {
		return false;
	} else {
		data_request.ref = ref;
	}

	user = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELATER_QUEST_USER);
	if (user == NULL) {
		return false;
	} else {
		strcpy(data_request.user, user);
		free(user);
	}

	puser = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELATER_QUEST_PUSER);
	if (puser == NULL) {
		return false;
	} else {
		strcpy(data_request.puser, puser);
		free(puser);
	}

	alias = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELATER_QUEST_ALIAS);
	if (alias == NULL) {
		return false;
	} else {
		strcpy(data_request.alias, alias);
		free(alias);
	}

	password = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELATER_QUEST_PASSWORD);
	if (password == NULL) {
		return false;
	} else {
		strcpy(data_request.password, password);
		free(password);
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELATER_QUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_request.link = link;
	}

	right = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELATER_QUEST_RIGHT);
	if (right < 0) {
		return false;
	} else {
		data_request.right = right;
	}

	if (data_request.ref == RELATER_REF_MAN) {
		muser = json_get_string(pkt->buff + DATA_HEADLEN,
		PROTO_DATATAG, RELATER_QUEST_MUSER);
		if (muser == NULL) {
			return false;
		} else {
			strcpy(data_request.muser, muser);
			free(muser);
		}
	}
	pkt->parsed_data = &data_request;
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	proto_relateacc_reply_start(pkt);
	return true;
}

//脱联帐号请求报文解析
bool proto_separate_request_parse(pkt_inf *pkt) {
	data_separate_request data_request;
	char *user = NULL;
	char *muser = NULL;
	char *puser = NULL;
	char ref;
	int link;

	ref = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	SEPARATE_REQUEST_REF);
	if (ref < 0) {
		return false;
	} else {
		data_request.ref = ref;
	}

	if (ref == SEPARATE_REF_MAN) {
		muser = json_get_string(pkt->buff + DATA_HEADLEN,
		PROTO_DATATAG, SEPARATE_REQUEST_MUSER);
		if (muser == NULL) {
			return false;
		} else {
			strcpy(data_request.muser, muser);
			free(muser);
		}
	}

	puser = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, SEPARATE_REQUEST_PUSER);
	if (puser == NULL) {
		return false;
	} else {
		strcpy(data_request.puser, puser);
	}

	user = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, SEPARATE_REQUEST_USER);
	if (user == NULL) {
		return false;
	} else {
		strcpy(data_request.user, user);
		free(user);
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	SEPARATE_REQUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_request.link = link;
	}

	pkt->parsed_data = &data_request;
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;

	proto_separate_reply_start(pkt);
	return true;
}

//获取关联帐号请求
bool proto_getaccount_request_parse(pkt_inf *pkt) {
	data_getacc_request data_request;
	char *user = NULL;
	char *puser = NULL;
	int kind;
	int link;

	memset(&data_request, 0, sizeof(data_request));
	kind = json_get_int(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, GETACC_REQUEST_KIND);
	if (kind < 0) {
		return false;
	} else {
		data_request.kind = kind;
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, GETACC_REQUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_request.link = link;
	}

	user = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, GETACC_REQUEST_USER);
	if (user == NULL) {
		return false;
	} else {
		strcpy(data_request.user, user);
		free(user);
	}

	puser = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, GETACC_REQUEST_PUSER);
	if (puser == NULL) {
		return false;
	} else {
		strcpy(data_request.puser, puser);
		free(puser);
	}
	pkt->parsed_data = &data_request;

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	proto_getaccount_reply_start(pkt);
	return true;
}

//关联同步应答
bool proto_synch_reply_parse(pkt_inf *pkt) {
	data_synch_reply data_reply;
	char *info = NULL;
	int ret;

	memset(&data_reply, 0, sizeof(data_reply));
	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	SYNCH_REPLY_RET);
	if (ret < -1) {
		return false;
	} else {
		data_reply.ret = ret;
	}

	if (data_reply.ret >= 0) {
		cJSON *root = cJSON_Parse(pkt->buff + DATA_HEADLEN);
		data_reply.json_info = cJSON_GetObjectItem(root, SYNCH_REPLY_INFO);
	} else {
		info = json_get_string(pkt->buff + DATA_HEADLEN,
		PROTO_DATATAG, SYNCH_REPLY_RET);
		if (info == NULL) {
			strcpy(data_reply.info, info);
			free(info);
		}
	}

	synch_start(&data_reply);
	PLOG("ret=%d\n", data_reply.ret);
	PLOG("info=%s\n", data_reply.info);
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	repsendlist_del(protodata->platinf.waitlist, pkt->basehead.ackseq);
	//free(data_reply.info);
	return true;
}

//关联同步请求解析(来自服务器)
bool proto_synch_request_parse(pkt_inf *pkt) {
	data_synch_request data_request;
	char *user = NULL;

	user = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	SYNCH_REQUEST_USER);
	if (user == NULL) {
		return false;
	} else {
		strcpy(data_request.user, user);
	}

	data_request.kind = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	SYNCH_REQUEST_KIND);

	cJSON *root = cJSON_Parse(pkt->buff + DATA_HEADLEN);
	if (root != NULL) {
		data_request.json_info = cJSON_GetObjectItem(root, SYNCH_REQUEST_INFO);

		pkt->parsed_data = &data_request;
		protodata->recvnum++;
		protodata->platinf.recvseq = pkt->basehead.sendseq;
		protodata->platinf.recvtime = pkt->recvtime;

		proto_synch_reply_start(pkt);
		free(data_request.json_info);
		free(root);
	}
	return true;
}

//变更关联信息请求解析
bool proto_relmod_request_parse(pkt_inf *pkt) {
	data_relmod_request data_requset;
	char ref;
	char *user = NULL;
	char *muser = NULL;
	char *puser = NULL;
	int kind;
	char *alias = NULL;
	char *pass = NULL;
	int right;
	int link;

	ref = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELMOD_REQUEST_REF);
	if (ref < 0) {
		return false;
	} else {
		data_requset.ref = ref;
	}

	user = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELMOD_REQUEST_USER);
	if (user == NULL) {
		return false;
	} else {
		strcpy(data_requset.user, user);
		free(user);
	}

	muser = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_MUSER);
	if (muser == NULL) {
		return false;
	} else {
		strcpy(data_requset.muser, muser);
		free(muser);
	}

	puser = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_PUSER);
	if (puser == NULL) {
		return false;
	} else {
		strcpy(data_requset.puser, puser);
		free(puser);
	}

	kind = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	RELMOD_REQUEST_KIND);
	if (kind < 1) {
		return false;
	} else {
		data_requset.kind = kind;
	}

	alias = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_ALIAS);
	if (alias == NULL) {
		return false;
	} else {
		strcpy(data_requset.alias, alias);
		free(alias);
	}

	pass = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_PASS);
	if (pass == NULL) {
		return false;
	} else {
		strcpy(data_requset.pass, pass);
		free(pass);
	}

	right = json_get_int(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_RIGHT);
	if (right < 0) {
		return false;
	} else {
		data_requset.right = right;
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, RELMOD_REQUEST_LINK);
	if (link < 0) {
		return false;
	} else {
		data_requset.link = link;
	}

	pkt->parsed_data = &data_requset;

	proto_relmod_reply_start(pkt);

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	return true;
}
/*****************************************************************************/

static bool synch_start(data_synch_reply *data_reply) {
	relater_inf *p = NULL;
	relater_inf user;
	int i = 0;
	int n = 0;

	if (data_reply->ret > 0) {
		memset(&user, 0, sizeof(user));
		n = cJSON_GetArraySize(data_reply->json_info);
		for (i = 0; i < n; i++) {
			cJSON *relater = cJSON_GetArrayItem(data_reply->json_info, i);
			cJSON *id = cJSON_GetObjectItem(relater, SYNCH_INFO_ID);
			cJSON *username = cJSON_GetObjectItem(relater, SYNCH_INFO_ACCOUNT);
			cJSON *alias = cJSON_GetObjectItem(relater, SYNCH_INFO_ALIAS);
			cJSON *right = cJSON_GetObjectItem(relater, SYNCH_INFO_RIGHT);
			cJSON *pass = cJSON_GetObjectItem(relater, SYNCH_INFO_PASS);
			cJSON *time = cJSON_GetObjectItem(relater, SYNCH_INFO_TIME);
			cJSON *state = cJSON_GetObjectItem(relater, SYNCH_INFO_STATE);
			if (state->valueint == REGSTATE_REJECT) {
				relaterList_delete(username->valuestring);
			} else if ((p = relater_get_by_name(username->valuestring)) != NULL) {
				if (p->regtime < (long) (time->valuedouble)) {
					user.recvtime = (long) (time->valuedouble);
					user.id = id->valueint;
					user.right = right->valueint;
					user.regstatus = state->valueint;
					strcpy(user.username, username->valuestring);
					strcpy(user.password, pass->valuestring);
					strcpy(user.alias, alias->valuestring);
					relaterList_modify(user);
				}
			} else {
				p = (relater_inf*) calloc(1, sizeof(relater_inf));
				p->recvtime = (long) (time->valuedouble);
				p->id = id->valueint;
				p->right = right->valueint;
				p->regstatus = state->valueint;
				strcpy(p->username, username->valuestring);
				strcpy(p->password, pass->valuestring);
				strcpy(p->alias, alias->valuestring);
				relaterList_add(p);
			}
		}
	} else if (data_reply->ret == 0) {

	}
	return true;
}
