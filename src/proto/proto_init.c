#include "proto.h"
#include "sys.h"

extern struct proto_data *protodata;

/*读取关联人配置文件*/
static char *init_readRelaterFile();
/*解析每个关联人的配置*/
static int init_Relaterparse(cJSON *relater_item);

bool init_protodata(moduleCallback callback) {
	int err;
	srand((unsigned) time(NULL));

	//初始化关联人链表互斥锁
	pthread_mutex_init(&protodata->relaterlist.mutex, NULL);

	pthread_mutex_init(&protodata->filelist_lock, NULL);

	pthread_mutex_init(&protodata->data_lock, NULL);

	//启动网络数据接收线程
	err = pthread_create(&protodata->nethandle_thread, NULL, proto_nethandle,
			&protodata->sockfd);
	if (err != 0) {
		proto_free();
		return false;
	} else {
		protodata->nethandle_run = 1;
	}

	//启动定时器处理线程
	err = pthread_create(&protodata->timer_thread, NULL, proto_timer, NULL);
	if (err != 0) {
		proto_free();
		return false;
	} else {
		protodata->timer_run = 1;
	}

	protodata->callback = callback;
	protodata->sequence = rand() % 100 + 1;
	return true;
}

//初始化平台信息
bool init_platinfo(void) {
	//初始化平台信息
	protodata->platinf.sendseq = 0;
	protodata->platinf.loginstatus = INIT_STATE;
	protodata->platinf.waitlist = NULL;
	protodata->platinf.server_addr.sin_family = AF_INET;
	protodata->platinf.server_addr.sin_port = htons(protodata->cfg.remport);
	protodata->platinf.server_addr.sin_addr.s_addr = inet_addr(
			protodata->cfg.ipaddr);

	if (pthread_mutex_init(&protodata->platinf.lock, NULL) != 0)
		return false;
	if (pthread_cond_init(&protodata->platinf.cond, NULL) != 0)
		return false;
	if (pthread_mutex_init(&protodata->platinf.waitlist_lock, NULL) != 0)
		return false;

	return true;
}

/*初始化本模块信息*/
bool init_moduleInfo(struct ModuleInfo *ret) {
	// 模块类型，0表示模块是静态的，1还是可动态升级的。静态的模块只能与主模块一起升级。动态模块可独立升级。
	//填充ret结构
	strcpy(ret->name, MODULE_NAME);
	ret->type = 0;
	ret->id = ret->maincode = MODULE_CONTROL;
	ret->subcode = 0;
	ret->version = MODULE_VERSION;
	ret->close = proto_close;
	ret->control = proto_control;
	ret->query = proto_query;
	ret->init = proto_init;
	ret->config = proto_config;
	return true;
}

/*创建scoket*/
int init_createSocket(void) {
	int sockfd = -1;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(protodata->cfg.locport);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
		return false;

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		return false;
	}

	return sockfd;
}

/*初始化本模块的配置*/
bool init_moduleConfig(struct KeyValueArray *cfg) {
	int i = 0;
	const int count = cfg->count;
	char flag[count];

	memset(flag, 0, count);
	for (i = 0; i < cfg->count; i++) {
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_DEVNAME) == true) {
			strcpy(protodata->cfg.devname, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_PASSWORD) == true) {
			strcpy(protodata->cfg.password, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_BARCODE) == true) {
			strcpy(protodata->cfg.barcode, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_IDENTITY) == true) {
			strcpy(protodata->cfg.identity, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_IPADDR) == true) {
			strcpy(protodata->cfg.ipaddr, cfg->keyValueList[i].value);
			protodata->cfg.ipaddr[strlen(protodata->cfg.ipaddr)] = '\0';
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_PLUGIN) == true) {
			strcpy(protodata->cfg.plugin, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_UPDATEDIR) == true) {
			strcpy(protodata->cfg.updatedir, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_RELATE) == true) {
			strcpy(protodata->cfg.relate, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ALARM) == true) {
			strcpy(protodata->cfg.alarm, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_CONTROL) == true) {
			strcpy(protodata->cfg.control, cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_MAXNUM) == true) {
			protodata->cfg.maxnum = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_LONGITUDE) == true) {
			protodata->cfg.longitude = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_LATITUDE) == true) {
			protodata->cfg.latitude = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ISSHOW) == true) {
			protodata->cfg.isshow = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ISAUTO) == true) {
			protodata->cfg.isauto = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ISACNCEL) == true) {
			protodata->cfg.iscanceal = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ISFIND) == true) {
			protodata->cfg.isfind = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_ISUPDATE) == true) {
			protodata->cfg.isupdate = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_REMPORT) == true) {
			protodata->cfg.remport = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_LOCPORT) == true) {
			protodata->cfg.locport = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
		if (strcmp(cfg->keyValueList[i].key, PROTO_CFG_DEVICEID) == true) {
			protodata->cfg.deviceid = atoi(cfg->keyValueList[i].value);
			flag[i] = 1;
		}
	}
	//protodata->cfg.deviceid = 100;   //设备没有注册时为0
	protodata->cfg.issock = false; //网络初始化时没有连通
	strcpy(protodata->cfg.sysdir, SYSDIR);

	// for (i = 0; i < cfg->count; i++) {
	//   if (flag[i] != 1)
	//     return false;
	// }

	return true;
}

