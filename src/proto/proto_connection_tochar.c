#include "proto.h"

static void to_json_login_request(data_login_request *pkt_request, cJSON *root); //登录报文的数据域转化为json数据
static void to_json_verify_request(data_verify_request *verify_data,
		cJSON *root); //发给服务器的验证请求报文转化为json数据
static void to_json_loginout_request(data_loginout_request *loginout_data,
		cJSON *root); //前端设备登出请求报文数据域转化为json数据
static void to_json_login_reply(data_login_reply *reply_data, cJSON *root); //关联人登录前端设备，登录应答报文数据域转化为json数据
static void to_json_verify_reply(data_verify_reply *reply_data, cJSON *root); //关联人登录前端设备，验证应答报文数据域转化为json数据

//登录协议的数据域转化为json数据
void to_json_login(struct pkt_inf *pkt, void *data, cJSON *root) {
	switch (pkt->basehead.type) {
	case TYPE_LOGIN_QUEST:
		to_json_login_request((data_login_request*) data, root);
		break;

	case TYPE_LOGIN_REPLY:
		to_json_login_reply((data_login_reply*) data, root);
		break;

	case TYPE_LOGIN_CONFIRMQUEST:
		to_json_verify_request((data_verify_request*) data, root);
		break;

	case TYPE_LOGIN_CONFIRMREP:
		to_json_verify_reply((data_verify_reply*) data, root);
		break;

	case TYPE_LOGIN_LOGINOUT:
		to_json_loginout_request((data_loginout_request*) data, root);
		break;
	}
}

//登录报文的数据域转化为json数据
static void to_json_login_request(data_login_request *pkt_request, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, LOGIN_QUEST_SNUM, pkt_request->snum);
	cJSON_AddStringToObject(data, LOGIN_QUEST_SVER, pkt_request->sver);
	cJSON_AddStringToObject(data, LOGIN_QUEST_USER, pkt_request->user);
	cJSON_AddNumberToObject(data, LOGIN_QUEST_LINK, pkt_request->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

static void to_json_verify_request(data_verify_request *verify_data,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, CONFIRM_QUEST_CODE, verify_data->code);
	cJSON_AddStringToObject(data, CONFIRM_QUEST_USER, verify_data->user);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//前端设备登出请求报文数据域转化为json数据
static void to_json_loginout_request(data_loginout_request *loginout_data,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, LOGINOUT_QUEST_USER, loginout_data->user);
	cJSON_AddNumberToObject(data, LOGINOUT_QUEST_LINK, loginout_data->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//关联人登录前端设备，登录应答报文数据域转化为json数据
static void to_json_login_reply(data_login_reply *reply_data, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, LOGIN_REPLY_INFO, reply_data->info);
	cJSON_AddNumberToObject(data, LOGIN_REPLY_RET, reply_data->ret);
	cJSON_AddNumberToObject(data, LOGIN_REPLY_LINK, reply_data->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//关联人登录前端设备，验证应答报文数据域转化为json数据
static void to_json_verify_reply(data_verify_reply *reply_data, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, VERIFY_REPLY_INFO, reply_data->info);
	cJSON_AddNumberToObject(data, VERIFY_REPLY_RET, reply_data->ret);
	cJSON_AddNumberToObject(data, VERIFY_REPLY_LINK, reply_data->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
