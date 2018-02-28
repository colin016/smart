drop table tb_config;
--配置模块表
create table tb_config(
	cfg_key text PRIMARY KEY not null,
	cfg_value text not null
);

insert into tb_config values ("MAXDEVICENUM","10");
insert into tb_config values ("MAXMEDIANUM","10");
insert into tb_config values ("ISSHOW","1");
insert into tb_config values ("ISAUTO","0");
insert into tb_config values ("ISCANCEAL","1");
insert into tb_config values ("ISFIND","1");
insert into tb_config values ("IPADDR","192.168.121");
insert into tb_config values ("REMPORT","5678");
insert into tb_config values ("LOCPORT","8765");
insert into tb_config values ("PLUGIN","http://192.168.1.12/plugin");
insert into tb_config values ("UPDATEDIR","http://192.168.1.12/update");
insert into tb_config values ("ISUPDATE","1");
insert into tb_config values ("TIMEOUT","10");
