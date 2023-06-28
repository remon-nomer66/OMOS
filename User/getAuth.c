#include "omos.h"

int getAuth(PGconn *__con, int __soc,  int __userid){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int auth;  //権限

    //DB接続
    sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
    PGconn *con = PQconnectdb(connInfo);

    //DB接続失敗時
    if( PQstatus(con) == CONNECTION_BAD ){
        printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
        printf("%s", PQerrorMessage(con));
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    //DB接続成功時
    }else{
        printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
    }

    //useridを元に、userテーブルからauthを取得
    sprintf(sql, "SELECT auth FROM user WHERE userid = %d;", __userid);
    PGresult *res = PQexec(con, sql);
    if( PQresultStatus(res) != PGRES_TUPLES_OK ){
        printf("SELECT failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    }else{
        auth = atoi(PQgetvalue(res, 0, 0));
        printf("auth: %d\n", auth);
    }

    //DB切断
    PQclear(res);
    PQfinish(con);
    con = NULL;

    return auth;
}