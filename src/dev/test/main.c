#include <stdio.h>
#include "dev.h"

extern struct dev_data *devdata;

void *main_timer(void *);
int main_callback(int sendid, int recvid, int type, void *input, void *output);
int main_menu();
int control_menu();
int query_menu();
void main_control(struct ModuleInfo *modinf);
void print(struct devctl_out pkg);
void init_cfg(struct KeyValueArray *cfg);
int execute_cmd(struct ModuleInfo *modinf);
int modify_media(struct ModuleInfo *modinf);
int modify_device(struct ModuleInfo *modinf);
int add_device(struct ModuleInfo *modinf);
int delete_device(struct ModuleInfo *modinf);
int get_device_file(struct ModuleInfo *modinf);
int set_device_file(struct ModuleInfo *modinf);
int get_medias_status(struct ModuleInfo *modinf);
int get_devices_status(struct ModuleInfo *modinf);
int print_devices();
int print_medias();
int print_device_on_media();
int ctl_humiture(struct ModuleInfo *modinf);
int ctl_raac(struct ModuleInfo *modinf);
int ctl_smoke(struct ModuleInfo *modinf);
int ctl_brightness(struct ModuleInfo *modinf);
int ctl_motor(struct ModuleInfo *modinf);
int ctl_reply(struct ModuleInfo *modinf);
int ctl_buzzer(struct ModuleInfo *modinf);
int ctl_power(struct ModuleInfo *modinf);
int ctl_gps(struct ModuleInfo *modinf);

pthread_t pth;

int main_callback(int sendid, int recvid, int type, void *input, void *output)
{
    int ret = 0;
    //struct ModuleInfo modinf;

    printf("callback is called!\n");
	printf("sendid=%d recvid=%d type=%d\n",sendid, recvid, type);

    return ret;
}

void *main_timer(void *args)
{
	struct devctl_out output;
    struct devctl_in input;
    struct ModuleInfo *modinf=NULL;

    modinf=(struct ModuleInfo *)args;
	while(1)
	{
		//获得温度
		memset(&input,0,sizeof(input));
        input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=0;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output);
        sleep(1);
        //获湿度
        memset(&input,0,sizeof(input));
        input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=1;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output);
         sleep(1);
        //获得气压值
        input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=4;
	    input.data[2]=0;input.data[3]=0;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output); 
        sleep(1);
        //获得土壤温度
        input.devid=2;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=5;
	    input.data[2]=0;input.data[3]=0;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output); 
        sleep(1);

        //获得土壤湿度
        input.devid=2;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=6;
	    input.data[2]=0;input.data[3]=0;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output); 
        sleep(1);      

         //获得土壤湿度
        input.devid=3;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=6;
	    input.data[2]=0;input.data[3]=0;
	    output.len=0;
        modinf->control(0,EXECUTE_CMD,&input,&output);
        print(output);  
         sleep(1);   
	}
	return NULL;
}

int main()
{
	int choice=-1;
	struct ModuleInfo modinf;
	struct KeyValueArray cfg;
	//char Buffer[40];

	memset(&modinf,0,sizeof(struct ModuleInfo));
	memset(&cfg,0,sizeof(struct KeyValueArray));
	init_cfg(&cfg);
	dev_init(&cfg, main_callback, &modinf);
    pthread_create(&pth,NULL,(void*)main_timer,(void*)&modinf);
	while(1)
	{
			choice=main_menu();
			switch (choice) {
				case 1://dev_control
					main_control(&modinf);
				break;
				case 2:
				break;
				case 3:
				break;
				case 4:
				break;
				case 0:
					modinf.close();
					exit(1);
				break;
			}
	}


    return 0;
}

