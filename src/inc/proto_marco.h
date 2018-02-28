#ifndef PROTOCOL_MARCO_H_
#define PROTOCOL_MARCO_H_

//模块用到的常量定义
//关联人文件宏定义
#define RELATER_FILE "../sys/auth.txt"

#define SYSDIR "../sys"
#define UPDATEDIR "../bak"

#define DOWNLOAD_DIR "../down/"
//软件定义
#define SOFTWARE_CODE "12345678"  /*软件编号*/
#define SOFTWARE_VERSION "011103" /*软件版本号*/
#define DATA_MAXLEN 14000          /*报文数据最大长度*/
#define DATA_HEADLEN 20           /*报文基本首部长度*/
#define FILETRAN_MAXLEN 3072   //文件传输每次最大长度

//模块属性定义
#define MODULE_NAME "RemoteControl"
#define MODULE_VERSION 1

#define CALLBACK_CONCTL_ADD 5

//延迟任务类别
#define DELAY_CLASS_DOWNLOAD 0 //下载
#define DELAY_CLASS_GETMSG 1 //获取未读信息

//远程控制宏定义
#define REMCTL_GET_ALARM 0 //获取最近报警信息
#define REMCTL_GET_ACCOUNT 1 //获取当前登录账户
#define REMCTL_ACCOUNT_LOGINOUT 2 //强制登录账号离线
#define REMCTL_GET_CFGINFO 3 //获取配置信息
#define REMCTL_GET_LOGNFO 4 //获取日志信息
#define REMCTL_GET_PICTURE 5 //获取当前图片
#define REMCTL_GET_VIDEO 6 //获取当前实时视频
#define REMCTL_GET_PLUGINFO 7 //获取插件信息

//数据查询协议宏定义
#define QUERY_ID_NUM 64	//用户id
#define QUERY_MSG_NUM 0 //所有未读信息数量数
#define QUERY_ALARMMSG_NUM 1 //未读报警消息数
#define QUERY_RELATERMSG_NUM 16 //未读关联通知数
//升级查询，指令定义
#define UPGRADE_POLICEDEV_PROGRAM 1
#define UPGRADE_POLICEDEV_PLUGIN 2
#define UPGRADE_POLICEDEV_SO 3

//文件的传输方式
#define FILE_ENCODE 0 //带内编码
#define TILE_BINARY 1 //带内二进制
#define FILE_FTP 2 //ftp
#define FILE_HTTP 4 //http
#define FILE_TRANSFER_FTP 8//tp服务器中转
#define FILE_TRANSFER_HTTP 16 //服务器中转

//文件类型(文件信息请求报文)
#define FILE_TYPE_APP 0 //app
#define FILE_TYPE_PLUGIN 1 //插件
#define FILE_TYPE_MSGATTACH 2 //消息附件
#define FILE_TYPE_NOTIFYATTACH 3 //通知附件
#define FILE_TYPE_DEVFILE 4 //设备文件
#define FILE_TYPE_PICTURE 40 //图片
#define FILE_TYPE_VIDEO 41 //视频
#define FILE_TYPE_LOG 42 //日志
#define FILE_TYPE_CONFIG 43 //配置
#define FILE_TYPE_OTHER 44 //其他

//控制接口proto_control，宏定义
#define PROTO_CONTROL_ALARM 1 //请求发新报警消息
#define PROTO_CONTROL_RESULT 2 // 通告外设控制结果
#define PROTO_CONTROL_GETCFG 3 //获取当前配置信息
#define PROTO_CONTROL_SETCFG 4 //设置当前配置信息
#define PROTO_CONTROL_REGISTER 5 //请求注册
#define PROTO_CONTROL_MODIFY 6 //修改设备信息
#define PROTO_CONTROL_LOGOUT 7 //请求注销
#define PROTO_CONTROL_LOGIN 8 //登录
#define PROTO_CONTROL_LOGINOUT 9 //登出
#define PROTO_CONTROL_UPGRADE 10 //软件更新
#define PROTO_CONTROL_GETRELATERNUM 11 //获取关联人数量
#define PROTO_CONTROL_RELATERLIST 12 //关联人列表（根据数量开缓冲区，获取联系人数组）
#define PROTO_CONTROL_RELATERAUDIT 13 //审核关联人
#define PROTO_CONTROL_RELATERMAN 14 //关联人管理
#define PROTO_CONTROL_LOGEDINRELATER 15 //登录关联人详情
#define PROTO_CONTROL_CTLDEVICE 16 //远程控制
#define PROTO_CONTROL_UPDATEPLUGIN 17 //更新插件
#define PROTO_CONTROL_PLATCFG 18 //平台配置
#define PROTO_CONTROL_FINDSETTING 19 //局域网发现设置
#define PROTO_CONTROL_FINDQUERY 20 //局域网发现查看
#define PROTO_CONTROL_SYNCTIME 21 //平台同步设置(设置平台同步时间间隔)
#define PROTO_CONTROL_SYNCTNOW 22 //立即进行平台同步
#define PROTO_CONTROL_GETALARMNUM 23 //获取报警消息数量
#define PROTO_CONTROL_ALARMQUERY 24 //查看报警消息管理
#define PROTO_CONTROL_ALARMDEL 25 //删除一条报警消息
#define PROTO_CONTROL_ALARMCLREAN 26 //清空报警消息
#define PROTO_CONTROL_IFLOGIN 27 //界面登录

