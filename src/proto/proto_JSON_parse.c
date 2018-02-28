#include "proto.h"

char *json_get_string(char *str, const char *opt, const char *opt1) {
	cJSON *json_root = NULL;
	cJSON *json_opt = NULL;
	cJSON *json_opt1 = NULL;
	char *valuestring = NULL;
	char *string = NULL;

	if (str == NULL || opt == NULL) {
		return NULL;
	}

	json_root = cJSON_Parse(str);
	if (json_root == NULL) {
		return NULL;
	}

	json_opt = cJSON_GetObjectItem(json_root, opt);
	if (json_opt == NULL) {
		return NULL;
	}

	if (opt1 == NULL) {
		string = json_opt->valuestring;
		valuestring = calloc(1, strlen(string));
		strcpy(valuestring, string);
	} else {
		json_opt1 = cJSON_GetObjectItem(json_opt, opt1);
		if (json_opt1 == NULL) {
			return NULL;
		}
		string = json_opt1->valuestring;
		valuestring = calloc(1, strlen(string));
		strcpy(valuestring, string);
	}
	cJSON_Delete(json_root);
	return valuestring;
}

int json_get_int(char *str, const char *opt, const char *opt1) {
	cJSON *root = NULL;
	cJSON *json_opt = NULL;
	cJSON *json_opt1 = NULL;
	int valueint = -10;

	if (str == NULL || opt == NULL) {
		return -10;
	}

	root = cJSON_Parse(str);
	if (root == NULL) {
		return -10;
	}

	json_opt = cJSON_GetObjectItem(root, opt);
	if (json_opt == NULL) {
		return -10;
	}

	if (opt1 == NULL) {
		valueint = json_opt->valueint;
	} else {
		json_opt1 = cJSON_GetObjectItem(json_opt, opt1);
		if (json_opt1 == NULL) {
			return -1;
		}
		valueint = json_opt1->valueint;
	}
	cJSON_Delete(root);
	return valueint;
}

char *json_print(char *str, const char *opt, const char *opt1) {
	cJSON *json_root = NULL;
	cJSON *json_opt = NULL;
	cJSON *json_opt1 = NULL;
	char *string = NULL;
	char *valuestring = NULL;

	if (str == NULL || opt == NULL) {
		return NULL;
	}

	json_root = cJSON_Parse((char *) str);
	if (json_root == NULL) {
		return NULL;
	}

	json_opt = cJSON_GetObjectItem(json_root, opt);
	if (json_opt == NULL) {
		return NULL;
	}

	if (opt1 == NULL) {
		string = cJSON_PrintUnformatted(json_opt);
		valuestring = calloc(1, strlen(string) + 2);
		strcpy(valuestring, string);
		free(string);
	} else {
		json_opt1 = cJSON_GetObjectItem(json_opt, opt1);
		if (json_opt1 == NULL) {
			return NULL;
		}
		string = cJSON_PrintUnformatted(json_opt1);
		valuestring = calloc(1, strlen(string) + 2);
		strcpy(valuestring, string);
		free(string);
	}
	cJSON_Delete(json_root);
	return valuestring;
}
