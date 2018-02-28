#include "proto.h"

extern struct proto_data *protodata;

sendpkt_node *repsendlist_add(sendpkt_node *head, pkt_inf node) {
	proto_mutex_lock(&protodata->platinf.waitlist_lock);
	sendpkt_node *p = head;
	sendpkt_node *q = NULL;

	q = (sendpkt_node*) calloc(1, sizeof(sendpkt_node));
	q->node = node;
	q->sendcnt = 0;
	q->next = NULL;
	q->front = NULL;
	q->sendtime = time(NULL);

	if (p == NULL) {
		head = q;
		PLOG("sendseq=%d packet add success:head==NULL\n",
				node.basehead.sendseq);
	} else {
		while (p->next != NULL) {
			p = p->next;
		}

		p->next = q;
		q->front = p;
		p = q;
		PLOG("sendseq=%d packet add success:head!=NULL\n",
				node.basehead.sendseq);
	}
	proto_mutex_unlock(&protodata->platinf.waitlist_lock);
	return head;
}

sendpkt_node * repsendlist_del(sendpkt_node *head, int sendid) {
	proto_mutex_lock(&protodata->platinf.waitlist_lock);
	sendpkt_node *p = head;
	sendpkt_node *temp = NULL;

	while (p != NULL) {
		if (p->node.basehead.sendseq == sendid) {
			if (p->front == NULL) {
				temp = p;
				temp = temp->next;
				if (temp != NULL) {
					temp->front = NULL;
					head = temp;
				} else {
					head = NULL;
				}
			} else {
				temp = p->front;
				temp->next = p->next;
			}
			PLOG("sendseq=%d删除成功\n", p->node.basehead.sendseq);
			free(p->node.buff);
			free(p);
			break;
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->platinf.waitlist_lock);
	return head;
}
