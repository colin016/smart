#include "proto.h"

extern struct proto_data *protodata;

static void register_successed(data_register_reply data_reply);
static void accmod_successed(data_accmod_reply data_reply);

bool proto_platman_parse(pkt_inf *pkt) {
	bool ret;
	switch (pkt->basehead.type) {
	case TYPE_ACCREG_REPLY:
		ret = proto_register_reply_parse(pkt);
		break;

	case TYPE_ACCMOD_REPLY:
		ret = proto_accmod_reply_parse(pkt);
		break;

	case TYPE_ACCKILL_REPLY:
		ret = proto_acckill_reply_parse(pkt);
		break;

	default:
		PLOG("type error\n");
		ret = false;
		break;
	}
	return ret;
}

//前端设备注册应答报文解析
bool proto_register_reply_parse(pkt_inf *pkt) {
	data_register_reply data_reply;
	bool ret;
	char *info = NULL;

	data_reply.ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REGISTER_REPLY_RET);

	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	REGISTER_REPLY_INFO);
	if (info == NULL) {
		PLOG("info==NULL\n");
		pthread_cond_signal(&protodata->platinf.cond);
		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	switch (data_reply.ret) {
	case REGISTER_RET_ERROR:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;

	case REGISTER_RET_SUCCESSED:
		register_successed(data_reply);
		ret = true;
		break;

	case REGISTER_RET_EXIST:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;
	}
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	pthread_cond_signal(&protodata->platinf.cond);
	return ret;
}

//前端设备帐号修改请求应答
bool proto_accmod_reply_parse(pkt_inf *pkt) {
	data_accmod_reply data_reply;
	bool ret;
	char *info = NULL;

	data_reply.ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	ACCMOD_REPLY_RET);

	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	ACCMOD_REPLY_INFO);
	if (info == NULL) {
		PLOG("info==NULL");
		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	switch (data_reply.ret) {
	case ACCMOD_RET_ERROR:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;

	case ACCMOD_RET_SUCCESSED:
		accmod_successed(data_reply);
		ret = true;
		break;

	case ACCMOD_RET_MODIFIED:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;
	}
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	pthread_cond_signal(&protodata->platinf.cond);
	return ret;
}

//前端设备帐号注销请求应答
bool proto_acckill_reply_parse(pkt_inf *pkt) {
	data_acckill_reply data_reply;
	bool ret = false;
	char *info = NULL;

	memset(&data_reply,0,sizeof(data_reply));
	data_reply.ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	ACCKILL_REPLY_RET);
	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	ACCKILL_REPLY_INFO);
	if (info == NULL) {
		PLOG("info==NULL");
		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);
		pthread_cond_signal(&protodata->platinf.cond);
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	switch (data_reply.ret) {
	case ACCKILL_RET_KILLED:
	case ACCKILL_RET_SUCCESSED:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;

	case ACCKILL_RET_ERROR:
		memset(protodata->platinf.info, 0, sizeof(protodata->platinf.info));
		strcpy(protodata->platinf.info, data_reply.info);
		ret = true;
		break;
	}
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	pthread_cond_signal(&protodata->platinf.cond);
	return ret;
}
/**************************文件内部使用函数*******************************/
static void register_successed(data_register_reply data_reply) {
	//把界面传来的配置项和服务器返回的id写入config.ini中
	KeyValue keyvalue;
	char key[10];
	char str[20];

	memset(key, 0, 10);
	strcpy(key, "DEVNAME");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.devname;
	keyvalue.valuelen=strlen(protodata->cfg.devname);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "PASSWORD");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.password;
	keyvalue.valuelen=strlen(protodata->cfg.password);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "BARCODE");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.barcode;
	keyvalue.valuelen=strlen(protodata->cfg.barcode);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "IDENTITY");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.identity;
	keyvalue.valuelen=strlen(protodata->cfg.identity);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "MAXNUM");
	sprintf(str, "%d", protodata->cfg.maxnum);
	keyvalue.key = key;
	keyvalue.value = str;
	keyvalue.valuelen=strlen(str);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "LONGITUDE");
	sprintf(str, "%lf", protodata->cfg.longitude);
	keyvalue.key = key;
	keyvalue.value = str;
	keyvalue.valuelen=strlen(str);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "LATITUDE");
	sprintf(str, "%lf", protodata->cfg.latitude);
	keyvalue.key = key;
	keyvalue.value = str;
	keyvalue.valuelen=strlen(str);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "DEVICEID");
	keyvalue.key = key;
	keyvalue.value = data_reply.info;
	keyvalue.valuelen=strlen(data_reply.info);
	protodata->cfg.deviceid = atoi(data_reply.info);
	PLOG("%s\n", data_reply.info);
	PLOG("%d\n", protodata->cfg.deviceid);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);
}

static void accmod_successed(data_accmod_reply data_reply) {
	//把更改后的信息和返回的id写入config.ini文件中
	KeyValue keyvalue;
	char key[10];
	char str[20];

	memset(key, 0, 10);
	strcpy(key, "DEVNAME");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.devname;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "PASSWORD");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.password;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "BARCODE");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.barcode;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "IDENTITY");
	keyvalue.key = key;
	keyvalue.value = protodata->cfg.identity;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "MAXNUM");
	sprintf(str, "%d", protodata->cfg.maxnum);
	keyvalue.key = key;
	keyvalue.value = str;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "LONGITUDE");
	sprintf(str, "%lf", protodata->cfg.longitude);
	keyvalue.key = key;
	keyvalue.value = str;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	memset(str, 0, 20);
	strcpy(key, "LATITUDE");
	sprintf(str, "%lf", protodata->cfg.latitude);
	keyvalue.key = key;
	keyvalue.value = str;
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);

	memset(key, 0, 10);
	strcpy(key, "DEVICEID");
	keyvalue.key = key;
	keyvalue.value = data_reply.info;
	protodata->cfg.deviceid = atoi(data_reply.info);
	PLOG("%s\n", data_reply.info);
	PLOG("%d\n", protodata->cfg.deviceid);
	protodata->callback(PROTO, CONFIG, CALLBACK_CONCTL_ADD, (void*) &keyvalue,
	NULL);
}
