#include "omos.h"
int tableDel(PGconn *__con, int __soc, int __desk_num, int __store_id){
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
  int recvLen, sendLen;   //送受信データ長
  pthread_t selfId = pthread_self();  //スレッドID
  char sql[BUFSIZE];
  PGresult *res;
  int resultRows;

  sprintf(sql,"SELECT * FROM order_t WHERE desk_num = %d AND store_id = %d",__desk_num ,__store_id); //order_tに注文情報が残っていないか確認する
  res = PQexec(__con, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK){//もし、注文情報を確認できなかった場合
    printf("%s",PQresultErrorMessage(res));
    sprintf(sendBuf, "エラー%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
	
    sendLen = sprintf(sendBuf,"%s%d%s%s",ER_STAT,resultRows,ENTER,DATA_END);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
	
    return -1;
  }
  resultRows=PQntuples(res);
  if (resultRows != 0){//注文情報を確認した時に、履歴が残っていた場合
    sprintf(sendBuf, "まだ注文が残っています.%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    sendLen = sprintf(sendBuf,"%s%d%s%s",ER_STAT,resultRows,ENTER,DATA_END);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
	
    return -1;
  }
  sprintf(sql,"UPDATE store_table_t SET desk_use = 0 WHERE desk_num = %d AND store_id = %d",__desk_num ,__store_id);//注文情報を確認した時に、履歴が残っていなかった場合卓削除
  res = PQexec(__con, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK){//アップデートできなかった場合エラーを表示する
    printf("%s",PQresultErrorMessage(res));
    sprintf(sendBuf, "エラー%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
  }
  sendLen = sprintf(sendBuf,"%s%d%s%s",OK_STAT,resultRows,ENTER,DATA_END);
  send(__soc, sendBuf, sendLen, 0);
  printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
  
  PQclear(res);
  return 0;
}    
