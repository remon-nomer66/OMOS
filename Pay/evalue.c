#include "omos.h"
#include "pay.h"

int evalue(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    //お客様の評価を良いか悪いかで聞く
    int recvLen, sendLen;   //送受信データ長
    int evalue; //評価
    char sql[BUFSIZE];  //SQL文

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res)){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //お客様の評価を良いか悪いかで聞く
    while(1){
        sprintf(sendBuf, "お客様の評価を良いか悪いか,y,nで聞きます。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);  //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        if(strcmp(recvBuf, "y") == 0){
            evalue = 1;
            break;
        }else if(strcmp(recvBuf, "n") == 0){
            evalue = 0;
            break;
        }else{
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2307, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);  //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }//END while()

    //user_point_tのuser_id=u_info[0]でSELECTする。うまくいかなかった場合、ロールバックする
    sprintf(sql, "SELECT user_mag FROM user_point_t WHERE user_id = %d;", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2302, ENTER);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //user_point_tのポイント倍率を取得
    float point_rate = atof(PQgetvalue(res, 0, 0));
    printf("%f\n",point_rate);

    if(point_rate == 0.0 || point_rate == 2.0){
        return point_rate;
    }

    //取得したポイント倍率を表示
    sendLen = sprintf(sendBuf, "ポイント倍率は%.1fです。%s", point_rate, ENTER);
    send(soc, sendBuf, sendLen, 0);  //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //良い場合は、ポイント倍率を+0.1する、悪い場合は-0.1する
    if(evalue == 1){
        point_rate = point_rate + 0.1;
        printf("%f\n",point_rate);
    }else{
        point_rate = point_rate - 0.1;
        printf("%f\n",point_rate);
    }

    //user_point_tのポイント倍率(user_mag)をpoint_rateに更新、うまくいかなかった場合、ロールバックする
    sprintf(sql, "UPDATE user_point_t SET user_mag = %f WHERE user_id = %d;", point_rate, u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("UPDATE failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2312, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //評価を送信
    sendLen = sprintf(sendBuf, "評価を送信しました。%s%s", ENTER, DATA_END);
    send(soc, sendBuf, sendLen, 0);  //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //トランザクション終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;
}
