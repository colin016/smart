#include "proto.h"

//文件传输请求报文
static void to_json_filetransmit_request(
		data_filetransmit_request *data_request, cJSON *root);
//文件传输终止请求
static void to_json_filetranstop_request(
		data_filetranstop_request *data_request, cJSON *root);
//文件信息请求
static void to_json_fileinfo_request(data_fileinfo_request *data_request,
		cJSON *root);
//文件传输请求应答报文
static void to_json_filetransmit_reply(data_filetransmit_reply *data_reply,
		cJSON *root);
//文件信息请求应答报文
static void to_json_fileinfo_reply(data_fileinfo_reply *data_reply, cJSON *root);

//文件传输协议的数据域转化为json数据
void to_json_filetransmit(struct pkt_inf *pkt, void *data, cJSON *root) {
	switch (pkt->basehead.type) {
	case TYPE_FILE_REQUEST:
		to_json_filetransmit_request((data_filetransmit_request*) data, root);
		break;

	case TYPE_FILESTOP_REQUEST:
		to_json_filetranstop_request((data_filetranstop_request*) data, root);
		break;

	case TYPE_FILEINFO_REQUEST:
		to_json_fileinfo_request((data_fileinfo_request*) data, root);
		break;
	case TYPE_FILE_REPLY:
		to_json_filetransmit_reply((data_filetransmit_reply*) data, root);
		break;
	case TYPE_FILEINFO_REPLY:
		to_json_fileinfo_reply((data_fileinfo_reply*) data, root);
		break;
	}
}

//文件传输请求报文
static void to_json_filetransmit_request(
		data_filetransmit_request *data_request, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	//cJSON_AddStringToObject(data, "USER", data_request->user);
	//cJSON_AddNumberToObject(data, FILETRAN_REQUEST_CLASS, data_request->class);
	cJSON_AddStringToObject(data, FILETRAN_REQUEST_FILE, data_request->file);
	cJSON_AddNumberToObject(data, FILETRAN_REQUEST_SIZE, data_request->size);
	cJSON_AddNumberToObject(data, FILETRAN_REQUEST_FROM, data_request->from);
	cJSON_AddNumberToObject(data, FILETRAN_REQUEST_INDEX, data_request->index);
	cJSON_AddNumberToObject(data, FILETRAN_REQUEST_LINK, data_request->link);
	//free(data_request->file);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//文件传输终止请求
static void to_json_filetranstop_request(
		data_filetranstop_request *data_request, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, FILETRANSTOP_REQUEST_FILE,
			data_request->file);
	cJSON_AddNumberToObject(data, FILETRANSTOP_REQUEST_LINK,
			data_request->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//文件信息请求
static void to_json_fileinfo_request(data_fileinfo_request *data_request,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, FILEINFO_REQUEST_FILE, data_request->file);
	cJSON_AddNumberToObject(data, FILEINFO_REQUEST_SORT, data_request->link);
	cJSON_AddNumberToObject(data, FILEINFO_REQUEST_LINK, data_request->link);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//文件传输请求应答报文
static void to_json_filetransmit_reply(data_filetransmit_reply *data_reply,
		cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddNumberToObject(data, FILETRAN_REPLY_RET, data_reply->ret);
	cJSON_AddStringToObject(data, FILETRAN_REPLY_INFO, data_reply->info);
	if (data_reply->ret != FILETRAN_RET_ERROR) {
		cJSON_AddStringToObject(data, FILETRAN_REPLY_CONTENT,
				data_reply->content);

		cJSON_AddNumberToObject(data, FILETRAN_REPLY_LEN, data_reply->len);
	}
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}

//文件信息请求应答报文
static void to_json_fileinfo_reply(data_fileinfo_reply *data_reply, cJSON *root) {
	cJSON *data = NULL;

	data = cJSON_CreateObject();
	cJSON_AddItemToObject(data, FILEINFO_REPLY_INFO, data_reply->json_info);
	cJSON_AddNumberToObject(data, FILEINFO_REPLY_RET, data_reply->ret);
	cJSON_AddItemToObject(root, PROTO_DATATAG, data);
}
