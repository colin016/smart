#include "dev.h"
#include "mxml.h"

//模块全局变量声明
extern struct dev_data *devdata;

//从json字符串中解析出介质所需的配置
static int init_parse_media(struct media_if *node,cJSON *media_item);
static int initmed_net_private(struct media_if *node,cJSON *net_para);
static int initmed_serial_private(struct media_if *node,cJSON *serial_para);
static int initmed_zigbee_private(struct media_if *node,cJSON *serial_para);
//从json字符串中解析出介质所需的配置
static int init_parse_device(struct ext_dev *node,cJSON *device_item);
static int initdev_scm_private(struct ext_dev *node,cJSON *scm_para);
static int parse_device_cmd(mxml_node_t *root);
static int parse_hasattach_cmd(int i,mxml_node_t *cmd,mxml_node_t *root,struct devctl_cmd *devctlcmd);

//初始化设备支持的控制命令
int init_devctlcmd(void)
{
  FILE *fp=NULL;
  mxml_node_t *root=NULL;

  fp=fopen(DEVCTL_CMD_FILE,"r");
  if (fp==NULL)
  {
    return -1;
  }
  root=mxmlLoadFile(NULL,fp,MXML_TEXT_CALLBACK);
  if (root==NULL)
  {
    fclose(fp);
    return -1;
  }
  fclose(fp);
  if (parse_device_cmd(root)==-1)
  {
    mxmlDelete(root);
    return -1;
  }
  mxmlDelete(root);
  return 0;
}

//初始化本模块的配置
int init_conf(struct KeyValueArray *cfg)
{
  int i=0;
  if(cfg!=NULL)
    for (i = 0; i < cfg->count; i++)
      if(cfg->keyValueList!=NULL&&cfg->keyValueList[i].key!=NULL)
      {
        if(!strcmp(cfg->keyValueList[i].key,KEY_MAX_MEDIA_NUM))
        {
          devdata->max_media_num=atoi(cfg->keyValueList[i].value);
        }

        if(!strcmp(cfg->keyValueList[i].key,KEY_MAX_DEVICE_NUM))
        {
          devdata->max_device_num=atoi(cfg->keyValueList[i].value);
        }

        if(!strcmp(cfg->keyValueList[i].key,KEY_TIMEOUT))
        {
          devdata->timeout=atoi(cfg->keyValueList[i].value);
        }
      }
      else
        return -1;
  else
    return -1;
return 0;
}

//给初始化函数的ret指针赋值
int init_ret(struct ModuleInfo *ret)
{
  if(NULL==strcpy(ret->name,CURRENT_MODULE_NAME))
  {
    return -1;
  }
  ret->type=MODULE_TYPE;
  ret->maincode=MODULE_ID;
  //=======================

  //=======================
  ret->init=dev_init;
  ret->control=dev_control;
  ret->config=dev_config;
  ret->query=dev_query;
  ret->close=dev_close;
	return 0;
}

//解析init_read_file（...）函数返回的JOSN字符串
int init_medias_devices(void)
{
  char *json_string=NULL;
  struct media_if *media_node=NULL;
  struct ext_dev *device_node=NULL;
  cJSON *media_item=NULL;
  cJSON *device_item=NULL;
  int mednum=0;
  int devnum=0;
  int i=0;
  int medlen=0;
  int devlen=0;

  json_string=init_read_file();
  if(json_string==NULL)
    return -1;

  cJSON *root=cJSON_Parse(json_string);//容错性处理
  if(root==NULL)
  {
    PLOG("devinit.c line %d:root==NULL\n",__LINE__);
    free(json_string);
    return -1;
  }
  free(json_string);

  cJSON *medias=cJSON_GetObjectItem(root,JSON_MEIDA);
  if(medias==NULL)
  {
    cJSON_Delete(root);
    return -1;
  }
  cJSON *devices=cJSON_GetObjectItem(root,JSON_DEVICE);
  if(devices==NULL)
  {
    cJSON_Delete(root);
    return -1;
  }
  mednum=cJSON_GetArraySize(medias);
  devnum=cJSON_GetArraySize(devices);
  medlen=sizeof(struct media_if);
  devlen=sizeof(struct ext_dev);
  //从JSON中解析出介质
  for ( i = 0; i < mednum; i++)
  {
    media_node=(struct media_if*)calloc(1,medlen);
    media_item=cJSON_GetArrayItem(medias,i);
    init_parse_media(media_node,media_item);
    media_list_init(media_node);
  }
  //从介质中解析出设备
  for(i=0;i<devnum;i++)
  {
      device_node=(struct ext_dev*)calloc(1,devlen);
      device_item=cJSON_GetArrayItem(devices,i);
      init_parse_device(device_node,device_item);
      device_list_init(device_node);
  }
  cJSON_Delete(root);
  return 0;
}

