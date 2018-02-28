#include "proto.h"

//关联帐号请求应答报文
static void to_json_relateacc_reply(data_relateacc_reply *data_reply,
		cJSON *root);
//脱联帐号请求应答报文
static void to_json_separate_reply(data_separate_reply *data_reply, cJSON *root);
//获取关联帐号应答
static void to_json_getaccount_reply(data_getacc_reply *data_reply, cJSON *root);
//关联同步请求
static void to_json_synch_request(data_synch_request *data_reply, cJSON *root);
//关联同步应答
static void to_json_synch_reply(data_synch_reply *data_reply, cJSON *root);
//变更关联信息应答
static void to_json_relmod_reply(data_relmod_reply *data_reply, cJSON *root);

//关联管理协议的数据域转化为json数据
void to_json_synch(struct pkt_inf *pkt, void *data, cJSON *root) {
	switch (pkt->basehead.type) {
	case TYPE_RELATE_REPLY: //关联帐号请求应答报文
		to_json_relateacc_reply((data_relateacc_reply*) data, root);
		break;

	case TYPE_RELOUT_REPLY: //脱联帐号请求应答报文
		to_json_separate_reply((data_separate_reply*) data, root);
		break;

	case TYPE_RELGET_REPLY: //获取关联帐号应答
		to_json_getaccount_reply((data_getacc_reply*) data, root);
		break;

	case TYPE_RELSYN_QUEST: //关联同步请求
		to_json_synch_request((data_synch_request*) data, root);
		break;

	case TYPE_RELSYN_REPLY: //关联同步应答
		to_json_synch_reply((data_synch_reply*) data, root);
		break;

	case TYPE_RELMOD_REPLY: //变更关联信息应答
		to_json_relmod_reply((data_relmod_reply*) data, root);
		break;
	}
}

//关联帐号请求应答报文
static void to_json_relateacc_reply(data_relateacc_reply *data_reply,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, RELATER_REPLY_RET, data_reply->ret);
	cJSON_AddStringToObject(data, RELATER_REPLY_INFO, data_reply->info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//脱联帐号请求应答报文
static void to_json_separate_reply(data_separate_reply *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, SEPARATE_REPLY_RET, data_reply->ret);
	cJSON_AddStringToObject(data, SEPARATE_REPLY_INFO, data_reply->info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//获取关联帐号应答
static void to_json_getaccount_reply(data_getacc_reply *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, GETACC_REPLY_RET, data_reply->ret);
	if (data_reply->ret == GETACC_RET_ERROR) {
		cJSON_AddStringToObject(data, GETACC_REPLY_INFO, data_reply->info);
	} else {
		cJSON_AddItemToObject(data, GETACC_REPLY_INFO, data_reply->json_info);
	}
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//关联同步请求
static void to_json_synch_request(data_synch_request *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, SYNCH_REQUEST_KIND, data_reply->kind);
	cJSON_AddStringToObject(data, SYNCH_REQUEST_USER, data_reply->user);

	if (data_reply->kind != SYNCH_GET_SERVER_INFO
			&& data_reply->kind != SYNCH_GET_DEV_INFO) {
		cJSON_AddItemToObject(data, SYNCH_REQUEST_INFO, data_reply->json_info);
	}

	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//关联同步应答
static void to_json_synch_reply(data_synch_reply *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, SYNCH_REPLY_RET, data_reply->ret);
	if (data_reply->ret == SYNCH_RET_ERROR) {
		cJSON_AddStringToObject(data, SYNCH_REPLY_INFO, data_reply->info);
	} else if (data_reply->ret >= 0) {
		cJSON_AddItemToObject(data, SYNCH_REPLY_INFO, data_reply->json_info);
	}
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//变更关联信息应答
static void to_json_relmod_reply(data_relmod_reply *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, RELMOD_REPLY_RET, data_reply->ret);
	if (data_reply->ret == RELMOD_RET_ERROR) {
		cJSON_AddStringToObject(data, RELMOD_REPLY_INFO, data_reply->info);
	}
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
