#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/************************************************************************
 * setup_connect
 * サーバとの接続を確立するまでのセットアップ関数
 *  [ARGUMENT] 
 *    __target :  サーバIPアドレスまたはホスト名の文字列 
 *    __port   :  サーバポート番号
 *  [RETURN]
 *    SUCCESS  :  サーバとのコネクションが確立したソケットディスクリプタ
 *    ERROR    :  -1
 *************************************************************************/
int setup_connect( char *__target, u_short __port){
  int sid;
  in_addr_t ip;
  struct hostent *host;
  struct sockaddr_in server;

  memset(&server, 0, sizeof(server));

  /*** 接続サーバのIPアドレス設定  ***/
  if( (ip = inet_addr(__target)) == -1 ){
    if( (host = gethostbyname(__target)) != NULL ){
      memcpy( &(server.sin_addr), *(host->h_addr_list), host->h_length);
    }else{
      fprintf(stderr, "%s Unknown host\n", __target);
      return -1;
    }
  }else{
    server.sin_addr.s_addr = ip;
  }

  server.sin_family = AF_INET;    //ドメインファミリにINETドメインを設定
  server.sin_port = htons(__port);  //接続サーバのポート番号を設定

  if((sid = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    return -1;
  }

  if(connect(sid, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("connect");
    return -1;
  }

  printf("connected to %s\n", inet_ntoa(server.sin_addr));

  return sid; //コネクションが確立したソケット識別子を返す
}









