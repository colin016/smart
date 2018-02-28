#include "proto.h"

static void to_json_upgrade_request(data_upgrade_request *data_request, cJSON *root);

//升级查询协议的数据域转化为json数据
void to_json_upgrade(struct pkt_inf *pkt, void *data, cJSON *root) {
	if (pkt->basehead.cmd == ZNAFCMD_UPDATE
			&& pkt->basehead.type == TYPE_UPGRADE_REQUEST) {
		to_json_upgrade_request((data_upgrade_request*) data, root);
	}
}

static void to_json_upgrade_request(data_upgrade_request *data_request, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, UPGRADE_REQUEST_USER, data_request->user);
	cJSON_AddNumberToObject(data, UPGRADE_REQUEST_SERIAL, data_request->serial);
	cJSON_AddStringToObject(data, UPGRADE_REQUEST_VERSION, data_request->version);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
