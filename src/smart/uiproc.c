/*
    test.c -- Unit test program for GoAhead

    Usage: goahead-test [options] [documents] [endpoints...]
        Options:
        --auth authFile        # User and role configuration
        --home directory       # Change to directory to run
        --log logFile:level    # Log to file file at verbosity level
        --route routeFile      # Route configuration file
        --verbose              # Same as --log stderr:2
        --version              # Output version information

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <stdio.h>
#include <stdlib.h>

#include    "goahead.h"
#include    "js.h"
#include    "smart.h"


/********************************* Defines ************************************/

static int finished = 0;

#undef ME_GOAHEAD_LISTEN
#if ME_COM_SSL
    #define ME_GOAHEAD_LISTEN "http://127.0.0.1:80, https://127.0.0.1:4443, http://[::1]:8090, https://[::1]:4453"
#else
    #define ME_GOAHEAD_LISTEN "http://127.0.0.1:80, http://[::1]:8090"
#endif

/********************************* Forwards ***********************************/

static void initPlatform();
static void logHeader();
static void usage();

static bool testHandler(Webs *wp);
#if ME_GOAHEAD_JAVASCRIPT
static int aspTest(int eid, Webs *wp, int argc, char **argv);
static int bigTest(int eid, Webs *wp, int argc, char **argv);
#endif
static void actionTest(Webs *wp);
static void sessionTest(Webs *wp);
static void showTest(Webs *wp);
static void login_server(Webs *wp);		//登录界面
static int set_t(int eid,Webs *wp,int argc,char **argv);
static void set_time(Webs *wp);			//时间配置
static int  set_net(int eid,Webs *wp,int argc,char **argv);
static int  set_net1(int eid,Webs *wp,int argc,char **argv);
static int  set_net2(int eid,Webs *wp,int argc,char **argv);
static int  set_net3(int eid,Webs *wp,int argc,char **argv);
static int  set_net4(int eid,Webs *wp,int argc,char **argv);
static void set_network(Webs *wp);		//网络配置
static void set_user(Webs *wp);			//用户设置
static void user_manage(Webs *wp);		//用户 管理
static void sys_deploy(Webs *wp);		//系统配置
static void set_relation(Webs *wp);		//关联设置
static void contact_person(Webs *wp);	//添加关联人
static int goahead_dev_add(Webs *wp);     //添加设备
static int goahead_dev_delete(Webs *wp);    //删除设备
static int goahead_media_add(Webs *wp);			//添加媒介
static int goahead_media_delete(Webs *wp);	//删除媒介
static int register_e(int eid,Webs *wp,int argc,char **argv);
static void register_exit(Webs *wp);		//登录登出
static int terrace_in1(int eid,Webs *wp,int argc,char **argv);
static int terrace_in2(int eid,Webs *wp,int argc,char **argv);
static void terrace_info(Webs *wp);		//云台信息
static void terrace_syn(Webs *wp);		//云台同步

static void device_restart(Webs *wp);		//设备重启/*未做*/
static void software_update(Webs *wp);		//软件更新/*未做*/
static void backup_recovery(Webs *wp);		//备份恢复/*未做*/
static void update_plug(Webs *wp);		//上传插件/*未做*/
static void login_manage(Webs *wp);		//登录管理/*未做*/

static int  sys_info(int eid,Webs *wp,int argc,char **argv);		//系统信息
static int inter_info(int eid,Webs *wp,int argc,char **argv);		//网络信息
static int equipment_info(int eid,Webs *wp,int argc,char **argv);	//设备信息
static int equipment_list(int eid,Webs *wp,int argc,char **argv);		//设备列表
static int media_list(int eid,Webs *wp,int argc,char **argv);			//媒介列表
static int monitor_s1(int eid,Webs *wp,int argc,char **argv);
static int monitor_s2(int eid,Webs *wp,int argc,char **argv);
static int monitor_s3(int eid,Webs *wp,int argc,char **argv);
static int monitor_s4(int eid,Webs *wp,int argc,char **argv);
static int monitor_sys(int eid,Webs *wp,int argc,char **argv);		//监控库管理
static int alarm_messages(int eid,Webs *wp,int argc,char **argv);	//报警消息管理
static int contact_look(int eid,Webs *wp,int argc,char **argv);	//查看关联人
static int contact_check(int eid,Webs *wp,int argc,char **argv);		//审核关联人
static int  view_log(int eid,Webs *wp,int argc,char **argv);		//查看日志
static int area_net(int eid,Webs *wp,int argc,char **argv);	//局域网发现管理
static int user_man(int eid,Webs *wp,int argc,char **argv);	//用户管理
static char check_login();		//登录接口
static char check_time(char *checkbox,char *time_set1,char *time_set2,char *time_set3,char *timezone,char *tserver);		//时间配置接口
static char check_net(char *net_add,char *subnet_mask,char *default_gateway,char *main_dns,char *other_dns);//网络配置接口
static char check_user(char *us_name,char *pass,char *serial_number,char *longitude,char *latitude,char *max_num);	//用户设置接口
static char check_manage(char *identity,char *username);			//用户管理接口
static char check_dep(char *de_name,char *contact_way,char *de_position);			//系统配置接口
static char check_register(char *name_txt,char *password_txt);		//登录登出界面接口
static char check_rel(char *con_type,char *net_add);		//关联设置接口
static char check_per(char *username,char *othername,char *jurisdiction);		//添加关联人接口
static char check_ter(char *acc_num,char *old_password,char *new_password,char *yes);	//云台信息接口
static char check_terrace(char *t_interval);		//云台同步接口
static char goahead_dev_check(char *de_name,char *de_kind,char *de_num,char *de_position,char *de_belong,char *de_person);
#if ME_GOAHEAD_UPLOAD && !ME_ROM
static void uploadTest(Webs *wp);
#endif
#if ME_GOAHEAD_LEGACY
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags);
#endif
#if ME_UNIX_LIKE
static void sigHandler(int signo);
#endif
static void exitProc(void *de_person, int de_num);


////////cuicui add for get device addr/////////////////



