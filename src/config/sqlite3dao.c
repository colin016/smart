#include "sqlite3dao.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdarg.h> 

static int sqlprintf(char *str, const char *format, va_list args);
static int db_open(struct sqlite3_DAO *sqlite3dao,const char *filename);
static void db_setcallback(struct sqlite3_DAO *sqlite3dao,sqlite3_callback callback);
static int db_execute(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...);
static const char * db_geterrmsg(struct sqlite3_DAO *sqlite3dao);
static int db_query(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...);
static void db_resultclear(struct sqlite3_DAO *sqlite3dao);
static void db_close(struct sqlite3_DAO *sqlite3dao);

int db_init(struct sqlite3_DAO  **sqlite3dao)
{
    if(sqlite3dao==NULL)
        return DAO_ERROR;

    if(*sqlite3dao==NULL)
    {
        *sqlite3dao=(struct sqlite3_DAO*)calloc(1,sizeof(struct sqlite3_DAO));
    }

    (*sqlite3dao)->db_open=db_open;
    (*sqlite3dao)->db_execute=db_execute;
    (*sqlite3dao)->db_setcallback=db_setcallback;
    (*sqlite3dao)->db_geterrmsg=db_geterrmsg;
    (*sqlite3dao)->db_query=db_query;
    (*sqlite3dao)->db_resultclear=db_resultclear;
    (*sqlite3dao)->db_close=db_close;
    return DAO_OK;
}

static void db_close(struct sqlite3_DAO *sqlite3dao)
{
    sqlite3_close(sqlite3dao->db); 
}

static int sqlprintf(char *str, const char *format, va_list args)
{
    int len=0;
    int i=0;
    int j=0;

    len=strlen(format);
    for(i=0;i<len;i++)
    {
        if(format[i]=='%')
        {
            if(format[i+1]=='d')
            {
                int s=0;
                s = va_arg(args, int);
                sprintf(str+j,"%d",s);
                j=strlen(str);
                i++;
            }else if(format[i+1]=='s')
            {
                char *s=0;
                s = va_arg(args, char *);
                strcpy(str+j,s);
                j=strlen(str);
                i++;
            }else
            {
                return -1;
            }
        }else{
            *(str+j)=*(format+i);
            j++;
        }
    }
    return strlen(str);
}

static int db_open(struct sqlite3_DAO *sqlite3dao,const char *filename)
{
    int ret=0;

    sqlite3dao->db_opttype=DAO_OPENDB;
    if(sqlite3dao==NULL)
    {
        return DAO_ERROR;
    }
    if(filename==NULL)
    {
        sqlite3dao->db_errno=DAO_ERRNO_FNN;
        return DAO_ERROR;
    }

    ret=sqlite3_open(filename,&sqlite3dao->db);
    if(ret!=SQLITE_OK)
    {
        return DAO_ERROR;
    }
    return DAO_OK;
}

static void db_setcallback(struct sqlite3_DAO *sqlite3dao,sqlite3_callback callback)
{
    sqlite3dao->db_callback=callback;
}

static  int db_execute(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...)
{
    int ret=0;
    char sql[1024];
    va_list args;
    
    memset(sql,0,sizeof(sql));
    va_start(args, sqlfmt);
    sqlprintf(sql,sqlfmt,args);
    va_end(args);

    sqlite3dao->db_opttype=DAO_SQLEXEC;
    if(sqlite3dao==NULL)
    {
        return DAO_ERROR;
    }
    PLOG("%s\n",sql);
    ret=sqlite3_exec(sqlite3dao->db,sql,sqlite3dao->db_callback,NULL,&sqlite3dao->db_execerr);
    if(ret!=SQLITE_OK)
    {
        if (ret==SQLITE_MISUSE)
        {
            PLOG("The database is closed.\n");
        }
        return DAO_ERROR;
    }
    return DAO_OK;
}

static const char *db_geterrmsg(struct sqlite3_DAO *sqlite3dao)
{
    if(sqlite3dao==NULL)
    {
        return NULL;
    }

    memset(sqlite3dao->db_errmsg,0,sizeof(sqlite3dao->db_errmsg));
    if(sqlite3dao->db_opttype==DAO_OPENDB)
    {
        if(sqlite3dao->db_errno==DAO_ERRNO_FNN)
        {
            strcpy(sqlite3dao->db_errmsg,"*dbname is NULL");
            return sqlite3dao->db_errmsg;
        }
        return sqlite3_errmsg(sqlite3dao->db);
    }else if(sqlite3dao->db_opttype==DAO_SQLEXEC)
    {
        if(sqlite3dao->db_execerr!=NULL)
        {
            strcpy(sqlite3dao->db_errmsg,sqlite3dao->db_execerr);
            sqlite3_free(sqlite3dao->db_execerr);
            sqlite3dao->db_execerr=NULL;
            return sqlite3dao->db_errmsg;
        }
    }
    return NULL;
}

static int db_query(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...)
{
    int ret=0;
    char sql[1024];
    va_list args;
    
    memset(sql,0,sizeof(sql));
    va_start(args, sqlfmt);
    sqlprintf(sql,sqlfmt,args);
    va_end(args);
    PLOG("%s\n",sql);
    sqlite3dao->db_opttype=DAO_SQLEXEC;
    ret=sqlite3_get_table( sqlite3dao->db , sql , &sqlite3dao->db_qresult , &sqlite3dao->db_qnrow , &sqlite3dao->db_qncolumn , &sqlite3dao->db_execerr);
    if(ret!=SQLITE_OK)
    {
        return DAO_ERROR;
    } 
    return DAO_OK;
}

static void db_resultclear(struct sqlite3_DAO *sqlite3dao)
{
    sqlite3_free_table(sqlite3dao->db_qresult );
    sqlite3dao->db_qresult =NULL;
    sqlite3dao->db_qnrow=0;
    sqlite3dao->db_qncolumn=0;
}