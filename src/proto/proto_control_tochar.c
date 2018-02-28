#include "proto.h"

static void to_json_verify_reply(data_control_reply *reply_data, cJSON *root);

//远程控制协议的数据域转化为json数据
void to_json_remctl(struct pkt_inf *pkt, void *data, cJSON *root) {
	if (pkt->basehead.cmd == ZNAFCMD_CONTROL
			&& pkt->basehead.type == TYPE_CONTROL_REPLY) {
		to_json_verify_reply((data_control_reply*) data, root);
	}
}

static void to_json_verify_reply(data_control_reply *reply_data, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, REMCTL_REPLY_RET, reply_data->ret);
	cJSON_AddStringToObject(data, REMCTL_REPLY_INFO, reply_data->info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);

	if (reply_data->info != NULL) {
		free(reply_data->info);
	}
}