void main_control(struct ModuleInfo *modinf)
{
	int choice;
	while(1)
	{
	choice=control_menu();
	switch (choice)
	{
		case EXECUTE_CMD:
			execute_cmd(modinf);
		break;
		case MODIFY_MEDIA:
			modify_media(modinf);
		break;
		case MODIFY_DEVICE:
			modify_device(modinf);
		break;
		case ADD_DEVICE:
			add_device(modinf);
		break;
		case DELETE_DEVICE:
			delete_device(modinf);
		break;
		case GET_DEVICE_FILE:
			get_device_file(modinf);
		break;
		case SET_DEVICE_FILE:
			set_device_file(modinf);
		break;
		case GET_MEDIAS_STATUS:
			get_medias_status(modinf);
		break;
		case GET_DEVICES_STATUS:
			get_devices_status(modinf);
			//print_devices();
			print_device_on_media();
		break;
		case 0:
		return;
		break;
			}
		}
}

int get_devices_status(struct ModuleInfo *modinf)
{
	struct ext_dev dev_head;
	//dev_head=(struct ext_dev*)calloc(1,sizeof(struct ext_dev));
	modinf->control(0,GET_DEVICES_STATUS,NULL,&dev_head);
	printf("%d\n",dev_head.devid);

	//free(dev_head);
	return 0;
}

int get_medias_status(struct ModuleInfo *modinf)
{
	struct media_if *med_head=NULL;
	med_head=calloc(1,sizeof(struct media_if));
	modinf->control(0,GET_MEDIAS_STATUS,NULL,med_head);
	med_head=med_head->next;
	while(med_head)
	{
		fprintf(stdout, "%s\n",med_head->name );
		med_head=med_head->next;
	}
	free(med_head);
	return 0;
}

int get_device_file(struct ModuleInfo *modinf)
{
	char filename[1024];
	modinf->control(0,GET_DEVICE_FILE,NULL,&filename);
	return 0;
}

int set_device_file(struct ModuleInfo *modinf)
{
	char filename[1024];

	modinf->control(0,SET_DEVICE_FILE,&filename,NULL);
	return 0;
}

int delete_device(struct ModuleInfo *modinf)
{
	int id;

	puts("input id of device:");
	scanf("%d",&id );
	modinf->control(0,DELETE_DEVICE,(void*)&id,NULL);
	return 0;
}

int add_device(struct ModuleInfo *modinf)
{
	struct ext_dev *dev=NULL;

	dev=(struct ext_dev*)calloc(1,sizeof(struct ext_dev));
	puts("input id of device:");
	scanf("%d",&dev->devid );
	dev->media=devdata->medtable.med_list;
	strcpy(dev->devname,"电机");
	dev->dev_type=1;
	strcpy(dev->location,"大厅");
	modinf->control(0,ADD_DEVICE,dev,NULL);
	return 0;
}

int modify_device(struct ModuleInfo *modinf)
{
	struct ext_dev *dev=NULL;

	dev=(struct ext_dev*)malloc(sizeof(struct ext_dev));
	puts("input id of device:");
	scanf("%d",&dev->devid );
	dev->media=devdata->medtable.med_list;
	strcpy(dev->devname,"电机1");
	dev->dev_type=1;
	strcpy(dev->location,"大厅1");
	dev->data=NULL;

	modinf->control(0,MODIFY_DEVICE,dev,NULL);
	free(dev);
	return 0;
}

int modify_media(struct ModuleInfo *modinf)
{
	struct media_if med;
  struct serial_private *medpara;
	medpara=(struct serial_private *)calloc(1,sizeof(struct serial_private));
	medpara->bandrate=9600;
	medpara->csize=8;
	medpara->cstop=1;
	medpara->verify=0;
	med.id=1;
	med.para=medpara;
	serial_init(&med);

	modinf->control(0,MODIFY_MEDIA,&med,NULL);

	return 0;
}

int execute_cmd(struct ModuleInfo *modinf)
{
   char buff[3]={0};
    
    puts("1.测距传感器");
    puts("2.亮度传感器");
    puts("3.烟雾检测传感器");
    puts("4.温湿度传感器");
    puts("5.电机");
    puts("6.继电器");
    puts("7.麦克风");
    puts("8.蜂鸣器");
    puts("9.摄像头");
    puts("10.电源管理模块");
    puts("11.GPS");

   read(0,buff,2);
   switch(atoi(buff))
   {
   	case 1:
   	ctl_raac(modinf);
   	break;
   	
   	case 2:
   	ctl_brightness(modinf);
   	break;

   	case 3:
   	ctl_smoke(modinf);
   	break;

   	case 4:
   	ctl_humiture(modinf);
   	break;

   	case 5:
   	ctl_motor(modinf);
   	break;

   	case 6:
   	ctl_reply(modinf);
   	break;
   	
   	case 7:
    break;

   	case 8:
   	ctl_buzzer(modinf);
   	break;

   	case 9:
   	break;

   	case 10:
   	ctl_power(modinf);
   	break;

   	case 11:
   	ctl_gps(modinf);
   	break;
   }

	return 0;
}

