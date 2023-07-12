#include "omos.h"

int reserveCheck_s(PGconn *con, int soc, int *u_info){
  int recvLen, sendLen;
  char recvBuf[BUFSIZE],sendBuf[BUFSIZE];//送受信用バッファ
  char sql[BUFSIZE];
  PGresult *res;
  int i;
  int store_id;

    
  store_id = u_info[2];
  
  //sql文作成
  sprintf(sql, "SET search_path to reserve");
  PQexec(con, sql);
  
  sprintf(sql, "SELECT user_id, people_num, reserve_date, reserve_time FROM reserve_t WHERE store_id= %d", store_id);			      					     
  res = PQexec(con, sql);

  if(PQresultStatus(res)!=PGRES_TUPLES_OK){
    printf("%s\n", PQresultErrorMessage(res));
    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
    sprintf(sql, "SET search_path to public");
    PQexec(con, sql);
    PQclear(res);
    return -1;
  }
  
  for(i = 0; i < PQntuples(res); i++){
    sprintf(sendBuf, "%s %s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3));//送信データ作成
    sendLen = strlen(sendBuf);//送信データ長
    send(soc, sendBuf , sendLen, 0);//
    // recvLen = recv(soc, recvBuf, BUFSIZE, 0);受信
    //recvBuf[recvLen] = '\0';受信データにNULLを追加
  }
  
  sprintf(sql, "SET search_path to reserve");
  PQexec(con, sql);
  
  sprintf(sendBuf, "表示できました%s%s", ENTER,DATA_END);
  sendLen = strlen(sendBuf);
  send(soc, sendBuf, sendLen, 0);
  
  //上手くいかなければエラーを表示する。
  if(PQresultStatus(res) != PGRES_TUPLES_OK){ 
    printf("店舗情報を確認できませんでした。\n");
    PQclear(res);
    return -1;
  }
  /*14
    sprintf(sendBuf, "削除しますか?(y/n) %s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(soc, recvBuf);
    recvBuf[recvLen-1] = '\0';　//受信データにNULLを追加


    if(strcmp(recvBuf,"y")==0){
    if(reserveDel_s(con, soc ,*__auth[2]) == 0){
    sprintf(sendBuf, "メニューの変更は正常に完了しました．%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    }else{
    sprintf(sendBuf,"予約確認を終了します。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    }
    }
  */
  PQclear(res);
  return 0;
}
