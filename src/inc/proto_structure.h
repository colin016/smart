#ifndef PROTOCOL_STRUCTURE_H_
#define PROTOCOL_STRUCTURE_H_

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "cJSON.h"
#include "dllinf.h"
#include "sys.h"

//面向界面
typedef struct interface_login {
	char username[20];
	char password[20];
} interface_login;

typedef struct data_find_request { //局域网广播通告报文选项
	char user[20]; //广播者帐号，8-20个字符
} data_find_request;

typedef struct data_find_reply { //前端设备应答报文选项
	char info[20]; //前端设备帐号。
	int ret; //与管理APP关系代码：0管理权限；1配置权限；2查看权限；3接收权限； -1 没有关系。
} data_find_reply;

typedef struct data_login_request { //登录请求报文选项表
	char snum[9];	//软件编号，8个数字
	char sver[7];	//软件版本号，6个十六进制数xxyyzz
	char user[20];	//用户帐号：8-20个字符构成字符串
	int link;		//短暂离线后快速登录时使用（可选）
} data_login_request;

typedef struct data_login_reply {		//登录应答报文选项
	/*错误原因、随机数或登录者ID，
	 * 当RET=-1时错误原因；
	 * 当RET=1时为随机数字符串；
	 * 当RET=0时返回登录者的ID。如果登录前端设备，
	 * 前端设备将SID作为登录ID返回。登录分配的LINK需要保存起来，以备后用。
	 **/
	char info[INFO_LEN];
	int link;		//RET=0时有效，增强安全性（可选）
	int ret;		//返回代码，0表示通过登录验证；-1表示出错；1表示需要验证
} data_login_reply;

typedef struct data_verify_request {		//验证请求报文选项
	char code[40];		//先对随机数用密码加密，采用XOR加密。然后对加密的数据用base64编码。
	char user[20];		//用户帐号：8-20个字符构成字符串
} data_verify_request;

typedef struct data_verify_reply {
	int ret;		//返回代码，0表示通过登录验证；-1表示出错
	int link;		//RET=0时有效，可增强安全性(可选)
	char info[INFO_LEN];
/*
 * 错误原因、登录者ID，当RET=-1时错误原因；
 * 当RET=0时返回登录者ID。
 * 如果登录前端设备，前端设备并不检查SID正确性，直接返回验证请求报文的SID即可
 */
} data_verify_reply;

typedef struct data_loginout_request {
	char user[20];		//用户帐号：8-20个字符构成字符串
	int link;		//增强安全性（可选）
} data_loginout_request;

struct register_dev {
	char serial[30]; //设备序列号
	char barcode[20]; // 设备条形码
	double longitude; //设备位置——经度
	double latitude; //设备位置——纬度
	int number; //最大关联人数量
	int class; //类型
	char phone[12]; //手机号
};

typedef struct data_register_request { //申请帐号报文选项
	char snum[9]; //软件编号，8个数字
	char sver[7]; //软件版本号，6个十六进制数
	char user[20]; //用户帐号：8-20个字符构成字符串
	char password[20]; //密码
	struct register_dev info; //注册辅助信息，不同类型要求不同。
} data_register_request;

typedef struct data_register_reply { //申请帐号报文选项
	int ret; //返回代码，0表示通过申请验证；-1表示帐号申请失败；1表示帐号存在。
	char info[INFO_LEN]; //错误原因或者ID，当RET=-1时错误原因；当RET>=0时为帐号成功申请后的ID。
} data_register_reply;

typedef struct data_accmod_request { //修改帐号请求报文选项
	char user[20]; //用户帐号：8-20个字符构成字符串
	int link; //增强安全性（可选）
	char password[20]; //修改后的用户密码（如果没有密码修改，不出现此选项）
	struct register_dev info; //依次填写新的辅助信息。（如果没有一个辅助信息修改，协议可以不出现此选项）
} data_accmod_request;

typedef struct data_accmod_reply { //修改帐号报文选项
	int ret; //返回代码，0表示通过变更；-1表示帐号变更失败；1表示帐号已经这样变更过
	char info[INFO_LEN]; //错误原因、时间，当RET=-1时错误原因；当RET>=0时为帐号变更成功后用户的ID（大多没有变化）
} data_accmod_reply;

typedef struct data_acckill_request { //注销帐号请求报文选项
	char user[20]; //要注销的帐号，8-20个字符
	int link; //RET=0时，可传输连接号增强安全性（可选）
	char info[INFO_LEN]; //用户密码加密的电话号码base64编码
} data_acckill_request;

