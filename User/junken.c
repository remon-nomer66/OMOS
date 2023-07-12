#include "omos.h"
int junken(pthread_t selfId,int soc,char *recvBuf,char *sendBuf){

  int computerhand, playerhand;
  int recvLen, sendLen;                    // 送受信データ長

  while(1){
    // お客さんにじゃんけんタイムを知らせ,入力もらう
    sprintf(sendBuf,"じゃんけんタイム！\n グーなら0,チョキなら1,パーなら2を押してください.\nオリオンじゃんけんじゃんけんぽい！%s%s" ,ENTER,DATA_END); //送信データ
    sendLen = strlen(sendBuf); //送信データ長
    send(soc,sendBuf,sendLen,0);//送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
    recvLen = recv(soc,recvBuf,BUFSIZE,0);//受信
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    playerhand = atoi(recvBuf);//文字列から数値に直した
    printf("%d",playerhand);
      

    if(playerhand==0 || playerhand==1 || playerhand==2){
      printf("test100");
      break;
    }
      
    sprintf(sendBuf, "yarinaosi%s%s", ENTER,DATA_END);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(soc, sendBuf, sendLen, 0); // 送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
     recvLen = recv(soc,recvBuf,BUFSIZE,0);//受信
    recvBuf[recvLen-1] = '\0';
    printf("[C_THREAD %ld]RECV=>%s",selfId,recvBuf);
  
  }
  
  srand((unsigned int)time(NULL)); // 乱数の初期化

  computerhand = rand() % 3 + 1;  // ランダムに画面にじゃんけんを表示する
 
  
  // お客さんとコンピュータ側が出した手を表示する
  
  if (playerhand == 0){
    sprintf(sendBuf, "あなたが出した手:グー\n%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(soc, sendBuf, sendLen, 0); // 送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
    
    
  }
  else if (playerhand == 1){
    sprintf(sendBuf, "あなたが出した手:チョキ%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(soc, sendBuf, sendLen, 0); // 送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
    
  
  }
  else{
    sprintf(sendBuf, "あなたが出した手:パー%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(soc, sendBuf, sendLen, 0); // 送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
  }  
  
  if (computerhand == 0){
    sprintf(sendBuf, "相手が出した手:グー%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(soc, sendBuf, sendLen, 0); // 送信
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
    
  }
  else if (computerhand == 1)
    {
      sprintf(sendBuf, "相手が出した手:チョキ%s", ENTER);
      sendLen = strlen(sendBuf);         // 送信データ長
      send(soc, sendBuf, sendLen, 0); // 送信
      printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
     
    }
  else 
    {
      sprintf(sendBuf, "相手が出した手:パー%s", ENTER);
      sendLen = strlen(sendBuf);         // 送信データ長
      send(soc, sendBuf, sendLen, 0); // 送信
      printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
    }

  if (playerhand == computerhand) // あいこの場合
    {
      sprintf(sendBuf, "あいこだったの,500ポイント付与します%s%s", ENTER,DATA_END);
      sendLen = strlen(sendBuf);         // 送信データ長
      send(soc, sendBuf, sendLen, 0); // 送信
      printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
      return 500;
    }
  else if (playerhand == 0 && computerhand == 1 || playerhand == 1 && computerhand == 2 || playerhand == 2 && computerhand == 0)
    { // 勝ちの場合

      sprintf(sendBuf, "勝ちだったので,1000ポイント付与します%s%s", ENTER,DATA_END);
      sendLen = strlen(sendBuf);         // 送信データ長
      send(soc, sendBuf, sendLen, 0); // 送信
      printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
      return 1000;
    }
  else
    {//負けの場合
      sprintf(sendBuf, "負けだったので,300ポイント付与します%s%s", ENTER,DATA_END);
      sendLen = strlen(sendBuf);         // 送信データ長
      send(soc, sendBuf, sendLen, 0); // 送信
      printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
      return 300;
    }
  return 0;
}
