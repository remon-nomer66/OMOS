/*
 * 例題No.1 rei1-messageclient.c
 * Messageクライアント
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

#define BUFSIZE 512

int main(int argc, char *argv[]){
  struct sockaddr_in server;
  char send_buf[BUFSIZE],recv_buf[BUFSIZE];
  int s,con_flag,send_len,recv_len;
  u_short port;
  
  if(argc != 3){
    printf("Usage: %s ipaddr port\n", argv[0]);
    exit(1);
  }

  /*** INETドメインコネクション型のソケットを作成 ***/
  s = socket(AF_INET, SOCK_STREAM, 0);
  if(s < 0){
    perror("socket");
    exit(1);
  }

  /*** サーバへ接続要求 ***/
  memset(&server, 0, sizeof(server));       //構造体serverを0で初期化
  server.sin_family = AF_INET;              //INETドメインを設定
  server.sin_addr.s_addr = inet_addr(argv[1]); //コマンド引数で指定した接続サーバのIPアドレスを設定
  port = atoi(argv[2]);              //コマンド引数で指定したポート番号を整数値へ変換
  server.sin_port = htons(port);     //接続サーバのポート番号を設定
  con_flag = connect(s, (struct sockaddr *)&server, sizeof(server));
  if(con_flag < 0){
    perror("connect");
    exit(1);
  }

  printf("connected to %s\n", inet_ntoa(server.sin_addr));
  while(1){
    fgets(send_buf, BUFSIZE, stdin);           //標準入力から読み込み
    if(strncmp(send_buf,"exit",4) == 0)        //入力文字列の検証(文字列"exit"との比較
      break;
    send_len = strlen(send_buf);               //入力文字列のバイト数取得
    send(s, send_buf, send_len, 0);            //サーバへ送信
    recv_len = recv(s, recv_buf, BUFSIZE, 0);  //サーバから受信
    recv_buf[recv_len] = '\0';                 //文字列の終端コードを設定
    printf("%s", recv_buf);                    //表示
  }
  close(s);

  return 0;
}