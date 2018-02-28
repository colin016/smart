#include "proto.h"

//定时器处理线程
void *proto_timer(void *arg) {
	long long cur_time = 0;
	time_t tm;
	while (1) {
		sleep(1);
		tm = time(NULL);
		cur_time++;
		check_isresend(tm);
		check_iskeep(tm);
		check_islogin(tm);
		check_filedownload_from_server(tm);
		check_filedownload_to_app(tm);
		//check_update(tm);
		check_relaters(tm);
	}
	return NULL;
}