//读取dev.txt文件并返回文件内容
char *init_read_file(void)
{
  FILE *fp=NULL;
  char *string=NULL;
  int strlen=0;
  int err=-1;

  fp=fopen(DEV_CONF_FILE,"r");
  if(fp==NULL)
    return NULL;
  //设置文件指针指向文件的末尾
  fseek(fp,0,SEEK_END);
  //获得文件的大小
  strlen=ftell(fp);
  fseek(fp,0,SEEK_SET);

  string=(char*)malloc(strlen+1);
  if(string==NULL)
    return NULL;
  err=fread(string,1,strlen,fp);
  if(strlen!=err)
    return NULL;

  fclose(fp);
  return string;
}

//从json字符串中解析出介质所需的配置
static int init_parse_media(struct media_if *node,cJSON *media_item)
{
    cJSON *media_id=cJSON_GetObjectItem(media_item,JSON_MEDIA_ID);
    cJSON *media_name=cJSON_GetObjectItem(media_item,JSON_MEDIA_NAME);
    cJSON *media_type=cJSON_GetObjectItem(media_item,JSON_MEDIA_TYPE);
    cJSON *media_devnum=cJSON_GetObjectItem(media_item,JSON_MEDIA_DEVNUM);
    cJSON *media_depend=cJSON_GetObjectItem(media_item,JSON_MEDIA_DEPEND);
    cJSON *media_para=cJSON_GetObjectItem(media_item,JSON_MEDIA_PRIVATE);
    if(media_name==NULL||media_type==NULL||media_devnum==NULL||media_depend==NULL||media_para==NULL)
      return -1;

    strcpy(node->name,media_name->valuestring);
    node->type=media_type->valueint;
    node->devnum=media_devnum->valueint;
    node->depend=media_depend->valueint;
    node->id=media_id->valueint;

    switch (node->type)
    {
      case  MEDIA_SERIAL:
        initmed_serial_private(node,media_para);
        serial_init(node);
        // if(init_serial_private(node,media_para))
        //     return -1;
        // if(serial_init(node)==-1)//初始化该串口
        //   return -1;
      break;
      case  MEDIA_USB:
      break;
      case  MEDIA_ETHERNET:
      initmed_net_private(node,media_para);
      if(net_init(node)==-1)
      {
        fprintf(stderr,"MEDIA_ETHERNET initialize failure\n");
      }
      break;
      case  MEDIA_WIFI:
      break;
      case  MEDIA_ZIGBEE:
      initmed_zigbee_private(node,media_para);
      zigbee_init(node);
      break;
      case  MEDIA_BLUETOOTH:
      break;
      default:
      return -1;
    }

  return 0;
}

