#include "proto.h"

extern struct proto_data *protodata;

//登录成功
static bool login_success(struct pkt_inf *pkt);
//关联人验证请求报文，解密
static char * login_parse_code(long int number, char *code);

bool proto_login_parse(pkt_inf *pkt) {

	bool ret = true;

	switch (pkt->basehead.type) {

	case TYPE_LOGIN_REPLY:
		ret = proto_login_reply_parse(pkt);
		break;

	case TYPE_LOGIN_CONFIRMQUEST:
		ret = proto_verify_request_parse(pkt);
		break;

	case TYPE_LOGIN_CONFIRMREP:
		ret = proto_verify_reply_parse(pkt);
		break;

	case TYPE_LOGIN_LOGINOUT:
		ret = proto_loginout_request_parse(pkt);
		break;

	case TYPE_LOGIN_KEEPQUEST:
		ret = proto_keeplive_request_parse(pkt);
		break;

	case TYPE_LOGIN_KEEPREPLY:
		ret = proto_keeplive_reply_parse(pkt);
		break;

	default:
		ret = false;
		break;
	}
	return ret;
}

//关联人登出前端设备
bool proto_loginout_request_parse(struct pkt_inf *pkt) {
	data_loginout_request pkt_request;
	relater_inf *user;
	char *username = NULL;
	int link;

	user = (relater_inf*) pkt->own_node;
	if (user == NULL) {
		user = relater_get_by_name(pkt_request.user);
	}
	memset(&pkt_request, 0, sizeof(pkt_request));

	username = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGINOUT_QUEST_USER);
	if (username == NULL) {
		return false;
	} else {
		strcpy(pkt_request.user, username);
		free(username);
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGINOUT_QUEST_LINK);
	if (link < -1) {
		return false;
	} else {
		pkt_request.link = link;
	}

	if (user != NULL) {
		user->loginstatus = INIT_STATE;
		protodata->recvnum++;
		user->recvseq = pkt->basehead.sendseq;
		user->recvtime = pkt->recvtime;
		PLOG("%s:登出成功\n", user->username);
	} else {
		PLOG("%s:登出失败,没有找到该用户\n", user->username);
		return false;
	}

	return true;
}

//前端设备登录到服务器时，服务器返回验证应答报文
bool proto_verify_reply_parse(struct pkt_inf *pkt) {
	data_verify_reply data;
	char *info = NULL;
	int link;
	int ret;

	memset(&data, 0, sizeof(data));
	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	VERIFY_REPLY_RET);
	if (ret < -1) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录验证应答ret<-1");

		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);

		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		data.ret = ret;
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	VERIFY_REPLY_LINK);
	if (link < 0) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录验证应答link<0");

		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);

		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		data.link = link;
	}

	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	VERIFY_REPLY_INFO);
	if (info == NULL) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录验证应答info==NULL");

		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		strcpy(data.info, info);
		free(info);
	}
	pkt->parsed_data = &data;

	if (data.ret == VERIFY_RET_ERROR) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data.info);
		//验证失败
	} else if (data.ret == VERIFY_RET_SUCCESSED) {
		login_success(pkt);
	}

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;

	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	pthread_cond_signal(&protodata->platinf.cond);
	return true;;
}

/*前端设备登录到服务器时，服务器返回的应答报文解析*/
bool proto_login_reply_parse(struct pkt_inf *pkt) {
	data_login_reply login_reply;
	char *info = NULL;
	int ret;
	int link;

	memset(&login_reply, 0, sizeof(login_reply));
	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_REPLY_INFO);
	if (info == NULL) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录应答info==NULL");

		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		strcpy(login_reply.info, info);
		free(info);
	}
	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_REPLY_RET);
	if (ret < -1) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录应答ret<-1");

		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		login_reply.ret = ret;
	}
	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_REPLY_LINK);
	if (link < -10) {
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, "登录应答link<0");

		protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
				pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		login_reply.link = link;
	}

	pkt->parsed_data = &login_reply;

	if (login_reply.ret == LOGIN_RET_SUCCESSED) {
		login_success(pkt);
		pthread_cond_signal(&protodata->platinf.cond);
	} else if (login_reply.ret == LOGIN_RET_ERROR) {
		protodata->platinf.loginstatus = INIT_STATE;
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, login_reply.info);
		pthread_cond_signal(&protodata->platinf.cond);
	} else if (login_reply.ret == LOGIN_RET_VERIFY) {
		protodata->platinf.loginstatus = VALIDAT_STATE;
		login_verify_request_start(pkt);
	} else {
		pthread_cond_signal(&protodata->platinf.cond);
	}

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);

	return true;
}

