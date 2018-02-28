#ifndef DEVSHARE_H_
#define DEVSHARE_H_

#define TYPE_FILE			1	//文件
#define TYPE_SENSOR         2 	//传感器设备
#define TYPE_GPS			4	//定位设备
#define TYPE_MICROPHONE		8	//声音采集设备	
#define TYPE_CAMERA			16	//视频采集设备
#define TYPE_RELAY			32	//继电器
#define TYPE_PLAY			64	//播放设备
#define TYPE_MOTOR			128  //电机

//模块用到的数据结构定义
//介质接口定义
struct media_if
{
	char type;              /*介质类型*/
	char subtype;				//介质子类型
	char status;            //0不可用，1正常可用
	char name[30];			/*媒介名称*/
	int fd;				//介质打开的标识符
	char *buf;			//接收信息缓冲区
	int len;			//当前接收信息长度

	void *para;               /*每种介质参数，比如以太网有ip，串口有波特率等*/
	int (*control)(struct media_if *p, int type, void *input, void *output);  //介质控制。
	int (*init)(struct media_if *p); //初始化函数指针
	int (*close)(struct media_if *p); //关闭函数指针
	int (*check)(struct media_if *p); //介质可用性检查函数指针
	int (*send) (struct media_if *p, char *msg, int len); 	//介质发送信息函数
	int (*recv)(struct media_if *p, char *buf, int len);		//接收处理函数

	int	devnum;				//该介质连接的设备数量
	struct ext_dev *list;			//该介质上存在的外设链表
	struct media_if  *next;       /*下一个媒介*/
};

struct reqctl
{
	char 	sign; 			//信息特征码。
	char	len;			//控制参数个数
	int 	code[2];		//控制参数
};

struct outctl
{
		char type;	//信息类别
		char sign;	//信息特征码
		char *buf;	//信息空间，由调用者分配空间
		int buflen;	//空间大小
		int	inflen;	//信息大小
};
//外接设备定义
struct ext_dev
{
	short	dev_type;		//设备类型，一个设备可以是多个类型的复合设备。
	int		devid;			/*设备编号*/
	char	devname[32];		/*设备名称*/
	char	location[32];		/*设备安装位置*/
	int		recvtime;			//最近收到设备信息时间
	short	status;			/*设备状态，1 运行状态，0失联状态*/

	int (*control)(struct ext_dev *p, struct reqctl *req, struct outctl *output);  //设备控制函数，针对已有设备类型，定义一个通用操作函数。req是输入指令；output是输出信息；
	int (*init)(struct ext_dev *p); //初始化函数指针
	int (*close)(struct ext_dev *p); //关闭函数指针
	int (*keepalive)(struct ext_dev *p); //保活函数指针

	void *data;

	struct media_if *media;		/*指向媒介*/
	struct ext_dev	*next;		//设备链表
	struct ext_dev	*ifnext;		//同介质设备链表
};

#endif
