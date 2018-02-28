#include "proto.h"

extern struct proto_data *protodata;

bool openfilelist_add(struct file_node* filenode) {
	proto_mutex_lock(&protodata->filelist_lock);
	struct file_node *p = protodata->filelist;
	bool ret = false;

	if (filenode == NULL) {
		ret = false;
	} else {
		filenode->next = NULL;

		if (p == NULL) {
			protodata->filelist = filenode;
		} else {
			while (p->next != NULL) {
				p = p->next;
			}
			p->next = filenode;
		}
		ret = true;
	}
	proto_mutex_unlock(&protodata->filelist_lock);
	return ret;
}

bool openfilelist_del(char *filename) {
	proto_mutex_lock(&protodata->filelist_lock);
	struct file_node *p = protodata->filelist;
	struct file_node *q = NULL;
	bool ret = false;
	if (filename == NULL) {
		ret = false;
	} else {
		ret = false;
		while (p != NULL) {
			if (strcmp(filename, p->node.filename) == 0) {
				if (q == NULL) {
					protodata->filelist = NULL;
				} else {
					q->next = p->next;
				}
				free(p);
				ret = true;
				break;
			}
			q = p;
			p = p->next;
		}
	}
	proto_mutex_unlock(&protodata->filelist_lock);
	return ret;
}

struct file_node *openfilelist_find(char *filename) {
	proto_mutex_lock(&protodata->filelist_lock);
	struct file_node *p = protodata->filelist;
	struct file_node *q = NULL;

	if (filename == NULL) {
		q = NULL;
	} else {
		q = NULL;
		while (p != NULL) {
			if (strcmp(filename, p->node.filename) == 0) {
				q = p;
				break;
			}
			p = p->next;
		}
	}
	proto_mutex_unlock(&protodata->filelist_lock);
	return q;
}
