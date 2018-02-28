#ifndef DEVSHARE_H_
#define DEVSHARE_H_

#include <pthread.h>

#define TYPE_FILE			1	//文件
#define TYPE_SENSOR         2 	//传感器设备
#define TYPE_GPS			4	//定位设备
#define TYPE_MICROPHONE		8	//声音采集设备
#define TYPE_CAMERA			16	//视频采集设备
#define TYPE_RELAY			32	//继电器
#define TYPE_PLAY			64	//播放设备
#define TYPE_MOTOR			128  //电机
#define TYPE_SCM            256  //单片机

#define MEDIA_SERIAL 0
#define MEDIA_USB 2
#define MEDIA_ETHERNET 3
#define MEDIA_WIFI 4
#define MEDIA_ZIGBEE 5
#define MEDIA_BLUETOOTH 6

#define DATA_SIZE 61

/*int dev_control(int sendid, int type, void * input, void* output)函数所需宏*/
#define EXECUTE_CMD 1				//执行需求指令
#define MODIFY_MEDIA 2				//修改介质配置参数
#define MODIFY_DEVICE 3				//修改设备参数
#define ADD_DEVICE 4				//添加设备
#define DELETE_DEVICE 5				//删除设备
#define GET_DEVICE_FILE 6			//获取设备描述文件
#define SET_DEVICE_FILE 7			//设置新设备描述文件
#define GET_MEDIAS_STATUS 8			//获取介质状态
#define GET_DEVICES_STATUS 9		//获取设备当前状态
#define ADD_MEDIA 10                //介质添加

//设备控制命令类型定义
#define DEVCTLCMD_ATTACH 1
#define DEVCTLCMD_NOATTACH 2


//dev_control函数执行指令需求时的input参数
struct devctl_in
{
	int devid;
	char 	sign; 			//信息特征码。
	char	len;			//控制参数个数
	char 	data[DATA_SIZE];		//控制参数
};
//dev_control函数执行指令需求时的output参数
struct devctl_out
{
	char msg[DATA_SIZE];
	int len;
};

struct reqctl
{
	int seq;
	char sign; 	//信息特征码(控制码)。
	char len;	//控制参数个数
	char data[DATA_SIZE];	//控制参数
};

// struct outctl
// {
// 	char type;	//信息类别
// 	char sign;	//信息特征码
// 	char *buf;	//信息空间，由调用者分配空间
// 	int buflen;	//空间大小
// 	int	inflen;	//信息大小
// };

//serial private parameter
struct serial_private
{
    char name[20];
    int bandrate;
    char csize;
    char cstop;
    char verify;
};

struct zigbee_private
{
    char name[20];
    int bandrate;
    char csize;
    char cstop;
    char verify;
};

struct scm_private
{
	union Private
	{
		struct Net{
			char ip[16];
	        int port;
		}net;
		struct Zigbee
		{
			short addr;
			char port;
		}zig;
	}private;
	char type;
};

//单片机附件支持的控制命令
struct attachment_cmd{
  char ctl_no;  //控制号
  char tip[60]; //提示
};

//单片机附件信息
struct device_attachment
{
  char attach_name[50];     //单片机附件名
  char bigtype;             //单片机附件大类
  char smalltype;           //单片机附件小类（具体设备）
  char no;                  //单片机附件编号
  int cmd_num;              //该附件支持的命令数
  struct attachment_cmd *cmd;   //该附件支持的命令
};

struct devctl_cmd
{
	union control{
       struct device_attachment attachment;
       struct attachment_cmd cmd;
	}ctl;
	char type;//该变量用来判断外设是否有附件
	struct devctl_cmd *next;
};
//模块用到的数据结构定义
//外接设备定义
struct ext_dev
{
	short	dev_type;				//设备类型，一个设备可以是多个类型的复合设备。
	int	devid;						/*设备编号*/
	char	devname[32];		    /*设备名称*/
	char	location[32];		    /*设备安装位置*/
	int	recvtime;					//最近收到设备信息时间
	short	status;					/*设备状态，1 运行状态，0失联状态*/
	int fd;                         /*设备打开标识符*/

    //设备控制函数，针对已有设备类型，定义一个通用操作函数。req是输入指令；output是输出信息；
	int (*control)(struct ext_dev *p, struct reqctl *req, struct devctl_out *output); 
	int (*init)(struct ext_dev *p);                     //初始化函数指针
	int (*close)(struct ext_dev *p);                    //关闭函数指针
	int (*keepalive)(struct ext_dev *p);                //保活函数指针
	int (*recv)(struct ext_dev *p,char *buf, int len);  //设备接收信息处理，如果是异步，调用upper;如果是同步处理，则让stm32_control返回。
    int (*upper)(struct devctl_out *out);
	void *data;  
    struct devctl_cmd *devctlcmd;//设备控制命令链表

	struct media_if *media;			/*指向媒介*/
	struct ext_dev	*next;			//设备链表
	struct ext_dev	*ifnext;		//同介质设备链表
};

//介质接口定义
struct media_if
{
	char id;
	char type;          /*介质类型*/
	char depend;		//依赖
	char status;        //0不可用，1正常可用
	char name[30];		/*媒介名称*/
	int fd;			    //介质打开的标识符
	char *buf;		    //接收信息缓冲区
	int len;		    //当前接收信息长度
    
    struct sockaddr_in *addr;//介质为网络时，保存客户端的地址信息
	void *para;          /*每种介质参数，比如以太网有ip，串口有波特率等*/
	int (*control)(struct media_if *p, int type, void *input, void *output);  //介质控制。
	int (*init)(struct media_if *p); 					  //初始化函数指针
	int (*close)(struct media_if *p); 					  //关闭函数指针
	int (*check)(struct media_if *p); 					  //介质可用性检查函数指针
	int (*send) (struct media_if *p, struct ext_dev *p_dev,struct reqctl *req); //介质发送信息函数
	int (*recv)(struct media_if *p, char *buf, int len);  //接收处理函
	int	devnum;								              //该介质连接的设备数量
	struct ext_dev *list;			                       //该介质上存在的外设链表
	struct media_if  *next;                                 /*下一个媒介*/
};


struct device_table
{
	int size;
	struct ext_dev *dev_list;
	pthread_mutex_t dev_lock;	//互斥锁
};

struct media_table
{
	int size;
	struct media_if *med_list;
	pthread_mutex_t med_lock;
};

#endif
