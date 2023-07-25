#include "omos.h"

int tableReg(PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int cnt = 0;
    char sql[BUFSIZE];
    int num;
    int store_id = u_info[2];
    int resultRows;

    while (1)
    {
        // ２桁のテーブル番号の登録
        sprintf(sendBuf, "テーブル番号を入力してください%s%s", ENTER, DATA_END); // 送信データ作成
        sendLen = strlen(sendBuf);                                                // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                         // 送信
        printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
        //cntに入力されたテーブル番号を代入
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);                              // 受信
        recvBuf[recvLen-1] = '\0';                                                  // 文字列化
        printf("[C_THREAD %ld] recv=%s\n", selfId, recvBuf);                             // 受信データ表示
        cnt = atoi(recvBuf);


       //入力された番号が0以上100以下であるかを調べる
        if(0 < cnt && cnt < 100){
            //sqlに入力された卓番号が存在するか確認する
            sprintf(sql, "SELECT COUNT(*) FROM store_table_t WHERE desk_num = %d ", cnt);
            PGresult *res = PQexec(con, sql);
            resultRows = atoi(PQgetvalue(res, 0, 0));
            if(resultRows == 0){
                sprintf(sendBuf, "テーブル番号%dは存在しません%s%s", cnt, ENTER, DATA_END); // 送信データ作成
                sendLen = strlen(sendBuf);                                                // 送信データ長
                send(soc, sendBuf, sendLen, 0);                                         // 送信
                printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
                break;
            }else{
                // sqlに入力された卓番号が存在するか確認し、存在した場合、desk_useの値をnumに代入する。
                sprintf(sql, "SELECT desk_use FROM store_table_t WHERE desk_num = %d ", cnt);
                PGresult *res = PQexec(con, sql);
                num = atoi(PQgetvalue(res, 0, 0));
                if(num == 0){
                    sprintf(sql, "UPDATE store_table_t SET desk_use = 1 WHERE desk_num = %d;", cnt);
                    PQexec(con, sql);
                    sprintf(sendBuf, "テーブル番号%dを登録しました%s", cnt, ENTER); // 送信データ作成
                    sendLen = strlen(sendBuf);                                                // 送信データ長
                    send(soc, sendBuf, sendLen, 0);                                         // 送信
                }else{
                    sprintf(sendBuf, "そのテーブル番号は既に登録されています%s", ENTER); // 送信データ作成
                    sendLen = strlen(sendBuf);                                                // 送信データ長
                    send(soc, sendBuf, sendLen, 0);                                         // 送信
                    printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
                }
            }
        }else{
            sprintf(sendBuf, "1以上100未満の数字を入力してください%s%s", ENTER, DATA_END); // 送信データ作成
            sendLen = strlen(sendBuf);                                                // 送信データ長
            send(soc, sendBuf, sendLen, 0);                                         // 送信
            printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
        break;
        }
                            
        s_info[0] = u_info[2];
        s_info[1] = cnt;
        return 0;
        
    }
    return 0;
}
