#include "sys.h"
#include "proto.h"

//模块全局变量声明
struct proto_data *protodata = NULL;

//设置默认配置
int get_defaultcfg(void) {
	struct proto_config *config;
	config = &protodata->cfg;
	strcpy(config->devname, "user");
	strcpy(config->password, "12345678");
	strcpy(config->barcode, "1234567890ABC");
	strcpy(config->identity, "7D503LV100R002208B011");
	config->maxnum = 10;
	config->longitude = 104.06;
	config->latitude = 30.67;
	config->isshow = FALSE;
	config->isauto = FALSE;
	config->iscanceal = FALSE;
	config->isfind = TRUE;
	config->isupdate = TRUE;
	config->issock = TRUE;
	strcpy(config->ipaddr, PROTO_SERVERIP);
	config->remport = PROTO_PORT;
	config->locport = PROTO_PORT;
	config->deviceid = 0;
	strcpy(config->sysdir, SYSDIR);
	strcpy(config->plugin, "plugin");
	strcpy(config->updatedir, UPDATEDIR);
	strcpy(config->relate, "relate");
	strcpy(config->alarm, "alarm");
	strcpy(config->control, "control");

	return true;
}

//对本模块进行初始化处理，由主框架模块调用
int proto_init(struct KeyValueArray *cfg, moduleCallback callback,
		struct ModuleInfo *ret) {

	if (cfg == NULL || ret == NULL)
		return false;

	//首先给全局变量分配空间，清空。
	protodata = (struct proto_data *) calloc(1, sizeof(struct proto_data));
	if ((NULL == protodata)) {
		return false;
	}

	//初始化ret指针
	if (init_moduleInfo(ret) == false)
		return false;

	//初始化配置，然后初始化网络和定时器，最后填写返回结构
	get_defaultcfg();

	/*初始化本模块信息*/
	if (init_moduleConfig(cfg) == -1) {
		PLOG("Initial of platinf failed!\n");
		proto_free();
		return false;
	} else {
		printf("Initial of platinf successfully!\n");
	}

	//打开套接字
	protodata->sockfd = init_createSocket();
	if (protodata->sockfd < 0) {
		PLOG("socket创建失败");
		proto_free();
		return false;
	}

	//初始化全局变量
	if (init_protodata(callback) == false) {
		return false;
	}

	//初始化平台信息
	if (init_platinfo() == false) {
		return false;
	}

	if (init_relater() == -1) {
		PLOG("Initial of relater failed!\n");
		return false;
	} else {
		printf("Initial of relater successfully!\n");
	}

	printf("proto_init finish!!\n");
	return true;
}

//模块控制，由其他模块调用本模块进行工作的接口
int proto_control(int sendid, int type, void *input, void *output) {
	int ret;

	PLOG("type = %d\n", type);
	switch (type) {
	case PROTO_CONTROL_ALARM: //请求发送新报警消息

		break;
	case PROTO_CONTROL_RESULT: //通告外设控制结果

		break;
	case PROTO_CONTROL_GETCFG: //获取当前配置信息

		break;
	case PROTO_CONTROL_SETCFG: //设置当前配置信息

		break;

	case PROTO_CONTROL_REGISTER: //请求注册,修改设备信息
		ret = proto_register_request_start((register_data*) input, output);
		break;

	case PROTO_CONTROL_MODIFY: //修改设备信息
		ret = proto_accmod_request_start((register_data*) input, output);
		break;

	case PROTO_CONTROL_LOGOUT: //请求注销
		ret=proto_acckill_request_start(output);
		break;

	case PROTO_CONTROL_LOGIN: //登录
		ret = proto_login_request_start(UI, (char*) output);
		break;

	case PROTO_CONTROL_LOGINOUT: //登出
		proto_loginout_request_start();
		break;

	case PROTO_CONTROL_UPGRADE: //软件更新

		break;

	case PROTO_CONTROL_GETRELATERNUM: //获取关联人数量

		break;

	case PROTO_CONTROL_RELATERLIST: //关联人列表（根据数量开缓冲区，获取联系人数组）

		break;

	case PROTO_CONTROL_RELATERAUDIT: //审核关联人

		break;

	case PROTO_CONTROL_RELATERMAN: //关联人管理

		break;

	case PROTO_CONTROL_LOGEDINRELATER: //登录关联人详情

		break;

	case PROTO_CONTROL_CTLDEVICE: //远程控制

		break;

	case PROTO_CONTROL_UPDATEPLUGIN: //更新插件

		break;

	case PROTO_CONTROL_PLATCFG: //平台配置

		break;

	case PROTO_CONTROL_FINDSETTING: //局域网发现设置

		break;

	case PROTO_CONTROL_FINDQUERY: //局域网发现查看

		break;
	case PROTO_CONTROL_SYNCTIME: //平台同步设置(设置平台同步时间间隔)

		break;

	case PROTO_CONTROL_SYNCTNOW: //立即进行平台同步

		break;

	case PROTO_CONTROL_GETALARMNUM: //获取报警消息数量

		break;

	case PROTO_CONTROL_ALARMQUERY: //查看报警消息管理

		break;

	case PROTO_CONTROL_ALARMDEL: //删除一条报警消息

		break;

	case PROTO_CONTROL_ALARMCLREAN: //清空报警消息

		break;

	case PROTO_CONTROL_IFLOGIN://界面登录
		ret=proto_interface_login((interface_login*)input,output);
		break;

	default:
		printf("正在建设中......\n");
		break;
	}
	return ret;
	//return true;
	//来自进程间通信模块：
	//收到设置信息修改配置信息或者启动协议处理

	//来自响应管理模块：
	//启动推送报警信息给服务器和登录的关联人

	//来自外设管理模块
	//启动控制协议后处理进行回复
}

//模块关闭处理，由主框架main模块调用
int proto_close(void) {
	//释放各种资源：内存、线程、锁等
	if (protodata) {
		if (protodata->timer_run) //退出已创建定时器线程
			protodata->timer_run = 0;

		if (protodata->nethandle_run)
			protodata->nethandle_run = 0;

		if (protodata->sockfd > 0) {
			close(protodata->sockfd);
			pthread_mutex_destroy(&protodata->data_lock);
		}

		sleep(1); //等待一秒钟，让所有线程退出

		proto_free(); //释放protodata中所有全局链表内容

		free(protodata);
		protodata = NULL;
	}
	return true;
}

//对模块的配置进行操作，具体定义如下
//功能要求			type	cfg
//设置修改多个配置项	1	KeyValueArray
//修改配置项为默认值	2	NULL
//查询配置项的当前值	3	KeyValueArray(模块重新申请内存？）
//查询配置项的默认值	4	NULL(模块申请内存？）
int proto_config(int type, void *cfg) {
	return true;
}

//提供给主模块的接口
int proto_query(int type, void *input, void *output) {
	return true;
}

//释放所有列表
void proto_free(void) {
	free(protodata);
	protodata = NULL;

	return;
}
