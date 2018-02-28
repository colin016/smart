#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "proto_marco.h"
#include "proto_structure.h"
#include "sys.h"
#include <math.h>
#include <time.h>
#include <sys/time.h>

//外部接口函数定义
int proto_init(struct KeyValueArray *cfg, moduleCallback callback,
		struct ModuleInfo *ret);
int proto_close(void);
int proto_query(int type, void *input, void *output);
int proto_config(int type, void *cfg);
int proto_control(int sendid, int type, void *input, void *output);

//面向界面

/*
 * 界面登录
 * 返回值：
 * 	0:验证通过
 * 	-1：密码或帐号错误！
 * 	-2：该帐号没有管理权限！
 * 	-3：没有该帐号
 */
int proto_interface_login(interface_login *data, void *output); //界面登录

//启动网络数据接收线程
void *proto_nethandle(void *arg);
//定时器处理线程
void *proto_timer(void *arg);

void check_isresend(time_t curtime);//检查平台是否需要重发报文
void check_iskeep(time_t curtime);  //检查平台是否需要保活报文
void check_relaters(time_t curtime); //检查关联人是否有超时或者重传超时
void check_postlist(time_t curtime); //检查是否有延迟处理超时
void check_update(time_t curtime); //检查是否需要进行更新
void check_synrelater(time_t curtime); //检查是否需要和平台同步关联人
void check_islogin(time_t curtime); //检查是否需要自动登录平台
void check_filedownload_from_server(time_t curtime);
void check_filedownload_to_app(time_t curtime);

//互斥锁加锁
int proto_mutex_lock(pthread_mutex_t *mutex);
int proto_mutex_unlock(pthread_mutex_t *mutex);

//初始化远程控制模块所需函数
bool init_protodata(moduleCallback callback); //初始化全局变量
bool init_moduleInfo(struct ModuleInfo *ret);      //初始化模块信息
int init_createSocket(void);                       //创建scoket
bool init_moduleConfig(struct KeyValueArray *cfg); //初始化模块配置
bool init_relater(void);                           //初始化关联人
bool init_platinfo(void);                          //初始化平台信息
void proto_free(void);

//关联人链表操作函数
bool relaterList_initialize(struct relater_inf *relnode); //关联人链表初始化
bool relaterList_add(struct relater_inf *relnode); //向关联人链表添加节点
bool relaterList_delete(char *relaterName);        //删除链表的节点
bool relaterList_modify(struct relater_inf relnode); //修改
struct relater_inf *relater_get_by_id(int id);
struct relater_inf *relater_get_by_name(char *username);

//重发报文链表
sendpkt_node *repsendlist_add(sendpkt_node *head, pkt_inf node);
sendpkt_node * repsendlist_del(sendpkt_node *head, int sendid);

//延迟处理链表
bool delaylist_add(struct delayhandle_node *node); //添加
bool delaylist_del(int sendseq); //删除
struct delayhandle_node *delaylist_find(int sendseq); //查找

//文件打开链表
bool openfilelist_add(struct file_node* filenode);
bool openfilelist_del(char *filename);
struct file_node *openfilelist_find(char *filename);

//加密算法
char * base64_encode(unsigned char *bindata, char * base64, int binlength); // base64加密
int base64_decode(const char * base64, unsigned char * bindata); // base64解密
char start_XOR(char *str0, char *str1, char *output); //异或加密
char * md54file(const char * filename, char * md5str, bool isupper); //md5

//解析报文里的json数据（string）
char *json_get_string(char *str, const char *opt, const char *opt1);
//解析报文里的json数据（int）
int json_get_int(char *str, const char *opt, const char *opt1);
char *json_print(char *str, const char *opt, const char *opt1);

//报文转换为数组
char *proto_basehead_to_str(struct pkt_inf *pkt);
char *proto_to_json(struct pkt_inf *pkt, void *data);
bool proto_sendmsg(int isrepeat, struct sockaddr_in *addr, struct pkt_inf *pkt,
		void *data); //报文发送(isrepeat:1-->是否表示重发报文)

void to_json_find(struct pkt_inf *pkt, void *data, cJSON *root); //局域网发现协议转化为json
void to_json_login(struct pkt_inf *pkt, void *data, cJSON *root); //登录协议的数据域转化为json数据
void to_json_accountman(struct pkt_inf *pkt, void *data, cJSON *root); //平台帐号管理协议的数据域转化为json数据
void to_json_remctl(struct pkt_inf *pkt, void *data, cJSON *root); //远程控制协议的数据域转化为json数据
void to_json_query(struct pkt_inf *pkt, void *data, cJSON *root); //数据查询协议的数据域转化为json数据
void to_json_deliver(struct pkt_inf *pkt, void *data, cJSON *root); //消息传递协议数据域转化为json数据
void to_json_upgrade(struct pkt_inf *pkt, void *data, cJSON *root); //升级查询协议的数据域转化为json数据
void to_json_filetransmit(struct pkt_inf *pkt, void *data, cJSON *root); //文件传输协议的数据域转化为json数据
void to_json_synch(struct pkt_inf *pkt, void *data, cJSON *root); //关联管理协议的数据域转化为json数据

//报文头部解析
bool proto_pkt_base_parse(struct pkt_inf *pkt);

//局域网发现协议
bool proto_find_request_parse(struct pkt_inf *pkt);
bool find_reply_start(struct pkt_inf *pkt, struct relater_inf *user);
//连接管理协议(前端设备登录)
bool proto_login_parse(pkt_inf *pkt);

