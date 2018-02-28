#include "proto.h"

static void to_json_query_reply(data_query_reply *data_reply, cJSON *root);

//数据查询协议的数据域转化为json数据
void to_json_query(struct pkt_inf *pkt, void *data, cJSON *root)
{
	switch(pkt->basehead.type)
	{
	case TYPE_QUERY_REPLY:
		to_json_query_reply((data_query_reply*)data,root);
		break;
	default:
		PLOG("type error\n");
		break;
	}
}

static void to_json_query_reply(data_query_reply *data_reply, cJSON *root)
{
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, QUERY_REPLY_RET, data_reply->ret);
	if(data_reply->ret==0)
	{
		cJSON_AddNumberToObject(data, QUERY_REPLY_ID, data_reply->id);
	}
	cJSON_AddStringToObject(data, QUERY_REPLY_INFO, data_reply->info);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
