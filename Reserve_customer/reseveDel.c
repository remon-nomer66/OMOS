#include "omos.h"

int reserveDel(PGconn *__con, int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, i, cnt, param;
    char reserve_date[RSRVMAX][10];
    char reserve_time[RSRVMAX][8];
    int reserve_store_id[RSRVMAX][5];
    int reserve_desk_num[RSRVMAX][2];
    char reserve_store_name[RSRVMAX][30];
    char buf[BUFSIZE], comm[BUFSIZE];

    while(1){
        //予約削除する対象があるかチェック
        sprintf(sql, "SELECT * FROM reserve_t WHERE account_id = %d", __auth[1]);
        res = PQexec(__con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "データベースエラー%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows <= 0){
            sprintf(sendBuf, "予約削除の対象がありません%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            return 0;
        }

        //予約削除可能な時
        for(i = 0; i < resultRows; i++){
            reserve_date[i] = PQgetvalue(res, i, 2);
            reserve_time[i] = PQgetvalue(res, i, 3);
            reserve_store_id[i] = atoi(PQgetvalue(res, i, 4));
            reserve_desk_num[i] = atoi(PQgetvalue(res, i, 5));
        }
        for(i = 0; i < resultRows; i++){
            sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", reserve_store_id[i]);
            res = PQexec(__con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                return -1;
            }
            reserve_store_name[i] = PQgetvalue(res, 0, 0);
        }
        sprintf(sendBuf, "削除する予約番号を入力してください%s予約番号 店舗名 予約日 予約時間%s予約削除から抜ける場合は\"END\"と入力してください%s", ENTER, ENTER, ENTER);
        for(i = 0; i < resultRows; i++){
            sprintf(buf, "%d %s %s %s%s", i, reserve_store_name[i], reserve_date
            [i], reserve_time[i], ENTER);
            strcat(sendBuf, buf);
        }
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        //削除対象を受け取り
        while(1){
            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
            if(recvLen != 0){   //文字が入力されたら
                cnt = sscanf(recvBuf, "%d", &param);

                //削除対象の確認
                if(cnt == 1 && 1 <= param && param <= RSRVMAX){
                    sprintf(sendBuf, "削除する対象は%s%3d %s %s %s%sでよろしいですか？よろしい場合は\"YES\"と入力してください%s", ENTER, &param, reserve_store_name[param], reserve_date[param], reserve_time[param], ENTER, ENTER);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                    if(recvLen != 0){
                        cnt = sscanf(recvBuf, "%s", comm);
                        if((cnt == 1) && (strcmp(comm, YES) == 0)){   //実際に削除
                            sprintf(sql, "DELETE FROM reserve_t WHERE account_id = %d AND reserve_date = %s AND reserve_time = %s AND store_id = %s AND desk_num = %s", __auth[1], reserve_date[param], reserve_time[param], reserve_store_id[param], reserve_desk_num[param]);
                            res = PQexec(__con, sql);
                            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                                printf("%s", PQresultErrorMessage(res));
                                sprintf(sendBuf, "データベースエラー%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                return -1;
                            }
                            resultRows = PQntuples(res);
                            if(resultRows != 1){
                                sprintf(sendBuf, "データベースエラー%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                return -1;
                            }
                            break;
                        }
                    }
                }else{
                    cnt = sscanf("%s", comm);
                    if(strcmp(comm, END) == 0){
                        return 0;
                    }
                }
            }
        }
    }
    PQclear(res);
    return 0;
}