//从json字符串中解析出介质所需的配置
static int init_parse_device(struct ext_dev *node,cJSON *device_item)
{
  cJSON *device_type=cJSON_GetObjectItem(device_item,JSON_DEVICE_TYPE);
  if(device_type==NULL)
    return -1;
  cJSON *device_id=cJSON_GetObjectItem(device_item,JSON_DEVICE_ID);
  if(device_id==NULL)
    return -1;
  cJSON *device_name=cJSON_GetObjectItem(device_item,JSON_DEVICE_NAME);
  if(device_name==NULL)
    return -1;
  cJSON *device_location=cJSON_GetObjectItem(device_item,JSON_DEVICE_LOCATION);
  if(device_location==NULL)
    return -1;
  cJSON *device_meia=cJSON_GetObjectItem(device_item,JSON_DEVICE_MEDIA);
  if(device_meia==NULL)
    return -1;
  cJSON *device_data=cJSON_GetObjectItem(device_item,JSON_DEVICE_DATA);
  if(device_data==NULL)
    return -1;

  strcpy(node->devname,device_name->valuestring);
  strcpy(node->location, device_location->valuestring);
  node->dev_type=device_type->valueint;
  node->devid=device_id->valueint;
  node->media=media_get(device_meia->valueint);
  if(node->media==NULL)
  {
    fprintf(stderr,"Device cannot get media\n");
    exit(-1);
  }

  switch(node->dev_type)
  {
    case TYPE_MOTOR:
    
    break;

    case TYPE_SCM:
    if(initdev_scm_private(node,device_data)==-1)
    {
      return -1;
    }
    if(scm_init(node)==-1)
    {
      fprintf(stderr,"SCM initialize failure\n");
      return -1;
    }
    break;
  }

  return 0;
}

static int initmed_net_private(struct media_if *node,cJSON *net_para)
{
  int *port=NULL;

  port=calloc(1,sizeof(int));
  cJSON *json_port=cJSON_GetObjectItem(net_para,JSON_NET_PORT);
  if(json_port==NULL)
  {
    return -1;
  }
  *port=json_port->valueint;
  node->para=port;
  return -1;
}

static int initmed_serial_private(struct media_if *node,cJSON *serial_para)
{
  struct serial_private *para=(struct serial_private*)malloc(sizeof(struct serial_private));
  if(para==NULL)
    return -1;
  cJSON *serial_bandrate=cJSON_GetObjectItem(serial_para,JSON_SERIAL_BANDRATE);
  if(serial_bandrate==NULL)
    return -1;
  cJSON *serial_csize=cJSON_GetObjectItem(serial_para,JSON_SERIAL_CSIZE);
  if(serial_csize==NULL)
    return -1;
  cJSON *serial_cstop=cJSON_GetObjectItem(serial_para,JSON_SERIAL_CSTOP);
  if(serial_cstop==NULL)
    return -1;
  cJSON *serial_verify=cJSON_GetObjectItem(serial_para,JSON_SERIAL_VERIFY);
  if(serial_verify==NULL)
    return -1;
  cJSON *serial_name=cJSON_GetObjectItem(serial_para,JSON_SERIAL_NAME);
  if(serial_name==NULL)
    return -1;

  para->bandrate=serial_bandrate->valueint;
  para->csize=serial_csize->valueint;
  para->cstop=serial_cstop->valueint;
  para->verify=serial_verify->valueint;
  strcpy(para->name,serial_name->valuestring);
  node->para=para;

  return 0;
}

static int initmed_zigbee_private(struct media_if *node,cJSON *serial_para)
{
  struct zigbee_private *para=(struct zigbee_private*)calloc(1,sizeof(struct zigbee_private));
  if(para==NULL)
    return -1;
  cJSON *serial_bandrate=cJSON_GetObjectItem(serial_para,JSON_ZIGBEE_BANDRATE);
  if(serial_bandrate==NULL)
    return -1;
  cJSON *serial_csize=cJSON_GetObjectItem(serial_para,JSON_ZIGBEE_CSIZE);
  if(serial_csize==NULL)
    return -1;
  cJSON *serial_cstop=cJSON_GetObjectItem(serial_para,JSON_ZIGBEE_CSTOP);
  if(serial_cstop==NULL)
    return -1;
  cJSON *serial_verify=cJSON_GetObjectItem(serial_para,JSON_ZIGBEE_VERIFY);
  if(serial_verify==NULL)
    return -1;
  cJSON *serial_name=cJSON_GetObjectItem(serial_para,JSON_ZIGBEE_NAME);
  if(serial_name==NULL)
    return -1;

  para->bandrate=serial_bandrate->valueint;
  para->csize=serial_csize->valueint;
  para->cstop=serial_cstop->valueint;
  para->verify=serial_verify->valueint;
  strcpy(para->name,serial_name->valuestring);
  node->para=para;

  return 0;
}

