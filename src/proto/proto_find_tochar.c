#include "proto.h"

static void to_json_find_reply(data_find_reply *data, cJSON *root); //局域网应答报文数据域转化为json

//局域网发现协议转化为json
void to_json_find(struct pkt_inf *pkt, void *data, cJSON *root) {
	if (pkt->basehead.type == TYPE_FIND_REPLY) {
		to_json_find_reply((data_find_reply*) data, root);
	}
}

//局域网应答报文数据域转化为json
static void to_json_find_reply(data_find_reply *pkt_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, FIND_REPLT_INFO, pkt_reply->info);
	cJSON_AddNumberToObject(data, FING_REPLY_RET, pkt_reply->ret);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