bool init_relater(void) {
	int relater_num = -1;
	int i = -1;
	char *string = NULL;
	cJSON *root = NULL;
	cJSON *relaters = NULL;
	cJSON *relater_item = NULL;

	string = init_readRelaterFile();
	if (string == NULL) {
		//PLOG("reading relater file failed\n");
		return false;
	} else {
		//PLOG("%s", string);
	}

	root = cJSON_Parse(string);
	if (root == NULL) {
		//  PLOG("json : root=NULL\n");
		return false;
	}

	relaters = cJSON_GetObjectItem(root, JSON_RELATERS);
	if (relaters == NULL) {
		//PLOG("The json-file of relater parse failure!\n");
		return false;
	}

	relater_num = cJSON_GetArraySize(relaters);
	if (relater_num < 0) {
		///PLOG("Getting the nuber of the relater unsuccessfully from json-file!\n");
		return false;
	}

	for (i = 0; i < relater_num; i++) {
		relater_item = cJSON_GetArrayItem(relaters, i);
		if (init_Relaterparse(relater_item) == false) {
			//  PLOG("Parsing a item of the relater unsuccessfully!\n");
			return false;
		}
	}
	return true;
}

/*读取关联人配置文件*/
static char *init_readRelaterFile() {
	int strlength = 0;
	int err = -1;
	FILE *fp = NULL;
	char *string = NULL;

	fp = fopen("../sys/auth.txt", "r");

	if (fp == NULL) {
		return NULL;
	}
	//设置文件指针指向文件的末尾
	fseek(fp, 0, SEEK_END);
	//获得文件的大小
	strlength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	string = (char *) calloc(1, strlength + 1);
	if (string == NULL)
		return NULL;
	err = fread(string, 1, strlength, fp);
	if (strlength != err)
		return NULL;

	fclose(fp);
	return string;
}
/*解析每个关联人的配置*/
static int init_Relaterparse(cJSON *relater_item) {
	cJSON *id = NULL;
	cJSON *username = NULL;
	cJSON *password = NULL;
	cJSON *regtime = NULL;
	cJSON *regstatus = NULL;
	cJSON *right = NULL;
	struct relater_inf *relaternode = NULL;

	id = cJSON_GetObjectItem(relater_item, JSON_ID);
	if (id == NULL) {
		///PLOG("id==NULL\n");
		return -1;
	}
	username = cJSON_GetObjectItem(relater_item, JSON_USERNAME);
	if (username == NULL) {
		//PLOG("username==NULL\n");
		return -1;
	}
	password = cJSON_GetObjectItem(relater_item, JSON_PASSWORD);
	if (password == NULL) {
		//PLOG("password==NULL\n");
		return -1;
	}
	regtime = cJSON_GetObjectItem(relater_item, JSON_REGTIME);
	if (regtime == NULL) {
		//PLOG("regtime==NULL");
		return -1;
	}
	regstatus = cJSON_GetObjectItem(relater_item, JSON_STATUS);
	if (regstatus == NULL) {
		//PLOG("status==NULL\n");
		return -1;
	}
	right = cJSON_GetObjectItem(relater_item, JSON_RIGHT);
	if (right == NULL) {
		//PLOG("authrity==NULL\n");
		return -1;
	}
	//为关联人节点开辟内存空间
	relaternode = (struct relater_inf *) calloc(1, sizeof(struct relater_inf));
	strcpy(relaternode->username, username->valuestring);
	strcpy(relaternode->password, password->valuestring);
	relaternode->id = id->valueint;
	relaternode->regtime = regtime->valueint;
	relaternode->right = right->valueint;
	relaternode->regstatus = regstatus->valueint;
	relaternode->link=-1;
	if (relaterList_initialize(relaternode) == -1)
		return false;
	return true;
}
