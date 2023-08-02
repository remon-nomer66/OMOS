/**
 * omos_http.h
 **/

#ifndef _INCLUDE_OMOS_
#define _INCLUDE_OMOS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <postgresql/libpq-fe.h>
#include <ctype.h>

#define BUFSIZE 1024
#define LONG_BUFSIZE 4096
#define PORT 10000
#define ENTER "\n"

//*** プロトコルコマンド ***//
#define RESERVE "reserve" //予約            OK
#define ORDER   "order"   //注文            OK

//その他の定数
#define CEKMAX  5         //パスワード試行回数の最大値
#define RSRVMAX 3         //予約可能最大数

#define LOGIN         "login"
#define RESERVE_CHCK  "reserveCheck"
#define STORE_INFO    "storeInfo"
#define ORDER_CHCK    "orderCheck"

//*** スレッド関数に渡す引数情報構造体 ***//
typedef struct _ThreadParamter {
  struct in_addr c_ipaddr;  //IPアドレス
  in_port_t      c_port;    //ポート番号
  int            soc;       //コネクション用ソケットディスクリプタ
  PGconn         *con;      //PGconnオブジェクト（データベース）
}ThreadParameter;

//関数
extern int receive_message(int __s, char *__buf, int __maxlen);
extern int setup_listen(unsigned short __port);
extern void *omos_http_controller(void *__arg);
extern int createResponseMessage(char *http_response, int status, char *http_header, char *http_body, int body_size);

extern void http_rq_re(pthread_t selfId, int soc, char *message, unsigned int size, int rsflag);
extern int httpGetCommand(char *tmpRecvBuf, char command[][BUFSIZE], char param[][BUFSIZE], int *u_info, char *pw, int *s_info, int *layer);
extern int httpPostCommand(char *tmpRecvBuf, char command[][BUFSIZE], char param[][BUFSIZE], int *u_info, char *pw, int *s_info, int *layer);
extern int omos_http_branch(pthread_t selfId, PGconn *con, int soc, int *u_info, char *pw, int *s_info, char command[][BUFSIZE], char param[][BUFSIZE], char *http_header, char *http_body, int *body_size, int layer);

extern int http_reserve_check(pthread_t selfId, PGconn *con, int soc, int *u_info);
extern int http_reserve_store(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *body_size);
extern int http_reserve_reg(pthread_t selfId, PGconn *con, int soc, int *u_info, char command[][BUFSIZE], char param[][BUFSIZE]);
extern int http_user_check(pthread_t selfId, PGconn *con, int soc, int *u_info, char *pw, char command[][BUFSIZE], char param[][BUFSIZE]);
extern int http_order_check(pthread_t selfId, PGconn *con, int soc, int *s_info, char command[][BUFSIZE], char param[][BUFSIZE], char *http_body);
extern int http_order(pthread_t selfId, PGconn *con, int soc, int *s_info, int *u_info, int layer, char command[][BUFSIZE], char param[][BUFSIZE], char *http_body, char trimmd_comm[][5]);

extern int response_user_OK(char *http_header, char *http_body, int *u_info, char *pw, int *body_size);
extern int response_user_NG(char *http_header, char *http_body, int *u_info, char *pw, int *body_size);

extern int response_reserve_check_OK(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size);
extern int response_reserve_check_NG(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size);

extern int response_reserve_reg_OK(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size, char param[][BUFSIZE]);
extern int response_reserve_reg_NG(char *http_header, char *http_body, int *u_info, int *body_size);
extern int response_reserve_reg_else(char *http_header, char *http_body, int *u_info, int *body_size);

extern int response_order_check_OK(char *http_header, char *http_body, int *u_info, int *body_size);
extern int response_order_check_NG(char *http_header, char *http_body, int *u_info, int *body_size);

extern int response_order_OK(pthread_t selfId, PGconn *con, int soc, char param[][BUFSIZE], char *http_header, char *http_body, int *u_info, int *body_size, char trimmd_comm[][5]);
extern int response_order_NG(char *http_header, char *http_body, int *u_info, int *body_size);

#endif