typedef struct data_acckill_reply { // 注销帐号应答报文选项
	int ret; //返回代码，0表示通过注销；-1验证失败；1表示帐号已经注销过。
	char info[INFO_LEN]; //错误原因或者注销时间，当RET=-1时为错误原因；>=0时为注销帐号成功的时间
} data_acckill_reply;

typedef struct data_control_request { //控制请求报文选项
	char user[20]; //请求者帐号，8-20个字符
	/*指令代码：
	 * 0获取最近报警信息；
	 * 1获取当前登录账户；
	 * 2强制登录账号离线；
	 * 3获取配置信息；
	 * 4获取日志信息；
	 * 5获取当前图片；
	 * 6获取当前实时视频；
	 * 7 获取插件信息；
	 * 10自定义命令*/
	int inst;
	/*命令参数：
	 * 0-1限制数量；
	 * 2账号；
	 * 5分辨率；
	 * 6实时视频参数；
	 * 10具体自定义命令所需参数，没有就填0*/
	char para[20];
	int link; //增强安全性（可选）
} data_control_request;

typedef struct data_control_reply { //控制应答报文选项
	/*返回代码，-1表示请求失败；0-n表示控制成功。不同代码表示成功后返回信息类别
	 * 0：文本信息
	 * 1：文件信息
	 * 2：Base64编码的二进制信息
	 * 3：自定义信息，与请求的自定义命令有关
	 * */
	int ret;
	/*错误原因，格式：错误代码:错误信息或控制结果信息，具体如下：
	 * 文件信息：文件名
	 * 二进制信息：base64编码
	 * 自定义信息：自定义*/
	char *info;
} data_control_reply;

typedef struct data_query_request { //数据查询请求报文选项
	char user[20]; //请求者帐号，8-20个字符
	/*数据查询代码，具体如下，可以扩展：
	 * 64用户id；
	 * 0所有未读信息数量数；
	 * 1未读报警消息数；
	 * 2未读聊天消息数；
	 * 4未读系统通知数；
	 * 8未读联系人通知数；
	 * 16未读关联通知数。*/
	int code;
	char para[20]; //数据查询需要的参数，没有参数不需此选项
	int link; //增强安全性（可选）
} data_query_request;

typedef struct data_query_reply { //数据查询应答报文选项
	int ret; //返回代码，-1表示请求失败；>=0表示查询成功的新消息数量。
	int id; //如果查询ID时，具有此选项
	char info[INFO_LEN]; //错误原因：代码:错误原因（出错时）未读消息代码（查询成功时有效）
} data_query_reply;

/*USER：消息产生者的ID
 DEST：消息接收者ID：消息接收者ID（>10000）；接收者类别(<100)：0 所有APP用户；1前端设备关联人；2 所有设备管理员；3 厂商设备用户
 CLASS: 1报警消息；2聊天消息； 4系统通告 8 联系人通知消息 16 关联人通知
 DATE：消息产生时间，一个整数值。
 CONTENT：消息内容，CLASS不同而不同。
 **当CLASS=1时，由SORT类别和LEVEL级别和ATTTCH构成；
 **=2时，由MSG消息和TYPE字型、COLOR构成；
 **=4时，由TITLE标题、MSG消息、SET字符集和ATTACH构成。
 **=8时，由 KIND, MSG构成（仅用于服务器向APP推送消息）
 **=16时，由PUSER, KIND,RIGHT, PASS, MSG构成（服务器或者前端设备发送通知）
 PUSER	整数	关联的前端设备ID。
 KIND	字节型	报文子类别：0请求确认；1用户确认；2、用户否认；3 解除通知；4 请求通知（不需要用户确认）
 RIGHT	字节型	权限代码：0管理权限；1操控权限；2查看权限；3接收权限；
 MSG	字符串	留言信息
 PASS	字符串	设定的登录前端设备密码（当不允许登录或者脱离关联时，此项为null)*/
typedef struct data_deliver_request { //消息推送请求报文选项
	int num; //消息数量
	//char *info;
	int link;	//增强安全性（可选）
	cJSON *json_info;	//消息体采用JSON数组格式，其关键字如下：(USER, DEST, CLASS,DATE,CONTENT)
} data_deliver_request;

