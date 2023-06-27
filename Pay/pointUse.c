#include "omos.h"

int pointUse(int __soc, int __totalPrice, int __userid, pthread_t __selfId){

    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int point;  //ポイント数
    int usePoint;   //使用するポイント数
    char sql[BUFSIZE];

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
    point = pointCheck(soc, userid, selfId);

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

    //DB切断
    PQclear(res);
    PQfinish(con);
    con = NULL;

    //合計金額を送信
    sendLen = sprintf(sendBuf, "合計金額は%d円です。%s", __totalPrice, ENTER);
    send(__lsoc, sendBuf, sendLen, 0);  //送信

    return __totalPrice;
}