//关联人登录请求报文解析
bool proto_login_request_parse(struct pkt_inf *pkt) {
	data_login_request data_request;
	relater_inf *user = NULL;
	char *snum = NULL;
	char *sver = NULL;
	char *username = NULL;
	int link;

	memset(&data_request, 0, sizeof(data_request));
	snum = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_QUEST_SNUM);
	if (snum == NULL) {
		PLOG("snum==NULL\n");
		return false;
	} else {
		strcpy(data_request.snum, snum);
		free(snum);
	}

	sver = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_QUEST_SVER);
	if (sver == NULL) {
		PLOG("sver==NULL\n");
		return false;
	} else {
		strcpy(data_request.sver, sver);
		free(sver);
	}

	username = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_QUEST_USER);
	if (username == NULL) {
		PLOG("username==NULL\n");
		return false;
	} else {
		strcpy(data_request.user, username);
		free(username);
	}

	link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	LOGIN_QUEST_LINK);
	if (link < 0) {
		PLOG("link<0\n");
		return false;
	} else {
		data_request.link = link;
	}
	pkt->parsed_data = &data_request;

	if (pkt->own_node == NULL) {
		user = relater_get_by_name(data_request.user);
		pkt->own_node = user;
	} else {
		user = (relater_inf*) pkt->own_node;
	}

	protodata->recvnum++;
	if (user != NULL) {
		user->addr = pkt->peeraddr;
		user->recvseq = pkt->basehead.sendseq;
		user->recvtime = pkt->recvtime;
		user->loginstatus = LOGINING_STATE;
		user->id = pkt->basehead.sid;
	}

	proto_login_reply_start(pkt);

	return true;
}

//关联人验证请求报文解析
bool proto_verify_request_parse(struct pkt_inf *pkt) {
	data_verify_request data_request;
	relater_inf *user = NULL;
	char *username = NULL;
	char *code = NULL;

	memset(&data_request, 0, sizeof(0));
	user = (relater_inf*) pkt->own_node;
	if (user == NULL) {
		user = relater_get_by_name(data_request.user);
	}

	code = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, CONFIRM_QUEST_CODE);
	if (code == NULL) {
		PLOG("code==NULL\n");
		return false;
	} else {
		strcpy(data_request.code, code);
		free(code);
	}
	username = json_get_string(pkt->buff + DATA_HEADLEN,
	PROTO_DATATAG, CONFIRM_QUEST_USER);
	if (username == NULL) {
		PLOG("username==NULL\n");
		return false;
	} else {
		strcpy(data_request.user, username);
		free(username);
	}

	login_parse_code(user->rond, data_request.code);

	if (user != NULL) {
		user->loginstatus = VALIDAT_STATE;
		user->addr = pkt->peeraddr;
		user->recvseq = pkt->basehead.sendseq;
		user->recvtime = pkt->recvtime;
	}
	protodata->recvnum++;

	proto_verify_reply_start(pkt);

	return true;
}

//服务器回应的应答报文
bool proto_keeplive_reply_parse(struct pkt_inf *pkt) {

	if (pkt->basehead.cmd == ZNAFCMD_LOGIN
			&& pkt->basehead.type == TYPE_LOGIN_KEEPREPLY) {
		protodata->platinf.keeplivetime = 0;
		protodata->recvnum++;
		protodata->platinf.recvseq = pkt->basehead.sendseq;
		protodata->platinf.recvtime = pkt->recvtime;
		protodata->platinf.keeplivetime++;
		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
	}
	return true;
}

//关联人发起的保活请求报文
bool proto_keeplive_request_parse(struct pkt_inf *pkt) {
	relater_inf *user;

	user = (relater_inf*) pkt->own_node;
	if (pkt->basehead.cmd == ZNAFCMD_LOGIN
			&& pkt->basehead.type == TYPE_LOGIN_KEEPQUEST) {
		if (user != NULL) {
			user->recvseq = pkt->basehead.sendseq;
			user->recvtime = pkt->recvtime;
		}
		protodata->recvnum++;
		proto_keeplive_reply_start(pkt);
	}
	return true;
}

/********************文件内部使用函数************************/

//关联人验证请求报文，解密
static char *login_parse_code(long int number, char *code) {
	char base64_decoded_code[40];
	char rond[20];
	int rondlen;
	int codelen;
	int i;

	sprintf(rond, "%ld", number);
	rondlen = strlen(rond);

	base64_decode(code, (unsigned char*) base64_decoded_code);
	codelen = strlen(base64_decoded_code);

	memset(code, 0, strlen(code));
// XOR加密
	if (rondlen >= codelen) {
		int j = 0;
		for (i = 0; i < rondlen; i++) {
			code[i] = rond[i] ^ base64_decoded_code[j++];
			if (j == codelen) {
				j = 0;
			}
		}
	} else {
		int j = 0;
		for (i = 0; i < codelen; i++) {
			code[i] = rond[j++] ^ base64_decoded_code[i];
			if (j == rondlen) {
				j = 0;
			}
		}
	}

	return code;
}

//登录成功
static bool login_success(struct pkt_inf *pkt) {
	data_verify_reply * data;
	data_login_reply *data_login;

	if (pkt->basehead.cmd == ZNAFCMD_LOGIN
			&& pkt->basehead.type == TYPE_LOGIN_REPLY) {
		data_login = (data_login_reply*) pkt->parsed_data;
		protodata->platinf.link = data_login->link;
		protodata->platinf.loginstatus = WORK_STATE;
		protodata->platinf.logintime = pkt->recvtime;
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_login->info);
	} else if (pkt->basehead.cmd == ZNAFCMD_LOGIN
			&& pkt->basehead.type == TYPE_LOGIN_CONFIRMREP) {
		data = (data_verify_reply*) pkt->parsed_data;
		protodata->platinf.link = data->link;
		protodata->platinf.loginstatus = WORK_STATE;
		protodata->platinf.logintime = pkt->recvtime;
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data->info);
		printf("link=%d\n", data->link);
	}
	return true;
}