typedef struct data_deliver_reply {	//消息推送应答报文选项
	int ret;	//返回代码，-1表示请求失败；0表示请求成功。
	char info[INFO_LEN];	//错误原因或“OK”。
} data_deliver_reply;

typedef struct data_upgrade_request {	//升级查询请求报文选项
	char user[20];	//用户账号
	int serial;/*请求查询程序类型码：0管理APP、1前端设备的程序、2前端设备插件、3前端设备动态库*/
	char version[20];	//程序当前版本号XXYYZZ
} data_upgrade_request;

typedef struct data_upgrade_reply {
	int ret;	//返回代码，-1表示请求失败；0表示已经是最新版本；n>0表示存在n个新版本。
	char info[INFO_LEN];
/*RET=-1: 错误原因，格式：错误代码:错误信息
 * RET =0: 当前版本号信息。
 * RET > 0: 当前最新版本信息，JSON格式表示。新版本号VERSION，文件名URL*/
} data_upgrade_reply;

typedef struct data_filetransmit_request {
	int class;
	char file[FILE_URL_MAXLEN];
	int size;
	int from;
	int index;
	int link;
} data_filetransmit_request;

typedef struct data_filetransmit_reply {
	int ret;
	char info[40];
	int len;
	char content[FILETRAN_MAXLEN];
	char buffer[FILETRAN_MAXLEN];
} data_filetransmit_reply;

typedef struct data_filetranstop_request {
	char file[FILE_URL_MAXLEN];
	int link;
} data_filetranstop_request;

typedef struct data_fileinfo_request {
	char file[FILE_URL_MAXLEN];
	int sort;
	int link;
} data_fileinfo_request;

struct fileinfo_reply_info {
	char MD5[33];
	int size;
	char file[FILE_URL_MAXLEN];
};

typedef struct data_fileinfo_reply {
	int ret;
	struct fileinfo_reply_info fileinfo;
	char info[INFO_LEN];
	cJSON *json_info;
} data_fileinfo_reply;

typedef struct data_relateacc_request {
	char ref;
	char user[20];
	char muser[20];
	char puser[20];
	char alias[20];
	int right;
	char password[20];
	int link;
} data_relateacc_request;

typedef struct data_relateacc_reply {	//关联帐号应答报文选项
	int ret;	//返回代码，0表示等待确认；-1表示关联帐号失败；1表示帐号已经存在关联关系或者自动确认。
	char info[INFO_LEN];	//错误原因、时间，当RET=-1时为错误原因；当RET>=0时为关联帐号成功的时间（毫秒数）
} data_relateacc_reply;

typedef struct data_separate_request { //脱联帐号请求报文选项
	char ref; //1表示管理员提出0表示自我申请
	char user[20]; //申请者帐号，8-20个字符
	char muser[20]; //管理关联人帐号，8-20个字符。（自我申请时不需要此选项）
	char puser[20]; //前端设备帐号，8-20个字符
	int link; //申请者的登录连接号
} data_separate_request;

typedef struct data_separate_reply { // 脱联帐号应答报文选项
	int ret; //返回代码，0表示通过脱联；-1表示脱联联帐号失败；1表示帐号已经脱联。
	char info[INFO_LEN]; //当RET=-1时为错误代码:错误原因；>=0时为脱联帐号成功的时间
} data_separate_reply;

typedef struct data_getacc_request { //获取关联帐号请求报文选项
	char user[20]; //申请者帐号，8-20个字符
	/*
	 * 获取类别：
	 * 0、所有关联的前端设备帐号；
	 * 1、获取某个前端设备上的所有管理关联人帐号；
	 * 2、获取前端设备上所有关联人帐号。
	 * */
	int kind;
	char puser[20]; //前端设备帐号，8-20个字符（仅当KIND为1、2时，且没有登录前端设备时有效）
	int link; //申请者的登录连接号
} data_getacc_request;

typedef struct data_getacc_reply { //获取关联帐号应答报文选项
	int ret; //返回代码，-1表示请求失败；n表示获得帐号数量
	/*当RET=-1时为错误原因；
	 * ET>0时为获取帐号的信息，采用JSON数组格式JSON关键字为ACCOUNT、RIGHT
	 **/
	char info[INFO_LEN];
	cJSON *json_info;
} data_getacc_reply;

