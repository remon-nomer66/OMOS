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

#define BUFSIZE 1024
#define PORT 10000
#define ENTER "\n"

//*** プロトコルコマンド ***//
#define UCHECK  "UCHECK"  //会員照会        不要
#define UREG    "UREG"    //会員登録        OK
#define UCHG    "UCHG"    //会員情報変更    OK
#define RESERVE "RESERVE" //予約            OK
#define REREG   "REREG"   //予約登録        OK
#define REDEL   "REDEL"   //予約削除        OK
#define ORDER   "ORDER"   //注文
#define LV      "LV"      //注文レベル選択
#define KITCHEN "KITCHEN" //キッチン        OK
#define KFLAG   "KFLAG"   //キッチン登録
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
//卓，キッチンの終了
//店員への権限付与
//店員としてログイン？

//その他の定数
#define GUEST    "GUEST"    //ゲスト
#define TELLEN  10        //電話番号の長さ(最初の0は含まない)
#define CEKMAX  5         //パスワード試行回数の最大値
#define YES     "YES"
#define NO      "NO"
#define TABEMAX 10        //卓の最大数
#define END     "END"
#define AGUEST  1         //お客様
#define AHQ     2         //本部     
#define ACOR    3         //COR
#define AMGR    4         //店長
#define ACLERK  5         //店員

//*** 預金口座構造体 ***//
typedef struct _AccountBank {
  int     id;            //口座ID
  char    *name;         //口座名
  int     balance;       //預金残高
  int     maxBalance;    //貯金限度額
  int     minBalance;    //借り入れ限度額
}AccountBank;

//*** スレッド関数に渡す引数情報構造体 ***//
typedef struct _ThreadParamter {
  struct in_addr c_ipaddr;  //IPアドレス
  in_port_t      c_port;    //ポート番号
  int            soc;       //コネクション用ソケットディスクリプタ
  PGconn         *con;      //PGconnオブジェクト（データベース）
}ThreadParameter;

//作成した関数を「extern int receive_message(int __s, char *__buf, int __maxlen);」の形で記述
extern int receive_message(int __s, char *__buf, int __maxlen);
extern int setup_listen(unsigned short __port);
extern void *omos_service(void *arg);

extern void userReg();

#endif