#include "omos.h"

void pointUse(int __lsoc, int __totalPrice, int __userid){

    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int point;  //ポイント数
    int usePoint;   //使用するポイント数

    char *dbHost = "kite.cs.miyazaki-u.ac.jp";
    char *dbPort = "5432";
    char *dbName = "db42";      //接続先を正しく入力
    char *dbLogin = "dbuser42";
    char *dbPwd = "dbpass42";
    char connInfo[BUFSIZE];

    //DB接続
    sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
    PGconn *con = PQconnectdb(connInfo);
    if( PQstatus(con) == CONNECTION_BAD ){
        printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
        printf("%s", PQerrorMessage(con));
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    }else{
        printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
    }

    //useridを元に、pointをデータベースから取得
    char sql[BUFSIZE];
    sprintf(sql, "SELECT point FROM users WHERE id = %d;", __userid);
    PGresult *res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT point FROM users WHERE id = %d; failed.\n", __userid);
        printf("%s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    }else{
        printf("SELECT point FROM users WHERE id = %d; success.\n", __userid);
    }

    //pointを取得
    point = atoi(PQgetvalue(res, 0, 0));
    printf("point = %d\n", point);

    //pointを使用
    while(1){
        sendLen = sprintf(sendBuf, "現在のポイント数は%dです。何ポイント使用しますか？%s", point, ENTER);
        send(__lsoc, sendBuf, sendLen, 0);  //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0);    //受信
        recvBuf[recvLen] = '\0';    //文字列化
        usePoint = atoi(recvBuf);   //使用するポイント数
        if(usePoint > point){   //現在のポイント数が使用したいポイントを超えていた場合、エラーを返し際入力を行う
            sendLen = sprintf(sendBuf, "使用したいポイントが現在のポイント数を超えています。%s", ENTER);    //送信
            send(__lsoc, sendBuf, sendLen, 0);  //送信
        }else{
            break;  //現在のポイント数が使用したいポイントを超えていない場合、ループを抜ける
        }
    }

    //合計金額を計算
    __totalPrice -= usePoint;

    return __totalPrice;

}

