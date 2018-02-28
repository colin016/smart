#include "proto.h"

extern struct proto_data *protodata;

//解析从服务器来的每个文件传输应答报文
static bool filetransmit_from_server(pkt_inf *pkt);
//带内二进制
static bool file_binary(pkt_inf *pkt);
//从url获得文件名
static char *get_filename(char *url);
//删除文件
static int remove_file(char *filename);
//创建打开文件节点
static bool create_filenode(pkt_inf *pkt);
static long filesize(FILE*stream);

bool proto_filetransmit_parse(pkt_inf *pkt) {
	bool ret;
	switch (pkt->basehead.type) {
	case TYPE_FILE_REPLY:
		ret = proto_filetransmit_reply_parse(pkt);
		break;

	case TYPE_FILEINFO_REPLY:
		ret = proto_fileinfo_reply_parse(pkt);
		break;

	case TYPE_FILE_REQUEST:
		ret = proto_filetransmit_request_parse(pkt);
		break;

	case TYPE_FILESTOP_REQUEST:
		ret = proto_filetranstop_request_parse(pkt);
		break;

	case TYPE_FILEINFO_REQUEST:
		ret = proto_fileinfo_request_parse(pkt);
		break;

	}
	return ret;
}

//前端设备收到服务器的应答报文
bool proto_filetransmit_reply_parse(pkt_inf *pkt) {
	struct delayhandle_node *delay_node;
	data_filetransmit_reply data_reply;
	char *content;
	char *info;
	int ret;
	int len;

	delay_node = delaylist_find(pkt->basehead.ackseq);
	if (delay_node == NULL) {
		PLOG("delay_node==NULL\n");
		return false;
	}

	memset(&data_reply, 0, sizeof(data_reply));
	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REPLY_RET);
	if (ret < -1) {
		return false;
	} else {
		data_reply.ret = ret;
	}

	info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REPLY_INFO);
	if (info == NULL) {
		PLOG("data_reply.info==NULL");
		return false;
	} else {
		strcpy(data_reply.info, info);
		free(info);
	}

	len = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REPLY_LEN);
	if (len < 0) {
		PLOG("data_reply.len==-1");
		return false;
	} else {
		data_reply.len = len;
	}

	content = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REPLY_CONTENT);
	if (content == NULL) {
		PLOG("content==NULL");
		return false;
	} else {
		memcpy(data_reply.content, content, strlen(content));
		free(content);
	}

	pkt->parsed_data = &data_reply;
	if (data_reply.ret == TILE_BINARY) {
		if (strcmp(data_reply.content, "OK") == 0) {
			int total_len = pkt->len;
			int startpos = total_len - data_reply.len;
			memcpy(data_reply.content, pkt->buff + startpos, data_reply.len);
		} else {
			return false;
		}
	}

	if (delay_node->down_info.total_len
			!= atoi(data_reply.info) - data_reply.len) {

		delay_node->down_info.is_send = true;
	} else {
		filetransmit_from_server(pkt);
	}
	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	delay_node->time = pkt->recvtime;
	return true;
}

