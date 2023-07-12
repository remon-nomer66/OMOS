#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>

#ifndef DELIMCHAR
#define DELIMCHAR '\n'
#endif

//******************************************************
// デリミタを区切り文字としてメッセージを取得する
//  [PARAMETER]
//     __s      : TCPコネクション ソケットディスクリプタ
//     __buf    : メッセージを格納するバッファのアドレス
//     __maxlen : メッセージを格納するバッファのサイズ
//   [RETURN]
//     Success  : メッセージのサイズ
//     Error    : -1 or 0
//*******************************************************
int receive_message(int __s, char *__buf, int __maxlen){
  int recv_total = 0;   //受信トータルサイズ
  int delim_count = 0;  //デリミタの数
  int recv_len;         //受信したサイズ

  while((recv_total < __maxlen) && (delim_count < 1)){
    //*** 1バイトずつ受信する ***//
    recv_len = recv(__s, __buf+recv_total, 1, 0);
    if(recv_len < 1){
      return recv_len;
    }
    //*** 現在受信した文字がデリミタであるか比較 ***//
    if( *(__buf+recv_total) == DELIMCHAR )
      delim_count++;
    //*** 受信トータルサイズを加算する ***//
    recv_total += recv_len;
  }

  __buf[recv_total] = '\0';
  return recv_total;
}//END receive_message()