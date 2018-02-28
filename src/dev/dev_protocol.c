#include "dev.h"
#include "devshare.h"

extern struct dev_data *devdata;

//串口：协议解析
static int proto_serial(const struct serial_proto *pkt,int len);
//zigbee：协议解析
static int proto_zigbee(const struct zigbee_proto *pkt,int len);
//网络：协议解析
static int proto_net(const struct net_proto *pkt,int len);
//一般应答报文解析
static int proto_comreply(int seq,const char *data,int len);
//保活应答
static int proto_keeplivereply(int seq,const char *data,int len);
//回答设备信息
static int proto_devinforeply(int seq,const char *data,int len);
//控制应答报文解析
static int proto_ctlreply(int seq,const char *data,int len);
//传感器类控制应答
static int proto_senser_ctlreply(int seq,const char *data,int len);
//动作类控制应答
static int proto_action_ctlreply(int seq,const char *data,int len);
//声音类控制应答
static int proto_audio_ctlreply(int seq,const char *data,int len);
//图像类控制应答
static int proto_picture_ctlreply(int seq,const char *data,int len);
//电源类控制应答
static int proto_powsou_ctlreply(int seq,const char *data,int len);
//定位类控制应答
static int proto_location_ctlreply(int seq,const char *data,int len);
//温湿度传感器控制应答
static int proto_humiture_ctlreply(int seq,const char *data,int len);
//测距传感器控制应答
static int proto_raac_ctlreply(int seq,const char *data,int len);
//亮度传感器控制应答
static int proto_brightness_ctlreply(int seq,const char *data,int len);
//烟雾传感器控制应答
static int proto_smoke_ctlreply(int seq,const char *data,int len);
//电机控制应答
static int proto_motor_ctlreply(int seq,const char *data,int len);
//继电器控制应答
static int proto_rely_ctlreply(int seq,const char *data,int len);
//电源模块控制应答
static int proto_power_ctlreply(int seq,const char *data,int len);
//GPS模块控制应答
static int proto_gps_ctlreply(int seq,const char *data,int len);
//气压模块控制应答
static int proto_baroceptor_ctlreply(int seq,const char *data,int len);
//土壤温度
static int proto_soiltemperature_ctlreply(int seq,const char *data,int len);
//土壤湿度
static int proto_soilhumidity_ctlreply(int seq,const char *data,int len);

/*SCM附件GPS获得经伟度*/
inline static int gps_latlon(int seq,const char *data,int len);
/*SCM附件GPS获得卫星时间*/
inline static int gps_time(int seq,const char *data,int len);
/*SCM附件GPS获得海拔高度*/
inline static int gps_altitude(int seq,const char *data,int len);
/*SCM附件GPS获得所有数据*/
inline static int gps_all(int seq,const char *data,int len);

int proto_send(struct devctl_in *input, struct devctl_out *output)
{
  struct reqctl req;
  struct ext_dev *p=NULL;

  memset(&req,0,sizeof(req));

  req.sign=input->sign;
  req.len=input->len;
  req.seq=htons(devdata->sendseq++);
  memcpy(req.data,input->data,DATA_SIZE);
  if (devdata->sendseq>=65535)
  {
    devdata->sendseq=0;
  }
  
  p=device_get(input->devid);
  if(p==NULL)
    return -1;
  
  if(p->status==CLOSE||p->control(p,&req,output)==-1)
      return -1;

  return 0;
}

int proto_recv(struct device_proto *pkt)
{
  switch(pkt->type)
  {
      case PKT_TYPE_SERIAL:
          return proto_serial(&pkt->proto.serial,pkt->pktlen);
          break;

      case PKT_TYPE_ZIGBEE:
          return proto_zigbee(&pkt->proto.zigbee,pkt->pktlen);
          break;
      case PKT_TYPE_NET:
          return proto_net(&pkt->proto.net,pkt->pktlen);
          break;
  }
  return 0;
}

