#include "omos.h"

int evalue(PGconn *__con, int __soc, int *__u_info, pthread_t __selfId){
    //お客様の評価を良いか悪いかで聞く
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    int evalue; //評価

    //トランザクション開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }
    
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

    //user_point_tのuser_id=u_info[0]でSELECTする。うまくいかなかった場合、ロールバックする
    sprintf(sql, "SELECT * FROM user_point_t WHERE user_id = %d", __u_info[0]);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //user_point_tのポイント倍率を取得
    float point_rate = atoi(PQgetvalue(res, 0, 2));

    //取得したポイント倍率を表示
    sendLen = sprintf(sendBuf, "ポイント倍率は%fです。%s", point_rate, ENTER);
    send(__soc, sendBuf, sendLen, 0);  //送信

    //良い場合は、ポイント倍率を+0.1する、悪い場合は-0.1する
    if(evalue == 1){
        point_rate = point_rate + 0.1;
    }else{
        point_rate = point_rate - 0.1;
    }

    //user_point_tのポイント倍率(user_mag)をpoint_rateに更新、うまくいかなかった場合、ロールバックする
    sprintf(sql, "UPDATE user_point_t SET user_mag = %f WHERE user_id = %d", point_rate, u_info[0]);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("UPDATE failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //評価を送信
    sendLen = sprintf(sendBuf, "評価を送信しました。%s", ENTER);
    send(__soc, sendBuf, sendLen, 0);  //送信

    //トランザクション終了
    res = PQexec(__con, "COMMIT");
    PQclear(res);

    return 0;

}