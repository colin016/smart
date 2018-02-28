/*
*Filename:config_work.c
*
*Author:ZhuGuoquan
*
*Date:2018-1-16
*
*Func:   The main function of the source file is to read key-value array from sqlite3 database
*      and turn the key-value array to 'struct KeyValueArray(KeyValueArray)'.
*        This source can also implement some interfaces that add key-value pair to sqlite3,
*      delete key-value pair from sqlite3 and update key-value pair in sqlite3.
*/


#include "config.h"

extern struct config_data *pcfgdata;

static int cfg_default(void);
static int cfg_read_keyvalues(void);
static int cfg_save_keyvalues(KeyValueArray *cfg);
inline static int cfg_clear_tbconfig(void);
inline static void cfg_keyvalue_release(void);

int cfg_init_keyvalues(void)
{
  if(cfg_read_keyvalues()==-1)
  {
    if(cfg_default()==-1)
    {
      return -1;
    }
  }
  return 0;
}

//更新一个配置项
int cfg_update(KeyValue *cfg) {
    int ret=0;
    int i=0;
    int flag=-1;

    for(i=0;i<pcfgdata->cfginf.count;i++)
    {
        if(strcmp(pcfgdata->cfginf.keyValueList[i].key,cfg->key)==0)
        {
          flag=0;
          break;
        }
    }
    //表示已找到
    if(flag==0)
    {
        ret=pcfgdata->sqlite3dao->db_open(pcfgdata->sqlite3dao,DB_CONFIG);
        if(ret==DAO_ERROR)
        {
          PLOG("%s\n",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
          goto error;
        }

        ret=pcfgdata->sqlite3dao->db_query(pcfgdata->sqlite3dao,
        "select * from "TABLE_CONFIG" where "TABLE_CONFIG_KEY"=\"%s\"",cfg->key);
        if(ret==DAO_ERROR)
        {
          PLOG("%s\n",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
          goto error;
        }

        if(pcfgdata->sqlite3dao->db_qnrow>=1)
        {
          ret=pcfgdata->sqlite3dao->db_execute(pcfgdata->sqlite3dao,
          "update "TABLE_CONFIG" set "TABLE_CONFIG_VALUE"=\"%s\" where "TABLE_CONFIG_KEY"=\"%s\"",cfg->value,cfg->key);
          if(ret==DAO_ERROR)
          {
            PLOG("%s\n",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
            goto error;
          }
          //查询完了之后立即关闭数据库
          pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
          cfg_read_keyvalues();
        }else
        {
          PLOG("key not found in database.");
          goto error;
        }

        //清除查询的结果，释放内存
        pcfgdata->sqlite3dao->db_resultclear(pcfgdata->sqlite3dao);
    }else
    {
        PLOG("key not found in keyvaluelist.");
        return -1;
    }
    return 0;

error:
    pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
    return -1;
}

//获得一个配置的结果
int cfg_getone(const char *key,char *cfg) {
  int i = 0;

  for (i = 0; i < pcfgdata->cfginf.count; i++) {
    if (strcmp(pcfgdata->cfginf.keyValueList[i].key, key) == 0) {
      strcpy(cfg, pcfgdata->cfginf.keyValueList[i].value);
      return 0;
    }
  }

  return -1;
}

int cfg_update_all(KeyValueArray *cfg) {
  int ret=0;

  ret=cfg_save_keyvalues(cfg);
  if(cfg_read_keyvalues()==-1)
  {
    return -1;
  }
  return ret;
}

//添加一个配置
int cfg_add_one(KeyValue *cfg)
{
  int ret=0;

  ret=pcfgdata->sqlite3dao->db_open(pcfgdata->sqlite3dao,DB_CONFIG);
  if (ret==DAO_ERROR)
  {
    goto error;
  }

  ret=pcfgdata->sqlite3dao->db_execute(pcfgdata->sqlite3dao,
  "insert into "TABLE_CONFIG" values (\"%s\",\"%s\")",cfg->key,cfg->value);
  if (ret==DAO_ERROR)
  {
    PLOG("%s",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
    goto error;
  }

  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  if(cfg_read_keyvalues()==-1)
  {
    return -1;
  }
  return 0;

error:
  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  return -1;
}

static int cfg_default(void) {

  return 0;
}

/*
*func:Reading key-value array from sqlite3 database.
*return:Reading key values successfully return 0,else return -1.
*/
static int cfg_read_keyvalues(void)
{
    int ret=0;
    int i = 0;
    int keylen = 0;
    int valuelen = 0;
    int alllen = 0;
    char *cfgbuf=NULL;
    KeyValue *keyvaluelist=NULL;

    ret=pcfgdata->sqlite3dao->db_open(pcfgdata->sqlite3dao,DB_CONFIG);
    if(ret==DAO_ERROR)
    {
      goto error;
    }
    //查询本模块的配置表（smart_cfg），查询的结果在pcfgdata->sqlite3dao->db_qresult中，用完后
    //需要释放pcfgdata->sqlite3dao->db_qresult的内存。
    ret=pcfgdata->sqlite3dao->db_query(pcfgdata->sqlite3dao,"select * from "TABLE_CONFIG);
    if(ret==DAO_ERROR)
    {
      PLOG("%s\n",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
      goto error;
    }

    //查询完了之后立即关闭数据库
    pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);

    cfgbuf = (char *)calloc(1,sizeof(char) *pcfgdata->sqlite3dao->db_qnrow*MAX_KEYVALUELEN);
    if(cfgbuf==NULL)
    {
      PLOG("Application memory failure.\n");
      goto error;
    }

    keyvaluelist=(KeyValue *)malloc(sizeof(KeyValue) * pcfgdata->sqlite3dao->db_qnrow);
    if(keyvaluelist==NULL)
    {
      PLOG("Application memory failure.\n");
      goto error;
    }

    for(i=1;i<=pcfgdata->sqlite3dao->db_qnrow;i++)
    {
      keylen=strlen(pcfgdata->sqlite3dao->db_qresult[i*pcfgdata->sqlite3dao->db_qncolumn])+1;
      valuelen=strlen(pcfgdata->sqlite3dao->db_qresult[i*pcfgdata->sqlite3dao->db_qncolumn+1])+1;

      strcpy(cfgbuf+alllen,pcfgdata->sqlite3dao->db_qresult[i*pcfgdata->sqlite3dao->db_qncolumn]);
      keyvaluelist[i-1].key=cfgbuf+alllen;
      alllen+=keylen;

      strcpy(cfgbuf+alllen,pcfgdata->sqlite3dao->db_qresult[i*pcfgdata->sqlite3dao->db_qncolumn+1]);
      keyvaluelist[i-1].value=cfgbuf+alllen;
      alllen+=valuelen;

      keyvaluelist[i-1].keylen=keylen-1;
      keyvaluelist[i-1].valuelen=valuelen-1;
    }

    pcfgdata->cfginf.count=pcfgdata->sqlite3dao->db_qnrow;
    //清除查询的结果，释放内存
    pcfgdata->sqlite3dao->db_resultclear(pcfgdata->sqlite3dao);
    //如果有键值对，则清除原来的键值对，反之什么也不做
    cfg_keyvalue_release();
    //重新赋值键值对
    pcfgdata->cfginf.cfgbuf=cfgbuf;
    pcfgdata->cfginf.keyValueList=keyvaluelist;
    return 0;

error:
    if(cfgbuf!=NULL)
    {
      free(cfgbuf);
    }
    if(keyvaluelist!=NULL)
    {
      free(cfgbuf);
    }
    return -1;
}

static int cfg_save_keyvalues(KeyValueArray *cfg)
{
  int ret=0;
  int i=0;

  ret=cfg_clear_tbconfig();
  if (ret!=0)
  {
    PLOG("Clear "TABLE_CONFIG" failure.\n");
    goto error;
  }

  ret=pcfgdata->sqlite3dao->db_open(pcfgdata->sqlite3dao,DB_CONFIG);
  if (ret==DAO_ERROR)
  {
    goto error;
  }

  for (i = 0; i < cfg->count; ++i)
  {
    ret=pcfgdata->sqlite3dao->db_execute(pcfgdata->sqlite3dao,
    "insert into "TABLE_CONFIG" values (\"%s\",\"%s\")",
    cfg->keyValueList[i].key,cfg->keyValueList[i].value);
    if (ret==DAO_ERROR)
    {
      PLOG("%s",pcfgdata->sqlite3dao->db_geterrmsg(pcfgdata->sqlite3dao));
      goto error;
    }
  }

  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  return 0;

error:
  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  return -1;
}

inline static void cfg_keyvalue_release(void)
{
  if(pcfgdata->cfginf.cfgbuf!=NULL)
  {
    free(pcfgdata->cfginf.cfgbuf);
  }
  if(  pcfgdata->cfginf.keyValueList!=NULL)
  {
    free(pcfgdata->cfginf.keyValueList);
  }
}

inline static int cfg_clear_tbconfig(void)
{
  int ret=0;
  char deletesql[]={"delete from "TABLE_CONFIG";"};
  char updatesql[]={"update sqlite_sequence SET seq = 0 where name ='"TABLE_CONFIG"';"};

  ret=pcfgdata->sqlite3dao->db_open(pcfgdata->sqlite3dao,DB_CONFIG);
  if(ret==DAO_ERROR)
  {
    goto error;
  }

  ret=pcfgdata->sqlite3dao->db_execute(pcfgdata->sqlite3dao,deletesql);
  if (ret==DAO_ERROR)
  {
    PLOG("execute sql failure.");
    goto error;
  }

  ret=pcfgdata->sqlite3dao->db_execute(pcfgdata->sqlite3dao,updatesql);
  if (ret==DAO_ERROR)
  {
    PLOG("execute sql failure.");
   goto error;
  }

  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  return 0;

error:
  pcfgdata->sqlite3dao->db_close(pcfgdata->sqlite3dao);
  return -1;
}