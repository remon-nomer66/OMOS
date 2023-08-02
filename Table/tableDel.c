#include "omos.h"
#include "table.h"

int tableDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    PGresult *res;
    PGresult *res2;
    int resultRows;
    int cnt = 0;
    int tmp = 0;
    int num = 0;

    int store_id = s_info[0];
    int desk_num = s_info[1];


    while(1){
        //削除したい卓番号を入力してください
        sprintf(sendBuf, "削除したい卓番号を入力してください%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0);   //送信
        printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
        //cntに入力されたテーブル番号を代入
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        printf("[C_THREAD %ld]RECV=>%s\n",selfId,recvBuf);
        cnt = atoi(recvBuf);

        printf("cnt = %d\n", cnt);
        printf("store_id = %d\n", store_id);

        //入力された番号が0以上100以下であるか調べる
        if(0 < cnt && cnt < 100){
            //sqlに入力された卓番号が存在するか確認する
            sprintf(sql, "SELECT COUNT(*) FROM store_table_t WHERE desk_num = %d AND store_id = %d", cnt, store_id);
            PGresult *res = PQexec(con, sql);
            tmp = atoi(PQgetvalue(res, 0, 0));
            printf("tmp = %d\n", tmp);
            if(tmp == 0){
                sprintf(sendBuf, "%s %d テーブル番号%dは存在しません%s", ER_STAT, E_CODE_1606, cnt, ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0);   //送信
                printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);
                break;
            }else{
                //入力された卓gasakuzyozuminobaai
                sprintf(sql, "SELECT desk_use FROM store_table_t WHERE desk_num = %d AND store_id = %d", cnt, store_id);
                PGresult *res2 = PQexec(con, sql);
                num = atoi(PQgetvalue(res2, 0, 0));
                if(num == 0){
                    sprintf(sendBuf, "%s %d テーブル番号%dは使用されていません%s", ER_STAT, E_CODE_1607, cnt, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf);  //送信データ長
                    send(soc, sendBuf, sendLen, 0);   //送信
                    printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);
                    break;
                }else{
                sprintf(sql,"SELECT * FROM order_t WHERE desk_num = %d AND store_id = %d;", cnt , store_id); //order_tに注文情報が残っていないか確認する
                res = PQexec(con, sql);
                if (PQresultStatus(res) != PGRES_TUPLES_OK){//クエリが失敗した場合エラーを表示する
                printf("%s",PQresultErrorMessage(res));
                sprintf(sendBuf, "%s %d エラー%s", ER_STAT, E_CODE_1610, ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0);   //送信
                printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
                return -1;
                }
                resultRows = PQntuples(res);
                PQclear(res);

                if(resultRows >= 1){
                    sprintf(sendBuf, "%s %d 注文情報が残っています%s", ER_STAT, E_CODE_1608, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf);  //送信データ長
                    send(soc, sendBuf, sendLen, 0);   //送信
                    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
                    return -1;
                }
                if(resultRows == 0){//注文情報が残っていなかった場合
                    sprintf(sql,"UPDATE store_table_t SET desk_use = 0 WHERE desk_num = %d AND store_id = %d;",cnt ,store_id);//注文情報を確認した時に、履歴が残っていなかった場合卓削除
                    res = PQexec(con, sql);
                    if (PQresultStatus(res) != PGRES_COMMAND_OK){//アップデートできなかった場合エラーを表示する
                        printf("%s",PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d エラー%s%s", ER_STAT, E_CODE_1609, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf);  //送信データ長
                        send(soc, sendBuf, sendLen, 0);   //送信
                        printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
                    }
                    sendLen = sprintf(sendBuf,"%s 1 %d店の%d番の卓情報を削除しました%s", OK_STAT, store_id, cnt, ENTER);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld]SEND=>%s",selfId,sendBuf);
                    PQclear(res);
                    return 0;
                }
                }

            }
        }else{
            sprintf(sendBuf, "%s %d1以上100未満の数字を入力してください%s%s", ER_STAT, E_CODE_1605, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0);   //送信
            printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);
            break;
        }
    }
}    