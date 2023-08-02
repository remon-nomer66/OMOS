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
#define UCHG    "UCHG"    //会員情報変更
#define RESERVE "RESERVE" //予約
#define REREG   "REREG"   //予約登録
#define RECHG   "RECHG"
#define REDEL   "REDEL"   //予約削除
#define ORDER   "ORDER"   //注文
#define KITCHEN "KITCHEN" //キッチン
#define KFLAG   "KFLAG"   //キッチン登録
#define TREG    "TREG"    //卓登録
#define TDEL    "TDEL"    //卓削除
#define MENU    "MENU"    //メニュー
#define MREG    "MREG"    //メニュー登録
#define MDEL    "MDEL"    //メニュー削除
#define MCHG    "MCHG"    //メニュー変更
#define DEMAND  "DEMAND"  //発注
#define DREG    "DREG"    //発注登録
#define RECHECK "RECHECK" //予約確認
#define REREG   "REREG"   //予約登録
#define RECHG   "RECHG"   //予約変更
#define RDEL    "RDEL"    //予約削除
#define STCHECK "STCHECK" //在庫確認
#define CORRECT "CORRECT" //集計
#define CCHECK  "CCHECK"  //集計確認
#define SACHECK "SACHECK" //売上確認
#define HISTORY "HISTORY" //履歴
#define PAY     "PAY"     //会計
#define PCHECK  "PCHECK"  //ポイントチェック
#define PUSE    "PUSE"    //ポイント使用
#define PUNUSE  "PUNUSE"  //ポイント不使用
#define EVALUE  "EVALUE"  //評価
#define HIST    "HIST"
#define KDEL    "KDEL"

#define ACHG    "ACHG"    //AuthChange
#define SCHG    "SCHG"    //StoreChange
#define TSREG   "TSREG"   //tableStoreReg

#define KINFO   "KINFO"
#define RINFO   "RINFO"

#define KREG    "KREG"
#define KDEL    "KDEL"


//その他の定数
#define GUEST    "GUEST"    //ゲスト
#define TELLEN  11        //電話番号の長さ(最初の0は含まない)
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
extern void service_guest(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int service_employee(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info, int *reg);

extern void service_menu(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern void userCheck(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int userCheckSQL(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, char *tel, char *pass, int *u_info);
extern int userReg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf);
extern int userChange(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf);

extern int janken(pthread_t selfId,int soc,char *recvBuf,char *sendBuf);

extern void reserve(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int reserveShow(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int reserveReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int reserveChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int reserveCheck(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int reg_chg_flag, int reserve_no);
extern int reserveDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern void reserve_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int reserveShow_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int reserveCheck_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info, int reg_chg_flag, int reserve_no);
extern int reserveReg_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int reserveChg_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int reserveDel_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern int kitchen(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info, int *uinfo, char *reg);
extern int kitchenReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, char *reg);
extern int kitchenDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, char *reg);
extern int kitchenView(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int kitchenFlag(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern int tableReg(PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);
extern int tableAlt(PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);
extern int tableDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int tableStoreReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern void service_menu(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int menuReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int menuDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);
extern int menuChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern int demand(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf);

extern int storageCheck(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info);

extern int correct(pthread_t selfId, PGconn *con, int soc, int *u_info);

extern int order(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info);
extern int history(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info);
extern int pay(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf);

#endif