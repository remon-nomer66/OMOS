#include "omos.h"

int pointCheck(PGconn *__con, int __soc, int userid, pthread_t __selfId){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int point;  //ポイント数

    //DB接続
    sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
    PGconn *con = PQconnectdb(connInfo);

    //useridを元に、pointをデータベースから取得
    sprintf(sql, "SELECT point FROM users WHERE userid = %d;", userid);
    PGresult *res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT point FROM users WHERE userid = %d; failed.\n", userid);
        printf("%s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    }else{
        printf("SELECT point FROM users WHERE userid = %d; success.\n", userid);
    }
    point = atoi(PQgetvalue(res, 0, 0)); //ポイント数

    //DB切断
    PQclear(res);
    PQfinish(con);
    con = NULL;

    return point;
}