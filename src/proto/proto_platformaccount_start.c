#include "proto.h"

extern struct proto_data *protodata;

static void register_data_record(register_data *regdata);

//前端设备帐号注册
bool proto_register_request_start(register_data *regdata, void *output) {
	pkt_inf pkt_request;
	data_register_request data_request;
	bool ret;

	if (strcmp(protodata->cfg.devname, regdata->account) == 0) {
		strcpy(output, "该帐号已注册");
		return false;
	}
	if (protodata->platinf.loginstatus == WORK_STATE) {
		strcpy(output, "该帐号已登录");
		return false;
	}

	memset(&pkt_request, 0, sizeof(pkt_request));
	pkt_request.basehead.cmd = ZNAFCMD_ACCOUNT;
	pkt_request.basehead.type = TYPE_ACCREG_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sid = 0;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;

	strcpy(data_request.snum, SOFTWARE_CODE);
	strcpy(data_request.sver, SOFTWARE_VERSION);
	strcpy(data_request.user, regdata->account);
	strcpy(data_request.password, regdata->password);
	strcpy(data_request.info.barcode, regdata->code);
	strcpy(data_request.info.serial, regdata->serial);
	strcpy(data_request.info.phone, "13281817720");
	data_request.info.class = SORT_POLICEDEV;
	data_request.info.latitude = regdata->latitude;
	data_request.info.longitude = regdata->longitude;
	data_request.info.number = regdata->number;

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		register_data_record(regdata);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
		pthread_mutex_lock(&protodata->platinf.lock);
		pthread_cond_wait(&protodata->platinf.cond, &protodata->platinf.lock);
		strcpy(output, protodata->platinf.info);
		ret = true;
		pthread_mutex_unlock(&protodata->platinf.lock);
	} else {
		ret = false;
	}
	return ret;
}

//前端设备帐号修改
bool proto_accmod_request_start(register_data *regdata, void *output) {
	pkt_inf pkt_request;
	data_accmod_request data_request;
	bool ret = false;

	if (protodata->platinf.loginstatus != WORK_STATE) {
		strcpy(output, "前端设备没有登录");
		return false;
	}
	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_ACCOUNT;
	pkt_request.basehead.type = TYPE_ACCMOD_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	strcpy(data_request.user, regdata->account);
	strcpy(data_request.password, regdata->password);
	strcpy(data_request.info.barcode, regdata->code);
	strcpy(data_request.info.serial, regdata->serial);
	strcpy(data_request.info.phone, "13281817720");
	data_request.info.class = SORT_POLICEDEV;
	data_request.info.latitude = regdata->latitude;
	data_request.info.longitude = regdata->longitude;
	data_request.info.number = regdata->number;
	data_request.link = protodata->platinf.link;

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		register_data_record(regdata);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
		pthread_mutex_lock(&protodata->platinf.lock);
		pthread_cond_wait(&protodata->platinf.cond, &protodata->platinf.lock);
		strcpy(output, protodata->platinf.info);
		ret = true;
		pthread_mutex_unlock(&protodata->platinf.lock);
	} else {
		ret = false;
	}
	return ret;
}

//前端设备帐号注销
bool proto_acckill_request_start(void *output) {
	pkt_inf pkt_request;
	data_acckill_request data_request;
	bool ret = false;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_ACCOUNT;
	pkt_request.basehead.type = TYPE_ACCKILL_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	strcpy(data_request.user, protodata->cfg.devname);
	data_request.link = protodata->platinf.link;
	//data_request.info=
	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
		pthread_mutex_lock(&protodata->platinf.lock);
		pthread_cond_wait(&protodata->platinf.cond, &protodata->platinf.lock);
		strcpy(output, protodata->platinf.info);
		ret = true;
		pthread_mutex_unlock(&protodata->platinf.lock);
	} else {
		ret = false;
	}

	return ret;
}

/****************************文件内部使用函数**************************/
static void register_data_record(register_data *regdata) {
	strcpy(protodata->cfg.devname, regdata->account);
	strcpy(protodata->cfg.password, regdata->password);
	strcpy(protodata->cfg.identity, regdata->serial);
	strcpy(protodata->cfg.barcode, regdata->code);
	protodata->cfg.latitude = regdata->latitude;
	protodata->cfg.longitude = regdata->longitude;
	protodata->cfg.maxnum = regdata->number;
}
