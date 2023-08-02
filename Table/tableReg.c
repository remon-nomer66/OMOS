#include "omos.h"
#include "table.h"

int tableReg(PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int cnt = 0;
    char sql[BUFSIZE];
    int num;
    int store_id = s_info[0];
    int resultRows;

    while (1)
    {

        printf("store_id = %d\n", s_info[0]);
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

        printf("store_id = %d\n", store_id);
        printf("cnt = %d\n", cnt);



       //入力された番号が0以上100以下であるかを調べる
        if(0 < cnt && cnt < 100){
            //sqlに入力された卓番号が存在するか確認する
            sprintf(sql, "SELECT COUNT(*) FROM store_table_t WHERE desk_num = %d ", cnt);
            PGresult *res = PQexec(con, sql);
            resultRows = atoi(PQgetvalue(res, 0, 0));
            if(resultRows == 0){
                sprintf(sendBuf, "%s %d テーブル番号%dは存在しません%s", ER_STAT, E_CODE_1604, cnt, ENTER); // 送信データ作成
                sendLen = strlen(sendBuf);                                                // 送信データ長
                send(soc, sendBuf, sendLen, 0);                                         // 送信
                printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
                return -1;
            }else{
                // sqlに入力された卓番号が存在するか確認し、存在した場合、desk_useの値をnumに代入する。
                sprintf(sql, "SELECT desk_use FROM store_table_t WHERE desk_num = %d ", cnt);
                PGresult *res = PQexec(con, sql);
                num = atoi(PQgetvalue(res, 0, 0));

                printf("num = %d\n", num);
                if(num == 0){
                    sprintf(sql, "UPDATE store_table_t SET desk_use = 1 WHERE desk_num = %d;", cnt);
                    PQexec(con, sql);
                    sprintf(sendBuf, "%s 1 テーブル番号%dを登録しました%s", OK_STAT, cnt, ENTER); // 送信データ作成
                    sendLen = strlen(sendBuf);                                                // 送信データ長
                    send(soc, sendBuf, sendLen, 0);                                         // 送信
                    break;
                }else{
                    sprintf(sendBuf, "%s %d そのテーブル番号は既に登録されています%s", ER_STAT, E_CODE_1603, ENTER); // 送信データ作成
                    sendLen = strlen(sendBuf);                                                // 送信データ長
                    send(soc, sendBuf, sendLen, 0);                                         // 送信
                    printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
                    return -1;
                }
            }
        }else{
        sprintf(sendBuf, "%s %d 1以上100未満の数字を入力してください%s%s", ER_STAT, E_CODE_1602, ENTER, DATA_END); // 送信データ作成
        sendLen = strlen(sendBuf);                                                // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                         // 送信
        printf("[C_THREAD %ld] send=%s\n",selfId, sendBuf);                              // 送信データ表示
        return -1;
        }

        printf("cnt: %d\n", cnt);
        
        }
        s_info[0] = u_info[2];
        s_info[1] = cnt;
        return 0;
    }