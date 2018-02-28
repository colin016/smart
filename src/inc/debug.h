#ifndef MYDEBUG_H_
#define MYDEBUG_H_
#define DEBUG
#ifdef DEBUG
#define PLOG(fmt,...) printf("[debug info:%s-%d line]>>"fmt ,__FILE__,__LINE__, ##__VA_ARGS__)
#else
#define PLOG(fmt, ...)
#endif

#endif
