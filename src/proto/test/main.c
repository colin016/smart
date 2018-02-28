#include <stdio.h>
#include "config.h"
#include "proto.h"
#include "sys.h"

int main_callback(int sendid, int recvid, int type, void *input, void *output);
void init_register_data(register_data *data);
void init_accmod_data(register_data *data);

int main() {
	int ret = 0, i;
	struct ModuleInfo modinf;
	struct ModuleInfo config_modinf;
	struct KeyValueArray proto_cfg;
	//char Buffer[40];

	memset(&modinf, 0, sizeof(struct ModuleInfo));
	config_init(NULL, main_callback, &config_modinf);
	config_control(0, 3, NULL, (void *) &proto_cfg);
	ret = proto_init(&proto_cfg, main_callback, &modinf);

	if (ret <= -1) {
		printf("初始化失败，程序退出\n");
		return -1;
	}

	while (1) {
		//提示信息
		puts("\n选择要测试的功能:");
		puts("1: 登录到服务器");
		puts("2: 登出服务器");
		puts("3: 前端设备注册");
		puts("4: 前端设备帐号修改");
		puts("5: 前端设备帐号注销");
		puts("6: 消息推送请求");
		puts("7: 升级查询");
		puts("8: 文件传输");
		puts("0: 退出程序\n");

		scanf("%d", &i);

		//执行用户选择
		switch (i) {
		case 1:
			modinf.control(0, PROTO_CONTROL_LOGIN, NULL, NULL);
			break;

		case 2:
			modinf.control(0, PROTO_CONTROL_LOGINOUT, NULL, NULL);
			break;

		case 3: {
			register_data data;
			char info[20] = { "\0" };
			init_register_data(&data);
			modinf.control(0, PROTO_CONTROL_REGISTER, (register_data*) &data,
					info);
			PLOG("register info:%s\n", info);
		}
			break;

		case 4: {
			register_data data;
			char info[20] = { "\0" };
			init_accmod_data(&data);
			modinf.control(0, PROTO_CONTROL_MODIFY, (register_data*) &data,
					info);
			PLOG("register info:%s\n", info);
		}
			break;
		case 5:
			modinf.control(0, PROTO_CONTROL_LOGOUT, NULL, NULL);
			PLOG("Logout suceess\n");
			break;
		case 6:
			proto_deliver_reauest_start();
			break;
		case 7:
			proto_upgrade_request_start();
			break;
		case 8:
			proto_fileinfo_request_start();
			break;
		case 0:
			modinf.control(0, PROTO_CONTROL_LOGINOUT, NULL, NULL);
			modinf.close();
			exit(1);
			break;

		default:
			printf("ERROR!\n");
		}
	}

	//程序退出处理
	modinf.close();

	return 0;
}

int main_callback(int sendid, int recvid, int type, void *input, void *output) {
	int ret = 0;

	printf("callback is called!\n");
	printf("sendid=%d recvid=%d type=%d\n", sendid, recvid, type);
	switch (recvid) {
	case CONFIG:
		ret = config_control(sendid, CALLBACK_CONCTL_ADD, input, output);
		break;
	}

	return ret;
}

void init_register_data(register_data *data) {
	strcpy(data->account, "moon167030");
	strcpy(data->password, "1234567890BC");
	strcpy(data->serial, "QSDGCDS1");
	strcpy(data->code, "12345");
	data->longitude = 20.366547;
	data->latitude = 130.256865;
	data->number = 10;
}

void init_accmod_data(register_data *data) {
	strcpy(data->account, "moon150");
	strcpy(data->password, "1234567890A");
	strcpy(data->serial, "QSDGCDS123");
	strcpy(data->code, "12345");
	data->longitude = 20.366547;
	data->latitude = 159.256865;
	data->number = 10;
}