//平台信息关键字的宏定义
#define PROTO_CFG_DEVNAME "DEVNAME"
#define PROTO_CFG_PASSWORD "PASSWORD"
#define PROTO_CFG_MAXNUM "MAXNUM"
#define PROTO_CFG_BARCODE "BARCODE"
#define PROTO_CFG_IDENTITY "IDENTITY"
#define PROTO_CFG_DEVICEID "DEVICEID"
#define PROTO_CFG_LONGITUDE "LONGITUDE"
#define PROTO_CFG_LATITUDE "LATITUDE"
#define PROTO_CFG_ISSHOW "ISSHOW"
#define PROTO_CFG_ISAUTO "ISAUTO"
#define PROTO_CFG_ISACNCEL "ISCANCEAL"
#define PROTO_CFG_ISFIND "ISFIND"
#define PROTO_CFG_IPADDR "IPADDR"
#define PROTO_CFG_REMPORT "REMPORT"
#define PROTO_CFG_LOCPORT "LOCPORT"
#define PROTO_CFG_PLUGIN "PLUGIN"
#define PROTO_CFG_UPDATEDIR "UPDATEDIR"
#define PROTO_CFG_ISUPDATE "ISUPDATE"
#define PROTO_CFG_RELATE "RELATE"
#define PROTO_CFG_ALARM "ALARM"
#define PROTO_CFG_CONTROL "CONTROL"

//关联人权限定义
#define RELATER_AUTHORITY_MAN 0  /*管理权限*/
#define RELATER_AUTHORITY_COTL 1 /*控制权限*/
#define RELATER_AUTHORITY_LOOK 2 /*查看权限*/
#define RELATER_AUTJORITY_RECV 3 /*接收权限（不需要设置前端设备密码）*/

// json关键子宏定义(关联人关键字)
#define JSON_RELATERS "relaters"
#define JSON_ID "id"
#define JSON_USERNAME "username"
#define JSON_PASSWORD "password"
#define JSON_REGTIME "regtime"
#define JSON_RIGHT "right"
#define JSON_STATUS "status"

//协议常量定义
#define PROTO_BASETAG_LEN 4
#define PROTO_BASETAG "ZNAF"       /*智能安防基本首部头*/
#define PROTO_HOPTTAG "HOPT"       /*智能安防扩展首部*/
#define PROTO_DATATAG "DATA"       /*智能安防数据选项*/
#define PROTO_HOPT_DID "DID"       /*目的ip*/
#define PROTO_HOPT_KEYSEQ "KEYSEQ" /*密钥序号*/
#define PROTO_HOPT_SIP "SIP"       /*源IP*/
#define PROTO_HOPT_SPORT "SPORT"   /*源端口号*/
#define PROTO_PORT 5678            /*通信固定端口*/
#define PROTO_SERVERIP "127.0.0.1"

//命令号定义
#define ZNAFCMD_TEST 0     /*连通测试报文*/
#define ZNAFCMD_LOGIN 1    /*连通管理协议*/
#define ZNAFCMD_ACCOUNT 2  /*账号管理协议*/
#define ZNAFCMD_CONTROL 3  /*控制协议*/
#define ZNAFCMD_QUERY 4    /*数据查询协议*/
#define ZNAFCMD_DELIVER 5  /*消息传递协议*/
#define ZNAFCMD_FIND 6     /*局域网发现协议*/
#define ZNAFCMD_FILE 7     /*文件数据传递协议*/
#define ZNAFCMD_ENCODE 8   /*加密协议*/
#define ZNAFCMD_RELATE 9   //关联人管理协议
#define ZNAFCMD_CONTACT 10 //联系人管理
#define ZNAFCMD_UPDATE 11  //升级查询
#define ZNAFCMD_MAXNUM 12  //协议数量