//串口协议解析
static int proto_serial(const struct serial_proto *pkt,int len)
{
  if (pkt->sign%2!=0)
  {
    return -1;
  }

  switch(pkt->sign)
  {
    case DEVPROTO_GEN_REP://一般应答
    return proto_comreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CA_REP://连接认证应答
    break;

    case DEVPROTO_ND_REP://邻居发现通告
    break;

    case DEVPROTO_EXCEPION://异常信息通告
    break;

    case DEVPROTO_DEVINFO_REP://回答设备信息
    return proto_devinforeply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CTL_REP://控制应答
    return proto_ctlreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_KEEPLIVE://联络保活
    return proto_keeplivereply(pkt->seq,pkt->data,len-3);
    break;
  }
  return 0;
}

static int proto_zigbee(const struct zigbee_proto *pkt,int len)
{
  if (pkt->sign%2!=0)
  {
    return -1;
  }
  printf("recv seq:%02x\n",pkt->seq );
   switch(pkt->sign)
  {
    case DEVPROTO_GEN_REP://一般应答
    return proto_comreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CA_REP://连接认证应答
    break;

    case DEVPROTO_ND_REP://邻居发现通告
    break;

    case DEVPROTO_EXCEPION://异常信息通告
    break;

    case DEVPROTO_DEVINFO_REP://回答设备信息
    return proto_devinforeply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CTL_REP://控制应答
    return proto_ctlreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_KEEPLIVE://联络保活
    return proto_keeplivereply(pkt->seq,pkt->data,len-3);
    break;
  }
  return 0;
}

static int proto_net(const struct net_proto *pkt,int len)
{
  if (pkt->sign%2!=0)
  {
    return -1;
  }

  switch(pkt->sign)
  {
    case DEVPROTO_GEN_REP://一般应答
    return proto_comreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CA_REP://连接认证应答
    break;

    case DEVPROTO_ND_REP://邻居发现通告
    break;

    case DEVPROTO_EXCEPION://异常信息通告
    break;

    case DEVPROTO_DEVINFO_REP://回答设备信息
    return proto_devinforeply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_CTL_REP://控制应答
    return proto_ctlreply(pkt->seq,pkt->data,len-3);
    break;

    case DEVPROTO_KEEPLIVE://联络保活
    return proto_keeplivereply(pkt->seq,pkt->data,len-3);
    break;
  }
  return 0;
}

static int proto_ctlreply(int seq,const char *data,int len)
{
  char main_class;//设备的大类

  main_class=data[1];

  switch(main_class)
  {
    case LCATTACH_SENSOR:
        return proto_senser_ctlreply(seq,data,len);
        break;

    case LCATTACH_ACTION:
        return proto_action_ctlreply(seq,data,len);
        break;
    
    case LCATTACH_AUDIO:
        return proto_audio_ctlreply(seq,data,len);
        break;

    case LCATTACH_PICTURE:
        return proto_picture_ctlreply(seq,data,len);
        break;

    case LCATTACH_POWSOU:
        return proto_powsou_ctlreply(seq,data,len);
        break;

    case LCATTACH_LOCATION:
        return proto_location_ctlreply(seq,data,len);
        break;
  }
  return 0;
}

//传感器类控制应答
static int proto_senser_ctlreply(int seq,const char *data,int len)
{

    char sub_class;//设备小类

    sub_class=data[2];
   
    switch(sub_class)
    {
      case LCATTACH_SENSOR_RAAC:
      return proto_raac_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_BRIGHTNESS:
      return proto_brightness_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_SMOKE:
      return proto_smoke_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_HUMITURE:
      return proto_humiture_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_BAROCEPTOR:
      return proto_baroceptor_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_SOILTEMPERATURE:
      return proto_soiltemperature_ctlreply(seq,data,len);
      break;

      case LCATTACH_SENSOR_SOILHUMIDITY:
      return proto_soilhumidity_ctlreply(seq,data,len);
      break;
    }
  return -1;
}