typedef struct data_synch_request { //关联同步请求报文选项
	char user[20]; //前端设备帐号，8-20个字符
	/*同步类型：
	 * 1 设备要求平台对比；
	 * 2 设备强制平台更新；
	 * 3 设备请求平台信息；
	 * 11 平台要求设备对比；
	 * 12 平台要求设备更新；
	 * 13 平台请求设备信息
	 **/
	int kind;
	//char *info;
	/*要求对方强制更新或者对比时，传递到对方的关联人信息，采用JSON数组格式，内容如下：
	 *((关联人帐号ACCOUNT，权限RIGHT，别名ALIAS，密码PASS，设置日期时间TIME，状态STATE)，.......)
	 *表中的密码为0表示不许登录设备
	 *状态含义：0待审核，1通过，-1拒绝
	 *同步成功后，平台或者设备删除状态为-1的关联人
	 *KIND为3或者13时，没有此选项*
	 **/
	cJSON *json_info;
} data_synch_request;

typedef struct data_synch_reply { //关联同步应答报文选项
	int ret; //返回代码，-1表示请求失败；n表示获得的关联信息数量，0表示没有INFO信息
	char info[INFO_LEN];
	cJSON *json_info;
} data_synch_reply;

typedef struct data_relmod_request { //变更关联信息请求报文选项
	char ref; //1表示管理关联人变更 0表示自己变更
	char user[20]; //申请者帐号，8-20个字符
	char muser[20]; //管理关联人帐号，8-20个字符。（自我申请时不需要此选项）
	char puser[20]; //前端设备帐号，8-20个字符
	int kind; //1别名 2密码 4 权限
	char alias[20]; //新别名（KIND&1为真时有此选项）
	char pass[20]; //新密码（KIND&2为真时有此选项）
	int right; //新权限（KIND&4为真时有此选项）
	int link; //申请者的登录连接号
} data_relmod_request;

typedef struct data_relmod_reply { //变更关联信息应答
	int ret;
	char info[INFO_LEN];
} data_relmod_reply;

//平台帐号注册数据
typedef struct register_data {
	char account[20]; //登录帐号
	char password[20]; //登录密码
	char serial[30]; //设备序列号
	char code[20]; // 设备条形码
	double longitude; //设备位置——经度
	double latitude; //设备位置——纬度
	int number; //最大关联人数量
} register_data;

// 1、配置信息，将上面保存在文件中的信息调入内存便于随时使用。
struct proto_config {
	char devname[20];  //设备名称
	char password[20]; //设备登录平台密码
	short int maxnum;  //最大关联人数量
	char barcode[20];  //设备条形码
	char identity[30]; //设备唯一标识符
	int deviceid;      //设备注册后的id，没有则为0

	double longitude; //设备位置——经度
	double latitude;  //设备位置——纬度

	char isnopass;  //是否需要密码（管理app登录前端设备时）
	char isshow;    //是否在界面显示密码
	char isauto;    //是否自动登录平台服务器
	char iscanceal; //是否在平台上已经注销
	char isfind;    //是否支持局域网自动发现协议
	char isupdate;  //是否自动更新前端设备软件
	char issock;    //网络是否连通

	char ipaddr[18];        //服务器IP地址
	unsigned short remport; //服务器登录端口
	unsigned short locport; //本地通信绑定端口

	char sysdir[20];    //系统文件目录名
	char plugin[30];    //插件文件名
	char updatedir[20]; //自动升级程序下载目录
	char relate[10];    //关联人文件名
	char alarm[10];     //报警消息文件名
	char control[10];   //远程控制日志文件名
};

struct pkt_base {
	// char tag[4];
	unsigned char cmd;
	unsigned char type;
	unsigned char opt;
	unsigned char sort;
	int sid;
	int sendseq;
	int ackseq;
};

typedef struct pkt_inf //协议报文信息
{
	char *buff;
	int len;
	time_t recvtime;
	struct sockaddr_in peeraddr;

	struct pkt_base basehead;

	int did;
	int keyseq;
	struct sockaddr_in souraddr;
	void *parsed_data;
	// char *data;
	void *own_node;
} pkt_inf;

typedef struct sendpkt_node {
	struct pkt_inf node;   //发送的报文
	unsigned char sendcnt; //最近报文重发次数
	time_t sendtime; //最近发送时间

	struct sendpkt_node *front;
	struct sendpkt_node *next;
} sendpkt_node;