int getdeviceipaddr(char *listenulr)
{
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];//缓冲区
    //初始化ifconf
    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket" );
        exit(1);
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)ifconf.ifc_buf;
    //printf("ifconf.ifc_len:%d\n",ifconf.ifc_len);
   // printf("sizeof (struct ifreq):%d\n",sizeof (struct ifreq));
	char tmpbuf[100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	int i=(ifconf.ifc_len/sizeof (struct ifreq));
    for (; i>0; i--)
    {
        if(ifreq->ifr_flags == AF_INET){ //for ipv4
            //printf("name =[%s]\n" , ifreq->ifr_name);
           // printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
		memset(tmpbuf, 0, sizeof(tmpbuf));
		sprintf(tmpbuf, "http://%s,",inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
		//printf("tmpbuf =%s\n",tmpbuf);
		strcat(listenulr, tmpbuf);
		ifreq++;
        }
    }
	//printf("Listen strint =%s\n",listenulr);
    //getchar();//system("pause");//not used in linux
    return 0;
}

/*********************************** Code *************************************/

int main(int argc, char **argv, char **envp)
{
    char    *argp, *home, *documents, *endpoints, *endpoint, *route, *tok, *lspec;
    int     argind, duration;

    route = "../sys/route.txt";
    //auth = "../sys/auth.txt";
    duration = 0;

    for (argind = 1; argind < argc; argind++) {
        argp = argv[argind];
        if (*argp != '-') {
            break;

        } else if (smatch(argp, "--auth") || smatch(argp, "-a")) {
            if (argind >= argc) usage();
            //auth = argv[++argind];

#if ME_UNIX_LIKE && !MACOSX
        } else if (smatch(argp, "--background") || smatch(argp, "-b")) {
            websSetBackground(1);
#endif

        } else if (smatch(argp, "--debugger") || smatch(argp, "-d") || smatch(argp, "-D")) {
            websSetDebug(1);

        } else if (smatch(argp, "--duration")) {
            if (argind >= argc) usage();
            duration = atoi(argv[++argind]);

        } else if (smatch(argp, "--home")) {
            if (argind >= argc) usage();
            home = argv[++argind];
            if (chdir(home) < 0) {
                error("Cannot change directory to %s", home);
                exit(-1);
            }
        } else if (smatch(argp, "--log") || smatch(argp, "-l")) {
            if (argind >= argc) usage();
            logSetPath(argv[++argind]);

        } else if (smatch(argp, "--verbose") || smatch(argp, "-v")) {
            logSetPath("stdout:2");

        } else if (smatch(argp, "--route") || smatch(argp, "-r")) {
            route = argv[++argind];

        } else if (smatch(argp, "--version") || smatch(argp, "-V")) {
            printf("%s\n", ME_VERSION);
            exit(0);

        } else if (*argp == '-' && isdigit((uchar) argp[1])) {
            lspec = sfmt("stdout:%s", &argp[1]);
            logSetPath(lspec);
            wfree(lspec);

        } else {
            usage();
        }
    }
    documents = ME_GOAHEAD_DOCUMENTS;
    if (argc > argind) {
        documents = argv[argind++];
    }

	man_init( ); //first init ourself  handler

    initPlatform();

    if (websOpen(documents, route) < 0) {
        error("Cannot initialize server. Exiting.");
        return -1;
    }
    logHeader();
    /*if (websLoad(auth) < 0) {
        error("Cannot load %s", auth);
        return -1;
    }*/
	// cuicui add for goahead listion
	char listenulr[500];
	memset(listenulr, 0, sizeof(listenulr));
	getdeviceipaddr(listenulr);
    if (argind < argc) {
        while (argind < argc) {
            endpoint = argv[argind++];
            if (websListen(endpoint) < 0) {
                return -1;
            }
        }
    } else {
        endpoints = sclone(listenulr);
        for (endpoint = stok(endpoints, ", \t", &tok); endpoint; endpoint = stok(NULL, ", \t,", &tok)) {
            if (getenv("TRAVIS")) {
                if (strstr(endpoint, "::1") != 0) {
                    /* Travis CI does not support IPv6 */
                    continue;
                }
            }
            if (websListen(endpoint) < 0) {
                return -1;
            }
        }
        wfree(endpoints);
    }
/*example for goahead*/
    websDefineHandler("test", testHandler, 0, 0, 0);
    websAddRoute("/test", "test", 0);
#if ME_GOAHEAD_LEGACY
    websUrlHandlerDefine("/legacy/", 0, 0, legacyTest, 0);
#endif
#if ME_GOAHEAD_JAVASCRIPT
    	websDefineJst("aspTest", aspTest);
    	websDefineJst("bigTest", bigTest);
    	websDefineJst("sys_info",sys_info);
    	websDefineJst("inter_info",inter_info);
   	websDefineJst("equipment_info",equipment_info);
	websDefineJst("equipment_list",equipment_list);
	websDefineJst("media_list",media_list);
	websDefineJst("monitor_sys",monitor_sys);
	websDefineJst("alarm_messages",alarm_messages);
	websDefineJst("contact_look",contact_look);
	websDefineJst("contact_check",contact_check);
	websDefineJst("view_log",view_log);
	websDefineJst("area_net",area_net);
	websDefineJst("set_net1", set_net1);
	websDefineJst("set_net2", set_net2);
	websDefineJst("set_net3", set_net3);
	websDefineJst("set_net4", set_net4);
	websDefineJst("set_net", set_net);
	websDefineJst("set_t", set_t);
	websDefineJst("terrace_in1", terrace_in1);
	websDefineJst("terrace_in2", terrace_in2);
	websDefineJst("register_e", register_e);
	websDefineJst("monitor_s1", monitor_s1);
	websDefineJst("monitor_s2", monitor_s2);
	websDefineJst("monitor_s3", monitor_s3);
	websDefineJst("monitor_s4", monitor_s4);
	websDefineJst("user_man", user_man);
#endif
    websDefineAction("test", actionTest);
    websDefineAction("sessionTest", sessionTest);
    websDefineAction("showTest", showTest);
#if ME_GOAHEAD_UPLOAD && !ME_ROM
    websDefineAction("uploadTest", uploadTest);
#endif


/*add goform for smart in this place*/
/*login_server is example for login,you can copy for other*/
// TODO:
	websDefineAction("login_server", login_server);
	websDefineAction("set_time", set_time);
	websDefineAction("set_network", set_network);
	websDefineAction("set_user",set_user);
	websDefineAction("sys_deploy",sys_deploy);
	websDefineAction("set_relation",set_relation);
	websDefineAction("terrace_info",terrace_info);
	websDefineAction("goahead_dev_add",goahead_dev_add);
    websDefineAction("goahead_dev_delete",goahead_dev_delete);
	websDefineAction("goahead_media_add",goahead_media_add);
    websDefineAction("goahead_media_delete",goahead_media_delete);
	websDefineAction("register_exit",register_exit);
	websDefineAction("terrace_syn",terrace_syn);
	websDefineAction("contact_person",contact_person);
	websDefineAction("user_manage",user_manage);

#if ME_UNIX_LIKE && !MACOSX
    /*
        Service events till terminated
    */

    if (websGetBackground()) {
        if (daemon(0, 0) < 0) {
            error("Cannot run as daemon");
            return -1;
        }
    }
#endif
    if (duration) {
        printf("Running for %d secs\n", duration);
        websStartEvent(duration * 1000, (WebsEventProc) exitProc, 0);
    }
    websServiceEvents(&finished);
    logmsg(1, "Instructed to exit\n");
    websClose();

	//man_close();
    return 0;
}


static void exitProc(void *de_person, int de_num)
{
    websStopEvent(de_num);
    finished = 1;
}


static void logHeader()
{
    char    home[ME_GOAHEAD_LIMIT_STRING];

    getcwd(home, sizeof(home));
    logmsg(2, "Configuration for %s", ME_TITLE);
    logmsg(2, "---------------------------------------------");
    logmsg(2, "Version:            %s", ME_VERSION);
    logmsg(2, "BuildType:          %s", ME_DEBUG ? "Debug" : "Release");
    logmsg(2, "CPU:                %s", ME_CPU);
    logmsg(2, "OS:                 %s", ME_OS);
    logmsg(2, "Host:               %s", websGetServer());
    logmsg(2, "Directory:          %s", home);
    logmsg(2, "Documents:          %s", websGetDocuments());
    logmsg(2, "Configure:          %s", ME_CONFIG_CMD);
    logmsg(2, "---------------------------------------------");
}


static void usage() {
    fprintf(stderr, "\n%s Usage:\n\n"
        "  %s [options] [documents] [IPaddress][:port]...\n\n"
        "  Options:\n"
        "    --auth authFile        # User and role configuration\n"
#if ME_UNIX_LIKE && !MACOSX
        "    --background           # Run as a Unix daemon\n"
#endif
        "    --debugger             # Run in debug mode\n"
        "    --home directory       # Change to directory to run\n"
        "    --log logFile:level    # Log to file file at verbosity level\n"
        "    --route routeFile      # Route configuration file\n"
        "    --verbose              # Same as --log stderr:2\n"
        "    --version              # Output version information\n\n",
        ME_TITLE, ME_NAME);
    exit(-1);
}


void initPlatform()
{
#if ME_UNIX_LIKE
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGKILL, sigHandler);
    #ifdef SIGPIPE
        signal(SIGPIPE, SIG_IGN);
    #endif
#elif ME_WIN_LIKE
    _fmode=_O_BINARY;
#endif
}