bool proto_login_request_start(int source, char *output); //前端设备登录到服务器
bool proto_login_reply_parse(struct pkt_inf *pkt); /*前端设备登录到服务器时，服务器返回的应答报文解析*/
bool proto_verify_reply_parse(struct pkt_inf *pkt); //前端设备登录到服务器时，服务器返回验证应答报文
bool login_verify_request_start(pkt_inf *pkt); //构造一个验证请求报文发给服务器
bool proto_loginout_request_start(void); //前端设备登出请求
bool proto_keeplive_request_start(int source); //前端设备向服务器保活请求报文
bool proto_keeplive_reply_parse(struct pkt_inf *pkt); //服务器回应的应答报文
//连接管理协议(关联人登录)
bool proto_login_request_parse(struct pkt_inf *pkt); //关联人登录请求报文解析
bool proto_login_reply_start(struct pkt_inf *pkt); //关联人登录应答报文
bool proto_verify_request_parse(struct pkt_inf *pkt); //关联人验证请求报文解析
bool proto_verify_reply_start(struct pkt_inf *pkt); //关联人验证应答报文
bool proto_keeplive_request_parse(struct pkt_inf *pkt); //关联人发起的保活请求报文
bool proto_keeplive_reply_start(struct pkt_inf *pkt); //前端设备，保活应答报文
bool proto_loginout_request_parse(struct pkt_inf *pkt);

//平台帐号管理协议
bool proto_platman_parse(pkt_inf *pkt);
bool proto_register_request_start(register_data *regdata, void *output); //前端设备帐号注册
bool proto_register_reply_parse(pkt_inf *pkt); //前端设备注册应答报文解析
bool proto_accmod_request_start(register_data *regdata, void *output); //前端设备帐号修改请求
bool proto_accmod_reply_parse(pkt_inf *pkt); //前端设备帐号修改请求应答
bool proto_acckill_request_start(void *output); //前端设备帐号注销
bool proto_acckill_reply_parse(pkt_inf *pkt); //前端设备帐号注销请求应答

//远程控制协议
bool proto_remote_control(pkt_inf *pkt_request, data_control_reply *data_reply); //执行远程控制命令
bool proto_remctl_parse(pkt_inf *pkt);
bool proto_control_request_parse(pkt_inf *pkt); //远程控制请求解析
bool proto_control_reply_start(pkt_inf *pkt); //远程控制请求应答

//数据查询协议
bool proto_query_parse(pkt_inf *pkt);
bool proto_query_request_parse(pkt_inf *pkt); //数据查询请求报文解析
bool proto_query_reply_start(pkt_inf *pkt); //数据查询请求报文应答

//消息传递协议
bool proto_deliver_parse(pkt_inf *pkt);
bool proto_deliver_reauest_start(void); //消息推送请求报文
bool proto_deliver_reply_parse(pkt_inf *pkt); //消息推送应答报文解析

//升级查询协议
bool proto_upgrade_parse(pkt_inf *pkt);
bool proto_upgrade_request_start(void); //升级查询请求
bool proto_upgrade_reply_parse(pkt_inf *pkt); //升级查询应答报文解析

//文件传输协议
bool proto_filetransmit_parse(pkt_inf *pkt);
bool proto_filetransmit_request_start(struct delayhandle_node *delay_node); //前端设备向服务器发送文件传输请求
bool proto_filetransmit_reply_parse(pkt_inf *pkt); //前端设备收到服务器的应答报文
bool proto_filetranstop_request_start(char *url); //前端设备向服务器发送终止文件传输请求
bool proto_fileinfo_request_start(void); //前端设备向服务器发送文件信息请求
bool proto_fileinfo_reply_parse(pkt_inf *pkt); //前端设备向收到务器发送文件信息应答

bool proto_filetransmit_request_parse(pkt_inf *pkt); //管理app发送的文件件传输请求
bool proto_filetransmit_reply_start(struct file_node *filenode); //给管理app的文件件传输请求应答报文
bool proto_filetranstop_request_parse(pkt_inf *pkt); //管理app发送的文件终止传输请求
bool proto_fileinfo_request_parse(pkt_inf *pkt); //管理app的文件信息请求
bool proto_fileinfo_reply_start(pkt_inf *pkt); //给管理app的文件信息请求应答报文

//关联管理协议
bool proto_synch_parse(pkt_inf *pkt);
bool proto_relateacc_request_parse(pkt_inf *pkt); //关联帐号请求解析
bool proto_relateacc_reply_start(pkt_inf *pkt); //关联帐号请求应答报文
bool proto_separate_request_parse(pkt_inf *pkt); //脱联帐号请求报文解析
bool proto_separate_reply_start(pkt_inf *pkt); //脱联帐号请求应答报文
bool proto_getaccount_request_parse(pkt_inf *pkt); //获取关联帐号请求
bool proto_getaccount_reply_start(pkt_inf *pkt); //获取关联帐号应答
bool proto_synch_request_start(void); //关联同步请求(发给服务器)
bool proto_synch_reply_parse(pkt_inf *pkt); //关联同步应答解析(来自服务器)
bool proto_synch_request_parse(pkt_inf *pkt); //关联同步请求解析(来自服务器)
bool proto_synch_reply_start(pkt_inf *pkt); //关联同步应答(发给服务器)
bool proto_relmod_request_parse(pkt_inf *pkt); //变更关联信息请求解析
bool proto_relmod_reply_start(pkt_inf *pkt); //变更关联信息应答
#endif
