#include "omos.h"
#include "reserve.h"

int reserveDel_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char sql[BUFSIZE];
    PGresult *res;
    char user_phone[12];
    int user_id;
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    int resultRows, i, cnt, param;
    int reserve_no[100];
    char reserve_date[100][11];
    char reserve_time[100][6];
    int reserve_store_id[100];
    int reserve_desk_num[100];
    char buf[BUFSIZE], comm[BUFSIZE];
    int tmp = 0;
    int recvLen, sendLen;

    //当日以前の予約削除
    sprintf(sql, "SET search_path to reserve");
    PQexec(con, sql);
    sprintf(sql, "DELETE FROM reserve_t WHERE reserve_date < '%d-%d-%d' AND store_id = %d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, u_info[2]);
    printf("%s\n", sql);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
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
    sprintf(sql, "SET search_path to public");
    PQexec(con, sql);

    //当該削除本体
    sprintf(sendBuf, "削除するユーザの電話番号を入力してください%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    recvLen = receive_message(soc, recvBuf, BUFSIZE);
    if(recvLen > 0){
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        cnt = sscanf(recvBuf, "%s", user_phone);
        if(cnt == 1){

            sprintf(sql, "SELECT user_id FROM user_t WHERE user_phone = %s", user_phone);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            user_id = atoi(PQgetvalue(res, 0, 0));

            printf("%d\n", user_id);

            //予約削除する対象があるかチェック
            sprintf(sql, "SET search_path to reserve");
            PQexec(con, sql);
            sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d AND store_id = %d", user_id, u_info[2]);
            printf("%s\n", sql);
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
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1219, ENTER, DATA_END);
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

            //予約削除可能な時
            for(i = 0; i < resultRows; i++){
                reserve_no[i] = atoi(PQgetvalue(res, i, 0));
                strncpy(reserve_date[i], PQgetvalue(res, i, 3), 10);
                reserve_date[i][10] = '\0';
                strncpy(reserve_time[i], PQgetvalue(res, i, 4), 5);
                reserve_time[i][5] = '\0';
                reserve_desk_num[i] = atoi(PQgetvalue(res, i, 6));
            }

            sprintf(sql, "SET search_path to public");
            PQexec(con, sql);
            
            sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", u_info[2]);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }

            sprintf(sendBuf, "削除する予約番号を入力してください%s予約削除から抜ける場合は\"END\"と入力してください%s予約番号 予約日 予約時間%s", ENTER, ENTER, ENTER);
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
            //削除対象を受け取り
            while(1){
                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                if(recvLen > 0){   //文字が入力されたら
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    cnt = sscanf(recvBuf, "%d", &param);

                    //削除対象の確認
                    if(cnt == 1 && 1 <= param){
                        sprintf(sendBuf, "削除する対象は%s%3d %s %s%sでよろしいですか？よろしい場合は\"YES\"と入力してください%s%s", ENTER, param, reserve_date[param - 1], reserve_time[param - 1], ENTER, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        recvLen = receive_message(soc, recvBuf, BUFSIZE);
                        if(recvLen > 0){
                            cnt = sscanf(recvBuf, "%s", comm);
                            if((cnt == 1) && (strcmp(comm, YES) == 0)){   //実際に削除
                                sprintf(sql, "SET search_path to reserve");
                                PQexec(con, sql);
                                printf("%d", reserve_no[param-1]);
                                sprintf(sql, "DELETE FROM reserve_t WHERE reserve_no = %d", reserve_no[param - 1]);
                                res = PQexec(con, sql);
                                if(PQresultStatus(res) != PGRES_COMMAND_OK){
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
                                sprintf(sql, "SET search_path to public");
                                PQexec(con, sql);
                    
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
        }
    }
}