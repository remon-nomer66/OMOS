#include "omos.h"

int getAuth(int __soc,  int __userid){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int auth;  //権限

    char *dbHost = "kite.cs.miyazaki-u.ac.jp";
    char *dbPort = "5432";
    char *dbName = "db42";      //接続先を正しく入力
    char *dbLogin = "dbuser42";
    char *dbPwd = "dbpass42";
    char connInfo[BUFSIZE];

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
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT auth FROM user WHERE userid = %d; failed.\n", __userid);
        printf("%s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        con = NULL;
        return -1;
    }
    auth = atoi(PQgetvalue(res, 0, 0));

    //DB切断
    PQclear(res);
    PQfinish(con);
    con = NULL;

    return auth;
}