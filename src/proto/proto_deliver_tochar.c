#include "proto.h"

static void to_json_deliver_request(data_deliver_request *data_request,
		cJSON *root);

//消息传递协议数据域转化为json数据
void to_json_deliver(struct pkt_inf *pkt, void *data, cJSON *root) {
	if (pkt->basehead.cmd == ZNAFCMD_DELIVER
			&& pkt->basehead.type == TYPE_DELIVER_QUEST) {
		to_json_deliver_request((data_deliver_request*) data, root);
	}
}

static void to_json_deliver_request(data_deliver_request *data_request,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, DELIVER_REQUEST_NUM, data_request->num);
	cJSON_AddNumberToObject(data, DELIVER_REQUEST_LINK, data_request->link);
	cJSON_AddItemToObject(data, DELIVER_REQUEST_INFO, data_request->json_info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
