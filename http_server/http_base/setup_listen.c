//***********************************************************
// listenソケット作成(コネクション受付開始状態)
//   [PARAMETER]
//     __port :  サーバポート番号
//   [RETURN]
//     Success : listenソケットのディスクリプタ
//     Error   : -1
//**********************************************************

#include "omos.h"

int setup_listen(u_short __port){
  struct sockaddr_in server;
  int soc;

  memset(&server, 0 , sizeof(server)); //構造体serverを0で初期化
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY); //IPアドレスにはすべてを設定
  server.sin_port = htons(__port); //ポート番号を設定

  /* ソケットを作成 */
  if( (soc = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    return -1;
  }

  /* ソケットアドレスを設定 */
  if(bind(soc, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("bind");
    return -1;
  }

  /* 接続受け付けの準備 */
  listen(soc, 50);

  printf("listen...\n");

  return soc;
}