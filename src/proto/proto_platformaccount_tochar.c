#include "proto.h"

//前端设备帐号注册请求报文，数据域转化为json数据
static void to_json_register_request(data_register_request *pkt_request,
		cJSON *root);
//前端设备帐号修改请求报文，数据域转化为json数据
static void to_json_accmod_request(data_accmod_request *pkt_request,
		cJSON *root);
//前端设备帐号注销请求报文，数据域转化为json数据
static void to_json_acckill_request(data_acckill_request *data_request,
		cJSON *root);

//平台帐号管理协议的数据域转化为json数据
void to_json_accountman(struct pkt_inf *pkt, void *data, cJSON *root) {
	switch (pkt->basehead.type) {
	case TYPE_ACCREG_QUEST:
		to_json_register_request((data_register_request*) data, root);
		break;

	case TYPE_ACCMOD_QUEST:
		to_json_accmod_request((data_accmod_request*) data, root);
		break;

	case TYPE_ACCKILL_QUEST:
		to_json_acckill_request((data_acckill_request*) data, root);
		break;
	}
}

//前端设备帐号注册请求报文，数据域转化为json数据
static void to_json_register_request(data_register_request *pkt_request,
		cJSON *root) {
	cJSON *data = NULL;
	cJSON *info = NULL;

	data = cJSON_CreateObject();
	info = cJSON_CreateObject();

	cJSON_AddStringToObject(data, REGISTER_REQUEST_SNUM, pkt_request->snum);
	cJSON_AddStringToObject(data, REGISTER_REQUEST_SVER, pkt_request->sver);
	cJSON_AddStringToObject(data, REGISTER_REQUEST_USER, pkt_request->user);
	cJSON_AddStringToObject(data, REGISTER_REQUEST_PASSWORD,
			pkt_request->password);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_SEQ,
			pkt_request->info.serial);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_BARCODE,
			pkt_request->info.barcode);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_PHONE,
			pkt_request->info.phone);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_LONGI,
			pkt_request->info.longitude);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_LATI,
			pkt_request->info.latitude);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_MAX,
			pkt_request->info.number);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_CLASS,
			pkt_request->info.class);

	cJSON_AddItemToObject(data, REGISTER_REQUEST_INFO, info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//前端设备帐号修改请求报文，数据域转化为json数据
static void to_json_accmod_request(data_accmod_request *pkt_request,
		cJSON *root) {
	cJSON *data = NULL;
	cJSON *info = NULL;

	data = cJSON_CreateObject();
	info = cJSON_CreateObject();

	cJSON_AddStringToObject(data, ACCMOD_REQUEST_USER, pkt_request->user);
	cJSON_AddStringToObject(data, ACCMOD_REQUEST_PASSWORD,
			pkt_request->password);

	cJSON_AddNumberToObject(data, ACCMOD_REQUEST_LINK, pkt_request->link);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_SEQ,
			pkt_request->info.serial);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_BARCODE,
			pkt_request->info.barcode);

	cJSON_AddStringToObject(info, REGISTER_REQUEST_INFO_PHONE,
			pkt_request->info.phone);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_LONGI,
			pkt_request->info.longitude);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_LATI,
			pkt_request->info.latitude);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_MAX,
			pkt_request->info.number);

	cJSON_AddNumberToObject(info, REGISTER_REQUEST_INFO_CLASS,
			pkt_request->info.class);

	cJSON_AddItemToObject(data, ACCMOD_REQUEST_INFO, info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//前端设备帐号注销请求报文，数据域转化为json数据
static void to_json_acckill_request(data_acckill_request *data_request,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, ACCKILL_REQUEST_USER, data_request->user);
	cJSON_AddStringToObject(data, ACCKILL_REQUEST_INFO, data_request->info);
	cJSON_AddNumberToObject(data, ACCKILL_REQUEST_LINK, data_request->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
