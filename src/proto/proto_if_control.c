/*
 * proto_if_control.c
 *
 *  Created on: Aug 6, 2017
 *      Author: moon
 */
#include "proto.h"

extern struct proto_data *protodata;


/*
 * 界面登录
 * 返回值：
 * 	0:验证通过
 * 	-1：密码或帐号错误！
 * 	-2：该帐号没有管理权限！
 * 	-3：没有该帐号
 */
int proto_interface_login(interface_login *data, void *output) {
	relater_inf *p = protodata->relaterlist.head;
	char *msg=(char*)output;
	while (p != NULL) {
		if (strcmp(data->username, p->username) == 0) {
			if (strcmp(data->password, p->password) == 0) {
				if (p->right == RELATER_AUTHORITY_MAN) {
					return 0;
				} else {
					strcpy(msg,"该帐号没有管理权限！");
					return -2;
				}
			} else {
				strcpy(msg,"密码或帐号错误！");
				return -1;
			}
		}
		p = p->next;
	}
	strcpy(msg,"没有该帐号！");
	return -3;
}

