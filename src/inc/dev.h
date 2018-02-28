#ifndef DEVICE_H_
#define DEVICE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/epoll.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include "devshare.h"
#include "sys.h"
#include "cJSON.h"

#define PKT_MAXSIZE 256

#define CTLCMD_OK "成功"
#define CTLCMD_FAILED "失败"

/************************************************************************/
#define MAXEVENTS 10                               //epoll最大的事件数量
#define LIVE 1                                     //设备或介质的状态：在线
#define CLOSE 0                                    //设备或介质的状态：离线
#define CURRENT_MODULE_NAME "device_management"	   //模块名称
#define MODULE_TYPE 0                              //模块类型(0表示静态1表示动态)
#define MODULE_ID 3                                //模块的ID
#define DEV_CONF_FILE "../sys/dev.txt"             //设备和介质的配置文件
#define DEVCTL_CMD_FILE "../sys/device_cmd.xml"    //外设支持的控制命令

#define KEY_MAX_DEVICE_NUM "MAXDEVICENUM"          //最大设备数量
#define KEY_MAX_MEDIA_NUM "MAXMEDIANUM"            //最大介质数量
#define KEY_TIMEOUT "TIMEOUT"                      //超时检测
#define VALUE_MAX_MEDIA_NUM 10
#define VALUE_MAX_DEVICE_NUM 256
#define VALUE_TIMEOUT 5
/************************************************************************/

/***************int config(int type, void*cfg)函数所需的宏****************/
#define MODIRY_MULTI_CONF 1                        //设置修改多个配置项
#define MODIRY_DEFAULT_CONF 2                      //设置修改配置项为默认值
#define QUERY_ALL_CONF 3                           //查询配置项的当前值
#define QUERY_DEFAULT_CONF 4                       //查询配置项的默认值
/***********************************************************************/

/************int config(int type, void*cfg)函数所需的宏******************/
#define QUERY_MODULE_INFO 1
#define QUERY_DETAILED_INFO 2
#define QUERY_MODULE_STASUS 0
#define QUERY_SPEND_MEMORY 1
#define QUERY_THREAD_NUM 2
#define QUERY_FILE_NUM 3
#define QUERY_SOCKET_NUM 4
#define QUERY_DETAILED_THREAD 1
#define QUERY_FILES_INFO 2
#define QUERY_SOCKETS_INFO 3
/***********************************************************************/

/*int dev_init( KeyValueArray *cfg, moduleCallback callback, struct ModuleInfo* ret)函数所需宏*/
#define JSON_MEIDA "media"
#define JSON_DEVICE "device"

#define JSON_MEDIA_TYPE "type"
#define JSON_MEDIA_ID "id"
#define JSON_MEDIA_NAME "name"
#define JSON_MEDIA_DEVNUM "devnum"
#define JSON_MEDIA_DEPEND "depend"
#define JSON_MEDIA_PRIVATE "private"

//JSON设备参数
#define JSON_DEVICE_TYPE "type"
#define JSON_DEVICE_NAME "name"
#define JSON_DEVICE_ID "id"
#define JSON_DEVICE_MEDIA "media"
#define JSON_DEVICE_LOCATION "location"
#define JSON_DEVICE_DATA "data"

//JSON：串口私有参数
#define JSON_SERIAL_BANDRATE "bandrate"		//波特率
#define JSON_SERIAL_CSIZE "csize"			//字符大小
#define JSON_SERIAL_CSTOP "cstop"			//停子位
#define JSON_SERIAL_VERIFY "verify"			//是否设置检验位
#define JSON_SERIAL_NAME "name"				//
//JSON：zegbee私有参数
#define JSON_ZIGBEE_BANDRATE "bandrate"		//波特率
#define JSON_ZIGBEE_CSIZE "csize"			//字符大小
#define JSON_ZIGBEE_CSTOP "cstop"			//停子位
#define JSON_ZIGBEE_VERIFY "verify"			//是否设置检验位
#define JSON_ZIGBEE_NAME "name"				//

#define _CS5 5
#define _CS6 6
#define _CS7 7
#define _CS8 8

//net:私有参数
#define JSON_NET_PORT "port"