static int initdev_scm_private(struct ext_dev *node,cJSON *scm_para)
{
  struct scm_private *para=NULL;

  para=calloc(1,sizeof(struct scm_private));
  cJSON *json_type=cJSON_GetObjectItem(scm_para,JSON_TYPE);
  if(json_type==NULL)
  {
    return -1;
  }
  para->type=json_type->valueint;
  if (para->type==JSON_TYPE_ZIGBEE)
  {
    cJSON *json_addr=cJSON_GetObjectItem(scm_para,JSON_ZIGSCM_ADDR);
    if(json_addr==NULL)
    {
    return -1;
    }
    cJSON *json_port=cJSON_GetObjectItem(scm_para,JSON_ZIGSCM_PORT);
    if(json_port==NULL)
    {
    return -1;
    }
    para->private.zig.addr=json_addr->valueint;
    para->private.zig.port=json_port->valueint;
  }else if (para->type==JSON_TYPE_NET)
  {
    cJSON *json_ip=cJSON_GetObjectItem(scm_para,JSON_NETSCM_IP);
    if(json_ip==NULL)
    {
    return -1;
    }
    cJSON *json_port=cJSON_GetObjectItem(scm_para,JSON_NETSCM_PORT);
    if(json_port==NULL)
    {
    return -1;
    }
    strcpy(para->private.net.ip,json_ip->valuestring);
    para->private.net.port=json_port->valueint;
  }else{
    return -1;
  }

   node->data=para;
  return 0;
}

