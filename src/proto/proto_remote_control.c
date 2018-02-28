/*
 * proto_remote_control.c
 *
 *  Created on: Jul 30, 2017
 *      Author: 朱国全
 */

#include "proto.h"

//extern struct proto_data *protodata;

//执行远程控制命令
bool proto_remote_control(pkt_inf *pkt_request, data_control_reply *data_reply) {
	data_control_request *data_request;
	relater_inf *user;
	char *output;
	int inst;
	bool ret;

	data_request = (data_control_request*) pkt_request->parsed_data;
	user = (relater_inf*) pkt_request->own_node;
	inst = data_request->inst;
	output = (char*) calloc(1, 1024);

	PLOG("Remote control instruction is %d\n", inst);
	switch (inst) {
	case REMCTL_GET_ALARM:
		PLOG("获取最近报警信息\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_ACCOUNT:
		PLOG("获取当前登录账户\n");
		strcpy(output, user->username);
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_ACCOUNT_LOGINOUT:
		PLOG("强制登录账号离线\n");
		user->loginstatus = INIT_STATE;
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_CFGINFO:
		PLOG("获取配置信息\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_LOGNFO:
		PLOG("获取日志信息\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_PICTURE:
		PLOG("获取当前图片\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_VIDEO:
		PLOG("获取当前实时视频\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	case REMCTL_GET_PLUGINFO:
		PLOG("获取插件信息\n");
		data_reply->ret = REMCTL_RET_TEXTINFO;
		break;

	default:
		PLOG("The system is not %d instruction\n", inst);
		ret = false;
		data_reply->ret = REMCTL_RET_ERROR;
		strcpy(output, ERROR_INST_INVALID);
		break;
	}

	data_reply->info = output;
	return ret;
}