int ctl_humiture(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};
    short a;

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获得温度");
    puts("2.获得湿度");
    puts("3.获得温湿度");
    puts("4.设置温度阀值");
    puts("5.设置湿度阀值");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
    {
	  input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=3;
	  input.data[2]=0;input.data[3]=1;
    }else if (buff[0]=='3')
    {
    	input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=2;
    }else if (buff[0]=='4')
	{
		input.devid=1;input.sign=33;input.len=6;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=3;
	    a=22;
	    memcpy(input.data+4,&a,2);
	}else if (buff[0]=='5')
	{
		input.devid=1;input.sign=33;input.len=6;input.data[0]=0;input.data[1]=3;
	    input.data[2]=0;input.data[3]=4;
	    a=20;
	    memcpy(input.data+4,&a,2);
	}else
	{
		return -1;
	}

  output.len=0;
  modinf->control(0,EXECUTE_CMD,&input,&output);
  print(output);
  return 0;
}

int ctl_raac(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};
    short a;

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获取最近的物体距离");
    puts("2.设置阀值");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=0;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=6;input.data[0]=0;input.data[1]=0;
	    input.data[2]=0;input.data[3]=1;
	    a=15;
	    memcpy(input.data+4,&a,2);
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_smoke(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};
    short a;

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获得烟雾浓度");
    puts("2.设置阀值");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=2;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=6;input.data[0]=0;input.data[1]=2;
	    input.data[2]=0;input.data[3]=1;
	    a=15;
	    memcpy(input.data+4,&a,2);
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_brightness(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};
    short a;

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获得亮度值");
    puts("2.设置阀值");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=0;input.data[1]=1;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=6;input.data[0]=0;input.data[1]=1;
	    input.data[2]=0;input.data[3]=1;
	    a=15;
	    memcpy(input.data+4,&a,2);
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_motor(struct ModuleInfo *modinf)
{
    struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.前进");
    puts("2.后退");
    puts("3.左转");
    puts("4.右转");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=0;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
    {
	  input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=0;
	  input.data[2]=0;input.data[3]=1;
    }else if (buff[0]=='3')
    {
    	input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=0;
	    input.data[2]=0;input.data[3]=2;
    }else if (buff[0]=='4')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=0;
	    input.data[2]=0;input.data[3]=3;
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_reply(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.继电器吸合");
    puts("2.继电器断开");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=1;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=1;input.data[1]=1;
	    input.data[2]=0;input.data[3]=1;
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_buzzer(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.继播放一段警告音");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=2;input.data[1]=1;
	    input.data[2]=0;input.data[3]=0;
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_power(struct ModuleInfo *modinf)
{
    struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获取电池电量");
    puts("2.获取供电方式");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=4;input.data[1]=0;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=4;input.data[1]=0;
	    input.data[2]=0;input.data[3]=1;
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

int ctl_gps(struct ModuleInfo *modinf)
{
	struct devctl_out output;
    struct devctl_in input;
    char buff[3]={0};

	memset(input.data, 0, sizeof(input.data));
	memset(&output,0,sizeof(output));

    puts("1.获取经纬度");
    puts("2.获取卫星时间");
    puts("3.获取海拔高度");
    puts("4.获取所有数据");

	read(0,buff,1);
	if (buff[0]=='1')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=5;input.data[1]=0;
	    input.data[2]=0;input.data[3]=0;
	}else if (buff[0]=='2')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=5;input.data[1]=0;
	    input.data[2]=0;input.data[3]=1;
	}else if (buff[0]=='3')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=5;input.data[1]=0;
	    input.data[2]=0;input.data[3]=2;
	}else if (buff[0]=='4')
	{
		input.devid=1;input.sign=33;input.len=4;input.data[0]=5;input.data[1]=0;
	    input.data[2]=0;input.data[3]=3;
	}else
	{
		return -1;
	}
	output.len=0;
	modinf->control(0,EXECUTE_CMD,&input,&output);
	print(output);
	return 0;
}

void init_cfg(struct KeyValueArray *cfg)
{
	char buf[]={"MAXMEDIANUM=10\nMAXDEVICENUM=10\nTIMEOUT=1"};
	char *ptr=NULL;
	int i=0;

	cfg->count=3;
	cfg->cfgbuf=(char*)malloc(sizeof(buf));
	strcpy(cfg->cfgbuf,buf);
	ptr=cfg->cfgbuf;
	cfg->keyValueList=(KeyValue*)malloc(sizeof(KeyValue)*cfg->count);

	for (i = 0; i < cfg->count; i++)
   {
     cfg->keyValueList[i].key=ptr;
     ptr=strchr(ptr,'=');
     cfg->keyValueList[i].value=ptr+1;
 		 ptr=strchr(ptr,'\n')+1;
   }

 for (i = 0; i <sizeof(buf); i++)
{
		if(cfg->cfgbuf[i]=='=')
			cfg->cfgbuf[i]=0;
		if(buf[i]<13)
			cfg->cfgbuf[i]=0;
}

for (i = 0; i <cfg->count; i++)
	{
		cfg->keyValueList[i].keylen=strlen(cfg->keyValueList[i].key);
		cfg->keyValueList[i].valuelen=strlen(cfg->keyValueList[i].value);
  }

}

void print(struct devctl_out pkg)
{
	
	FILE *fp=NULL;
	struct tm *t;
    time_t tt;
	char current[1024]={0};

    if(pkg.len==0)
		return;

    time(&tt);
    t = localtime(&tt);
    sprintf(current, "%4d年%02d月%02d日 %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	
	printf("msg:%s %s\n",current,pkg.msg );
	printf("len=%d\n",pkg.len);
    fp=fopen("../data/data.dat","a");
	if (fp==NULL)
	{
		return;
	}
	fprintf(fp,"%s %s\n",current,pkg.msg);
	fclose(fp);
}

int main_menu()
{
	int choice=-1;
	puts("1.dev_contrl");
	puts("2.dev_query");
	puts("3.dev_config");
	puts("4.dev_defaultcfg");
	puts("0.close");
	printf(">>");
	a:scanf("%d",&choice);
	getchar();
	if(choice>4||choice<0)
	{
		printf(">>");
		goto a;
	}
	return choice;
}

int query_menu()
{
	int choice=-1;

	scanf("%d",&choice);
	getchar();
	return choice;
}

int print_device_on_media()
{
	struct ext_dev *dev=NULL;
	struct media_if *med=devdata->medtable.med_list;

	while(med!=NULL)
	{
		dev=med->list;
		printf("med:");
		while(dev!=NULL)
		{
			printf("%d ",dev->devid);
			dev=dev->ifnext;
		}
		puts("");
		med=med->next;
	}
	return 0;
}

int print_devices()
{
	struct ext_dev *p=devdata->devtable.dev_list;

	printf("id:");
	while (p!=NULL)
	{
		printf("%d ",p->devid);
		p=p->next;
	}
	puts("");

	return 0;
}
int print_medias()
{
	return 0;
}

int control_menu()
{
	int choice=-1;
	fputs("1、执行需求指令\n",stdout);
	fputs("2、修改介质配置参数\n",stdout);
	fputs("3、修改设备参数\n",stdout);
	fputs("4、添加设备\n",stdout);
	fputs("5、删除设备\n",stdout);
	fputs("6、获取设备描述文件\n",stdout);
	fputs("7、设置新的设备描述文件\n",stdout);
	fputs("8、获取介质状态\n",stdout);
	fputs("9、获取设备当前状态\n",stdout);
	fputs("0、退出\n",stdout);
	scanf("%d",&choice);
	getchar();
	return choice;
}