static int parse_device_cmd(mxml_node_t *root)
{
    mxml_node_t *xmldevcmd_node=NULL;
    mxml_node_t *xmlctlcmd_node=NULL;
    const char *c8_num=NULL;
    const char *c8_devid=NULL;
    char xmllabel_name[20];
    int i=0;
    int i32_num;
    int i32_devid=-1;

    xmldevcmd_node=mxmlFindElement(root, root, "device_cmd",NULL, NULL,MXML_DESCEND);
    c8_num=mxmlElementGetAttr(xmldevcmd_node,"number");
    if (c8_num==NULL)
    {
      return -1;
    }
    i32_num=atoi(c8_num);

    for (i = 0; i < i32_num; ++i)
    {
        struct devctl_cmd *devctlcmd=NULL;
        struct ext_dev *pdev=NULL;
        mxml_node_t *name=NULL;
        mxml_node_t *bigtype=NULL;
        mxml_node_t *smalltype=NULL;
        mxml_node_t *no=NULL;
        mxml_node_t *cmd=NULL;
        const char *c8_num=NULL;
        const char *c8_hasattach=NULL;
        char cmd_name[20];
        int i32_num=0;
        int j=0;

        memset(xmllabel_name,0,sizeof(xmllabel_name));
        sprintf(xmllabel_name,"ctlcmd%d",i);
        xmlctlcmd_node=mxmlFindElement(xmldevcmd_node,root,xmllabel_name,NULL,NULL,MXML_DESCEND);
        if (xmlctlcmd_node==NULL)
        {
          return -1;
        }
        c8_hasattach=mxmlElementGetAttr(xmlctlcmd_node,"hasattachment");
        if (c8_hasattach==NULL)
        {
          return -1;
        }
        if (strcmp(c8_hasattach,"true")==0)//有附件的设备
        {

          name=mxmlFindElement(xmlctlcmd_node,root,"name",NULL,NULL,MXML_DESCEND);
          if (name==NULL)
          {
            return -1;
          }
          c8_devid=mxmlElementGetAttr(xmlctlcmd_node,"devid");
          if (c8_devid==NULL)
          {
            return -1;
          }
          bigtype=mxmlFindElement(xmlctlcmd_node,root,"bigtype",NULL,NULL,MXML_DESCEND);
          if (bigtype==NULL)
          {
            return -1;
          }
          smalltype=mxmlFindElement(xmlctlcmd_node,root,"smalltype",NULL,NULL,MXML_DESCEND);
          if (smalltype==NULL)
          {
            return -1;
          }
          no=mxmlFindElement(xmlctlcmd_node,root,"no",NULL,NULL,MXML_DESCEND);
          if (no==NULL)
          {
            return -1;
          }
          cmd=mxmlFindElement(xmlctlcmd_node,root,"cmd",NULL,NULL,MXML_DESCEND);
          if (cmd==NULL)
          {
            return -1;
          }
          c8_num=mxmlElementGetAttr(cmd,"number");
          if (c8_num==NULL)
          {
            return -1;
          }
          i32_devid=atoi(c8_devid);
          pdev=device_get(i32_devid);
          if (pdev==NULL)
          {
            return -1;
          }
          devctlcmd=(struct devctl_cmd*)calloc(1,sizeof(struct devctl_cmd));
          if (devctlcmd==NULL)
          {
            return -1;
          }

          devctlcmd->type=DEVCTLCMD_ATTACH;
          strcpy(devctlcmd->ctl.attachment.attach_name,name->child->value.text.string);
          devctlcmd->ctl.attachment.bigtype=atoi(bigtype->child->value.text.string);
          devctlcmd->ctl.attachment.smalltype=atoi(smalltype->child->value.text.string);
          devctlcmd->ctl.attachment.no=atoi(no->child->value.text.string);
          i32_num=atoi(c8_num);
          devctlcmd->ctl.attachment.cmd_num=i32_num;
          devctlcmd->ctl.attachment.cmd=(struct attachment_cmd*)calloc(1,i32_num*sizeof(struct attachment_cmd));
          for (j = 0; j < i32_num; ++j)
          {
            mxml_node_t *cmd_i=NULL;
            memset(cmd_name,0,sizeof(cmd_name));
            sprintf(cmd_name,"cmd%d",j);
            cmd_i=mxmlFindElement(cmd,root,cmd_name,NULL,NULL,MXML_DESCEND);
            if (cmd_i==NULL)
            {
              free(devctlcmd->ctl.attachment.cmd);
              free(devctlcmd);
              return -1;
            }
            if (parse_hasattach_cmd(j,cmd_i,root,devctlcmd)==-1)
            {
              free(devctlcmd->ctl.attachment.cmd);
              free(devctlcmd);
              return -1;
            }
          }
          pdev->devctlcmd=devctlcmd_add(pdev->devctlcmd,devctlcmd);
       }else
       {
        //暂时没有 没有附件的设备
       }
    }
  return 0;
}

static int parse_hasattach_cmd(int i,mxml_node_t *cmd,mxml_node_t *root,struct devctl_cmd *devctlcmd)
{
  mxml_node_t *ctlno=NULL;
  mxml_node_t *tip=NULL;

  ctlno=mxmlFindElement(cmd,root,"ctlno",NULL,NULL,MXML_DESCEND);
  if (ctlno==NULL)
  {
    return -1;
  }
  tip=mxmlFindElement(cmd,root,"tip",NULL,NULL,MXML_DESCEND);
  if (tip==NULL)
  {
    return -1;
  }
  strcpy(devctlcmd->ctl.attachment.cmd[i].tip,tip->child->value.text.string);
  devctlcmd->ctl.attachment.cmd[i].ctl_no=atoi(ctlno->child->value.text.string);
  return 0;
}