//前端设备向收到务器发送文件信息应答
bool proto_fileinfo_reply_parse(pkt_inf *pkt) {
	struct delayhandle_node *delay_node;
	data_fileinfo_reply data_reply;
	char *info;
	char *url;
	char *md5;
	int size;
	int ret;

	memset(&data_reply, 0, sizeof(data_reply));

	ret = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILEINFO_REPLY_RET);
	if (ret < -1) {
		return false;
	} else {
		data_reply.ret = ret;
	}

	if (ret == FILEINFO_RET_ERROR) {
		info = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
		FILEINFO_REPLY_INFO);
		if (info == NULL) {
			return false;
		} else {
			strcpy(data_reply.info, info);
			free(info);
		}
	} else {
		info = json_print(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
		FILEINFO_REPLY_INFO);
		if (info == NULL) {
			return false;
		}
		delay_node = (struct delayhandle_node *) calloc(1,
				sizeof(struct delayhandle_node));
		memset(&delay_node->down_info, 0, sizeof(delay_node->down_info));

		url = json_get_string(info, FILEINFO_INFO_FILE, NULL);
		if (url == NULL) {
			free(info);
			return false;
		} else {
			strcpy(delay_node->down_info.url, url);
			free(url);
		}

		md5 = json_get_string(info, FILEINFO_INFO_MD5, NULL);
		if (md5 == NULL) {
			free(info);
			return false;
		} else {
			strcpy(delay_node->down_info.md5, md5);
			free(md5);
		}

		size = json_get_int(info, FILEINFO_INFO_SIZE, NULL);
		free(info);

		delay_node->down_info.size = size;
		delay_node->taskclass = DELAY_CLASS_DOWNLOAD;
		delay_node->down_info.is_send = true;

		remove_file(get_filename(delay_node->down_info.url));

		delaylist_add(delay_node);
	}

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;
	protodata->platinf.waitlist = repsendlist_del(protodata->platinf.waitlist,
			pkt->basehead.ackseq);
	return true;
}

//管理app发送的文件件传输请求
bool proto_filetransmit_request_parse(pkt_inf *pkt) {
	data_filetransmit_request data_request;
	char *file;

	memset(&data_request, 0, sizeof(data_request));
	data_request.class = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_CLASS);

	data_request.size = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_SIZE);

	data_request.from = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_FROM);

	data_request.index = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_INDEX);

	data_request.link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_LINK);

	file = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRAN_REQUEST_FILE);
	strcpy(data_request.file, file);

	pkt->parsed_data = &data_request;
	create_filenode(pkt);
	//proto_filetransmit_reply_start(pkt);

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;

	return true;
}

//管理app发送的文件终止传输请求
bool proto_filetranstop_request_parse(pkt_inf *pkt) {
	data_filetranstop_request data_request;
	char *file;

	memset(&data_request, 0, sizeof(data_request));
	file = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRANSTOP_REQUEST_FILE);
	data_request.link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILETRANSTOP_REQUEST_LINK);
	strcpy(data_request.file, file);
	free(file);
	pkt->parsed_data = &data_request;

	bool ret = openfilelist_del(get_filename(data_request.file));
	if (ret == true) {
		PLOG("%s删除成功\n", data_request.file);
	}
	PLOG("file=%s\n", data_request.file);
	PLOG("link=%d\n", data_request.link);
	return true;
}

//管理app的文件信息请求
bool proto_fileinfo_request_parse(pkt_inf *pkt) {
	data_fileinfo_request data_request;
	char *file;

	memset(&data_request, 0, sizeof(data_request));
	file = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILEINFO_REQUEST_FILE);
	data_request.sort = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILEINFO_REQUEST_SORT);
	data_request.link = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
	FILEINFO_REQUEST_LINK);

	strcpy(data_request.file, file);
	free(file);

	pkt->parsed_data = &data_request;

	proto_fileinfo_reply_start(pkt);

	protodata->recvnum++;
	protodata->platinf.recvseq = pkt->basehead.sendseq;
	protodata->platinf.recvtime = pkt->recvtime;

	return true;
}

/**************************************************************/

//解析从服务器来的每个文件传输应答报文
static bool filetransmit_from_server(pkt_inf *pkt) {
	data_filetransmit_reply *data_reply;

	data_reply = (data_filetransmit_reply*) pkt->parsed_data;

	switch (data_reply->ret) {
	case FILE_ENCODE:
		break;

	case TILE_BINARY: //报文后面二进制文件信息
		file_binary(pkt);
		break;

	case FILE_FTP:
	case FILE_HTTP:
		break;

	case FILE_TRANSFER_FTP:
	case FILE_TRANSFER_HTTP:
		break;

	}
	return true;
}

//从url获得文件名
static char *get_filename(char *url) {
	char FileURL[3000];
	char *dst = NULL;
	char *filename = NULL;

	memset(FileURL, '\0', sizeof(FileURL));
	strcpy(FileURL, url);
	dst = strtok(FileURL, "/");

	while (dst != NULL) {
		filename = dst;
		dst = strtok(NULL, "/");
	}
	return filename;
}

