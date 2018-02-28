#ifndef _SQLITE3_DAO_H_
#define _SQLITE3_DAO_H_

#include "sqlite3.h"
#define DAO_OK 0
#define DAO_ERROR -1

#define PUBLIC
#define PRIVATE

#define DAO_OPENDB 1
#define DAO_SQLEXEC 2


#define DAO_ERRNO_FNN 1 /*数据库文件名为空*/

typedef struct sqlite3_DAO{
    /*sqlite3数据库变量*/
    PRIVATE sqlite3 *db;
    PRIVATE char db_opttype;
    PRIVATE char db_errno;
    PRIVATE char db_errmsg[256];
    PRIVATE char *db_execerr;
    PUBLIC char **db_qresult;
    PUBLIC int db_qnrow;
    PUBLIC int db_qncolumn;


    PRIVATE int  (*db_callback ) (void  *params , int columnsize, char **columnvalue ,char ** columnname);

    /*
     *func:打开sqlite3数据库
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *       2、*filename数据库的文件名
     *return：
     *       1、成功：返回DAO_OK
     *       2、失败：返回DAO_ERROR
     */
    PUBLIC int (*db_open)(struct sqlite3_DAO *sqlite3dao,const char *filename);

    /*
     *func:设置sqlite3的回调函数
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *       2、callback具体的回调函数实现
     */
    PUBLIC void (*db_setcallback)(struct sqlite3_DAO *sqlite3dao,sqlite3_callback callback);

    /*
     *func:执行sql语句
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *       2、*sqlfmt为sql语句
     * return：
     *       1、成功：返回DAO_OK
     *       2、失败：返回DAO_ERROR
     */
    PUBLIC int (*db_execute)(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...);

    /*
     *func:查表
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *       2、*sqlfmt为sql语句
     */
    PUBLIC int (*db_query)(struct sqlite3_DAO *sqlite3dao,char *sqlfmt,...);

    /*
     *func:清除查询的结果
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *return：
     *       1、成功：返回DAO_OK
     *       2、失败：返回DAO_ERROR
     */
    PUBLIC void (*db_resultclear)(struct sqlite3_DAO *sqlite3dao);

    /*
     *func:得到sqlite3数据库的错误信息
     *param:
     *       1、*sqlite3dao数据库访问对象指针
     *return：
     *      1、成功：返回错误信息
     *      2、失败：返回NULL
     */
    PUBLIC const char *(*db_geterrmsg)(struct sqlite3_DAO *sqlite3dao);

    /*
     *func:关闭数据库及释放申请的系统资源
     *param:
     *      1、*sqlite3dao数据库访问对象指针
     */
    PUBLIC void (*db_close)(struct sqlite3_DAO *sqlite3dao);

}sqlite3_DAO;

 /*
 *func:初始化struct sqlite3_DAO变量*sqlite3dao
 *param:
 *      1、*sqlite3dao数据库访问对象指针
 *return：
 *      1、成功：返回DAO_OK
 *      2、失败：返回DAO_ERROR
 */
PUBLIC int db_init(struct sqlite3_DAO **sqlite3dao);

#endif