typedef struct plat_inf {
	int link;
	time_t logintime;               //最近登录平台时间
	int loginstatus;                //登录状态
	char info[50];
	struct sockaddr_in server_addr; /*保存服务器的网络，根据配置得到*/
	pthread_mutex_t waitlist_lock;                //等带重发报互斥锁
	pthread_mutex_t lock;	//登录互斥锁
	pthread_cond_t cond;	//登录条件量

	time_t update;  //最近检查更新的时间
	time_t syntime; //最近检查同步关联人的时间

	int recvseq;     //最近收到的平台报文序号
	time_t recvtime; //最近收到的平台报文时间
	time_t sendtime; //最近发送给平台报文时间
	int sendseq;     //最近发送给平台报文序号
	int keeplivetime;     //保活次数统计

	int waitnum;                   //等待重发报文数量
	struct sendpkt_node *waitlist; //等带重发报文列表
} plat_inf;

typedef struct relater_inf {
	char username[20]; //关联人登录名称
	char password[20]; //登录密码
	char alias[20];
	char right;    //用户权限
	char regstatus;    //用户状态
	char loginstatus;

	struct sockaddr_in addr;

	time_t rond;    //登录时生成的随机数
	time_t link;
	time_t logintime; //登录时间
	time_t regtime;
	int id; //关联人的id，中转时用

	int recvseq;     //最近收到关联人报文序号
	time_t recvtime; //最近收到关联人报文时间
	int sendseq;     //最近发送关联人报文序号
	time_t sendtime; //最近发送关联人报文时间

	int waitnum;                   //等待重发报文数量
	struct sendpkt_node *waitlist; //等带重发报文列表
	struct relater_inf *next;
} relater_inf;

struct relater_list {
	int size;
	int maxsize;
	pthread_mutex_t mutex;
	struct relater_inf *head;
};

typedef struct file_inf {
	char filename[20]; //文件名
	char url[FILE_URL_MAXLEN];
	int index;
	int from;
	int sendseq; 		//请求报文的序号
	FILE *fp;          	//文件打开指针
	int req_size;   	//每次请求大小
	int send_size; 		//对于没次的请求，发送的大小
	int total_sendsize;
	int size;         	//文件的大小
	time_t time; 		//最近操作文件时间，便于定时器检查关闭
	bool is_send;
	struct sockaddr_in addr;
} file_inf;

struct file_node {
	struct file_inf node;
	struct file_node *next;
};

struct download {
	char url[FILE_URL_MAXLEN]; //要下载文件名
	char md5[32];
	int size;
	char buffer[FILETRAN_MAXLEN + 1]; //下载文件时的缓冲区
	int total_len;
	int req_size;
	int recv_size;
	char is_send;
};

//延迟处理节点信息
struct delayhandle_node {
	int sendseq;   //发送报文的序号
	time_t time;       //处理时间
	char taskclass; //任务类别：0下载，1获取未读信息

	struct download down_info;

	char code;    //要查询信息代码
	struct delayhandle_node *next;
};

struct proto_data {
	int recvnum;      //接收报文数量
	int sendnum;      //发送报文数量
	int recvrenum;    //接收重复报文数量
	int sendrenum;    //发送重复报文数量
	int relateallnum; //关联人总数
	int relatelogin;  //登录关联人数量
	int filenum;      //打开文件数量

	int sequence; //系统唯一数产生器

	pthread_mutex_t data_lock; //访问除文件以外的其他锁
	pthread_mutex_t filelist_lock; //访问文件链表锁

	pthread_t timer_thread;     //定时器线程
	pthread_t nethandle_thread; //网络数据接收处理线程
	int timer_run;              //定时器是否运行
	int nethandle_run;          //网络接收线程是否运行
	int sockfd;                 //远程通信网络套接字描述符
	int epollfd;                // epoll描述符

	moduleCallback callback; //访问其他模块功能的回调函数指针
	struct proto_config cfg; //模块配置

	struct plat_inf platinf;         //平台数据信息
	struct relater_list relaterlist; //关联人列表

	struct file_node *filelist;         //打开文件列表
	struct delayhandle_node *delaylist; //延迟处理列表

// int (*proto_proc[12])(struct pkt_inf *msg);
};

struct loginf {
	char type; // 0错误，1 local operation， 2 remote control, 3 alarm;
	char username[20];
	char ret;      // 0成功，1失败
	char data[58]; //操作或者错误信息
};

//关联人管理结构体
struct relateinf {
	char type;         // 1增加2删除3修改
	char username[20]; //关联人登录名
	char password[20]; //登录密码
	char authority;    //用户权限
	char alias[20];    //关联人别名
};

#endif
