#include "omos.h"

void resereveDel_s(PGconn *__con, int __soc, int id){

int recvLen, sendLen;
char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
char sql[BUFSIZE];
PGresult *res;

sprintf(sql,"DELETE FROM reserve WHERE WHERE account_id= %d", account_id); 
res = PQexec(__con, sql);
if (PQresultStatus(res) != PGRES_COMMAND_OK){
    printf("%s",PQresultErrorMessage(res));
    sprintf(sendBuf, "エラー%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    return -1;
}
PQclear(res);
return 0;
}