//带内二进制
static bool file_binary(pkt_inf *pkt) {
	struct delayhandle_node *delay_node;
	data_filetransmit_reply *data_reply;
	FILE *fp;
	char *filename;
	char loc_url[300];
	char md5[33];
	char *buffer;
	char *url;
	int recvlen;
	int reqlen;

	memset(loc_url, 0, sizeof(loc_url));
	memset(md5, 0, sizeof(md5));
	data_reply = (data_filetransmit_reply*) pkt->parsed_data;
	delay_node = delaylist_find(pkt->basehead.ackseq);
	if (delay_node == NULL) {
		PLOG("delay_node==NULL:ackseq不正确\n");
		return false;
	}

	recvlen = delay_node->down_info.recv_size;
	reqlen = delay_node->down_info.req_size;
	buffer = delay_node->down_info.buffer;
	url = delay_node->down_info.url;

	memcpy(buffer + recvlen, data_reply->content, data_reply->len);
	delay_node->down_info.recv_size += data_reply->len;
	delay_node->down_info.total_len += data_reply->len;
	recvlen = delay_node->down_info.recv_size;

	printf("delay_node->delay_task.down_info.total_len=%d\n",
			delay_node->down_info.total_len);

	if (recvlen == reqlen) {
		protodata->platinf.waitlist = repsendlist_del(
				protodata->platinf.waitlist, pkt->basehead.ackseq);

		filename = get_filename(url);
		strcpy(loc_url, DOWNLOAD_DIR);
		strcpy(loc_url + strlen(DOWNLOAD_DIR), filename);
		fp = fopen(loc_url, "a");
		if (fp == NULL) {
			return false;
		}

		int n = fwrite(buffer, 1, recvlen, fp);
		PLOG("n=%d\n", n);
		fclose(fp);
		md54file(loc_url, md5, false);
		PLOG("recvlen=%d\nmd5=%s\ndelay_node->down_info.md5=%s\n\n\n", recvlen,
				md5, delay_node->down_info.md5);
		int err = memcmp(md5, delay_node->down_info.md5, 32);
		PLOG("\n\nerr=%d\n\n\n", err);
		if (err == 0) {
			//proto_filetranstop_request_start(url);
			delaylist_del(delay_node->sendseq);
			return true;
		}
		memset(buffer, 0, sizeof(delay_node->down_info.buffer));
		delay_node->down_info.recv_size = 0;
		delay_node->down_info.is_send = true;
	}
	return true;
}

//删除文件
static int remove_file(char *filename) {
	char url[256] = { 0 };

	if (filename == NULL)
		return -1;

	strcpy(url, DOWNLOAD_DIR);
	strcpy(url + strlen(DOWNLOAD_DIR), filename);

	return remove(url);
}

//创建打开文件节点
static bool create_filenode(pkt_inf *pkt) {
	data_filetransmit_request *data_request;
	struct file_node *filenode;
	char *filename;

	data_request = pkt->parsed_data;
	filename = get_filename(data_request->file);

	if ((filenode = openfilelist_find(filename)) == NULL) {
		filenode = calloc(1, sizeof(struct file_node));

		strcpy(filenode->node.filename, filename);
		filenode->node.fp = fopen(data_request->file, "r");
		if (filenode->node.fp == NULL) {
			return false;
		} else {
			filenode->node.size = filesize(filenode->node.fp);
		}
		bool ret = openfilelist_add(filenode);
		if (ret == true) {
			PLOG("filenode添加成功\n");
		}
	}
	filenode->node.addr = pkt->peeraddr;
	filenode->node.is_send = true;
	filenode->node.req_size = data_request->size;
	filenode->node.from = data_request->from;
	filenode->node.index = data_request->index;
	filenode->node.send_size = 0;
	filenode->node.sendseq = pkt->basehead.sendseq;
	filenode->node.time = time(NULL);
	strcpy(filenode->node.url, data_request->file);
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
