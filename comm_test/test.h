/**
 * test.h
 **/

#ifndef _INCLUDE_TEST_
#define _INCLUDE_TEST_

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
#define PORT 10000
#define ENTER "\n"

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
#define TEST    "TEST"
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

//*** スレッド関数に渡す引数情報構造体 ***//
typedef struct _ThreadParamter {
  struct in_addr c_ipaddr;  //IPアドレス
  in_port_t      c_port;    //ポート番号
  int            soc;       //コネクション用ソケットディスクリプタ
  PGconn         *con;      //PGconnオブジェクト（データベース）
}ThreadParameter;

extern int receive_message(int __s, char *__buf, int __maxlen);
extern int setup_listen(unsigned short __port);
extern void *test_service(void *__arg);

//以下に「extern 型名 関数名(引数, ...);」の形で関数を記述
extern void test_func(PGconn *__con, int __soc); //例(削除してください)

#endif