/*报文类型定义*/
#define TYPE_TEST_QUEST 1 /*连通测试请求报文*/
#define TYPE_TEST_REPLY 2 /*连通测试应答报文*/

#define TYPE_LOGIN_QUEST 1        /*登录请求报文*/
#define TYPE_LOGIN_REPLY 2        /*登录应答报文*/
#define TYPE_LOGIN_CONFIRMQUEST 3 /*验证请求报文*/
#define TYPE_LOGIN_CONFIRMREP 4   /*验证应答报文*/
#define TYPE_LOGIN_LOGINOUT 5     /*登出*/
#define TYPE_LOGIN_KEEPQUEST 6    /*保活请求报文*/
#define TYPE_LOGIN_KEEPREPLY 7    /*保活应答报文*/

#define TYPE_ACCREG_QUEST 1  /*申请账号请求报文*/
#define TYPE_ACCREG_REPLY 2  /*申请账号应答报文*/
#define TYPE_ACCMOD_QUEST 3  //修改帐号请求
#define TYPE_ACCMOD_REPLY 4  //修改帐号应答
#define TYPE_ACCKILL_QUEST 5 //注销帐号请求
#define TYPE_ACCKILL_REPLY 6 //注销帐号应答

#define TYPE_UPGRADE_QUEST 1 //升级查询请求
#define TYPE_UPGRADE_REPLY 2 //升级应答

#define TYPE_CONTROL_QUEST 1 /*控制请求报文*/
#define TYPE_CONTROL_REPLY 2 /*控制应答报文*/

#define TYPE_QUERY_QUEST 1 /*数据查询请求报文*/
#define TYPE_QUERY_REPLY 2 /*数据查询应答报文*/

#define TYPE_DELIVER_QUEST 1 /*消息推送请求报文*/
#define TYPE_DELIVER_REPLY 2 /*消息推送应答报文*/
//#define TYPE_DELIVER_GROUP 3 /*消息推送请求报文*/

#define TYPE_FIND_QUEST 1 /*局域网广播通报报文*/
#define TYPE_FIND_REPLY 2 /*监控端应答报文*/

//升级查询
#define TYPE_UPGRADE_REQUEST 1 //升级查询请求
#define TYPE_UPGRADE_REPLY 2 //升级查询应答

#define TYPE_FILE_REQUEST 1 /*文件传输请求报文*/
#define TYPE_FILE_REPLY 2 /*文件传输应答报文*/
#define TYPE_FILESTOP_REQUEST 3  /*文件传输终止报文*/
#define TYPE_FILEINFO_REQUEST 4  /*文件信息请求*/
#define TYPE_FILEINFO_REPLY 5    /*文件信息应答*/

#define TYPE_RELATE_QUEST 1 /*关联账号请求报文*/
#define TYPE_RELATE_REPLY 2 /*关联账号应答报文*/
#define TYPE_RELOUT_QUEST 3 /*脱联账号请求报文*/
#define TYPE_RELOUT_REPLY 4 /*脱联账号应答报文*/
//#define TYPE_RELINF_QUEST 5 /*脱联/关联账号通知请求报文*/
//#define TYPE_RELINF_REPLY 6 /*脱联/关联账号通知应答报文*/
#define TYPE_RELGET_QUEST 5 /*获取关联账号请求报文*/
#define TYPE_RELGET_REPLY 6 /*获取关联账号应答报文*/
#define TYPE_RELSYN_QUEST 9 /*同步关联账号请求报文*/
#define TYPE_RELSYN_REPLY 10 /*同步关联账号应答报文*/
#define TYPE_RELMOD_REQUEST 11 /*变更关联信息请求*/
#define TYPE_RELMOD_REPLY 12 /*变更关联信息应答*/

//登录请求报文关键字
#define LOGIN_QUEST_SNUM "SNUM"
#define LOGIN_QUEST_SVER "SVER"
#define LOGIN_QUEST_USER "USER"
#define LOGIN_QUEST_LINK "LINK"
//登录应答报文关键字
#define LOGIN_REPLY_INFO "INFO"
#define LOGIN_REPLY_RET "RET"
#define LOGIN_REPLY_LINK "LINK"

