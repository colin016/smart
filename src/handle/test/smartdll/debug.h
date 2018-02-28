#ifndef MYDEBUG_H_
#define MYDEBUG_H_

#define DEBUG
#ifdef DEBUG
#define DBG(fmt,...) printf("%s %d;" fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else 
#define DBG(fmt, ...) 
#endif

#endif
