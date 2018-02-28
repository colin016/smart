#include "proto.h"

extern struct proto_data *protodata;

//关联人生成
static bool relateacc_create(pkt_inf *pkt, data_relateacc_reply *data_reply);
//执行脱离关联
static bool separateacc__start(pkt_inf *pkt, data_separate_reply *data_reply);
//执行获取关联帐号
static bool getacc_start(pkt_inf *pkt, data_getacc_reply *data_reply);
static bool getacc_allmanacc(data_getacc_reply *data_reply); //获取某个前端设备上的所有管理关联人帐号
static bool getacc_allacc(data_getacc_reply *data_reply); //获取前端设备上所有关联人帐号。
static bool synch_create_info(data_synch_request *data_request); //同步更新，生成info数据
static bool synch_relinfo(pkt_inf *pkt, data_synch_reply *data_reply); //同步关联人信息
static bool synch_cmp_by_server(pkt_inf *pkt, data_synch_reply *data_reply); //平台要求设备对比；
static bool synch_update_by_server(pkt_inf *pkt, data_synch_reply *data_reply); //平台要求设备更新
static bool synch_get_dev_info(pkt_inf *pkt, data_synch_reply *data_reply); //平台请求设备信息
static bool relmod_start(pkt_inf *pkt, data_relmod_reply *data_reply); //执行变更关联人

//关联帐号请求应答报文
bool proto_relateacc_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_relateacc_reply data_reply;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_RELATE;
	pkt_reply.basehead.type = TYPE_RELATE_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;
	relateacc_create(pkt, &data_reply);

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply)==true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	free(pkt_reply.buff);
	return true;
}

//脱联帐号请求应答报文
bool proto_separate_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_separate_reply data_reply;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_RELATE;
	pkt_reply.basehead.type = TYPE_RELOUT_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	separateacc__start(pkt, &data_reply);
	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);

	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply)==true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}

	return true;
}

//获取关联帐号应答
bool proto_getaccount_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_getacc_reply data_reply;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_RELATE;
	pkt_reply.basehead.type = TYPE_RELGET_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;
	getacc_start(pkt, &data_reply);
	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply)==true) {

		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	free(pkt_reply.buff);
	return true;
}

//关联同步请求
bool proto_synch_request_start(void) {
	pkt_inf pkt_request;
	data_synch_request data_request;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_RELATE;
	pkt_request.basehead.type = TYPE_RELSYN_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	synch_create_info(&data_request);

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {

		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
	}
	return true;
}

