#include "omos.h"

int evalue(int __soc, pthread_t __selfId, int __user_id){
    //お客様の評価を良いか悪いかで聞く
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int evalue; //評価

    char *dbHost = "kite.cs.miyazaki-u.ac.jp";
    char *dbPort = "5432";
    char *dbName = "db42";      //接続先を正しく入力
    char *dbLogin = "dbuser42";
    char *dbPwd = "dbpass42";
    
    //お客様の評価を良いか悪いかで聞く
    while(1){
        sendLen = sprintf(sendBuf, "お客様の評価を良いか悪いかで聞きます。良い場合は1、悪い場合は0を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);  //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);    //受信
        recvBuf[recvLen] = '\0';    //文字列化
        evalue = atoi(recvBuf);   //評価
        if(evalue != 0 && evalue != 1){   //評価が0か1でない場合、エラーを返し再入力を行う
            sendLen = sprintf(sendBuf, "評価は0か1で入力してください。%s", ENTER);    //送信
            send(__soc, sendBuf, sendLen, 0);  //送信
        }else{
            break;  //評価が0か1の場合、ループを抜ける
        }
    }

    //データベースに接続
    PGconn *conn = PQsetdbLogin(dbHost, dbPort, NULL, NULL, dbName, dbLogin, dbPwd);
    if(PQstatus(conn) == CONNECTION_BAD){
        printf("データベースへの接続に失敗しました。");
        exit(1);
    }

    //userDBのポイント倍率を取得
    char *sql = "SELECT point_rate FROM userDB WHERE user_id = $1";
    PGresult *res = PQexecParams(conn, sql, 1, NULL, (const char * const *)&__user_id, NULL, NULL, 0);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("データベースの検索に失敗しました。");
        exit(1);
    }
    int point_rate = atoi(PQgetvalue(res, 0, 0));   //ポイント倍率
    PQclear(res);

    //取得したポイント倍率を表示
    sendLen = sprintf(sendBuf, "ポイント倍率は%dです。%s", point_rate, ENTER);
    send(__soc, sendBuf, sendLen, 0);  //送信

    //良い場合は、ポイント倍率を+0.1する、悪い場合は-0.1する
    if(evalue == 1){
        point_rate = point_rate + 0.1;
    }else{
        point_rate = point_rate - 0.1;
    }

    //userDBのポイント倍率を更新
    char *sql2 = "UPDATE userDB SET point_rate = $1 WHERE user_id = $2";
    const char *paramValues[2];
    char point_rate_str[10];
    sprintf(point_rate_str, "%d", point_rate);
    paramValues[0] = point_rate_str;
    paramValues[1] = __user_id;
    res = PQexecParams(conn, sql2, 2, NULL, paramValues, NULL, NULL, 0);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("データベースの更新に失敗しました。");
        exit(1);
    }
    PQclear(res);

    //データベースから切断
    PQfinish(conn);

    //評価を送信
    sendLen = sprintf(sendBuf, "評価を送信しました。%s", ENTER);
    send(__soc, sendBuf, sendLen, 0);  //送信

    return 0;

}