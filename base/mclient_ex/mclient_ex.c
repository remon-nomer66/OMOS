/*
 * mclient_ex.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 1024
#define BUFSIZE_MAX 4096
#define RECORD_MAX 1000
#define DATA_END ".\n"

extern int setup_connect( char *__target, u_short __port);

int recv_data(int s, char recv_buf[], int buf_size);
int record_division(char *block, char *records[]);

int main(int argc, char *argv[]){
  char sendBuf[BUFSIZE];
  char recvBuf[BUFSIZE_MAX];
  int s, sendLen, recvLen, i;
  u_short port;
  char *records[RECORD_MAX];  //データレコードのポインタ配列
  int recordCount;  //データレコードの件数
  
  if(argc != 3){
    printf("Usage: %s ipaddr port\n", argv[0]);
    exit(1);
  }

  port = atoi(argv[2]); 

  if(( s = setup_connect(argv[1], port)) < 0) exit(1);

  while(1){
    fgets(sendBuf, BUFSIZE, stdin);           //標準入力から読み込み
    if(strncmp(sendBuf,"exit",4) == 0)        //入力文字列の検証(文字列"exit"との比較
      break;
    sendLen = strlen(sendBuf);                //入力文字列のバイト数取得
    send(s, sendBuf, sendLen, 0);             //サーバへ送信
    recvLen = recv_data(s, recvBuf, BUFSIZE_MAX); //データ終了デリミタまでサーバからデータ受信
    recordCount = record_division(recvBuf, records);
    
    for(i=0; i<recordCount; i++){
      printf("[%2d]%s\n", i, records[i]);
    }
  }
  close(s);
}


//=========================================================
// recv_data
// データ終了デリミタ(".\n")まで受信を繰り返す
//   [PARAMETER]
//     int    s         : ソケットディスクリプタ
//     char  *recvBuf   : 受信データを格納するバッファのポインタ
//     int    bufSize   : 受信データを格納するバッファサイズ
//   [RETURN]
//     success : 受信データのトータルサイズ
//     error   : -1 
//=========================================================
int recv_data(int s, char *recvBuf, int bufSize){
  char tmpBuf[BUFSIZE];
  int recvLen, recvTotal=0;
  
  memset(recvBuf, 0, bufSize);  //受信データ格納用バッファを初期化
  do{
    memset(tmpBuf, 0, BUFSIZE);                   //受信用のバッファを初期化
    recvLen = recv(s, tmpBuf, BUFSIZE-1, 0);      //サーバからデータ受信
    if(recvTotal == 0) 
      sprintf(recvBuf, "%s", tmpBuf);
    else if(recvTotal + recvLen < bufSize)
      sprintf(recvBuf, "%s%s", recvBuf, tmpBuf);  //受信データを結合
    else
      return -1;
    recvTotal += recvLen;                        //トータルの受信サイズを更新
  }while(strcmp(&recvBuf[recvTotal-2], DATA_END) != 0); //データ終了デリミタがあるまで繰り返す
  return recvTotal; //トータル受信データ数を返す
} 

//=========================================================
// record_division
// レコードの塊文字列から改行('\n')をレコードの終端としてレコードを分割
//   [PARAMETER]
//     char  *block     : レコードの塊文字列（分割対象文字列）
//     char  *records[] : レコードを表現するポインタ配列
//   [RETURN]
//     success : レコードの件数
//=========================================================
int record_division(char *block, char *records[]){
  int n=0;

  records[n++] = block;
 
  while(n < RECORD_MAX){
    while(*block && *block != '\n') block++; //改行('\n')になるまでポインタを進める
    *block = '\0';                           //改行を文字列終端文字('\0')に変更
    if(strncmp(block+1, DATA_END, 2)==0) break; //データ終了デリミタであれば終了
    records[n++] = ++block; //レコード表現ポインタ配列に次のレコードのポインタを設定
  }
  return n;  //レコード件数を返す
}