//动作类控制应答
static int proto_action_ctlreply(int seq,const char *data,int len)
{
  char sub_class;//设备小类

  sub_class=data[2];
 
  switch(sub_class)
  {
    case LCATTACH_ACTION_MOTOR:
      return proto_motor_ctlreply(seq,data,len);
      break;

    case LCATTACH_ACTION_RELY:
      return proto_rely_ctlreply(seq,data,len);
      break;
  }
  return -1;
}

//声音类控制应答
static int proto_audio_ctlreply(int seq,const char *data,int len)
{
  return 0;
}

//图像类控制应答
static int proto_picture_ctlreply(int seq,const char *data,int len)
{
  return 0;
}

//电源类控制应答
static int proto_powsou_ctlreply(int seq,const char *data,int len)
{
  char sub_class;//设备小类

  sub_class=data[2];
 
  switch(sub_class)
  {
    case LCATTACH_POWSOU_POWER:
      return proto_power_ctlreply(seq,data,len);
      break;
  }
  return -1;
}

//定位类控制应答
static int proto_location_ctlreply(int seq,const char *data,int len)
{
  char sub_class;//设备小类

  sub_class=data[2];
 
  switch(sub_class)
  {
    case LCATTACH_LOCATION_GPS:
      return proto_gps_ctlreply(seq,data,len);
      break;
  }
  return -1;;
}

