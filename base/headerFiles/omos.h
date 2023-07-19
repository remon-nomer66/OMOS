/**
 * omos.h
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
#define ENTER "\n"     //<LF>
#define DATA_END ".\n" //レスポンスデータの終端デリミタ

//*** レスポンスステータス ***//
#define OK_STAT    "+OK"     //成功
#define ER_STAT    "-ERR"    //失敗

//*** エラーコード ***//
#define E_CODE_100    100  //データベースエラー
#define E_CODE_200    200  //リクエストコマンドの引数エラー
#define E_CODE_300    300  //リクエストコマンドが存在しない

//*** プロトコルコマンド ***//
#define UCHECK  "UCHECK"  //会員照会        不要
#define UREG    "UREG"    //会員登録        OK
#define UCHG    "UCHG"    //会員情報変更    OK
#define RESERVE "RESERVE" //予約            OK
#define REREG   "REREG"   //予約登録        OK
#define REDEL   "REDEL"   //予約削除        OK
#define ORDER   "ORDER"   //注文            OK
#define LV      "LV"      //注文レベル選択  関数内部
#define KITCHEN "KITCHEN" //キッチン        OK
#define KFLAG   "KFLAG"   //キッチン登録    OK
#define TREG    "TREG"    //卓登録          OK
#define TDEL    "TDEL"    //卓削除          不要
#define MENU    "MENU"    //メニュー        OK
#define MREG    "MREG"    //メニュー登録    OK
#define MDEL    "MDEL"    //メニュー削除    OK
#define MCHG    "MCHG"    //メニュー変更    OK
#define DEMAND  "DEMAND"  //発注            OK
#define DREG    "DREG"    //発注登録        OK
#define RECHECK "RECHECK" //予約確認        OK
#define RDEL    "RDEL"    //予約削除        重複
#define STCHECK "STCHECK" //在庫確認        OK
#define CORRECT "CORRECT" //集計            OK
#define CCHECK  "CCHECK"  //集計確認        OK
#define SACHECK "SACHECK" //売上確認        OK
#define HISTORY "HISTORY" //履歴            OK
#define PAY     "PAY"     //会計            OK
#define PCHECK  "PCHECK"  //ポイントチェック 関数内部
#define PUSE    "PUSE"    //ポイント使用    関数内部
#define PUNUSE  "PUNUSE"  //ポイント不使用  関数内部
#define EVALUE  "EVALUE"  //評価            関数内部
#define HIST    "HIST"

#define KDEL    "KDEL"

//その他の定数
#define GUEST    "GUEST"    //ゲスト
#define TELLEN  10        //電話番号の長さ(最初の0は含まない)
#define CEKMAX  5         //パスワード試行回数の最大値
#define YES     "YES"
#define NO      "NO"
#define OK      "OK"
#define TABEMAX 10        //卓の最大数
#define END     "END"
#define AGUEST  1         //お客様
#define AHQ     2         //本部     
#define ACOR    3         //COR
#define AMGR    4         //店長
#define ACLERK  5         //店員
#define ANUREG  6         //未登録
#define RSRVMAX 3         //予約可能最大数
#define STREMAX 20        //店舗数
#define INIT    100       //初期化

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
extern void *omos_controller(void *__arg);
extern void service_user(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int service_table(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);
extern int service_kitchen(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);
extern void service_guest(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int service_employee(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);

//extern int userCheck(void);
//extern int userReg(void);
//extern int userChg(void);
//extern int userCheck(void);

//extern int janken(int soc, char *recvBuf, char *sendBuf);

//extern void reserve(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
//extern int reserveReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
//extern int reserveChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
//extern int reserveReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
//extern int reserveCheck(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int reg_chg_flag, int reserve_no);

//extern int kitchen(PGconn *__con, int __soc, int __tableNum);
//extern int kitchenView(void);
//extern int kitchenFlag(void);

//extern int tableAlt(PGconn *__con, int __soc);
//extern int tableReg(PGconn *__con, int __soc, int *__u_info, int *__s_info);
//extern int tableDel(PGconn*__con,int__soc,int__desk_num,int __store_id);

//extern int kitchenDel(void);

//extern void menu(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
//extern int menuReg(PGconn *__con, int __soc, int *__u_info);
//extern int menuDel(PGconn *__con, int __soc, int *__u_info);
//extern int menuChg(PGconn *__con, int __soc, int *__u_info);

//extern int demandReg(void);

//extern int reserveCheck_s(void);
//extern int reserveDel_s(void);

//extern int storageCheck(PGconn *__con, int __soc, int __auth);

//extern int correct(PGconn *__con, int __soc, int __auth);

//extern int saleCheck(void);

//extern int order(pthread_t selfId, PGconn *con, char *recvBuf, char *sendBuf, int soc, int *u_info, int *s_info);
//extern int history(void);
//extern int pay(void);

#endif