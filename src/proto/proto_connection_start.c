#include "proto.h"

extern struct proto_data *protodata;

// //初始化登录报文的基本头部
// static void login_init_basehead(pkt_inf *pkt);
//先对随机数用密码加密，采用XOR加密。然后对加密的数据用base64编码
static char *login_create_code(char *rond, char *password, int password_length);
//检查关联人提供的数据是否有效，并构建应答报文的数据域
static bool login_check(data_login_reply *data_reply, struct pkt_inf *pkt);
//关联人登录成功
static bool login_successed(struct pkt_inf *pkt);

//前端设备登出请求
bool proto_loginout_request_start(void) {
	pkt_inf pkt_loginout;
	data_loginout_request loginout_data;

	pkt_loginout.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_loginout.basehead.type = TYPE_LOGIN_LOGINOUT;
	pkt_loginout.basehead.opt |= OPT_REQUEST;
	pkt_loginout.basehead.sort = SORT_POLICEDEV;
	pkt_loginout.basehead.sid = protodata->cfg.deviceid;
	pkt_loginout.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_loginout.basehead.ackseq = 0;

	strcpy(loginout_data.user, protodata->cfg.devname);
	loginout_data.link = protodata->platinf.link;

	pkt_loginout.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_loginout, (void *) &loginout_data) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.loginstatus = INIT_STATE;
		return true;
	}
	free(pkt_loginout.buff);
	return false;
}

//前端设备登录到服务器
bool proto_login_request_start(int source, char *output) {
	bool ret = false;
	pkt_inf pkt_request;
	data_login_request data_request;

	memset(&data_request, 0, sizeof(data_request));
	memset(&pkt_request, 0, sizeof(pkt_request));

	if (protodata->platinf.loginstatus == WORK_STATE) {
		PLOG("前端设备已登录\n");
		return false;
	}
	pkt_request.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_request.basehead.type = TYPE_LOGIN_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;

	strcpy(data_request.snum, SOFTWARE_CODE);
	strcpy(data_request.sver, SOFTWARE_VERSION);
	strcpy(data_request.user, protodata->cfg.devname);
	data_request.link = protodata->platinf.link;

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, (void*) &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.loginstatus = LOGINING_STATE;
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
		if (source == UI) {
			pthread_mutex_lock(&protodata->platinf.lock);
			pthread_cond_wait(&protodata->platinf.cond,
					&protodata->platinf.lock);
			output = protodata->platinf.info;
			ret = true;
			pthread_mutex_unlock(&protodata->platinf.lock);
		}
	} else {
		ret = false;
	}

	return ret;
}

//前端设备向服务器保活请求报文
bool proto_keeplive_request_start(int source) {
	pkt_inf pkt_keep;

	pkt_keep.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_keep.basehead.type = TYPE_LOGIN_KEEPQUEST;
	pkt_keep.basehead.opt |= OPT_REQUEST;
	pkt_keep.basehead.sort = SORT_POLICEDEV;
	pkt_keep.basehead.sid = protodata->cfg.deviceid;
	pkt_keep.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_keep.basehead.ackseq = 0;

	pkt_keep.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_keep,
			NULL) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_keep);
		protodata->platinf.keeplivetime++;
	}

	return true;
}

//构造一个验证请求报文发给服务器
bool login_verify_request_start(pkt_inf *pkt) {
	pkt_inf verify_request;
	data_verify_request verify_data;
	data_login_reply *login_reply;
	int password_length;
	bool ret = false;
	char *code;

	memset(&verify_request, 0, sizeof(verify_request));
	memset(&verify_data, 0, sizeof(verify_data));
	login_reply = (data_login_reply *) pkt->parsed_data;

	verify_request.basehead.cmd = ZNAFCMD_LOGIN;
	verify_request.basehead.type = TYPE_LOGIN_CONFIRMQUEST;
	verify_request.basehead.opt |= OPT_REQUEST;
//verify_request.basehead.opt |= OPT_ENCRYPT;
	verify_request.basehead.sort = SORT_POLICEDEV;
	verify_request.basehead.sid = protodata->cfg.deviceid;
	verify_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	verify_request.basehead.ackseq = pkt->basehead.sendseq;

	password_length = strlen(protodata->cfg.password);
	code = login_create_code(login_reply->info, protodata->cfg.password,
			password_length);
	strcpy(verify_data.code, code);
	free(code);
	strcpy(verify_data.user, protodata->cfg.devname);

	verify_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&verify_request, (void*) &verify_data) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.loginstatus = VALIDAT_STATE;
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, verify_request);
		ret = true;
	}
	return ret;
}

//关联人登录应答报文
bool proto_login_reply_start(struct pkt_inf *pkt) {
	data_login_reply data_reply;

	relater_inf *user;
	pkt_inf pkt_reply;

//登录请求解析时，已经通过username在关联人链表查询过
	user = (relater_inf*) pkt->own_node;
	PLOG("\n");
	pkt_reply.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_reply.basehead.type = TYPE_LOGIN_REPLY;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	if (login_check(&data_reply, pkt) == false) {
		return false;
	}

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			(void*) &data_reply) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		if (user != NULL) {
			user->sendseq = protodata->sequence;
			user->sendtime = time(NULL);
		}
		return true;
	}
	free(pkt_reply.buff);
	return false;
}

