#include "dev.h"

static int cjson_device_add(cJSON *devices,struct ext_dev *dev_node);
static int cjson_save(struct cJSON *root);
static int cjson_device_delete(cJSON *devices,int devid);
static int cjson_device_update(cJSON *devices,struct ext_dev *dev_node);
static int cjson_media_add(cJSON *medias,struct media_if *med_node);
static int media_private(cJSON *media_private,struct media_if *med_node);
static int serial_private(cJSON *media_private,struct media_if *med_node);
static int cjson_media_update(cJSON *medias,struct media_if *med_node);
static int cjson_media_delete(cJSON *medias,struct media_if *med_node);


/*
*   type        input           function
*   1        struct ext_dev     add device
*   2        struct ext_dev     delete device
*   3        struct ext_dev     update device
*   4        struct media_if    update media
*/
int control_cjson_control(int type,void *input)
{
  char *json_string=NULL;
  json_string=init_read_file();
  if(json_string==NULL)
   return -1;
 cJSON *root=cJSON_Parse(json_string);
 if(root==NULL)
   return -1;
 cJSON *medias=cJSON_GetObjectItem(root,JSON_MEIDA);
 if(medias==NULL)
   return -1;
 cJSON *devices=cJSON_GetObjectItem(root,JSON_DEVICE);
 if(devices==NULL)
   return -1;

char *str=cJSON_Print(devices);
puts(str);
free(str);
switch (type)
{
  case JSON_DEVICE_ADD:
    cjson_device_add(devices,(struct ext_dev*)input);
    cjson_save(root);
  break;

  case JSON_DEVICE_DELETE:
    cjson_device_delete(devices,*(int *)input);
    cjson_save(root);
  break;

  case JSON_DEVICE_UPDATE:
    cjson_device_update(devices,(struct ext_dev*)input);
    cjson_save(root);
  break;

  case JSON_MEDIA_UPDATE:
    cjson_media_update(medias,(struct media_if*)input);
    cjson_save(root);
  break;

  case JSON_MEDIA_ADD:
  cjson_media_add(medias,(struct media_if*)input);
  cjson_save(root);
  break;

  case JSON_MEDIA_DELETE:
  cjson_media_delete(medias,(struct media_if*)input);
  cjson_save(root);
  break;

  default:
  break;
}
str=cJSON_Print(devices);
puts(str);
free(str);
return 0;
}

static int cjson_device_add(cJSON *devices,struct ext_dev *dev_node)
{
    cJSON *new_device_item=cJSON_CreateObject();
    cJSON *new_device_data=cJSON_CreateObject();
    if(new_device_data==NULL||new_device_item==NULL)
      return -1;

    cJSON_AddNumberToObject(new_device_item,JSON_DEVICE_ID,dev_node->devid);
    cJSON_AddStringToObject(new_device_item,JSON_DEVICE_NAME,dev_node->devname);
    cJSON_AddStringToObject(new_device_item,JSON_DEVICE_LOCATION,dev_node->location);
    cJSON_AddNumberToObject(new_device_item,JSON_DEVICE_TYPE,dev_node->dev_type);
    cJSON_AddNumberToObject(new_device_item,JSON_DEVICE_MEDIA,dev_node->media->id);
    cJSON_AddItemToObjectCS(new_device_item,JSON_DEVICE_DATA,new_device_data);
    cJSON_AddItemToArray(devices,new_device_item);

  return 0;
}

static int cjson_device_delete(cJSON *devices,int devid)
{
  int devnum=0;
  int i=0;
  struct cJSON *device_item=NULL;
  struct cJSON *id=NULL;

  devnum=cJSON_GetArraySize(devices);
  for(i=0;i<devnum;i++)
  {
      device_item=cJSON_GetArrayItem(devices,i);
      id=cJSON_GetObjectItem(device_item,JSON_DEVICE_ID);
      if(id->valueint==devid)
      {
        cJSON_DeleteItemFromArray(devices,i);
          return 0;
      }
  }
  return -1;
}