//关联同步应答(发给服务器)
bool proto_synch_reply_start(pkt_inf *pkt) {
//	data_synch_request *data_request;
	data_synch_reply data_reply;
	pkt_inf pkt_reply;

	memset(&data_reply, 0, sizeof(data_reply));
	memset(&pkt_reply, 0, sizeof(pkt_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_RELATE;
	pkt_reply.basehead.type = TYPE_RELSYN_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;
	synch_relinfo(pkt, &data_reply);

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_reply, &data_reply) == true) {

		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	return true;
}

//变更关联信息应答
bool proto_relmod_reply_start(pkt_inf *pkt) {
	data_relmod_reply data_reply;
	pkt_inf pkt_reply;

	memset(&data_reply, 0, sizeof(data_reply));
	memset(&pkt_reply, 0, sizeof(pkt_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_RELATE;
	pkt_reply.basehead.type = TYPE_RELMOD_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	relmod_start(pkt, &data_reply);

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_reply, &data_reply) == true) {

		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	return true;
}

/********************************************************************************/
static bool relateacc_create(pkt_inf *pkt, data_relateacc_reply *data_reply) {
	data_relateacc_request *data_request;
	relater_inf *user;
	relater_inf *existuser;
	struct timeval tv;
	struct timezone tz;

	data_request = (data_relateacc_request*) pkt->parsed_data;
	if (strcmp(data_request->puser, protodata->cfg.devname) != 0) {
		data_reply->ret = RELATER_RET_ERROR;
		strcpy(data_reply->info, "前端设备帐号不匹配");
	}
	if ((existuser = relater_get_by_name(data_request->user)) == NULL) {
		user = (relater_inf*) calloc(1, sizeof(relater_inf));
		strcpy(user->username, data_request->user);
		strcpy(user->password, data_request->password);
		user->right = data_request->right;
		user->regstatus = REGSTATE_WAIT;
		gettimeofday(&tv, &tz);
		user->regtime = ((long) tv.tv_sec) * 1000 + (long) tv.tv_usec / 1000;
		if (relaterList_add(user) == false) {
			PLOG("关联人添加失败\n");
			free(user);
		}

		data_reply->ret = RELATER_RET_OK;
		sprintf(data_reply->info, "%ld", user->regtime);
	} else {
		data_reply->ret = RELATER_RET_OK;
		sprintf(data_reply->info, "%ld", existuser->regtime);
	}

	return true;
}

//执行脱离关联
static bool separateacc__start(pkt_inf *pkt, data_separate_reply *data_reply) {
	data_separate_request *data_request;
	relater_inf *user;
	struct timeval tv;
	struct timezone tz;

	data_request = (data_separate_request*) pkt->parsed_data;
	user = relater_get_by_name(data_request->user);

	if (user == NULL) {
		data_reply->ret = SEPARATE_RET_ERROR;
		strcpy(data_reply->info, ERROR_NO_USER);
	} else {
		relaterList_delete(user->username);
		data_reply->ret = SEPARATE_RET_OK;
		gettimeofday(&tv, &tz);
		sprintf(data_reply->info, "%ld",
				((long) tv.tv_sec) * 1000 + (long) tv.tv_usec / 1000);
	}

	return true;
}

//执行获取关联帐号
static bool getacc_start(pkt_inf *pkt, data_getacc_reply *data_reply) {
	data_getacc_request *data_request;
	relater_inf *user;

	data_request = (data_getacc_request*) pkt->parsed_data;
	user = (relater_inf*) pkt->own_node;

	if (user == NULL) {
		user = relater_get_by_name(data_request->user);
	}
	if (user == NULL) {
		data_reply->ret = GETACC_RET_ERROR;
		strcpy(data_reply->info, ERROR_LINK_INVAILD);
		return true;
	}
	if (user->loginstatus == WORK_STATE
			&& data_request->link != protodata->platinf.link) {

		data_reply->ret = GETACC_RET_ERROR;
		strcpy(data_reply->info, ERROR_NO_USER);
		return true;
	}
	if (user->loginstatus != WORK_STATE
			&& (data_request->kind == GETACC_KIND_ALLMANREL
					|| data_request->kind == GETACC_KIND_ALLREL)) {

		if (strcmp(data_request->puser, protodata->cfg.devname) != 0) {
			data_reply->ret = GETACC_RET_ERROR;
			strcpy(data_reply->info, ERROR_DEVNAME_INVAILD);
			return true;
		}
	}

	switch (data_request->kind) {
	case GETACC_KIND_ALLDEVACC: //所有关联的前端设备帐号

		break;

	case GETACC_KIND_ALLMANREL: //获取某个前端设备上的所有管理关联人帐号
		getacc_allmanacc(data_reply);
		break;

	case GETACC_KIND_ALLREL: //获取前端设备上所有关联人帐号。
		getacc_allacc(data_reply);
		break;
	}

	return true;
}

static bool getacc_allmanacc(data_getacc_reply *data_reply) {
	int count = 0;
	cJSON *root;
	relater_inf *p = protodata->relaterlist.head;

	root = cJSON_CreateArray();

	while (p != NULL) {
		if (p->right == RELATER_AUTHORITY_MAN) {
			cJSON *user = cJSON_CreateObject();
			cJSON_AddStringToObject(user, GETACC_INFO_ACCOUNT, p->username);
			cJSON_AddNumberToObject(user, GETACC_INFO_RIGHT, p->right);
			cJSON_AddItemToArray(root, user);
			count++;
		}
		p = p->next;
	}
	data_reply->json_info = root;
	data_reply->ret = count;
	return true;
}

//获取前端设备上所有关联人帐号。
static bool getacc_allacc(data_getacc_reply *data_reply) {
	int count = 0;
	cJSON *root;
	relater_inf *p = protodata->relaterlist.head;

	root = cJSON_CreateArray();

	while (p != NULL) {
		cJSON *user = cJSON_CreateObject();
		cJSON_AddStringToObject(user, GETACC_INFO_ACCOUNT, p->username);
		cJSON_AddNumberToObject(user, GETACC_INFO_RIGHT, p->right);
		cJSON_AddItemToArray(root, user);
		count++;
		p = p->next;
	}
	data_reply->json_info = root;
	data_reply->ret = count;
	return true;
}

//同步更新，生成info数据
static bool synch_create_info(data_synch_request *data_request) {
	cJSON *root;
	relater_inf *p = protodata->relaterlist.head;

	root = cJSON_CreateArray();

	while (p != NULL) {
		cJSON *user = cJSON_CreateObject();
		cJSON_AddStringToObject(user, SYNCH_INFO_ACCOUNT, p->username);
		cJSON_AddNumberToObject(user, SYNCH_INFO_RIGHT, p->right);
		cJSON_AddStringToObject(user, SYNCH_INFO_ALIAS, p->alias);
		cJSON_AddStringToObject(user, SYNCH_INFO_PASS, p->password);
		cJSON_AddNumberToObject(user, SYNCH_INFO_TIME, p->regtime);
		cJSON_AddNumberToObject(user, SYNCH_INFO_STATE, p->regstatus);
		cJSON_AddItemToArray(root, user);
		p = p->next;
	}
	data_request->json_info = root;
	strcpy(data_request->user, protodata->cfg.devname);
	data_request->kind = 2;
	return true;
}

//同步关联人信息
static bool synch_relinfo(pkt_inf *pkt, data_synch_reply *data_reply) {
	data_synch_request *data_request;

	data_request = (data_synch_request*) pkt->parsed_data;

	switch (data_request->kind) {
	case SYNCH_CMP_WITH_DEV: //平台要求设备对比
		synch_cmp_by_server(pkt, data_reply);
		break;

	case SYNCH_UPDATE_BY_SERVER: //平台要求设备更新；
		synch_update_by_server(pkt, data_reply);
		break;

	case SYNCH_GET_DEV_INFO: //平台请求设备信息
		synch_get_dev_info(pkt, data_reply);
		break;
	}
	return true;
}

//平台要求设备对比；
static bool synch_cmp_by_server(pkt_inf *pkt, data_synch_reply *data_reply) {
	data_synch_request *data_request = NULL;
	cJSON *root = NULL;
	relater_inf *p = NULL;
	relater_inf user;
	int i = 0;
	int n = 0;

	memset(&user, 0, sizeof(user));
	data_reply->ret = 0;
	data_request = (data_synch_request*) pkt->parsed_data;
	n = cJSON_GetArraySize(data_request->json_info);
	for (i = 0; i < n; i++) {
		cJSON *relater = cJSON_GetArrayItem(data_request->json_info, i);
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
		free(relater);
		free(id);
		free(username);
		free(right);
		free(pass);
		free(time);
		free(state);
	}
	p = protodata->relaterlist.head;
	root = cJSON_CreateArray();
	while (p != NULL) {
		cJSON *user = cJSON_CreateObject();
		cJSON_AddStringToObject(user, SYNCH_INFO_ACCOUNT, p->username);
		cJSON_AddNumberToObject(user, SYNCH_INFO_RIGHT, p->right);
		cJSON_AddStringToObject(user, SYNCH_INFO_ALIAS, p->alias);
		cJSON_AddStringToObject(user, SYNCH_INFO_PASS, p->password);
		cJSON_AddNumberToObject(user, SYNCH_INFO_TIME, p->regtime);
		cJSON_AddNumberToObject(user, SYNCH_INFO_STATE, p->regstatus);
		cJSON_AddItemToArray(root, user);
		data_reply->ret += 1;
		p = p->next;
	}
	data_reply->json_info = root;
	return true;
}

//平台要求设备更新
static bool synch_update_by_server(pkt_inf *pkt, data_synch_reply *data_reply) {
	data_synch_request *data_request = NULL;
	relater_inf *p = NULL;
	relater_inf user;
	cJSON *root = NULL;
	int n = 0;
	int i = 0;

	data_reply->ret = 0;
	data_request = (data_synch_request*) pkt->parsed_data;
	memset(&user, 0, sizeof(user));
	n = cJSON_GetArraySize(data_request->json_info);

	for (i = 0; i < n; i++) {
		cJSON *relater = cJSON_GetArrayItem(data_request->json_info, i);
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
			user.recvtime = (long) (time->valuedouble);
			user.id = id->valueint;
			user.right = right->valueint;
			user.regstatus = state->valueint;
			strcpy(user.username, username->valuestring);
			strcpy(user.password, pass->valuestring);
			strcpy(user.alias, alias->valuestring);
			relaterList_modify(user);
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
	p = protodata->relaterlist.head;
	root = cJSON_CreateArray();
	while (p != NULL) {
		cJSON *user = cJSON_CreateObject();
		cJSON_AddStringToObject(user, SYNCH_INFO_ACCOUNT, p->username);
		cJSON_AddNumberToObject(user, SYNCH_INFO_RIGHT, p->right);
		cJSON_AddStringToObject(user, SYNCH_INFO_ALIAS, p->alias);
		cJSON_AddStringToObject(user, SYNCH_INFO_PASS, p->password);
		cJSON_AddNumberToObject(user, SYNCH_INFO_TIME, p->regtime);
		cJSON_AddNumberToObject(user, SYNCH_INFO_STATE, p->regstatus);
		cJSON_AddItemToArray(root, user);
		data_reply->ret += 1;
		p = p->next;
	}
	data_reply->json_info = root;
	return true;
}

//平台请求设备信息
static bool synch_get_dev_info(pkt_inf *pkt, data_synch_reply *data_reply) {

	return true;
}

//执行变更关联人
static bool relmod_start(pkt_inf *pkt, data_relmod_reply *data_reply) {
	data_relmod_request *data_request;
	relater_inf *user;

	data_request = (data_relmod_request *) pkt->parsed_data;
	user = relater_get_by_name(data_request->user);
	if (strcmp(protodata->cfg.devname, data_request->puser) != 0) {
		data_reply->ret = RELMOD_RET_ERROR;
		strcpy(data_reply->info, ERROR_DEVNAME_INVAILD);
	} else if (user == NULL) {
		data_reply->ret = RELMOD_RET_ERROR;
		strcpy(data_reply->info, ERROR_NO_USER);
	} else {
		if (user->link != data_request->link) {
			data_reply->ret = RELMOD_RET_ERROR;
			strcpy(data_reply->info, ERROR_LINK_INVAILD);
		} else {
			if (data_request->kind & RELMOD_KIND_ALIAS) {
				memset(user->alias, 0, sizeof(user->alias));
				strcpy(user->alias, data_request->alias);
			}
			if (data_request->kind & RELMOD_KIND_PASS) {
				memset(user->password, 0, sizeof(user->password));
				strcpy(user->password, data_request->pass);
			}
			if (data_request->kind & RELMOD_KIND_PASS) {
				user->right = data_request->right;
			}
			data_reply->ret = RELMOD_RET_OK;
		}
	}
	return true;
}
