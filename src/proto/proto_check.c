#include "proto.h"

extern struct proto_data *protodata;

//检查平台是否需要重发报文
void check_isresend(time_t curtime) {
	proto_mutex_lock(&protodata->platinf.waitlist_lock);
	sendpkt_node *p = protodata->platinf.waitlist;
	sendpkt_node *temp = NULL;
	int tm = 30;

	while (p != NULL) {
		if ((curtime - p->sendtime) > tm) {
			if (p->sendcnt < 3) {
				proto_sendmsg(IS_REPEAT_TRUE, &protodata->platinf.server_addr,
						&p->node, NULL);
				p->sendtime = curtime;
				p->sendcnt++;
			} else {
				if (p->node.basehead.cmd == ZNAFCMD_LOGIN
						&& p->node.basehead.type == TYPE_LOGIN_QUEST) {
					protodata->platinf.loginstatus = INIT_STATE;
					pthread_cond_signal(&protodata->platinf.cond);
				}
				if (p->node.basehead.cmd == ZNAFCMD_ACCOUNT
						&& (p->node.basehead.type == TYPE_ACCREG_QUEST
								|| p->node.basehead.type == TYPE_ACCMOD_QUEST)) {
					protodata->platinf.loginstatus = INIT_STATE;
					pthread_cond_signal(&protodata->platinf.cond);
				}
				if (p->node.basehead.cmd == ZNAFCMD_FILE
						&& p->node.basehead.type == TYPE_FILE_REQUEST) {
					struct delayhandle_node *del_node = NULL;
					del_node = delaylist_find(p->node.basehead.sendseq);
					if (del_node != NULL) {
						del_node->down_info.is_send = true;
					}
				}

				if (p->front == NULL) {
					temp = p;
					temp = p->next;
					if (temp != NULL) {
						temp->front = NULL;
						protodata->platinf.waitlist = temp;
					} else {
						protodata->platinf.waitlist = NULL;
					}
				} else {
					temp = p->front;
					temp->next = p->next;
				}
				PLOG("sendseq=%d删除成功\n", p->node.basehead.sendseq);
				free(p->node.buff);
				free(p);
			}
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->platinf.waitlist_lock);
}

//检查平台是否需要保活报文
void check_iskeep(time_t curtime) {
	time_t platrecvtime;
	time_t platsendtime;
	int tm = 30;

	platrecvtime = protodata->platinf.recvtime;
	platsendtime = protodata->platinf.sendtime;

	if ((abs(platrecvtime - curtime) >= tm || abs(platsendtime - curtime) >= tm)
			&& protodata->platinf.loginstatus == WORK_STATE) {

		proto_keeplive_request_start(TIMER);
	}

}

/**
 * 检查关联人是否有超时或者重传超时
 *
 * 检查关联人链表，查看最近收到报文时间是否超过设定值，如果超过，将其状态设置为离线。
 * 检查关联人链表是否有等待重传报文，最近发送时间是否超过设定值，如果超过，对于应答报文删除；对于请求报文如没有达到最大重发次数，重发
 * */
void check_relaters(time_t curtime) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	relater_inf *p = protodata->relaterlist.head;

	while (p != NULL) {
		if (abs(p->recvtime - curtime) > 30) {
			p->loginstatus = INIT_STATE;
			p->logintime = 0;
			p->recvseq = 0;
			p->recvtime = 0;
			p->rond = 0;
			p->waitnum = 0;
			p->sendtime = 0;
			p->sendseq = 0;
		}
		p = p->next;
	}

	proto_mutex_unlock(&protodata->relaterlist.mutex);
}

//检查是否有延迟处理超时
void check_postlist(time_t curtime) {

}

/*
 * 检查设置及检查更新的时间是否到来，如果到来，则启动一次更新查询。
 * */
void check_update(time_t curtime) {
	if (protodata->cfg.isupdate == 1 && (curtime % 30) == 0) {
		proto_upgrade_request_start();
	}
}

/*
 * 检查设置及检查平台同步的时间是否到来，如果到来，则启动一次同步。
 * */
void check_synrelater(time_t curtime) {

}

//检查是否需要自动登录平台
void check_islogin(time_t tm) {
	if (protodata->platinf.loginstatus == INIT_STATE) {
		if (protodata->cfg.isauto == 1) {
			proto_login_request_start(TIMER, NULL);
		}
	}
}

void check_filedownload_from_server(time_t curtime) {
	proto_mutex_lock(&protodata->data_lock);
	struct delayhandle_node *p = protodata->delaylist;

	while (p != NULL) {
		printf("%d\n", abs(p->time - curtime));
		if (p->taskclass == DELAY_CLASS_DOWNLOAD) {
			if (p->down_info.is_send == true) {

				proto_filetransmit_request_start(p);
			}
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->data_lock);
}

void check_filedownload_to_app(time_t curtime) {
	proto_mutex_lock(&protodata->filelist_lock);
	struct file_node *p = protodata->filelist;

	while (p != NULL) {
		if (p->node.is_send == true || (abs(p->node.time - curtime) > 30)) {
			proto_filetransmit_reply_start(p);
		}
	}
	proto_mutex_unlock(&protodata->filelist_lock);
}
