#include "proto.h"

//互斥锁加锁
int proto_mutex_lock(pthread_mutex_t *mutex) {
	return pthread_mutex_lock(mutex);
}

//互斥锁解锁
int proto_mutex_unlock(pthread_mutex_t *mutex) {
	return pthread_mutex_unlock(mutex);
}