//温湿度传感器控制应答
static int proto_humiture_ctlreply(int seq,const char *data,int len)
{
    struct devctl_inf *ctl_node=NULL;
    char ctl_no;

    ctl_no=data[4];
    mutex_ctlinflist_lock(&devdata->ctltable);
    ctl_node=devctl_inf_get(seq);
    if (ctl_node==NULL)
    {
      mutex_ctlinflist_unlock(&devdata->ctltable);
      return -1;
    }
    switch(ctl_no)
    {
      case HUMITURE_GET_TEMPERATURE://获得温度
      sprintf(ctl_node->output->msg,"温度：%d",data[5]);
      ctl_node->output->len=strlen(ctl_node->output->msg);
      break;

      case HUMITURE_GET_HUMIDITY://获得湿度
      sprintf(ctl_node->output->msg,"湿度：%d",data[5]);
      ctl_node->output->len=strlen(ctl_node->output->msg);
      break;

      case HUMITURE_GET_ALL://获得湿度和温度
      sprintf(ctl_node->output->msg,"湿度：%d 湿度：%d",data[5],data[6]);
      ctl_node->output->len=strlen(ctl_node->output->msg);
      break;

      case HUMITURE_SET_THRESHOLD_TEMPERATURE://设置温度阀值
       if (data[5]==0)
      {
        sprintf(ctl_node->output->msg,"设置温度阀值："CTLCMD_OK);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }else
      {
        sprintf(ctl_node->output->msg,"设置温度阀值："CTLCMD_FAILED);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }
      break;

      case HUMITURE_SET_THRESHOLD_HUMIDITY://设置湿度阀值
       if (data[5]==0)
      {
        sprintf(ctl_node->output->msg,"设置湿度阀值："CTLCMD_OK);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }else
      {
        sprintf(ctl_node->output->msg,"设置湿度阀值："CTLCMD_FAILED);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }
      break;
    }
    sem_post(&ctl_node->sem);
    mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//测距传感器控制应答
static int proto_raac_ctlreply(int seq,const char *data,int len)
{
   struct devctl_inf *ctl_node=NULL;
    char ctl_no;

    ctl_no=data[4];
    mutex_ctlinflist_lock(&devdata->ctltable);
    ctl_node=devctl_inf_get(seq);
    if (ctl_node==NULL)
    {
      mutex_ctlinflist_unlock(&devdata->ctltable);
      return -1;
    }

    switch(ctl_no)
    {
      case RAAC_GET_MINRANGE://获得最近距离
      sprintf(ctl_node->output->msg,"最近物体距离：%d",data[5]);
      ctl_node->output->len=strlen(ctl_node->output->msg);
      break;

      case RAAC_SET_THRESHOLD_RANGE://设置阀值
      if (data[5]==0)
      {
        sprintf(ctl_node->output->msg,"设置最近物体距离阀值："CTLCMD_OK);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }else
      {
        sprintf(ctl_node->output->msg,"设置最近物体距离阀值："CTLCMD_FAILED);
        ctl_node->output->len=strlen(ctl_node->output->msg);
      }
      break;
    }
    sem_post(&ctl_node->sem);
    mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//亮度传感器控制应答
static int proto_brightness_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  switch(ctl_no)
  {
    case BRIGHTNESS_GET_BRIGHTNESS://获得亮度
    sprintf(ctl_node->output->msg,"亮度：%d",data[5]);
    ctl_node->output->len=strlen(ctl_node->output->msg);
    break;

    case BRIGHTNESS_SET_THRESHOLD_BRIGHTNESS://设置阀值
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"设置亮度阀值："CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"设置亮度阀值："CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//烟雾传感器控制应答
static int proto_smoke_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  switch(ctl_no)
  {
    case BRIGHTNESS_GET_BRIGHTNESS://获得烟雾浓度
    sprintf(ctl_node->output->msg,"烟雾浓度：%d",data[5]);
    ctl_node->output->len=strlen(ctl_node->output->msg);
    break;

    case BRIGHTNESS_SET_THRESHOLD_BRIGHTNESS://设置阀值
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"设置烟雾浓度阀值："CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"设置烟雾浓度阀值："CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//气压模块控制应答
static int proto_baroceptor_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  switch(ctl_no)
  {
    case BAROCEPTOR_GET_AIRPRESSURE://获得气压值
    sprintf(ctl_node->output->msg,"大气压强：%d",data[5]);
    ctl_node->output->len=strlen(ctl_node->output->msg);
    break;

    case BAROCEPTOR_SET_THRESHOLD_AIRPRESSURE://设置阀值
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"设置大气压强阀值："CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"设置大气压强阀值："CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//土壤温度
static int proto_soiltemperature_ctlreply(int seq,const char *data,int len)
{
   struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  switch(ctl_no)
  {
    case SOILTEMPERATURE_GET_TEMPERATURE://获得土壤温度
    sprintf(ctl_node->output->msg,"土壤温度：%d",data[5]);
    ctl_node->output->len=strlen(ctl_node->output->msg);
    break;

    case SOILTEMPERATURE_SET_THRESHOLD_TEMPERATURE://设置阀值
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"设置土壤温度阀值："CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"设置土壤温度阀值："CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//土壤湿度
static int proto_soilhumidity_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  switch(ctl_no)
  {
    case SOILHUMIDITY_GET_HUMIDITY://获得土壤湿度
    {
      short d;
      memcpy(&d,data+5,2);
      sprintf(ctl_node->output->msg,"土壤湿度：%d",d);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;

    case SOILHUMIDITY_SET_THRESHOLD_HUMIDITY://设置阀值
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"设置土壤湿度阀值："CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"设置土壤湿度阀值："CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//电机控制应答
static int proto_motor_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;

 mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }
  
  switch(data[4])
  {
    case MOTOR_TOADVANCE:
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"前进:"CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"前进:"CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;

    case MOTOR_FALLBACK:
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"后退:"CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"后退:"CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;

    case MOTOR_LEFT:
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"左转:"CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"左转:"CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;

    case MOTOR_RIGHT:
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,"右转:"CTLCMD_OK);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,"右转:"CTLCMD_FAILED);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//继电器控制应答
static int proto_rely_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;

  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }
  
  if (data[5]==0)
  {
    sprintf(ctl_node->output->msg,"继电器吸合:"CTLCMD_OK);
    ctl_node->output->len=strlen(ctl_node->output->msg);
  }else
  {
    sprintf(ctl_node->output->msg,"继电器断开:"CTLCMD_FAILED);
    ctl_node->output->len=strlen(ctl_node->output->msg);
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//电源模块控制应答
static int proto_power_ctlreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  char ctl_no;

  ctl_no=data[4];
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }
  
  switch(ctl_no)
  {
    case POWER_GET_QUANTITY://获得电量
    sprintf(ctl_node->output->msg,"剩余电量：%d",data[5]);
    ctl_node->output->len=strlen(ctl_node->output->msg);
    break;

    case POWER_GET_WAY://获得供电方式
    if (data[5]==0)
    {
      sprintf(ctl_node->output->msg,POWER_WAY_BATTERY);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }else
    {
      sprintf(ctl_node->output->msg,POWER_WAY_WIRELESS);
      ctl_node->output->len=strlen(ctl_node->output->msg);
    }
    break;
  }
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//GPS模块控制应答
static int proto_gps_ctlreply(int seq,const char *data,int len)
{
  char ctl_no;

  ctl_no=data[4];
  
  switch(ctl_no)
  {
    case GPS_GET_LATLON://获得经纬度
    return gps_latlon(seq,data,len);
    break;

    case GPS_GET_TIME://获取卫星时间
    return gps_time(seq,data,len);
    break;

    case GPS_GET_ALTITUDE://获取海拔高度
    return gps_altitude(seq,data,len);
    break;

    case GPS_GET_ALL://所有数据
    return gps_all(seq,data,len);
    break;
  }
  return 0;
}

inline static int gps_latlon(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  int lat;
  int lon;

  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  memcpy(&lat,data+5,4);
  memcpy(&lon,data+5+4,4);
  sprintf(ctl_node->output->msg,"经度：%d 纬度：%d",lat,lon);

  ctl_node->output->len=strlen(ctl_node->output->msg);
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

/*SCM附件GPS获得卫星时间*/
inline static int gps_time(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  int h;
  int m;
  int s;

  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }
  memcpy(&h,data+5,1);
  memcpy(&m,data+5+1,1);
  memcpy(&s,data+5+2,1);
  sprintf(ctl_node->output->msg,"%d:%d:%d",h,m,s);
  ctl_node->output->len=strlen(ctl_node->output->msg);

  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

/*SCM附件GPS获得海拔高度*/
inline static int gps_altitude(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  int altitude;

  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
     mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  memcpy(&altitude,data+5,2);
  sprintf(ctl_node->output->msg,"海拔：%d",altitude);
  ctl_node->output->len=strlen(ctl_node->output->msg);
  
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

/*SCM附件GPS获得所有数据*/
inline static int gps_all(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
  int lat;
  int lon;
  int h;
  int m;
  int s;
  int a;

  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }

  
  memcpy(&lat,data+5,4);
  memcpy(&lon,data+5+4,4);
  memcpy(&h,data+5+8,1);
  memcpy(&m,data+5+8+1,1);
  memcpy(&s,data+5+8+2,1);
  memcpy(&a,data+5+8+3,2);

  sprintf(ctl_node->output->msg,"经度：%d 纬度：%d 卫星时间：%d:%d:%d 海拔：%d",lat,lon,h,m,s,a);
  ctl_node->output->len=strlen(ctl_node->output->msg);
  
  sem_post(&ctl_node->sem);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//通用应答
static int proto_comreply(int seq,const char *data,int len)
{
  struct devctl_inf *ctl_node=NULL;
   
  mutex_ctlinflist_lock(&devdata->ctltable);
  ctl_node=devctl_inf_get(seq);
  if (ctl_node==NULL)
  {
    mutex_ctlinflist_unlock(&devdata->ctltable);
    return -1;
  }
  
  memcpy(ctl_node->output->msg,data+5,1);
  ctl_node->output->len=len-5;

  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}

//保活应答
static int proto_keeplivereply(int seq,const char *data,int len)
{

  return 0;
}

//回答设备信息
static int proto_devinforeply(int seq,const char *data,int len)
{
  mutex_ctlinflist_lock(&devdata->ctltable);
  mutex_ctlinflist_unlock(&devdata->ctltable);
  return 0;
}