static int cjson_device_update(cJSON *devices,struct ext_dev *dev_node)
{
  int devnum=0;
  int i=0;
  struct cJSON *device_item=NULL;
  struct cJSON *id=NULL;

  devnum=cJSON_GetArraySize(devices);
  for(i=0;i<devnum;i++)
  {
      device_item=cJSON_GetArrayItem(devices,i);
      id=cJSON_GetObjectItem(device_item,JSON_DEVICE_ID);
      if(id->valueint==dev_node->devid)
      {
        cJSON_DeleteItemFromArray(devices,i);
        if(cjson_device_add(devices,dev_node)==-1)
          return -1;
        return 0;
      }
  }
  return -1;
}


static int cjson_media_add(cJSON *medias,struct media_if *med_node)
{
  cJSON *new_media_item=cJSON_CreateObject();
  cJSON *new_media_private=cJSON_CreateObject();
  if(new_media_item==NULL||new_media_private==NULL)
    return -1;

  cJSON_AddNumberToObject(new_media_item,JSON_MEDIA_ID,med_node->id);
  cJSON_AddStringToObject(new_media_item,JSON_MEDIA_NAME,med_node->name);
  cJSON_AddNumberToObject(new_media_item,JSON_MEDIA_TYPE,med_node->type);
  cJSON_AddNumberToObject(new_media_item,JSON_MEDIA_DEVNUM,med_node->devnum);
  cJSON_AddNumberToObject(new_media_item,JSON_MEDIA_DEPEND,med_node->depend);
  //介质私有参数
  if(media_private(new_media_private,med_node)==-1)
    return -1;

  cJSON_AddItemToObjectCS(new_media_item,JSON_MEDIA_PRIVATE,new_media_private);
  cJSON_AddItemToArray(medias,new_media_item);
return 0;
}

static int media_private(cJSON *media_private,struct media_if *med_node)
{

  switch (med_node->type)
  {
    case MEDIA_SERIAL:
    if(serial_private(media_private,med_node)==-1)
      return -1;
    break;

    case MEDIA_ETHERNET:
    break;
  }

  return 0;
}


static int serial_private(cJSON *media_private,struct media_if *med_node)
{
  struct serial_private *private=(struct serial_private*)med_node->para;

  cJSON_AddNumberToObject(media_private,JSON_SERIAL_BANDRATE,private->bandrate);
  cJSON_AddNumberToObject(media_private,JSON_SERIAL_CSIZE,private->csize);
  cJSON_AddNumberToObject(media_private,JSON_SERIAL_CSTOP,private->cstop);
  cJSON_AddNumberToObject(media_private,JSON_SERIAL_VERIFY,private->verify);

  return 0;
}

static int cjson_media_delete(cJSON *medias,struct media_if *med_node)
{
  int mednum=0;
  int i=0;
  struct cJSON *media_item=NULL;
  struct cJSON *id=NULL;

  mednum=cJSON_GetArraySize(medias);
  for(i=0;i<mednum;i++)
  {
      media_item=cJSON_GetArrayItem(medias,i);
      id=cJSON_GetObjectItem(media_item,JSON_MEDIA_ID);
      if(id->valueint==med_node->id)
      {
        cJSON_DeleteItemFromArray(medias,i);
        return 0;
      }
  }
  return -1;
}

static int cjson_media_update(cJSON *medias,struct media_if *med_node)
{
  int mednum=0;
  int i=0;
  struct cJSON *media_item=NULL;
  struct cJSON *id=NULL;

  mednum=cJSON_GetArraySize(medias);
  for(i=0;i<mednum;i++)
  {
      media_item=cJSON_GetArrayItem(medias,i);
      id=cJSON_GetObjectItem(media_item,JSON_MEDIA_ID);
      if(id->valueint==med_node->id)
      {
        cJSON_DeleteItemFromArray(medias,i);
        if(cjson_media_add(medias,med_node)==-1)
          return -1;
        return 0;
      }
  }
  return -1;
}

static int cjson_save(struct cJSON *root)
{
  FILE *fp=NULL;
  char *str=NULL;
  str=cJSON_Print(root);

  fp=fopen(DEV_CONF_FILE,"w");
  if(fp==NULL)
    return -1;

  fprintf(fp,"%s",str);
  fclose(fp);
  return 0;
}
