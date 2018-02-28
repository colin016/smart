#include "dev.h"

static int serial_set_opt(struct media_if *med_inf);
static int serial_set_bandrate(struct termios *opt,struct serial_private *para);
static int serial_set_csize(struct termios *opt,struct serial_private *para);
static int serial_set_parent(struct termios *opt,struct serial_private *para);

int serial_open(struct media_if *med_inf)
{
	struct serial_private *para=(struct serial_private *)med_inf->para;

	if (med_inf->type==MEDIA_SERIAL||med_inf->type==MEDIA_ZIGBEE)
	{
		med_inf->fd=open(para->name,O_RDWR|O_NOCTTY|O_NDELAY);
		PLOG("serial:%s\n",para->name );
	    if(med_inf->fd<=0)
	    {
		    perror("open serial");
		    med_inf->status=CLOSE;
		    return -1;
	    }

	    if(serial_set_opt(med_inf)==-1)
	    {
		    med_inf->status=CLOSE;
		    return -1;
	    }
	    med_inf->status=LIVE;
	    dev_addevent(med_inf);
	}
	return 0;
}

static int serial_set_opt(struct media_if *med_inf)
{
	struct termios opt;
	struct serial_private *para=(struct serial_private *)med_inf->para;
		// Geting the current options for the port...
	if(tcgetattr(med_inf->fd,&opt)==-1)
		return -1;
		//设置波特率
	if(serial_set_bandrate(&opt,para)==-1)
		return -1;
	if(serial_set_csize(&opt,para)==-1)
		return -1;
	if(serial_set_parent(&opt,para)==-1)
		return -1;
	if(para->cstop==2)
	{
		opt.c_cflag|=CSTOPB;
	}
	else if(para->cstop==1)
	{
		opt.c_cflag&=~CSTOPB;
	}
	//设置控制模式状态，本地连接，接收使能
	opt.c_cflag |=(CLOCAL|CREAD);
	opt.c_cflag &= ~CRTSCTS;
	opt.c_oflag &= ~OPOST;  
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    opt.c_cc[VTIME] = 1;//可以在select中设置
    opt.c_cc[VMIN] = 1;//最少读取一个字符
    //串口设置中c_iflag和c_oflag中存在从NL-CR和CR-NL的映射，即串口能把回车和换行当成同一个字符，可以进行如下设置屏蔽之
    opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
    opt.c_oflag &= ~(ONLCR | OCRNL);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	tcflush(med_inf->fd, TCIFLUSH);
    if(tcsetattr(med_inf->fd, TCSANOW, &opt)==-1)
    {
    	return -1;
    }
		return 0;
}

static int serial_set_bandrate(struct termios *opt,struct serial_private *para)
{
	switch (para->bandrate)
	{
		case 300:
			if(cfsetispeed(opt, B300)==-1)
				return -1;
			if(cfsetospeed(opt, B300)==-1)
				return -1;
		break;
		case 600:
			if(cfsetispeed(opt, B600)==-1)
				return -1;
			if(cfsetospeed(opt, B600)==-1)
				return -1;
		break;
		case 1200:
			if(cfsetispeed(opt, B1200)==-1)
				return -1;
			if(cfsetospeed(opt, B1200)==-1)
				return -1;
		break;
		case 2400:
			if(cfsetispeed(opt, B2400)==-1)
				return -1;
			if(cfsetospeed(opt, B2400)==-1)
				return -1;
		break;
		case 4800:
			if(cfsetispeed(opt, B4800)==-1)
				return -1;
			if(cfsetospeed(opt, B4800)==-1)
				return -1;
		break;
		case 9600:
			if(cfsetispeed(opt, B9600)==-1)
				return -1;
			if(cfsetospeed(opt, B9600)==-1)
				return -1;
		break;
		case 19200:
			if(cfsetispeed(opt, B19200)==-1)
				return -1;
			if(cfsetospeed(opt, B19200)==-1)
				return -1;
		break;
		case 38400:
			if(cfsetispeed(opt, B38400)==-1)
				return -1;
			if(cfsetospeed(opt, B38400)==-1)
				return -1;
		break;
		case 57600:
			if(cfsetispeed(opt, B57600)==-1)
				return -1;
			if(cfsetospeed(opt, B57600)==-1)
		break;
		case 115200:
			if(cfsetispeed(opt, B115200)==-1)
				return -1;
			if(cfsetospeed(opt, B115200)==-1)
				return -1;
		break;
	}

	return 0;
}

static int serial_set_csize(struct termios *opt,struct serial_private *para)
{
	opt->c_cflag&= ~CSIZE;
	switch (para->csize)
	{
		case _CS8:
		opt->c_cflag|=CS8;
		break;

		case _CS7:
		opt->c_cflag|=CS7;
		break;

		case _CS6:
		opt->c_cflag|=CS6;
		break;

		case _CS5:
		opt->c_cflag|=CS5;
		break;

		default:
			return -1;
		break;
	}

	return 0;
}

static int serial_set_parent(struct termios *opt,struct serial_private *para)
{
	switch (para->verify)
	{
		case 0:
			opt->c_cflag &= ~PARENB;
			opt->c_iflag &= ~INPCK;
		break;

		case 1:
			opt->c_cflag |= PARENB;
            opt->c_cflag &= ~PARODD;
            opt->c_iflag |= INPCK;
		break;

		case 2:
			opt->c_cflag |= PARENB;
            opt->c_cflag |= PARODD;
            opt->c_iflag |= INPCK; 
		break;

		default:
			return -1;
		break;
	}

	return 0;
}

