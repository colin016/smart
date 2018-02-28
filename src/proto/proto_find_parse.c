#include "proto.h"

extern struct proto_data *protodata;

bool proto_find_request_parse(struct pkt_inf *pkt) {
	data_find_request find_request;
	relater_inf *user;
	char *username = NULL;

	memset(&find_request, 0, sizeof(find_request));
	user = (relater_inf*) pkt->own_node;

	if (!((pkt->basehead.opt & OPT_TRANSFER) > 0)
			&& pkt->basehead.sort == SORT_MANAGEAPP) {
		//find_data_parse(pkt);

		username = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_DATATAG,
		FING_REQUEST_USER);
		if (username == NULL) {
			return false;
		} else {
			strcpy(find_request.user, username);
			free(username);
		}
		pkt->parsed_data = &find_request;

		if (user == NULL) {
			pkt->own_node = relater_get_by_name(find_request.user);
			user = (relater_inf*) pkt->own_node;
		}
		if (user != NULL) {
			user->addr = pkt->peeraddr;
			protodata->recvnum++;
			user->recvtime = pkt->recvtime;
			user->recvseq = pkt->basehead.sendseq;
			find_reply_start(pkt, user);
		} else {
			PLOG("pkt->own_node == NULL");
			find_reply_start(pkt, NULL);
		}
	}
	return true;
}
