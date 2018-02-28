#include "dev.h"

extern struct dev_data *devdata;

int query_module_info(void *input,void *output)
{
  switch(*(int*)input)
  {
      case QUERY_MODULE_STASUS:

      break;
      case QUERY_SPEND_MEMORY:
      break;
      case QUERY_THREAD_NUM:
      break;
      case QUERY_FILE_NUM:
      break;
      case QUERY_SOCKET_NUM:
      break;
  }
  return 0;
}

int query_detailed_info(void *input,void *output)
{
  switch (*(int*)input)
  {
    case QUERY_DETAILED_THREAD:
    break;
    case QUERY_FILES_INFO:
    break;
    case QUERY_SOCKETS_INFO:
    break;
  }
  return 0;
}
