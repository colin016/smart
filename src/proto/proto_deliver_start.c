#include "proto.h"

extern struct proto_data *protodata;

static void set_data_request(data_deliver_request *data_request);

//消息推送请求报文
bool proto_deliver_reauest_start(void) {
	pkt_inf pkt_request;
	data_deliver_request data_request;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));

	pkt_request.basehead.cmd = ZNAFCMD_DELIVER;
	pkt_request.basehead.type = TYPE_DELIVER_QUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	set_data_request(&data_request);

	pkt_request.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &protodata->platinf.server_addr,
			&pkt_request, &data_request) == true) {
		protodata->sequence += protodata->sequence + protodata->sendnum;
		protodata->sendnum++;
		protodata->platinf.sendseq = protodata->sequence;
		protodata->platinf.sendtime = time(NULL);
		protodata->platinf.waitlist = repsendlist_add(
				protodata->platinf.waitlist, pkt_request);
	} else {
		return false;
	}
	return true;
}

static void set_data_request(data_deliver_request *data_request) {
	cJSON *content;
	cJSON *info_child;
	struct timeval tv;
	struct timezone tz;
	long int t;

	gettimeofday(&tv, &tz);
	t = ((long) tv.tv_sec) * 1000 + (long) tv.tv_usec / 1000;
	content = cJSON_CreateObject();
	info_child = cJSON_CreateObject();
	data_request->json_info = cJSON_CreateArray();

	cJSON_AddNumberToObject(info_child, "USER", protodata->cfg.deviceid);
	cJSON_AddNumberToObject(info_child, "DEST", 1);
	cJSON_AddNumberToObject(info_child, "CLASS", 1);
	cJSON_AddNumberToObject(info_child, "DATE", t);
	cJSON_AddNumberToObject(content, "SORT", 1);
	cJSON_AddNumberToObject(content, "LEVEL", 1);
	cJSON_AddStringToObject(content, "ATTACH", "1.txt");
	cJSON_AddItemToObject(info_child, "CONTENT", content);
	cJSON_AddItemToArray(data_request->json_info, info_child);
	data_request->num = 1;
	data_request->link = protodata->platinf.link;
}

