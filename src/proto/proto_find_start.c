#include "proto.h"

extern struct proto_data *protodata;

bool find_reply_start(struct pkt_inf *pkt, struct relater_inf *user) {
	struct pkt_inf pkt_reply;
	data_find_reply find_reply;

	if (pkt == NULL) {
		return false;
	}

	memset(&pkt_reply, 0, sizeof(pkt_reply));
	memset(&find_reply, 0, sizeof(find_reply));
	pkt_reply.basehead.cmd = ZNAFCMD_FIND;
	pkt_reply.basehead.type = TYPE_FIND_REPLY;
	pkt_reply.basehead.opt |= OPT_RESPONSE;
	pkt_reply.basehead.sort = SORT_POLICEDEV;
	pkt_reply.basehead.sid = protodata->cfg.deviceid;
	pkt_reply.basehead.sendseq = protodata->sequence + protodata->sendnum;
	pkt_reply.basehead.ackseq = pkt->basehead.sendseq;

	strcpy(find_reply.info, protodata->cfg.devname);
	if (user==NULL)
	{
		find_reply.ret=-1;//表示没有关系
	}else
	{
        find_reply.ret = user->right;
	}

	pkt_reply.buff = (char*) calloc(1, DATA_MAXLEN);
	if (proto_sendmsg(IS_REPEAT_FALSE, &pkt->souraddr, &pkt_reply, &find_reply)
			== true) {
		protodata->sequence += protodata->sendnum;
		protodata->sendnum++;
		if (user!=NULL)
		{
			user->sendseq = protodata->sequence;
		    user->sendtime = time(NULL);
		}
	}
	free(pkt_reply.buff);
	return true;
}