#define LOGIN_RET_SUCCESSED 0
#define LOGIN_RET_ERROR -1
#define LOGIN_RET_VERIFY 1
//验证请求关键字
#define CONFIRM_QUEST_CODE "CODE"
#define CONFIRM_QUEST_USER "USER"
//验证应答关键字
#define VERIFY_REPLY_RET "RET"
#define VERIFY_REPLY_LINK "LINK"
#define VERIFY_REPLY_INFO "INFO"

#define VERIFY_RET_SUCCESSED 0
#define VERIFY_RET_ERROR -1
//登出请求关键字
#define LOGINOUT_QUEST_USER "USER"
#define LOGINOUT_QUEST_LINK "LINK"

//局域网协议
#define FING_REQUEST_USER "USER"

#define FING_REPLY_RET "RET"
#define FIND_REPLT_INFO "INFO"

//前端设备注册请求协议
#define REGISTER_REQUEST_SNUM "SNUM"
#define REGISTER_REQUEST_SVER "SVER"
#define REGISTER_REQUEST_USER "USER"
#define REGISTER_REQUEST_PASSWORD "PASSWORD"
#define REGISTER_REQUEST_INFO "INFO"
#define REGISTER_REQUEST_INFO_SEQ "SEQ"
#define REGISTER_REQUEST_INFO_BARCODE "BARCODE"
#define REGISTER_REQUEST_INFO_LONGI "LONGI"
#define REGISTER_REQUEST_INFO_LATI "LATI"
#define REGISTER_REQUEST_INFO_MAX "MAX"
#define REGISTER_REQUEST_INFO_CLASS "CLASS"
#define REGISTER_REQUEST_INFO_PHONE "PHONE"
//前端设备注册应答协议
#define REGISTER_REPLY_RET "RET"
#define REGISTER_REPLY_INFO "INFO"
#define REGISTER_RET_SUCCESSED 0 //表示通过申请验证
#define REGISTER_RET_ERROR -1 //表示帐号申请失败
#define REGISTER_RET_EXIST 1  //表示帐号存在
//前端设备帐号修改请求
#define ACCMOD_REQUEST_USER "USER"
#define ACCMOD_REQUEST_LINK "LINK"
#define ACCMOD_REQUEST_PASSWORD "PASSWORD"
#define ACCMOD_REQUEST_INFO "INFO"
//前端设备帐号修改请求应答
#define ACCMOD_REPLY_RET "RET"
#define ACCMOD_REPLY_INFO "INFO"
#define ACCMOD_RET_ERROR -1
#define ACCMOD_RET_SUCCESSED 0
#define ACCMOD_RET_MODIFIED 1
//前端设备帐号注销请求
#define ACCKILL_REQUEST_USER "USER"
#define ACCKILL_REQUEST_LINK "LINK"
#define ACCKILL_REQUEST_INFO "INFO"
//前端设备帐号请求应答
#define ACCKILL_REPLY_RET "RET"
#define ACCKILL_REPLY_INFO "INFO"
#define ACCKILL_RET_SUCCESSED 0
#define ACCKILL_RET_ERROR -1
#define ACCKILL_RET_KILLED 1
//数据查询协议
#define QUERY_REPLY_RET "RET"
#define QUERY_REPLY_CODE "CODE"
#define QUERY_REPLY_INFO "INFO"
//远程控制
#define REMCTL_QUEST_USER "USER"
#define REMCTL_QUEST_INST "INST"
#define REMCTL_QUEST_RARA "RARA"
#define REMCTL_QUEST_LINK "LINK"
//远程控制应答
#define REMCTL_REPLY_RET "RET"
#define REMCTL_REPLY_INFO "INFO"
#define REMCTL_RET_ERROR -1
#define REMCTL_RET_TEXTINFO 0
#define REMCTL_RET_FILEINFO 1
#define REMCTL_RET_BASE64BIN 2
//#define REMCTL_RET_

//数据查询请求报文
#define QUERY_REQUEST_USER "USER"
#define QUERY_REQUEST_CODE "CODE"
#define QUERY_REQUEST_PARA "PARA"
#define QUERY_REQUEST_LINK "LINK"
//数据查询应答报文
#define QUERY_REPLY_RET "RET"
#define QUERY_REPLY_ID "ID"
#define QUERY_REPLY_ "INFO"

