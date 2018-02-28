#include "proto.h"

//模块全局变量声明
extern struct proto_data *protodata;

static bool relaterList_write_to_file();

bool relaterList_initialize(struct relater_inf *relnode) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	struct relater_inf *p = protodata->relaterlist.head;

	if (relnode == NULL)
		return false;

	if (protodata->cfg.maxnum <= protodata->relaterlist.size) {
		PLOG("The relater`s list too many\n");
		return -1;
	}
	relnode->next = NULL;

	if (p == NULL) {
		protodata->relaterlist.head = relnode;
		p = relnode;
		protodata->relaterlist.size += 1;
	} else {
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = relnode;
		protodata->relaterlist.size += 1;
	}
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return true;
}

//向关联人链表添加节点
bool relaterList_add(struct relater_inf *relnode) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	struct relater_inf *p = protodata->relaterlist.head;

	if (relnode == NULL)
		return -1;

	if (protodata->cfg.maxnum <= protodata->relaterlist.size) {
		PLOG("The relater`s list too many\n");
		return -1;
	}
	relnode->next = NULL;

	if (p == NULL) {
		protodata->relaterlist.head = relnode;
		p = relnode;
		protodata->relaterlist.size += 1;
	} else {
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = relnode;
		protodata->relaterlist.size += 1;
	}
	relaterList_write_to_file();
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return true;
}

//删除链表的节点
bool relaterList_delete(char *relaterName) {
	proto_mutex_lock(&protodata->relaterlist.mutex);

	relaterList_write_to_file();
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return true;
}

//修改
bool relaterList_modify(struct relater_inf relnode) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	relaterList_write_to_file();
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return true;
}

struct relater_inf *relater_get_by_id(int id) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	struct relater_inf *p = protodata->relaterlist.head;

	while (p != NULL) {
		if (p->id == id) {
			break;
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return p;
}

struct relater_inf *relater_get_by_name(char *username) {
	proto_mutex_lock(&protodata->relaterlist.mutex);
	struct relater_inf *p = protodata->relaterlist.head;
	if (username == NULL) {
		p = NULL;
		proto_mutex_unlock(&protodata->relaterlist.mutex);
		return p;
	}
	while (p != NULL) {
		if (strcmp(username, p->username) == 0) {
			break;
		}
		p = p->next;
	}
	proto_mutex_unlock(&protodata->relaterlist.mutex);
	return p;
}

static bool relaterList_write_to_file() {
	relater_inf *p = protodata->relaterlist.head;
	cJSON *root = NULL;
	cJSON *relater = NULL;
	FILE *fp;
	char *str;

	root = cJSON_CreateObject();
	relater = cJSON_CreateArray();

	while (p != NULL) {
		cJSON *user = cJSON_CreateObject();
		cJSON_AddNumberToObject(user, "id", time(NULL));
		cJSON_AddStringToObject(user, "username", p->username);
		cJSON_AddStringToObject(user, "password", p->password);
		cJSON_AddNumberToObject(user, "regtime", p->regtime);
		cJSON_AddNumberToObject(user, "right", p->right);
		cJSON_AddNumberToObject(user, "status", p->regstatus);
		cJSON_AddItemToArray(relater, user);
		p = p->next;
	}
	cJSON_AddItemToObject(root,"relaters", relater);

	fp = fopen("../sys/auth.txt", "w");
	if (fp == NULL) {
		PLOG("打开../sys/auth.txt失败\n");
		return false;
	} else {
		str=cJSON_Print(root);
		fprintf(fp,"%s",str);
		if(str!=NULL)
		{
			free(str);
			free(root);
		}
		fclose(fp);
	}
	return true;
}
