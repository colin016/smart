#include "proto.h"

extern struct proto_data *protodata;

//解析opt选项
static bool opt_parse(struct pkt_inf *pkt);
// opt选项支持NAT透传
static void opt_nat(struct pkt_inf *pkt);
//更新关联人信息
static bool update_user_info(struct pkt_inf *pkt);
//不是保活报文时的处理
static bool is_no_keeplive(struct pkt_inf *pkt);
//进入具体的报文
static bool proto_specific(struct pkt_inf *pkt);

//报文头部解析
bool proto_pkt_base_parse(struct pkt_inf *pkt) {
	char err;

	if (pkt->basehead.cmd > 11) {
		return false;
	}
	if (((pkt->basehead.sort == SORT_MANAGEAPP
			&& (pkt->basehead.opt & OPT_RESPONSE) > 0))
			|| pkt->basehead.sort == SORT_PLATSERVER) {
		if (pkt->basehead.sid <= 0) {
			return false;
		}
	}
	//扩展首部解析
	opt_parse(pkt);

	//进入连通测试报文
	if (pkt->basehead.cmd == ZNAFCMD_TEST) {
	}

	if (pkt->basehead.sort == SORT_PLATSERVER) {
		pkt->own_node = NULL;
	} else if (pkt->basehead.sort == SORT_MANAGEAPP) {
		if (pkt->basehead.sid != 0) {
			if ((pkt->own_node = relater_get_by_id(pkt->basehead.sid))
					== NULL) {
				pkt->own_node = NULL;
			} else {
				if (update_user_info(pkt) == false)
					return false;
			}
		} else {
			pkt->own_node = NULL;
		}
	}

	if ((err = is_no_keeplive(pkt)) == false)
		return false;
	else if (err == abandon)
		return abandon;

	if (pkt->basehead.sort != SORT_PLATSERVER) {
		if (pkt->basehead.cmd == ZNAFCMD_LOGIN
				&& pkt->basehead.type == TYPE_LOGIN_QUEST) {
			//进入登录请求报文处理
			proto_login_request_parse(pkt);
		} else if (pkt->basehead.cmd == ZNAFCMD_FIND
				&& pkt->basehead.type == TYPE_FIND_QUEST) {
			//进入局域网发现请求处理
			proto_find_request_parse(pkt);
		}
	}

	if (proto_specific(pkt) == false) {
		return false;
	}
	return true;
}

static bool proto_specific(struct pkt_inf *pkt) {
	bool ret;

	switch (pkt->basehead.cmd) {
	case ZNAFCMD_LOGIN://连接管理协议
		ret = proto_login_parse(pkt);
		break;

	case ZNAFCMD_ACCOUNT://平台帐号管理协议
		ret=proto_platman_parse(pkt);
		break;

	case ZNAFCMD_CONTROL://远程管理协议
		ret=proto_remctl_parse(pkt);
		break;

	case ZNAFCMD_QUERY://数据查询协议
		ret=proto_query_parse(pkt);
		break;

	case ZNAFCMD_DELIVER://消息传递协议
		ret=proto_deliver_parse(pkt);
		break;

	case ZNAFCMD_FILE://文件传输协议
		ret=proto_filetransmit_parse(pkt);
		break;

	case ZNAFCMD_ENCODE://加密协议

		break;

	case ZNAFCMD_RELATE://关联协议
		ret=proto_synch_parse(pkt);
		break;

	case ZNAFCMD_UPDATE://系统更新协议
		ret=proto_upgrade_parse(pkt);
		break;

	default:
		ret = false;
		break;
	}
	return ret;
}

static bool is_no_keeplive(struct pkt_inf *pkt) {
	relater_inf *user = (relater_inf *) (pkt->own_node);

	if (pkt->basehead.cmd == 1
			&& (pkt->basehead.type == TYPE_LOGIN_KEEPQUEST
					|| pkt->basehead.type == TYPE_LOGIN_KEEPREPLY)) {
		if (pkt->basehead.sort == SORT_PLATSERVER) {
			if (protodata->platinf.recvseq > pkt->basehead.sendseq) {
				return abandon;
			} else if (protodata->platinf.recvseq == pkt->basehead.sendseq) {
				//删除等待队列报文
			} else if (protodata->platinf.recvseq < pkt->basehead.sendseq) {
				protodata->platinf.recvseq = pkt->basehead.sendseq;
				protodata->platinf.recvtime = time(NULL);
			}
		} else if (pkt->basehead.sort == SORT_MANAGEAPP) {
			if (user->recvseq == pkt->basehead.sendseq) {
				//表示收到重复请求，则重发等待队列的应答报文，处理结束。
			} else if (user->recvseq < pkt->basehead.sendseq) {
				user->recvseq = pkt->basehead.sendseq;
				//删除等待队列报文
			}
		}
	}
	return true;
}

static bool update_user_info(struct pkt_inf *pkt) {
	relater_inf *user = (relater_inf *) (pkt->own_node);

	if (pkt->own_node != NULL) {
		user->recvtime = pkt->recvtime;
		user->addr = pkt->peeraddr;
	}
	return true;
}

static bool opt_parse(struct pkt_inf *pkt) {
	char *str = NULL;

	if (pkt == NULL) {
		return false;
	}

	//表示此报文在服务器进行转发处理
	if ((pkt->basehead.opt & OPT_TRANSFER) > 0) {
		str = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_HOPTTAG,
				PROTO_HOPT_DID);
	}

	//表示此报文加密处理过
	if ((pkt->basehead.opt & OPT_ENCRYPT) > 0) {
		str = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_HOPTTAG,
				PROTO_HOPT_KEYSEQ);
		pkt->keyseq = atoi(str);
	}

	//表示此报文携带源地址，便于NAT穿透处理
	if ((pkt->basehead.opt & OPT_SOURADDR) > 0) {
		opt_nat(pkt);
	}

	return true;
}

static void opt_nat(struct pkt_inf *pkt) {
	char *ip = NULL;
	int port;

	pkt->souraddr.sin_family = AF_INET;
	ip = json_get_string(pkt->buff + DATA_HEADLEN, PROTO_HOPTTAG,
			PROTO_HOPT_SIP);
	pkt->souraddr.sin_addr.s_addr = inet_addr(ip);
	port = json_get_int(pkt->buff + DATA_HEADLEN, PROTO_HOPTTAG,
			PROTO_HOPT_SPORT);
	pkt->souraddr.sin_port = htons(port);
}
