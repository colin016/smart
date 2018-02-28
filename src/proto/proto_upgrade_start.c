#include "proto.h"

extern struct proto_data *protodata;

//升级查询请求
bool proto_upgrade_request_start(void) {
	pkt_inf pkt_request;
	data_upgrade_request data_request;

	memset(&pkt_request, 0, sizeof(pkt_request));
	memset(&data_request, 0, sizeof(data_request));
	pkt_request.basehead.cmd = ZNAFCMD_UPDATE;
	pkt_request.basehead.type = TYPE_UPGRADE_REQUEST;
	pkt_request.basehead.opt |= OPT_REQUEST;
	pkt_request.basehead.sid = protodata->cfg.deviceid;
	pkt_request.basehead.sort = SORT_POLICEDEV;
	pkt_request.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_request.basehead.ackseq = 0;
	strcpy(data_request.user, protodata->cfg.devname);
	data_request.serial = UPGRADE_POLICEDEV_PROGRAM;
	strcpy(data_request.version, SOFTWARE_VERSION);

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