//scm:私有数据
#define JSON_TYPE "type" //通信类型（zigbee，wifi）
#define JSON_NETSCM_IP "ip"
#define JSON_NETSCM_PORT "port"
#define JSON_ZIGSCM_ADDR "addr"
#define JSON_ZIGSCM_PORT "port"

#define JSON_TYPE_ZIGBEE 0
#define JSON_TYPE_NET 1
/**********************************************************************/
#define JSON_DEVICE_ADD 1			//json文件中 设备添加
#define JSON_DEVICE_DELETE 2		//json文加中 设备删除
#define JSON_DEVICE_UPDATE 3		//json文件中 设备修改
#define JSON_MEDIA_UPDATE 4			//json文件中 介质新
#define JSON_MEDIA_ADD 5            //json文件中 介质添加
#define JSON_MEDIA_DELETE 6         //json文件中 介质删除

#define DEVPROTO_GEN_REP 0    //一般应答
#define DEVPROTO_CA_REQ 1     //连接认证请求
#define DEVPROTO_CA_REP 2     //连接认证应答
#define DEVPROTO_ND_REQ 3     //邻居发现请求
#define DEVPROTO_ND_REP 4     //邻居发现通告
#define DEVPROTO_KEEPLIVE 5   //联络保活
#define DEVPROTO_EXCEPION 6   //异常信息通告
#define DEVPROTO_SET_NO 7     //设置设备编号
#define DEVPROTO_SET_CODE 8   //设置特征码
#define DEVPROTO_SET_NAME 11    //设置名字串
#define DEVPROTO_DEVINFO_REQ 13 //请求设备信息
#define DEVPROTO_DEVINFO_REP 14 //回答设备信息

#define DEVPROTO_CTL_REQ 33   //控制请求
#define DEVPROTO_CTL_REP 34   //控制应答
#define DEVPROTO_GPS_REQ 35   //查询GPS
#define DEVPROTO_GPS_REP 36   //返回GPS信息


/**********************************************************************/

#define PKT_TYPE_SERIAL 1
#define PKT_TYPE_ZIGBEE 2
#define PKT_TYPE_WIFI   3
#define PKT_TYPE_NET    4

/******************单片机附件类型定义*************************************/
/*大类定义*/
#define LCATTACH_SENSOR   0 //传感器类
#define LCATTACH_ACTION   1 //动作类
#define LCATTACH_AUDIO    2 //声音类
#define LCATTACH_PICTURE  3 //图像类
#define LCATTACH_POWSOU   4 //电源类
#define LCATTACH_LOCATION 5 //定位类

/*具体设备定义*/
#define LCATTACH_SENSOR_RAAC            0   //测距（range acquisition）传感器
#define LCATTACH_SENSOR_BRIGHTNESS      1   //亮度传感器
#define LCATTACH_SENSOR_SMOKE           2   //烟雾传感器
#define LCATTACH_SENSOR_HUMITURE        3   //温湿度传感器
#define LCATTACH_SENSOR_BAROCEPTOR      4   //气压传感器
#define LCATTACH_SENSOR_SOILTEMPERATURE 5   //土壤温度传感器
#define LCATTACH_SENSOR_SOILHUMIDITY    6   //土壤温度传感器
#define LCATTACH_ACTION_MOTOR           0   //电机
#define LCATTACH_ACTION_RELY            1   //继电器
#define LCATTACH_AUDIO_MICRO            0   //麦克风
#define LCATTACH_AUDIO_BUZZER           1   //蜂鸣器
#define LCATTACH_PICTURE_CAMERA         0   //摄像头
#define LCATTACH_POWSOU_POWER           0   //电源
#define LCATTACH_LOCATION_GPS           0   //GPS