#define QUERY_RET_ERROR -1
//消息传递协议请求报文
#define DELIVER_REQUEST_NUM "NUM"
#define DELIVER_REQUEST_INFO "INFO"
#define DELIVER_REQUEST_LINK "LINK"
//消息传递协议应答报文
#define DELIVER_REPLY_RET "RET"
#define DELIVER_REPLY_INFO "INFO"
//升级查询请求
#define UPGRADE_REQUEST_USER "USER"
#define UPGRADE_REQUEST_SERIAL "SERIAL"
#define UPGRADE_REQUEST_VERSION "VERSION"
//升级查询应答
#define UPGRADE_REPLY_RET "RET"
#define UPGRADE_REPLY_INFO "INFO"
//文件传输请求
#define FILETRAN_REQUEST_CLASS "CLASS"
#define FILETRAN_REQUEST_FILE "FILE"
#define FILETRAN_REQUEST_SIZE "SIZE"
#define FILETRAN_REQUEST_FROM "FROM"
#define FILETRAN_REQUEST_INDEX "INDEX"
#define FILETRAN_REQUEST_LINK "LINK"
//文件传输应答
#define FILETRAN_REPLY_RET "RET"
#define FILETRAN_REPLY_INFO "INFO"
#define FILETRAN_REPLY_LEN "LEN"
#define FILETRAN_REPLY_CONTENT "CONTENT"

#define FILETRAN_RET_ERROR -1 //表示请求失败

//终止文件传输
#define FILETRANSTOP_REQUEST_FILE "FILE"
#define FILETRANSTOP_REQUEST_LINK "LINK"
//文件信息请求
#define FILEINFO_REQUEST_FILE "FILE"
#define FILEINFO_REQUEST_SORT "SORT"
#define FILEINFO_REQUEST_LINK "LINK"
//文件信息应答
#define FILEINFO_REPLY_RET "RET"
#define FILEINFO_REPLY_INFO "INFO"

#define FILEINFO_INFO_FILE "FILE"
#define FILEINFO_INFO_SIZE "SIZE"
#define FILEINFO_INFO_MD5 "MD5"

#define FILEINFO_RET_OK 0
#define FILEINFO_RET_ERROR -1
//关联帐号请求
#define RELATER_QUEST_REF "REF"
#define RELATER_QUEST_USER "USER"
#define RELATER_QUEST_MUSER "MUSER"
#define RELATER_QUEST_PUSER "PUSER"
#define RELATER_QUEST_ALIAS "ALIAS"
#define RELATER_QUEST_RIGHT "RIGHT"
#define RELATER_QUEST_PASSWORD "PASSWORD"
#define RELATER_QUEST_LINK "LINK"

#define RELATER_REF_MAN 1 //表示管理员分享
#define RELATER_REF_SELF 0 //表示自我申请

//关联帐号请求应答
#define RELATER_REPLY_RET "RET"
#define RELATER_REPLY_INFO "INFO"
#define RELATER_RET_ERROR -1
#define RELATER_RET_WAIT 0
#define RELATER_RET_OK 1

//脱联帐号请求报文
#define SEPARATE_REQUEST_REF "REF"
#define SEPARATE_REQUEST_USER "USER"
#define SEPARATE_REQUEST_MUSER "MUSER"
#define SEPARATE_REQUEST_PUSER "PUSER"
#define SEPARATE_REQUEST_LINK "LINK"

#define SEPARATE_REF_MAN 1 //表示管理员提出
#define SEPARATE_REF_SELF 0 //表示自我申请
//脱联帐号请求应答报文
#define SEPARATE_REPLY_RET "RET"
#define SEPARATE_REPLY_INFO "INFO"

#define SEPARATE_RET_ERROR -1 //表示脱联联帐号失败
#define SEPARATE_RET_OK 0 //表示通过脱联
#define SEPARATE_RET_SEPARATED 1 //表示帐号已经脱联
//获取关联帐号请求
#define GETACC_REQUEST_USER "USER"
#define GETACC_REQUEST_PUSER "PUSER"
#define GETACC_REQUEST_KIND "KIND"
#define GETACC_REQUEST_LINK "LINK"

#define GETACC_KIND_ALLDEVACC 0 //所有关联的前端设备帐号
#define GETACC_KIND_ALLMANREL 1 //获取某个前端设备上的所有管理关联人帐号
#define GETACC_KIND_ALLREL 2 //获取前端设备上所有关联人帐号
//获取关联帐号应答
#define GETACC_REPLY_RET "RET"
#define GETACC_REPLY_INFO "INFO"

#define GETACC_INFO_ACCOUNT "ACCOUNT"
#define GETACC_INFO_RIGHT "RIGHT"

