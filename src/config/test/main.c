#include <stdio.h>
#include "config.h"

int main_callback(int sendid, int recvid, int type, void *input, void *output);
void menu() {
  //得到要发送的信息字符串
  puts("\n选择要进行的操作:");
  puts("1: 更新一个配置");
  puts("2: 读取选择的配置项");
  puts("3: 显示所有配置内容");
  puts("4: 重新写配置文件内容");
  puts("5:添加一个配置项");
  /*puts("6: 1：查询消耗内存数	消耗内存字节（int）");
  puts("7: 2：查询线程数	线程数");
  puts("8: 3：查询打开文件数	打开文件数");
  puts("9: 4：查询打开套接字数	打开套接字数");
  puts("10: 1：查询线程具体信息");
  puts("11: 2：查询打开文件信息");
  puts("12: 3：查询打开套接字信息");*/
  puts("0: 退出程序");
  puts("请输入选择：");
}

int main() {
  int ret = 0, c, i = 0;
  struct ModuleInfo modinf;
  // char Buffer[40];
  KeyValue keyvalue;
  KeyValueArray keyvaluearay;
  char key[] = {"IPADDR"}, value[12];

  memset(&modinf, 0, sizeof(struct ModuleInfo));

  ret = config_init(NULL, main_callback, &modinf);
  if (ret == -1) {
    printf("初始化失败，程序退出\n");
    return -1;
  }

  while (1) {
    menu();
    scanf("%d", &c);
    if(c>12&&c<0)
    {
      fflush(stdin);
      continue;
    }
    //system("clear");
    switch (c) {
    case 1:
      keyvalue.key = "IPADDR";
      keyvalue.value = "192.168.1.121";
      keyvalue.keylen = 6;
      keyvalue.valuelen = 13;
      if (config_control(0, CFGCTL_UPDATE_ONE, &keyvalue, NULL) == 0)
        printf("更新一个配置完成(%s=%s;keylen=%d;valuelen=%d)\n", keyvalue.key,
               keyvalue.value, keyvalue.keylen, keyvalue.valuelen);
      else
        puts("更新一个配置失败！");
      break;

    case 2:
      if (config_control(0, CFGCTL_GET_ONE, key, value) == 0)
        printf("%s=%s\n", key, value);
      else
        puts("获取失败！");
      break;

    case 3:
      if (config_control(0, CFGCTL_READ_CFG, NULL, &keyvaluearay) == 0)
        for (i = 0; i < keyvaluearay.count; i++) {
          printf("%-10s=%-15s\t", keyvaluearay.keyValueList[i].key,
                 keyvaluearay.keyValueList[i].value);
          printf("keylen=%-d\t", keyvaluearay.keyValueList[i].keylen);
          printf("valuelen=%-d\n", keyvaluearay.keyValueList[i].valuelen);
        }
      break;

    case 4:
      keyvaluearay.count = 1;
      keyvaluearay.cfgbuf = (char *)calloc(1, 1024);
      keyvaluearay.keyValueList = (KeyValue *)calloc(1, sizeof(KeyValue));
      memcpy(keyvaluearay.cfgbuf, "IPADDR\0 192.168.1.23\0", 24);
      keyvaluearay.keyValueList[0].key = keyvaluearay.cfgbuf;
      keyvaluearay.keyValueList[0].value = keyvaluearay.cfgbuf + 8;
      keyvaluearay.keyValueList[0].keylen =
          strlen(keyvaluearay.keyValueList[0].key);
      keyvaluearay.keyValueList[0].valuelen =
          strlen(keyvaluearay.keyValueList[0].value);
      config_control(0, CFGCTL_NEW_CFG, (void *)&keyvaluearay, NULL);
      break;
    case 5:
    keyvalue.key = "DEVICEID";
    keyvalue.value = "277";
    keyvalue.keylen = 6;
    keyvalue.valuelen = 13;
    if(cfg_add_one(&keyvalue)==-1)
    {
      printf("已有该配置项!\n");
    }
      break;
    case 6:

      break;
    case 7:

      break;
    case 8:

      break;
    case 9:

      break;
    case 10:

      break;
    case 11:

      break;
    case 12:

      break;

    case 0:
      modinf.close();
      exit(1);
      break;
    default:

    break;
    }
  }

  modinf.close();

  return 0;
}

int main_callback(int sendid, int recvid, int type, void *input, void *output) {
  int ret = 0;

  printf("callback is called!\n");
  printf("sendid=%d recvid=%d type=%d\n", sendid, recvid, type);

  return ret;
}


////////////////
