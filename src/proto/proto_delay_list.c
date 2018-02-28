/*
 * proto_delay_list.c
 *
 *  Created on: Aug 6, 2017
 *      Author: moon
 */
#include "proto.h"

extern struct proto_data *protodata;

//延迟处理链表
//添加
bool delaylist_add(struct delayhandle_node *node) {
	proto_mutex_lock(&protodata->data_lock);
	struct delayhandle_node *p = protodata->delaylist;

	node->next = NULL;

	if (p == NULL) {
		protodata->delaylist = node;
		PLOG("p == NULL:添加成功\n");
	} else {
		while (p->next != NULL) {
			p = p->next;
		}
		PLOG("p->next = node:添加成功\n");
		p->next = node;
	}
	proto_mutex_unlock(&protodata->data_lock);
	return true;
}

//删除
bool delaylist_del(int sendseq) {
	proto_mutex_lock(&protodata->data_lock);
	struct delayhandle_node *p = protodata->delaylist;
	struct delayhandle_node *q = NULL;
	bool ret = false;

	while (p != NULL) {
		if (p->sendseq == sendseq) {
			if (q == NULL) {
				protodata->delaylist = NULL;
				PLOG("sendseq=%d:删除成功,q == NULL\n",sendseq);
			} else {
				q->next = p->next;
				PLOG("sendseq=%d:删除成功\n",sendseq);
			}
			free(p);
			ret = true;
			break;
		}
		q = p;
		p = p->next;
	}
	proto_mutex_unlock(&protodata->data_lock);
	return ret;
}

//查找
struct delayhandle_node *delaylist_find(int sendseq) {
	proto_mutex_lock(&protodata->data_lock);
	struct delayhandle_node *p = protodata->delaylist;
	struct delayhandle_node *q = NULL;

	while (p != NULL) {
		if (p->sendseq == sendseq) {
			q = p;
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->data_lock);
	return q;
}