#define GETACC_RET_ERROR -1
//关联同步请求报文
#define SYNCH_REQUEST_USER "USER"
#define SYNCH_REQUEST_KIND "KIND"
#define SYNCH_REQUEST_INFO "INFO"
//info信息定义
#define SYNCH_INFO_ID "ID"
#define SYNCH_INFO_ACCOUNT "ACCOUNT"
#define SYNCH_INFO_RIGHT "RIGHT"
#define SYNCH_INFO_ALIAS "ALIAS"
#define SYNCH_INFO_PASS "PASS"
#define SYNCH_INFO_TIME "TIME"
#define SYNCH_INFO_STATE "STATE"
//同步类型定义
#define SYNCH_CMP_WITH_SERVER 1 //设备要求平台对比
#define SYNCH_ENFORE_BY_DEV 2 //设备强制平台更新
#define SYNCH_GET_SERVER_INFO 3 //设备请求平台信息
#define SYNCH_CMP_WITH_DEV 11 //平台要求设备对比
#define SYNCH_UPDATE_BY_SERVER 12 //平台要求设备更新
#define SYNCH_GET_DEV_INFO 13 //平台请求设备信息

//关联同步应答
#define SYNCH_REPLY_RET "RET"
#define SYNCH_REPLY_INFO "INFO"

#define SYNCH_RET_ERROR -1

//变更关联信息请求
#define RELMOD_REQUEST_REF "REF"
#define RELMOD_REQUEST_USER "USER"
#define RELMOD_REQUEST_MUSER "MUSER"
#define RELMOD_REQUEST_PUSER "PUSER"
#define RELMOD_REQUEST_KIND "KIND"
#define RELMOD_REQUEST_ALIAS "ALIAS"
#define RELMOD_REQUEST_PASS "PASS"
#define RELMOD_REQUEST_RIGHT "RIGHT"
#define RELMOD_REQUEST_LINK "LINK"

#define RELMOD_KIND_ALIAS 1
#define RELMOD_KIND_PASS 2
#define RELMOD_KIND_RIGHT 4
//变更关联信息应答
#define RELMOD_REPLY_RET "RET"
#define RELMOD_REPLY_INFO "INFO"
#define RELMOD_RET_ERROR -1
#define RELMOD_RET_OK 0

//设备类型
#define SORT_MANAGEAPP 0
#define SORT_POLICEAPP 1
#define SORT_POLICEDEV 2
#define SORT_PLATSERVER 3

//选项宏定义
#define OPT_TRANSFER 1 /*表示此报文在服务器进行转发处理*/
#define OPT_ENCRYPT 2  /*表示此报文加密处理过*/
#define OPT_SOURADDR 4 /*表示此报文携带源地址，便于NAT穿透处理*/
#define OPT_REQUEST 8 /*表示此报文是请求报文或者需要确认的报文*/
#define OPT_RESPONSE 16 /*表示此报文是应答报文，确认号有效*/
#define OPT_NONJSON                                                            \
  32 /*表示此报文后面有非JSON格式数据，主要用于传输有二进制格式数据，不能完全使用JSON解析器进行解析*/

//模块定义
#define MODULE_MAIN 0
#define MODULE_CFG 1
#define MODULE_LOG 2
#define MODULE_ATTACH 3
#define MODULE_SMART 4
#define MODULE_FACE 5
#define MODULE_CONTROL 6

//状态定义
#define INIT_STATE 0
#define LOGINING_STATE 1
#define VALIDAT_STATE 2
#define WORK_STATE 3

#define REGSTATE_WAIT 0
#define REGSTATE_OK 1
#define REGSTATE_REJECT -1

//****************************************************
// source来源（0-定时器, 1-界面）
#define TIMER 0
#define UI 1

#define IS_REPEAT_TRUE 1
#define IS_REPEAT_FALSE 0

#define SUCCCESS 0
#define FAIL -1

#define true 0
#define false - 1
#define abandon 1

#define bool char
#define FILE_URL_MAXLEN 256

#define INFO_LEN 50
#define ERROR_NO_USER "10000:没有该用户"
#define ERROR_PASSWORD_WRONG "10001:密码错误"
#define ERROR_LOGINED "10002:帐号已登录"
#define ERROR_INST_INVALID "10003:指令无效"
#define ERROR_QUERYCODE_INVAILD "10004:查询代码无效"
#define ERROR_LINK_INVAILD "10005:link值不正确"
#define ERROR_DEVNAME_INVAILD "10006:前端设备帐号不正确"
#define ERROR_FILEURL_INVAILD "10007:文件url错误"
#define ERROR_USER_LONGINOUT "10008:该用户没有登录"
#endif