/*具体设备控制号定义*/
/*温度传感器控制号*/
#define HUMITURE_GET_TEMPERATURE            0 //获得温度
#define HUMITURE_GET_HUMIDITY               1 //获得湿度
#define HUMITURE_GET_ALL                    2 //获得湿度和温度
#define HUMITURE_SET_THRESHOLD_TEMPERATURE  3 //设置温度阀值
#define HUMITURE_SET_THRESHOLD_HUMIDITY     4 //设置湿度阀值
/*测距传感器控制号*/
#define RAAC_GET_MINRANGE                   0 //获得最近距离
#define RAAC_SET_THRESHOLD_RANGE            1 //设置阀值
/*亮度传感器控制号*/
#define BRIGHTNESS_GET_BRIGHTNESS           0 //获得亮度
#define BRIGHTNESS_SET_THRESHOLD_BRIGHTNESS 1 //设置阀值
/*气压传感器控制号*/
#define BAROCEPTOR_GET_AIRPRESSURE          0 //获得气压值
#define BAROCEPTOR_SET_THRESHOLD_AIRPRESSURE 1 //设置阀值
/*土壤温度传感器*/
#define SOILTEMPERATURE_GET_TEMPERATURE     0 //获得土壤温度
#define SOILTEMPERATURE_SET_THRESHOLD_TEMPERATURE  1 //设置温度阀值
/*土壤湿度传感器*/
#define SOILHUMIDITY_GET_HUMIDITY     0 //获得土壤湿度
#define SOILHUMIDITY_SET_THRESHOLD_HUMIDITY  1 //设置湿度阀值

/*电源管理模块控制号*/
#define POWER_GET_QUANTITY                  0 //获得电量
#define POWER_GET_WAY                       1 //获得供电方式
/*供电方式定义*/
#define POWER_WAY_BATTERY            "电池供电" //电池供电
#define POWER_WAY_WIRELESS           "无线供电" //无线供电
/*GPS控制号*/
#define GPS_GET_LATLON                      0 //获取经纬度
#define GPS_GET_TIME                        1 //获取卫星时间
#define GPS_GET_ALTITUDE                    2 //获取海拔高度
#define GPS_GET_ALL                         3 //所有数据
/*电机控制号*/
#define MOTOR_TOADVANCE                     0 //前进
#define MOTOR_FALLBACK                      1 //后退
#define MOTOR_LEFT                          2 //左转
#define MOTOR_RIGHT                         3 //右转
/**********************控制要求表**************************************/
struct devctl_inf
{
	int	devid;			/*设备编号*/
	int	requestid;		//请求者id
	time_t time;        //发送时间
	unsigned short seq;            //报文序号
	sem_t sem;          //信号量
	struct devctl_out *output;//控制报文的输出
	struct devctl_inf *next	;	//下一个节点
};

struct control_table
{
	int size;
	struct devctl_inf *ctl_list;
	pthread_mutex_t ctl_lock;	//互斥锁
};
/***********************************************************************/

/***********************单片机协议***************************************/
struct serial_proto
{
    char dst;
	char next;
	char src;
	char sign;
	short seq;
	short len;
	char data[DATA_SIZE];
};

struct net_proto
{
	char sign;
	short seq;
	short len;
	char data[DATA_SIZE];
};

struct zigbee_proto
{
    char sign;
    short seq;
    char data[DATA_SIZE];
};

struct device_proto
{
	union protocol
	{
        struct serial_proto serial;
        struct net_proto net;
        struct zigbee_proto zigbee;
	}proto;
	int pktlen;
	char type;
};
/***********************************************************************/ 

/****************************全局变量***********************************/
struct dev_data
{
	int timeout;
	int max_device_num;
	int max_media_num;
	int epoll_fd;
	short sendseq;//报文发送序号
	FILE *dev_debug_log;
	pthread_t timer_thread_id;
	pthread_t recv_thread_id;
	struct control_table ctltable;
	struct device_table devtable;
	struct media_table medtable;
	struct epoll_event events[MAXEVENTS];
	moduleCallback	callback;		//访问其他模块功能的回
};
/***********************************************************************/

/******************本模块对外接口函数定义*********************************/
int dev_init(struct KeyValueArray  *cfg, moduleCallback callback, struct ModuleInfo *ret);
int dev_close(void);
int dev_query(int type, void *input, void *output);
int dev_config(int type, void*cfg);
int dev_defaultcfg( void);
int dev_control(int sendid, int type, void * input, void * output);
/***********************************************************************/

/************************线程处理函数定义*********************************/
void *dev_timer(void *arg);
void *dev_recvhandle(void *arg);
int dev_delevent(struct media_if *medinf);
int dev_addevent(struct media_if *medinf);
int  check_medias(void);
int  check_ctltable(void);
/***********************************************************************/

/**************************协议处理函数定义******************************/
int proto_send(struct devctl_in *input, struct devctl_out *output);
int proto_recv(struct device_proto *pkt);
/***********************************************************************/

