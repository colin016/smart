#include "proto.h"

extern struct proto_data *protodata;

static long filesize(FILE*stream);
static bool filetransmit_start(struct file_node *filenode,
		data_filetransmit_reply *data_reply);

//前端设备向服务器发送文件传输请求
bool proto_filetransmit_request_start(struct delayhandle_node *delay_node) {
	pkt_inf pkt_request;
	data_filetransmit_request data_request;

	if (delay_node == NULL) {
		PLOG("delay_node==NULL\n");
		return false;
	}
	if (delay_node->taskclass != DELAY_CLASS_DOWNLOAD) {
		return false;
	}

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_FILE;
	pkt_request.basehead.type = TYPE_FILE_REQUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;

	if (delay_node->down_info.size
			- delay_node->down_info.total_len>=FILETRAN_MAXLEN) {

		data_request.size = FILETRAN_MAXLEN;
	} else {
		data_request.size = delay_node->down_info.size
				- delay_node->down_info.total_len;
	}

	data_request.from = delay_node->down_info.total_len;
	data_request.index = TILE_BINARY;
	data_request.link = protodata->platinf.link;
	strcpy(data_request.file, delay_node->down_info.url);

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);

		delay_node->time = protodata->platinf.sendtime;
		delay_node->sendseq = pkt_request.basehead.sendseq;
		delay_node->down_info.is_send = false;
		delay_node->down_info.req_size = data_request.size;
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
	}
	return true;
}

//前端设备向服务器发送终止文件传输请求
bool proto_filetranstop_request_start(char *url) {
	pkt_inf pkt_request;
	data_filetranstop_request data_request;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_FILE;
	pkt_request.basehead.type = TYPE_FILESTOP_REQUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	strcpy(data_request.file, url);
	data_request.link = protodata->platinf.link;

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
	}

	return true;
}

//前端设备向服务器发送文件信息请求
bool proto_fileinfo_request_start(void) {
	pkt_inf pkt_request;
	data_fileinfo_request data_request;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_FILE;
	pkt_request.basehead.type = TYPE_FILEINFO_REQUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;

	strcpy(data_request.file, "file:///C:/Users/Administrator/Desktop/123.zip");
	data_request.sort = FILE_TYPE_LOG;
	data_request.link = protodata->platinf.link;

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
	}

	return true;
}

//给管理app的文件件传输请求应答报文
bool proto_filetransmit_reply_start(struct file_node *filenode) {
	pkt_inf pkt_reply;
	data_filetransmit_reply data_reply;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));

	pkt_reply.basehead.cmd = ZNAFCMD_FILE;
	pkt_reply.basehead.type = TYPE_FILE_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = filenode->node.sendseq;

	filetransmit_start(filenode, &data_reply);

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &filenode->node.addr, &pkt_reply,
			&data_reply) == true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	free(pkt_reply.buff);
	return true;
}

//给管理app的文件信息请求应答报文
bool proto_fileinfo_reply_start(pkt_inf *pkt) {
	pkt_inf pkt_reply;
	data_fileinfo_reply data_reply;
	data_fileinfo_request *data_request;
	FILE *fp;

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&data_reply, 0, sizeof(data_reply));

	data_request = (data_fileinfo_request*) pkt->parsed_data;
	pkt->basehead.cmd = ZNAFCMD_FILE;
	pkt->basehead.type = TYPE_FILEINFO_REPLY;
	pkt->basehead.opt |= OPT_RESPONSE;
	pkt->basehead.sid = protodata->cfg.deviceid;
	pkt->basehead.sort = SORT_POLICEDEV;
	pkt->basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt->basehead.ackseq = pkt->basehead.sendseq;

	fp = fopen(data_request->file, "r");
	if (fp == NULL) {
		data_reply.ret = FILEINFO_RET_ERROR;
		strcpy(data_reply.info, ERROR_FILEURL_INVAILD);
	} else {
		cJSON *json_info = NULL;
		int file_size = 0;
		char md5[33] = { 0 };

		file_size = filesize(fp);
		fclose(fp);
		md54file(data_request->file, md5, false);
		data_reply.ret = FILEINFO_RET_OK;

		json_info = cJSON_CreateObject();
		cJSON_AddStringToObject(json_info, "FILE", data_request->file);
		cJSON_AddNumberToObject(json_info, "SIZE", file_size);
		cJSON_AddStringToObject(json_info, "MD5", md5);
		data_reply.json_info = json_info;
	}
	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->peeraddr, &pkt_reply,
			&data_reply) ==true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
	}
	free(pkt_reply.buff);
	return true;
}

static long filesize(FILE*stream) {
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

static bool filetransmit_start(struct file_node *filenode,
		data_filetransmit_reply *data_reply) {

	if (filenode->node.fp == NULL) {
		data_reply->ret = FILETRAN_RET_ERROR;
		strcpy(data_reply->info, ERROR_FILEURL_INVAILD);
	} else {
		if (filenode->node.index == TILE_BINARY) {

			if (filenode->node.total_sendsize < filenode->node.size) {
				int len = 0;

				data_reply->ret = filenode->node.index;
				fseek(filenode->node.fp, filenode->node.from, SEEK_SET);

				len = fread(data_reply->buffer, filenode->node.req_size, 1,
						filenode->node.fp);

				data_reply->len = len;
				filenode->node.send_size += len;
				filenode->node.total_sendsize = filenode->node.from + len;
				sprintf(data_reply->info, "%d", filenode->node.total_sendsize);
				strcpy(data_reply->content, "OK");
				filenode->node.time = time(NULL);

				if (filenode->node.send_size == filenode->node.req_size) {
					filenode->node.is_send = false;
				}
			} else {
				PLOG("文件已发送完\n");
			}
		}
	}
	return true;
}
