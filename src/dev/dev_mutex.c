#include "dev.h"

extern struct dev_data *devdata;

int mutex_ctlinflist_lock(struct control_table *ctltable)
{
  pthread_mutex_lock(&devdata->ctltable.ctl_lock);
  return 0;
}

int mutex_ctlinflist_unlock(struct control_table *ctltable)
{
  pthread_mutex_unlock(&devdata->ctltable.ctl_lock);
  return 0;
}

int mutex_devicelist_lock(struct device_table *devtable)
{
  pthread_mutex_lock(&devdata->devtable.dev_lock);
  return 0;
}

int mutex_devicelist_unlock(struct device_table *devtable)
{
  pthread_mutex_unlock(&devdata->devtable.dev_lock);
  return 0;
}

int mutex_medialist_lock(struct media_table *medtable)
{
  pthread_mutex_lock(&devdata->medtable.med_lock);
  return 0;
}

int mutex_medialist_unlock(struct media_table *medtable)
{
  pthread_mutex_unlock(&devdata->medtable.med_lock);
  return 0;
}
