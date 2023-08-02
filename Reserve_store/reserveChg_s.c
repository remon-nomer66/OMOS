#include "omos.h"
#include "reserve.h"

int reserveChg_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, i, cnt, param;
    int reserve_no[100];
    char reserve_date[100][11];
    char reserve_time[100][6];
    int reserve_desk_num[100];
    char buf[BUFSIZE], comm[BUFSIZE];
    char store_name[BUFSIZE];
    int reg_chg_flag = 1;   //chg: 1
    int tmp = 0;
    char user_phone[12];
    int user_id;

    while(1){
        user_phone[0] = '\0';
        sprintf(sendBuf, "変更するユーザの電話番号を入力ください%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%s", user_phone);
            if(cnt == 1 && strlen(user_phone) == TELLEN){
                user_phone[strlen(user_phone)] = '\0';
                printf("%s\n", user_phone);
                sprintf(sql, "SELECT user_id FROM user_t WHERE user_phone = %s", user_phone);
                printf("sql: %s\n", sql);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
                user_id = atoi(PQgetvalue(res, 0, 0));

                printf("user_id: %d\n", user_id);
        
                //予約変更する対象があるかチェック
                sprintf(sql, "SET search_path to reserve");
                res = PQexec(con, sql);
                sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d AND store_id = %d", user_id, s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);
                    PQclear(res);
                    return -1;
                }
                tmp = resultRows = PQntuples(res);
                if(resultRows <= 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1201, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);
                    PQclear(res);
                    return -1;
                }
                sprintf(sql, "SET search_path to public");
                PQexec(con, sql);

                printf("sql: %s\n", sql);

                //予約変更可能な時
                for(i = 0; i < resultRows; i++){
                    reserve_no[i] = atoi(PQgetvalue(res, i, 0));
                    strncpy(reserve_date[i], PQgetvalue(res, i, 3), 10);
                    reserve_date[i][10] = '\0';
                    strncpy(reserve_time[i], PQgetvalue(res, i, 4), 5);
                    reserve_time[i][5] = '\0';
                    reserve_desk_num[i] = atoi(PQgetvalue(res, i, 6));
                }

                sprintf(sendBuf, "変更する予約番号を入力してください%s予約変更から抜ける場合は\"END\"と入力してください%s予約番号 予約日 予約時間%s", ENTER, ENTER, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                for(i = 0; i < tmp; i++){
                    sprintf(sendBuf, "%d %s %s%s", i + 1, reserve_date[i], reserve_time[i], ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                sprintf(sendBuf, "%s", DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                //変更対象を受け取り
                while(1){
                    recvLen = receive_message(soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){   //文字が入力されたら
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        cnt = sscanf(recvBuf, "%d", &param);

                        //削除対象の確認
                        if(cnt == 1 && 1 <= param){
                            sprintf(sendBuf, "変更する対象は%s%3d %s %s%sでよろしいですか？よろしい場合は\"YES\"と入力してください%sそうでない場合は\"NO\"と入力してください%s予約の変更を取り止める場合は\"END\"と入力してください%s%s", ENTER, param, reserve_date[param - 1], reserve_time[param - 1], ENTER, ENTER, ENTER, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            recvLen = receive_message(soc, recvBuf, BUFSIZE);
                            if(recvLen != 0){
                                cnt = sscanf(recvBuf, "%s", comm);
                                if((cnt == 1) && (strcmp(comm, YES) == 0)){
                                    reserveCheck_s(selfId, con, soc, recvBuf, sendBuf, s_info, reg_chg_flag, reserve_no[param - 1]);
                                    break;
                                }else if(strcmp(comm, END) == 0){
                                    PQclear(res);
                                    return 0;
                                }else{
                                    break;
                                }
                            }
                        }else{
                            cnt = sscanf(recvBuf, "%s", comm);
                            if(strcmp(comm, END) == 0){
                                PQclear(res);
                                return 0;
                            }
                        }
                    }
                }
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_200, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }
    PQclear(res);
    return -1;
}
