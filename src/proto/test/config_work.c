#include "config.h"

extern struct config_data *pcfgdata;

//将配置文件内容读到缓冲区，事先要给buf分配合适的空间
// int cfg_getfile(char *buf) { return 0; }

static int cfg_init_keyvalue(void);

int cfg_default(void) {
  int err;
  char default_keyvalue[] = {"count=0002\nIPADDR=192.168.1.1\nPORT=1024\n"};

  pcfgdata->fp = fopen(CONFIG_FILE, "w");
  if (pcfgdata->fp == NULL)
    return -1;

  err = fprintf(pcfgdata->fp, "%s\n", default_keyvalue);
  if (err < 0)
    return -1;

  fclose(pcfgdata->fp);

  return 0;
}

int cfg_get_keyvalue(void) {
  return cfg_init_keyvalue();
}

//更新一个配置项
int cfg_update(KeyValue *cfg) {
  int i = 0;
  FILE *fp = NULL;
  dictionary *ini;

  if (cfg == NULL)
    return -1;
  ini = iniparser_load(CONFIG_FILE);
  if (ini == NULL) {
    PLOG("loading config file unsuccessfully\n");
    return -1;
  }

  for (i = 0; i < pcfgdata->cfginf.count; i++) {
    if (strcmp(pcfgdata->cfginf.keyValueList[i].key, cfg->key) == 0)
    {
      iniparser_set(ini, cfg->key, cfg->value);
      fp = fopen(CONFIG_FILE, "w");
      if (fp == NULL) {
        iniparser_freedict(ini);
        return -1;
      }
      iniparser_dump(ini, fp);
      fclose(fp);
      break;
    }
  }
  iniparser_freedict(ini);
  if(pcfgdata->cfginf.count==i)
    return -1;
  else
    cfg_init_keyvalue();

return 0;
}

//获得一个配置的结果
int cfg_getone(char *key, char *cfg) {
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
  int i = 0;
  int err = -1;
  FILE *fp = NULL;
  dictionary *ini = NULL;

  ini = dictionary_new(0);
  if (ini == NULL) {
    PLOG("loading config file unsuccessfully!\n");
    return -1;
  }

  for (i = 0; i < cfg->count; i++) {
    err = iniparser_set(ini, cfg->keyValueList[i].key,
                        cfg->keyValueList[i].value);
    if (err == -1) {
      iniparser_freedict(ini);
      PLOG("setting failure\n");
      return -1;
    }
  }

  fp = fopen(CONFIG_FILE, "w");
  if (fp == NULL) {
    PLOG("open config file unsuccessfully!\n");
    iniparser_freedict(ini);
    return -1;
  }
  iniparser_dump(ini, fp);

  fclose(fp);
  iniparser_freedict(ini);
  cfg_init_keyvalue();
  return 0;
}

//添加一个配置
int cfg_add_one(KeyValue *cfg)
{
  int i = 0;
  FILE *fp = NULL;
  dictionary *ini;

  if (cfg==NULL)
    return -1;

  ini = iniparser_load(CONFIG_FILE);
  if (ini == NULL) {
    PLOG("loading config file unsuccessfully\n");
    return -1;
  }

  for (i = 0; i < pcfgdata->cfginf.count; i++) {
    if (strcmp(pcfgdata->cfginf.keyValueList[i].key, cfg->key) == 0) {
      break;
    }
  }

  if(i<pcfgdata->cfginf.count)
  {
    iniparser_freedict(ini);
    return -1;
  }
  fp = fopen(CONFIG_FILE, "w");
  if (fp == NULL) {
    iniparser_freedict(ini);
    return -1;
  }
  iniparser_set(ini, cfg->key,cfg->value);
  iniparser_dump(ini, fp);
  fclose(fp);
  iniparser_freedict(ini);
  cfg_init_keyvalue();
  return 0;
}

static int cfg_init_keyvalue(void)
{
  int i = 0;
  int keylen = 0;
  int valuelen = 0;
  int alllen = 0;
  dictionary *ini;

  if(pcfgdata->cfginf.cfgbuf!=NULL)
  {
    free(pcfgdata->cfginf.cfgbuf);
  }
  if(  pcfgdata->cfginf.keyValueList!=NULL)
  {
    free(pcfgdata->cfginf.keyValueList);
  }

  ini = iniparser_load(CONFIG_FILE);
  if (ini == NULL) {
    PLOG("cannot parse file");
    return -1;
  }

  pcfgdata->cfginf.count = ini->n;
  pcfgdata->filesize = ini->n * CFG_LINE_LENGTH;

  pcfgdata->cfginf.cfgbuf = (char *)malloc(sizeof(char) * pcfgdata->filesize);
  if (pcfgdata->cfginf.cfgbuf == NULL)
    return -1;

  pcfgdata->cfginf.keyValueList =
      (KeyValue *)malloc(sizeof(KeyValue) * pcfgdata->cfginf.count);
  if (pcfgdata->cfginf.keyValueList == NULL)
    return -1;

  for (i = 0; i < pcfgdata->cfginf.count; i++) {
    if (ini->key[i] == NULL)
      continue;

    if (ini->val[i] != NULL) {
      strcpy(pcfgdata->cfginf.cfgbuf + alllen, ini->key[i]);
      keylen = strlen(ini->key[i]) + 1;
      pcfgdata->cfginf.keyValueList[i].key = pcfgdata->cfginf.cfgbuf + alllen;
      pcfgdata->cfginf.keyValueList[i].keylen = keylen - 1;

      strcpy(pcfgdata->cfginf.cfgbuf + alllen + keylen, ini->val[i]);
      pcfgdata->cfginf.keyValueList[i].value =
          pcfgdata->cfginf.cfgbuf + alllen + keylen;
      valuelen = strlen(ini->val[i]) + 1;
      alllen = alllen + keylen + valuelen;
      pcfgdata->cfginf.keyValueList[i].valuelen = valuelen - 1;
    } else {
      // fprintf(stdout, "[%s]=UNDEF\n", ini->key[i]);
    }
  }
  iniparser_freedict(ini);
  return 0;
}