#if ME_UNIX_LIKE
static void sigHandler(int signo)
{
    finished = 1;
}
#endif


/*
    Simple handler and route test
    Note: Accesses to "/" are normally remapped automatically to /index.html
 */
static bool testHandler(Webs *wp)
{
    if (smatch(wp->path, "/")) {
        websRewriteRequest(wp, "/home.html");
        /* Fall through */
    }
    return 0;
}


#if ME_GOAHEAD_JAVASCRIPT
/*
    Parse the form variables: name, address and echo back
 */
static int aspTest(int eid, Webs *wp, int argc, char **argv)
{
	char	*name, *address;

	if (jsArgs(argc, argv, "%s %s", &name, &address) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	return (int) websWrite(wp, "Name: %s, Address %s", name, address);
}


/*
    Generate a large response
 */
static int bigTest(int eid, Webs *wp, int argc, char **argv)
{
    int     i;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html>\n");
    for (i = 0; i < 800; i++) {
        websWrite(wp, " Line: %05d %s", i, "aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccccddddddd<br/>\r\n");
    }
    websWrite(wp, "</html>\n");
    websDone(wp);
    return 0;
}
#endif


/*
    Implement /action/actionTest. Parse the form variables: name, address and echo back.
 */
static void actionTest(Webs *wp)
{
	char	*name, *address;

	name = websGetVar(wp, "name", NULL);
	address = websGetVar(wp, "address", NULL);
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
	websWrite(wp, "<html><body><h2>name: %s, address: %s</h2></body></html>\n", name, address);
    websFlush(wp, 0);
	websDone(wp);
}

/*登录界面以及后台接口*/
static char check_login(char *name , char *password){
	int ret=-30;
	interface_login de_person;
	char output[50]={0};
	strcpy(de_person.username,name);
	strcpy(de_person.password,password);
	ret=man_control(PROTO, PROTO_CONTROL_IFLOGIN, (void*)&de_person,output);
	printf("check_login:ret=%d\n",ret);
	return ret;
}
static void login_server(Webs *wp)		//用户登录界面
{
	char	*name,  *password;
	char ret;
	printf("\nenter login_server:\n");
	name = websGetVar(wp, "username", NULL);
	password = websGetVar(wp, "password", NULL);
	printf("name =%s\n",name);
	printf("password =%s\n",password);
	ret = check_login(name,password);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*时间配置界面以及后台接口*/
static char check_time(char *checkbox,char *time_set1,char *time_set2,char *time_set3,char *timezone,char *tserver)		//时间配置接口
{
	return 0;
}
static int set_t(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"1.1.1.1");
	return 0;
}
static void set_time(Webs *wp)		//时间配置
{
	char *time_set1,*time_set2,*time_set3;
	char  *timezone,*tserver,*checkbox;
	char ret;
	printf("\nenter set_time:\n");
	checkbox = websGetVar(wp,  "checkbox" ,NULL);
	time_set1 = websGetVar(wp,  "time_set1" ,NULL);
	time_set2 = websGetVar(wp,  "time_set2" ,NULL);
	time_set3 = websGetVar(wp,  "time_set3" ,NULL);
	timezone = websGetVar(wp, "timezone" ,NULL);
	tserver = websGetVar(wp, "tserver" ,NULL);
	printf("checkbox = %s:%s:%s\n",time_set1,time_set2,time_set3);
	printf("timezone = %s \n",timezone);
	printf("tserver = %s \n",tserver);
	ret = check_time(checkbox,time_set1,time_set2,time_set3,timezone,tserver);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*网络配置界面以及后台接口*/
static char check_net(char *net_add,char *subnet_mask,char *default_gateway,char *main_dns,char *other_dns)//网络配置接口
{
		return 0;
}
static int set_net(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"192.168.1.1");
	return 0;
}
static int set_net1(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"255.255.255.0");
	return 0;
}
static int set_net2(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"114.255.114.255");
	return 0;
}
static int set_net3(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"114.114.114.114");
	return 0;
}
static int set_net4(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"8.8.8.8");
	return 0;
}
static void set_network(Webs *wp)			//网络配置
{
	char *connection_type,*net_add,*subnet_mask;
	char *default_gateway,*main_dns,*other_dns;
	char ret;
	printf("\nenter set_network:\n");
	connection_type = websGetVar(wp,"connection_type",NULL);
	net_add = websGetVar(wp,"net_add",NULL);
	subnet_mask = websGetVar(wp,"subnet_mask",NULL);
	default_gateway = websGetVar(wp,"default_gateway",NULL);
	main_dns = websGetVar(wp,"main_dns",NULL);
	other_dns = websGetVar(wp,"other_dns",NULL);
	printf("connection_type = %s \n",connection_type);
	printf("net_add = %s \n",net_add);
	printf("subnet_mask = %s \n",subnet_mask);
	printf("default_gateway = %s \n",default_gateway);
	printf("main_dns = %s \n",main_dns);
	printf("other_dns = %s \n",other_dns);
	ret = check_net(net_add,subnet_mask,default_gateway,main_dns,other_dns);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*用户设置界面以及后台接口*/
static char check_user(char *us_name,char *pass,char *serial_number,char *longitude,char *latitude,char *max_num)
{
	return 0;
}
static void set_user(Webs *wp)			//用户设置
{
	char *us_name,*pass,*serial_number;
	char *longitude,*latitude,*max_num;
	char ret;
	printf("\nenter set_user:\n");
	us_name = websGetVar(wp,"us_name",NULL);
	pass = websGetVar(wp,"pass",NULL);
	serial_number = websGetVar(wp,"serial_number",NULL);
	longitude = websGetVar(wp,"longitude",NULL);
	latitude = websGetVar(wp,"latitude",NULL);
	max_num = websGetVar(wp,"max_num",NULL);
	printf("us_name = %s \n",us_name);
	printf("pass = %s \n",pass);
	printf("serial_number = %s \n",serial_number);
	printf("longitude = %s \n",longitude);
	printf("latitude = %s \n",latitude);
	printf("max_num = %s \n",max_num);
	ret = check_user(us_name,pass,serial_number,longitude,latitude,max_num);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*用户管理界面以及后台接口*/
static char check_manage(char *identity,char *username)
{
	return 0;
}
static void user_manage(Webs *wp)			//用户管理
{
	char *identity,*username;
	char ret;
	printf("\nenter user_man:\n");
	identity = websGetVar(wp,"identity",NULL);
	username = websGetVar(wp,"username",NULL);
	printf("identity = %s \n",identity);
	printf("username = %s \n",username);
	ret = check_manage(identity,username);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

static int user_man(int eid,Webs *wp,int argc,char **argv)      //用户管理
{
    int i;
    typedef struct UserMan{
        char *de_num;
        char *username;
        };
    struct UserMan man[7]={{"1","content"},
                            {"2","content"},
                            {"3","content"},
                            {"4","content"},
                            {"5","content"},
                            {"6","content"},
                            {"7","content"}
    };
    printf("\nenter user_man:\n");
    for(i=0;i<7;i++){
        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",man[i].de_num);
        websWrite(wp,"<td>%s</td>",man[i].username);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",man[i].de_num);
        websWrite(wp,"<td>%s</td>",man[i].username);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",man[i].de_num);
        websWrite(wp,"<td>%s</td>",man[i].username);
        websWrite(wp,"</tr>");
    }
    return 0;
}

/*系统配置界面以及后台接口*/
static char check_dep(char *de_name,char *contact_way,char *de_position)
{
	return 0;
}
static void sys_deploy(Webs *wp)		//系统配置
{
	char *de_name,*contact_way,*de_position;
	char ret;
	printf("\nenter sys_deploy:\n");
	de_name = websGetVar(wp,"de_name",NULL);
	contact_way = websGetVar(wp,"contact_way",NULL);
	de_position = websGetVar(wp,"de_position",NULL);
	printf("de_name = %s \n",de_name);
	printf("contact_way = %s \n",contact_way);
	printf("de_position = %s \n",de_position);
	ret = check_dep(de_name,contact_way,de_position);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*关联设置界面以及后台接口*/
static char check_rel(char *con_type,char *net_add)
{
	return 0;
}
static void set_relation(Webs *wp)		//关联设置
{
	char *con_type,*net_add;
	char ret;
	printf("\nenter set_relation:\n");
	con_type = websGetVar(wp,"con_type",NULL);
	net_add = websGetVar(wp,"net_add",NULL);
	printf("con_type = %s \n",con_type);
	printf("net_add = %s \n",net_add);
	ret = check_rel(con_type,net_add);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*添加关联人界面以及后台接口*/
static char check_per(char *username,char *othername,char *jurisdiction)
{
	return 0;
}
static void contact_person(Webs *wp)		//添加关联人
{
	char *username;
	char *othername;
	char *jurisdiction;
	char ret;
	printf("\nenter contact_person:\n");
	username = websGetVar(wp,"username",NULL);
	othername= websGetVar(wp,"othername",NULL);
	jurisdiction = websGetVar(wp,"jurisdiction",NULL);
	printf("username = %s \n",username);
	printf("othername = %s \n",othername);
	printf("jurisdiction = %s \n",jurisdiction);
	ret = check_per(username,othername,jurisdiction);

// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}


/*云台信息界面以及后台接口*/
static char check_ter(char *acc_num,char *old_password,char *new_password,char *yes)
{
	return 0;
}
static int terrace_in1(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"192.168.1.1");
	return 0;
}
static int terrace_in2(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"1.1.1.1");
	return 0;
}
static void terrace_info(Webs *wp)		//云台信息
{
	char *acc_num,*old_password;
	char *new_password,*yes;
	char ret;
	printf("\nenter terrace_info:\n");
	acc_num = websGetVar(wp,"acc_num",NULL);
	old_password = websGetVar(wp,"old_password",NULL);
	new_password = websGetVar(wp,"new_password",NULL);
	yes = websGetVar(wp,"yes",NULL);
	printf("acc_num = %s \n",acc_num);
	printf("old_password = %s \n",old_password);
	printf("new_password = %s \n",new_password);
	printf("yes = %s \n",yes);
	ret  = check_ter(acc_num,old_password,new_password,yes);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

/*云台同步界面以及后台接口*/
static char check_terrace(char *t_interval)
{
	return 0;
}
static void terrace_syn(Webs *wp)		//云台同步
{
	char *t_interval;
	char ret;
	printf("\nenter terrace_syn:\n");
	t_interval = websGetVar(wp,"t_interval",NULL);
	printf("t_interval = %s \n",t_interval);
	ret = check_terrace(t_interval);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}


/*登录 登出界面以及后台接口*/
static char check_register(char *name_txt,char *password_txt)
{
	return 0;
}
static int register_e(int eid,Webs *wp,int argc,char **argv)
{
	websWrite(wp,"ONLINE/OFFLINE");
	return 0;
}
static void register_exit(Webs *wp)		//登录 登出
{
	char *name_txt,*password_txt;
	char ret;
	printf("\nenter register_exit:\n");
	name_txt = websGetVar(wp,"name_txt",NULL);
	password_txt = websGetVar(wp,"password_txt",NULL);
	printf("name_txt = %s \n",name_txt);
	printf("password_txt = %s \n",password_txt);
	ret = check_register(name_txt,password_txt);
// TODO:后台处理用户名和密码是否正确
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}

static int sys_info(int eid,Webs *wp,int argc,char **argv)			//系统信息
{
/*	char *versions="1";
	char *run_time="2";
	char *unit_type="3";
	char *unit_loc="4";
	char *memory_utilization="5";
	char *cpu_utilization="6";
	char *flash_capacity="7";
	char *flash_surplus="8";
*/
	printf("\nenter sys_info:\n");
	websWrite(wp,"<tr>\n<td>softversion:%s </td>\n <td>run time: %s </td> \n </tr>",	"v1.0","1 day 0 hour 12 min 45 sec");
	websWrite(wp,"<tr>\n<td>unit_type:%s </td>\n <td>unit_loc: %s </td> \n </tr>",	"smart","office503");
	websWrite(wp,"<tr>\n<td>memory_utilization:%s </td>\n <td>cpu_utilization: %s </td> \n </tr>",	"20%","20%");
	websWrite(wp,"<tr>\n<td>flash_capacity:%s </td>\n <td>flash_surplus: %s </td> \n </tr>",	"64G","8G");
	return 0;
}

static int inter_info(int eid,Webs *wp,int argc,char **argv)		//网络信息
{
/*	char *line_type;
	char *network_add;
	char *subnet_mask;
	char *default_gateway;
	char *main_DNS;
	char *from_DNS;
*/
	printf("\nenter inter_info:\n");
	websWrite(wp,"<tr>\n<td>line_type:%s </td>\n <td>network_add: %s </td> \n </tr>",	"static","192.168.1.115");
	websWrite(wp,"<tr>\n<td>subnet_mask:%s </td>\n <td>default_gateway: %s </td> \n </tr>",	"255.255.255.0","192.168.1.1");
	websWrite(wp,"<tr>\n<td>main_DNS:%s </td>\n <td>from_DNS: %s </td> \n </tr>",	"202.105.80.130","202.105.80.131");
	return 0;
}

static int equipment_info(int eid,Webs *wp,int argc,char **argv)		//设备信息
{
/*	char *equipment_type;
	char *equipment_sum;
	char *online_sum;
	char *offline_sum;
	char *Camera;
	char *Sensor;
	char *Steerable_dev;
*/
	printf("\nenter equipment_info:\n");
	websWrite(wp,"<tr>\n<td>Camera </td>\n <td>1</td>\n <td>2</td>\n <td>5</td> \n </tr>");
	websWrite(wp,"<tr>\n<td>Sensor </td>\n <td>1</td>\n <td>2</td>\n <td>5</td> \n </tr>");
	websWrite(wp,"<tr>\n<td>Steerable_dev </td>\n <td>1</td>\n <td>2</td>\n <td>5</td> \n </tr>");
	return 0;
}

/*
设备列表
 */
static int equipment_list(int eid,Webs *wp,int argc,char **argv)		//设备列表
{
    struct ext_dev *dev_list=NULL;
    struct ext_dev *dev_head=NULL;

	printf("\nenter equipment_list:\n");
    dev_list=calloc(1,sizeof(struct ext_dev));
    dev_head=dev_list;
    man_control(DEVICE,GET_DEVICES_STATUS,NULL,dev_list);

    dev_list=dev_list->next;
	while(dev_list!=NULL)
    {
		websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
		websWrite(wp,"<td>%s</td>",dev_list->devname);
		websWrite(wp,"<td>%d</td>",dev_list->dev_type);
		websWrite(wp,"<td>%d</td>",dev_list->devid);
		websWrite(wp,"<td>%s</td>",dev_list->location);
		websWrite(wp,"<td>%s</td>",dev_list->media->name);
		websWrite(wp,"<td>%d</td>",dev_list->status);
		websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
    websWrite(wp,"<td>%s</td>",dev_list->devname);
    websWrite(wp,"<td>%d</td>",dev_list->dev_type);
    websWrite(wp,"<td>%d</td>",dev_list->devid);
    websWrite(wp,"<td>%s</td>",dev_list->location);
    websWrite(wp,"<td>%s</td>",dev_list->media->name);
    websWrite(wp,"<td>%d</td>",dev_list->status);
    websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
		websWrite(wp,"<td>%s</td>",dev_list->devname);
		websWrite(wp,"<td>%d</td>",dev_list->dev_type);
		websWrite(wp,"<td>%d</td>",dev_list->devid);
		websWrite(wp,"<td>%s</td>",dev_list->location);
		websWrite(wp,"<td>%s</td>",dev_list->media->name);
		websWrite(wp,"<td>%d</td>",dev_list->status);
		websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
    websWrite(wp,"<td>%s</td>",dev_list->devname);
    websWrite(wp,"<td>%d</td>",dev_list->dev_type);
    websWrite(wp,"<td>%d</td>",dev_list->devid);
    websWrite(wp,"<td>%s</td>",dev_list->location);
    websWrite(wp,"<td>%s</td>",dev_list->media->name);
    websWrite(wp,"<td>%d</td>",dev_list->status);
    websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
		websWrite(wp,"<td>%s</td>",dev_list->devname);
		websWrite(wp,"<td>%d</td>",dev_list->dev_type);
		websWrite(wp,"<td>%d</td>",dev_list->devid);
		websWrite(wp,"<td>%s</td>",dev_list->location);
		websWrite(wp,"<td>%s</td>",dev_list->media->name);
		websWrite(wp,"<td>%d</td>",dev_list->status);
		websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
    websWrite(wp,"<td>%s</td>",dev_list->devname);
    websWrite(wp,"<td>%d</td>",dev_list->dev_type);
    websWrite(wp,"<td>%d</td>",dev_list->devid);
    websWrite(wp,"<td>%s</td>",dev_list->location);
    websWrite(wp,"<td>%s</td>",dev_list->media->name);
    websWrite(wp,"<td>%d</td>",dev_list->status);
    websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
		websWrite(wp,"<td>%s</td>",dev_list->devname);
		websWrite(wp,"<td>%d</td>",dev_list->dev_type);
		websWrite(wp,"<td>%d</td>",dev_list->devid);
		websWrite(wp,"<td>%s</td>",dev_list->location);
		websWrite(wp,"<td>%s</td>",dev_list->media->name);
		websWrite(wp,"<td>%d</td>",dev_list->status);
		websWrite(wp,"</tr>");

    websWrite(wp,"<tr>");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
    websWrite(wp,"<td>%s</td>",dev_list->devname);
    websWrite(wp,"<td>%d</td>",dev_list->dev_type);
    websWrite(wp,"<td>%d</td>",dev_list->devid);
    websWrite(wp,"<td>%s</td>",dev_list->location);
    websWrite(wp,"<td>%s</td>",dev_list->media->name);
    websWrite(wp,"<td>%d</td>",dev_list->status);
    websWrite(wp,"</tr>");

    dev_list=dev_list->next;
	}
    free(dev_head);
	return 0;
}

/*
添加设备
 */
static char goahead_dev_check(char *de_name,char *de_kind,char *de_num,char *de_position,char *de_belong,char *de_person)
{
    struct ext_dev *device;
    int ret;
    device=calloc(1,sizeof(struct ext_dev));
    strcpy(device->devname,de_name);
    strcpy(device->location,de_position);
    device->dev_type=atoi(de_kind);
    device->devid=atoi(de_num);
    device->status=atoi(de_person);
    ret=man_control(DEVICE,ADD_DEVICE,device,NULL); 
    printf("check_login:ret=%d\n",ret);
    return ret;

}

static int goahead_dev_add(Webs *wp)		//添加设备
{
	char *de_name;     
	char *de_kind;
	char *de_num;
	char *de_position;
	char *de_belong;
	char *de_person;
	int ret;

	printf("\nenter goahead_dev_add:\n");
	de_name = websGetVar(wp,"de_name",NULL);
	de_kind = websGetVar(wp,"de_kind",NULL);
	de_num = websGetVar(wp,"de_num",NULL);
	de_position = websGetVar(wp,"de_position",NULL);
	de_belong= websGetVar(wp,"de_belong",NULL);
	de_person = websGetVar(wp,"de_person",NULL);
	printf("de_name = %s \n",de_name);
	printf("de_kind = %s \n",de_kind);
	printf("de_num = %s \n",de_num);
	printf("de_position = %s \n",de_position);
	printf("de_belong = %s \n",de_belong);
	printf("de_person = %s \n",de_person);
    ret = goahead_dev_check(de_name,de_kind,de_num,de_position,de_belong,de_person);
 
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return;
}


static int goahead_dev_delete(Webs *wp)
{
    printf("Entry goahead_dev_delete\n");
}

/*
媒介列表
 */
static int media_list(int eid,Webs *wp,int argc,char **argv)		//媒介列表
{
    struct media_if *med_list=NULL;
    struct media_if *med_head=NULL;

    med_list=calloc(1,sizeof(struct media_if));
    med_head=med_list;
    man_control(DEVICE,GET_MEDIAS_STATUS,NULL,med_list);

    med_list=med_list->next;
    while(med_list)
    {
        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");


        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");


        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr>");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"/>");
        websWrite(wp,"<td>%s</td>",med_list->name);
        websWrite(wp,"<td>%d</td>",med_list->type);
        websWrite(wp,"<td>%d</td>",med_list->id);
        websWrite(wp,"<td>%d</td>",med_list->status);
        websWrite(wp,"<td>%d</td>",med_list->devnum);
        websWrite(wp,"</tr>");

     med_list=med_list->next;
    }
    free(med_head);
	return 0;
}

/*
添加媒介
 */
static int goahead_media_add(Webs *wp)		//添加媒介
{
    struct media_if *media;
    struct serial_private *serial_private;
    int *i32_port;
	char *me_name;
	char *me_id;
	char *me_type;
	char *baudrate;
	char *csize;
	char *parenb;
    char *port;
    char *cstop;
	int ret=0;
    int i32_type;

	printf("\nenter goahead_media_add:\n");
	me_name = websGetVar(wp,"me_name",NULL);
	me_id = websGetVar(wp,"me_id",NULL);
	me_type = websGetVar(wp,"me_type",NULL);
	baudrate= websGetVar(wp,"baudrate",NULL);
	csize = websGetVar(wp,"csize",NULL);
	parenb = websGetVar(wp,"parenb",NULL);
    cstop=websGetVar(wp,"cstop",NULL);
	printf("me_name = %s \n",me_name);
	printf("me_id = %s \n",me_id);
	printf("me_type = %s \n",me_type);
	printf("baudrate = %s \n",baudrate);
	printf("csize = %s \n",csize);
	printf("parenb = %s \n",parenb);
    printf("cstop = %s \n",cstop);
    i32_type=atoi(me_type);
    printf("type=%d\n",i32_type);

    media=calloc(1,sizeof(struct media_if));
    strcpy(media->name,me_name);
    media->type=atoi(me_type);
    media->id=atoi(me_id);
    switch(i32_type)
    {
        case MEDIA_SERIAL:
        serial_private=calloc(1,sizeof(struct serial_private));
        serial_private->bandrate=atoi(baudrate);
        serial_private->csize=atoi(csize);
        serial_private->cstop=atoi(cstop);
        serial_private->verify=atoi(parenb);
        media->para=serial_private;
        break;
    }

  ret=man_control(DEVICE,ADD_MEDIA,media,NULL);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if (ret == 0) {
		websWrite(wp, "0");
	}
	else{
		websWrite(wp, "-1");
	}

	websFlush(wp, 0);
	websDone(wp);
	return 0;
}

static int goahead_media_delete(Webs *wp){
  printf("\nEntry goahead_media_delete\n");
}

/*
监控库管理以及监控库管理接口
 */
static int monitor_s1(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"MONITOR");
	return 0;
}
static int monitor_s2(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"V1.0");
	return 0;
}
static int monitor_s3(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"2017.1.1");
	return 0;
}
static int monitor_s4(int eid,Webs *wp,int argc,char **argv){
	websWrite(wp,"NOLINE");
	return 0;
}
static int monitor_sys(int eid,Webs *wp,int argc,char **argv)		//监控库管理
{
	int i;
	typedef struct MonitorSys{
		char *de_num;
		char *content;
		};
	struct MonitorSys mon[7]={{"1","content"},
							{"2","content"},
							{"3","content"},
							{"4","content"},
							{"5","content"},
							{"6","content"},
							{"7","content"}};
	printf("\nenter monitor_sys:\n");
	for(i=0;i<7;i++){
		websWrite(wp,"<tr>");
		websWrite(wp,"<td>%s</td>",mon[i].de_num);
		websWrite(wp,"<td>%s</td>",mon[i].content);
	}
	return 0;
}

/*
报警消息管理
 */
static int alarm_messages(int eid,Webs *wp,int argc,char **argv)		//报警消息管理
{
	int i;
	typedef struct AlarmMes{
		char *time;
		char *category;
		char *level;
		char *illustrate;
		};
	struct AlarmMes mes[7]={{"2015.10.1","cd15001","balcony","wolf"},
							{"2015.10.2","cd15002","balcony","wolf"},
							{"2015.10.3","cd15005","balcony","wolf"},
							{"2015.10.5","cd15007","balcony","wolf"},
							{"2015.10.6","cd15004","balcony","wolf"},
							{"2015.10.7","cd15000","balcony","wolf"},
							{"2015.10.8","cd15003","balcony","wolf"}};
	printf("\nenter alarm_messages:\n");
	for(i=0;i<7;i++){
		websWrite(wp,"<tr name=\"number\" class=\"behind\">");
		websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
		websWrite(wp,"<td>%s</td>",mes[i].time);
		websWrite(wp,"<td>%s</td>",mes[i].category);
		websWrite(wp,"<td>%s</td>",mes[i].level);
		websWrite(wp,"<td>%s</td>",mes[i].illustrate);
		websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",mes[i].time);
        websWrite(wp,"<td>%s</td>",mes[i].category);
        websWrite(wp,"<td>%s</td>",mes[i].level);
        websWrite(wp,"<td>%s</td>",mes[i].illustrate);
        websWrite(wp,"</tr>");
	}
	return 0;
}

/*
查看关联人
 */

static int contact_look(int eid,Webs *wp,int argc,char **argv)		//查看关联人
{
	int i;
	typedef struct ContactLook{
		char *user_name;
		char *alias;
		char *power;
		char *contact_time;
		char *state;
		};
	struct ContactLook con[7]={{"1","zhangsan","NO.1","2015.10.1","ONLINE"},
							{"2","lisi","NO.2","2015.10.2","OFFLINE"},
							{"3","wangwu","NO.3","2015.10.2","ONLINE"},
							{"4","zhaosi","MO.4","2015.11.2","OFFLINE"},
							{"5","wanger","NO.5","2015.10.2","ONLINE"},
							{"6","zhouqi","NO.6","2015.12.2","ONLINE"},
							{"7","liming","NO.7","2015.09.2","OFFLINE"}};
	printf("\nenter contact_look:\n");
	for(i=0;i<7;i++){
    websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
		websWrite(wp,"<td>%s</td>",con[i].user_name);
		websWrite(wp,"<td>%s</td>",con[i].alias);
		websWrite(wp,"<td>%s</td>",con[i].power);
		websWrite(wp,"<td>%s</td>",con[i].contact_time);
		websWrite(wp,"<td>%s</td>",con[i].state);
		websWrite(wp,"</tr>");
	}
	return 0;
}

/*
审核关联人
 */

static int contact_check(int eid,Webs *wp,int argc,char **argv)		//审核关联人
{
	int i;
	typedef struct ContactCheck{
		char *de_num;
		char *user_name;
		char *managing_ass;
		char *power;
		char *info;
		};
	struct ContactCheck con[7]={{"1","001","NO.1","2015001","NULL"},
							{"2","002","NO.2","2015002","NULL"},
							{"3","003","NO.3","2015003","NULL"},
							{"4","004","MO.4","2015004","NULL"},
							{"5","005","NO.5","2015005","NULL"},
							{"6","006","NO.6","2015006","NULL"},
							{"7","007","NO.7","2015007","NULL"}};
	printf("\nenter contact_check:\n");
	for(i=0;i<7;i++){
    websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
		websWrite(wp,"<td>%s</td>",con[i].de_num);
		websWrite(wp,"<td>%s</td>",con[i].user_name);
		websWrite(wp,"<td>%s</td>",con[i].managing_ass);
		websWrite(wp,"<td>%s</td>",con[i].power);
		websWrite(wp,"<td>%s</td>",con[i].info);
		websWrite(wp,"</tr>");

         websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",con[i].de_num);
        websWrite(wp,"<td>%s</td>",con[i].user_name);
        websWrite(wp,"<td>%s</td>",con[i].managing_ass);
        websWrite(wp,"<td>%s</td>",con[i].power);
        websWrite(wp,"<td>%s</td>",con[i].info);
        websWrite(wp,"</tr>");

         websWrite(wp,"<tr name=\"number\" class=\"behind\">");
    websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",con[i].de_num);
        websWrite(wp,"<td>%s</td>",con[i].user_name);
        websWrite(wp,"<td>%s</td>",con[i].managing_ass);
        websWrite(wp,"<td>%s</td>",con[i].power);
        websWrite(wp,"<td>%s</td>",con[i].info);
        websWrite(wp,"</tr>");
	}
	return 0;
}



// static int contact_check(int eid,Web *wp,int argc,char **argv){

// }

static int  view_log(int eid,Webs *wp,int argc,char **argv)		//查看日志
{
	int i;
	typedef struct ViewLog{
		char *de_num;
		char *content;
		};
	struct ViewLog log[7]={{"1","NULL"},
							{"2","NULL"},
							{"3","NULL"},
							{"4","NULL"},
							{"5","NULL"},
							{"6","NULL"},
							{"7","NULL"}};
	printf("\nenter view_log:\n");
	for(i=0;i<7;i++){
		websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",log[i].de_num);
        websWrite(wp,"<td>%s</td>",log[i].content);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",log[i].de_num);
        websWrite(wp,"<td>%s</td>",log[i].content);
        websWrite(wp,"</tr>");

        websWrite(wp,"<tr name=\"number\" class=\"behind\">");
        websWrite(wp,"<td><input type=\"checkbox\" class=\"box\"></td>");
        websWrite(wp,"<td>%s</td>",log[i].de_num);
        websWrite(wp,"<td>%s</td>",log[i].content);
        websWrite(wp,"</tr>");
	}
	return 0;
}

static int area_net(int eid,Webs *wp,int argc,char **argv)		//局域网发现管理
{
	int i;
	typedef struct AreaNet{
		char *de_num;
		char *name;
		char *IP_add;
		char *time;
		};
	struct AreaNet net[7]={{"1","NO.1","cd15001","20h18min05s "},
							{"2","NO.2","cd15002"," 20h18min05s"},
							{"3","NO.3","cd15003"," 20h18min05s"},
							{"4","MO.4","cd15004"," 20h18min05s"},
							{"5","NO.5","cd15005"," 20h18min05s"},
							{"6","NO.6","cd15006"," 20h18min05s"},
							{"7","NO.7","cd15007"," 20h18min05s"}};
	printf("\nenter area_net:\n");
	for(i=0;i<7;i++){
		websWrite(wp,"<tr>");
		websWrite(wp,"<td>%s</td>",net[i].de_num);
		websWrite(wp,"<td>%s</td>",net[i].name);
		websWrite(wp,"<td>%s</td>",net[i].IP_add);
		websWrite(wp,"<td>%s</td>",net[i].time);
		websWrite(wp,"</tr>");
	}
	return 0;
}

static void device_restart(Webs *wp)		//设备重启/*未做*/
{
	return 0;
}
static void software_update(Webs *wp)		//软件更新/*未做*/
{
	return 0;
}
static void backup_recovery(Webs *wp)		//备份恢复/*未做*/
{
	return 0;
}
static void update_plug(Webs *wp)		//上传插件/*未做*/
{
	return 0;
}
static void login_manage(Webs *wp)		//登录管理/*未做*/
{
	return 0;
}


static void sessionTest(Webs *wp)
{
	char	*number;

    if (scaselessmatch(wp->method, "POST")) {
        number = websGetVar(wp, "number", 0);
        websSetSessionVar(wp, "number", number);
    } else {
        number = websGetSessionVar(wp, "number", 0);
    }
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><p>Number %s</p></body></html>\n", number);
    websDone(wp);
}


static void showTest(Webs *wp)
{
    WebsKey     *s;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><pre>\n");
    for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
        websWrite(wp, "%s=%s\n", s->name.value.string, s->content.value.string);
    }
    websWrite(wp, "</pre></body></html>\n");
    websDone(wp);
}


#if ME_GOAHEAD_UPLOAD && !ME_ROM
/*
    Dump the file upload details. Don't actually do anything with the uploaded file.
 */
static void uploadTest(Webs *wp)
{
    WebsKey         *s;
    WebsUpload      *up;
    char            *upfile;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    if (scaselessmatch(wp->method, "POST")) {
        for (s = hashFirst(wp->files); s; s = hashNext(wp->files, s)) {
            up = s->content.value.symbol;
            websWrite(wp, "FILE: %s\r\n", s->name.value.string);
            websWrite(wp, "FILENAME=%s\r\n", up->filename);
            websWrite(wp, "CLIENT=%s\r\n", up->clientFilename);
            websWrite(wp, "TYPE=%s\r\n", up->contentType);
            websWrite(wp, "SIZE=%d\r\n", up->size);
            upfile = sfmt("%s/tmp/%s", websGetDocuments(), up->clientFilename);
            if (rename(up->filename, upfile) < 0) {
                error("Cannot rename uploaded file: %s to %s, errno %d", up->filename, upfile, errno);
            }
            wfree(upfile);
        }
        websWrite(wp, "\r\nVARS:\r\n");
        for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
            websWrite(wp, "%s=%s\r\n", s->name.value.string, s->content.value.string);
        }
    }
    websDone(wp);
}
#endif


#if ME_GOAHEAD_LEGACY
/*
    Legacy handler with old parameter sequence
 */
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags)
{
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    websWrite(wp, "Hello Legacy World\n");
    websDone(wp);
    return 1;
}

#endif