//关联人验证应答报文
bool proto_verify_reply_start(struct pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_verify_reply data_reply;
	relater_inf *user;
	data_verify_request *data_request;
	time_t rond;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));
	data_request = (data_verify_request*) pkt->parsed_data;
	//登录验证解析应经在链表里找过了
	user = (relater_inf*) pkt->own_node;

	if (user == NULL && strcmp(data_request->code, user->password) != 0) {
		strcpy(data_reply.info, ERROR_PASSWORD_WRONG);
		data_reply.ret = VERIFY_RET_ERROR;
		data_reply.link = 0;
		PLOG("user==NULL&&strcmp(data_request->code,user->password)!=0\n");
	} else {
		char userid[10];
		rond = time(NULL);
		memset(userid, 0, sizeof(userid));
		sprintf(userid, "%d", user->id);
		strcpy(data_reply.info, userid);
		user->link = rond;
		data_reply.ret = VERIFY_RET_SUCCESSED;
		data_reply.link = rond;
		PLOG("%s:登录成功\n", user->username);
	}

	pkt_reply.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_reply.basehead.type = TYPE_LOGIN_CONFIRMREP;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		if (user != NULL) {
			time_t tm = time(NULL);
			user->sendtime = tm;
			user->sendseq = protodata->sequence;
			login_successed(pkt);
		}
		return true;
	}
	free(pkt_reply.buff);
	return false;
}

//前端设备发给关联人，保活应答报文
bool proto_keeplive_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_keep;

	pkt_keep.basehead.cmd = ZNAFCMD_LOGIN;
	pkt_keep.basehead.type = TYPE_LOGIN_KEEPREPLY;
	pkt_keep.basehead.opt |= OPT_RESPONSE;
	pkt_keep.basehead.sort = SORT_POLICEDEV;
	pkt_keep.basehead.sid = protodata->cfg.deviceid;
	pkt_keep.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_keep.basehead.ackseq = pkt->basehead.sendseq;

	pkt_keep.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_keep, NULL) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
	}
	free(pkt_keep.buff);
	return true;
}

/********************文件内部使用函数************************/

//先对随机数用密码加密，采用XOR加密。然后对加密的数据用base64编码
static char *login_create_code(char *rond, char *password, int password_length) {
	char *base64_code;
	char xor_code[20] = { '\0' };
//char *xor_code;
	int i;
	int j;
	int rondlen;
	int xorcodelen;
//xor_code=(char*)calloc(1,20);
	rondlen = strlen(rond);
	base64_code = (char*) calloc(1, 20 * 2);

// XOR加密
	if (password_length <= rondlen) {
		int j = 0;
		for (i = 0; i < rondlen; i++) {
			xor_code[i] = rond[i] ^ password[j++];
			xorcodelen = i;
			if (j == password_length) {
				j = 0;
			}
		}
	} else {
		int j = 0;
		for (i = 0; i < password_length; i++) {
			xor_code[i] = rond[j++] ^ password[i];
			xorcodelen = i;
			if (j == rondlen) {
				j = 0;
			}
		}
	}
	for (j = 0; j < xorcodelen + 1; j++) {
		printf("%d ", xor_code[j]);
	}
	puts("");
	printf("xorcodelen=%d\n", xorcodelen);
	printf("password=%s\n", password);
	printf("rond=%s\n", rond);
//base64_encode((unsigned*)xor_code, base64_code,xorcodelen+1);
	base64_encode((unsigned char*) xor_code, base64_code, xorcodelen + 1);
	printf("%s\n", base64_code);
	return base64_code;
}

//检查关联人提供的数据是否有效，并构建应答报文的数据域
static bool login_check(data_login_reply *data_reply, pkt_inf *pkt) {
	relater_inf *user;
	data_login_request *data_request;

	if (data_reply == NULL || pkt == NULL) {
		return false;
	}
	user = (relater_inf*) pkt->own_node;
	data_request = (data_login_request*) pkt->parsed_data;

	if (user == NULL) {
		strcpy(data_reply->info, ERROR_NO_USER);
		data_reply->ret = LOGIN_RET_ERROR;
		data_reply->link = 0;
		PLOG("没有找到%s用户\n", data_request->user);
	} else if (user->loginstatus == WORK_STATE) {
		strcpy(data_reply->info, ERROR_LOGINED);
		data_reply->ret = LOGIN_RET_ERROR;
		data_reply->link = 0;
		PLOG("%s:已经登录\n", user->username);
	} else if (user->link != data_request->link) {
		time_t t = time(NULL);
		user->rond = t;
		user->loginstatus = VALIDAT_STATE;
		data_reply->ret = LOGIN_RET_VERIFY;
		sprintf(data_reply->info, "%ld", t);
		data_reply->link = 0;
		PLOG("%s:登录验证\n", user->username);
	} else //登录c成功
	{
		time_t t = time(NULL);
		user->rond = t;
		data_reply->ret = LOGIN_RET_SUCCESSED;
		sprintf(data_reply->info, "%d", pkt->basehead.sid);
		data_reply->link = t;
		login_successed(pkt);
	}
	return true;
}

//关联人登录成功
static bool login_successed(struct pkt_inf *pkt) {
	relater_inf *user;

	if (pkt == NULL) {
		return false;
	}

	user = (relater_inf*) pkt->own_node;
	user->loginstatus = WORK_STATE;
	user->logintime = time(NULL);
	user->id = pkt->basehead.sid;
	PLOG("%s:登录成功\n", user->username);
	return true;
}
