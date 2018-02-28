#include "dllif.h"

int mutex_lock(pthread_mutex_t *mutex)
{
     return pthread_mutex_lock(mutex);
}

int mutex_unlock(pthread_mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}