/**************************设备控支持的制命令链表***************************/
struct devctl_cmd* devctlcmd_add(struct devctl_cmd *head,struct devctl_cmd *pnode);
int devctlcmd_clear(struct ext_dev *pdev);
/***********************************************************************/

/**************************设备函数管理函数定义***************************/
int device_list_init(struct ext_dev *node);
int device_add(struct ext_dev *node);
int device_delete(int device_id);
int device_update(struct ext_dev *node);
int device_clear(void);
struct ext_dev *device_get(int id);
/***********************************************************************/

/***********************同介质设备管理***********************************/
int same_media_device_add(struct media_if *med,struct ext_dev *device_node);
int same_media_device_delete(struct media_if *med,int devid);
int same_media_device_clear(struct media_if *med);
/**********************************************************************/

/*****************************介质管理函数定义***************************/
int media_list_init(struct media_if *node);
int media_add(struct media_if *node);
int media_delete(char id);
int media_update(struct media_if *node);
int media_clear(void);
struct media_if *media_get(char id);
/***********************************************************************/

/******************************控制信息表处理****************************/
int devctl_inf_create(int sendid,void *input,struct devctl_inf *node);
int devctl_inf_add(struct devctl_inf *node);
int devctl_inf_delete(int devid);
struct devctl_inf *devctl_inf_get(unsigned short devid);
int devctl_inf_clear(void);
/***********************************************************************/

/**********************提供给对外查询函数所调用****************************/
int query_module_info(void *input,void *output);
int query_detailed_info(void *input,void *output);
/***********************************************************************/

/*********************给模块初始化调用的函数******************************/
int init_ret(struct ModuleInfo *ret);        //模块初始化时给参数ret赋值
int init_conf(struct KeyValueArray *cfg);	 //初始化本模块的配置
int init_medias_devices(void);               //用cJSON解析init_read_file函数读取dev.txt返回的内容
char *init_read_file(void);
int init_devctlcmd(void);                    //初始化设备支持的控制命令
/***********************************************************************/

/****************************串口操作************************************/
int serial_open(struct media_if *med_inf);
/***********************************************************************/

/**************************具体介质所具有的函数***************************/
int serial_init(struct media_if *med_inf);
int serial_control(struct media_if *med_inf, int type, void *input, void *output);
int serial_close(struct media_if *med_inf);
int serial_check(struct media_if *med_inf);
int serial_send(struct media_if *med_inf, struct ext_dev *p_dev,struct reqctl *req);
int serial_recv(struct media_if *med_inf, char *msg, int len);

int net_init(struct media_if *med_inf);
int net_control(struct media_if *med_inf, int type, void *input, void *output);
int net_close(struct media_if *med_inf);
int net_check(struct media_if *med_inf);
int net_send(struct media_if *med_inf,struct ext_dev *p_dev,struct reqctl *req);
int net_recv(struct media_if *med_inf, char *msg, int len);

int zigbee_control(struct media_if *med_inf, int type, void *input, void *output);
int zigbee_init(struct media_if *med_inf);
int zigbee_close(struct media_if *med_inf);
int zigbee_check(struct media_if *med_inf);
int zigbee_send(struct media_if *med_inf,struct ext_dev *p_dev,  struct reqctl *req);
int zigbee_recv(struct media_if *med_inf, char *buf, int len);
/**********************************************************************/

/**********************具体设备所具有的函数*******************************/
//单片机
int scm_control(struct ext_dev *p, struct reqctl *req, struct devctl_out *output);
int scm_init(struct ext_dev *extdev);
int scm_close(struct ext_dev *p);
int scm_keepalive(struct ext_dev *p);

/***********************************************************************/

/*************************介质和设备json文件操作*********************/
int control_cjson_control(int type,void *input);
/***********************************************************************/

/********************设备，介质，控制信息表的互斥锁************************/
int mutex_ctlinflist_lock(struct control_table *ctltable);
int mutex_ctlinflist_unlock(struct control_table *ctltable);
int mutex_devicelist_lock(struct device_table *devtable);
int mutex_devicelist_unlock(struct device_table *devtable);
int mutex_medialist_lock(struct media_table *medtable);
int mutex_medialist_unlock(struct media_table *medtable);
/***********************************************************************/
#endif
