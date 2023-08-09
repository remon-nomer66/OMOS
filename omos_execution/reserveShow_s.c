#include "omos.h"

int reserveShow_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp, i, cnt;
    char comm[BUFSIZE];
    int user_id[100];
    char user_phone[100][11];
    char reserve_time[100][6];
    int people_num[100];
    int desk_num[100];

    time_t t = time(NULL);
    struct tm *local = localtime(&t);

    while(1){
        sprintf(sendBuf, "確認を終了する場合は\"END\"と入力してください%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%s", comm);
            if(cnt == 1 && strcmp(comm, END) == 0){
                return 0;
            }
        }

        sprintf(sql, "SET search_path to reserve");
        PQexec(con, sql);
        sprintf(sql, "SELECT user_id, reserve_time, people_num, desk_num FROM reserve_t WHERE store_id = %d AND reserve_date = '%d-%d-%d'", u_info[2], local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
        res = PQexec(con, sql);
        printf("%s", sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            sprintf(sql, "SET search_path to public");
            PQexec(con, sql);
            PQclear(res);
            return -1;
        }
        tmp = resultRows = PQntuples(res);
        if(resultRows == 0){
            sprintf(sendBuf, "%s 予約なし%s", OK_STAT, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            sprintf(sql, "SET search_path to public");
            PQexec(con, sql);
            PQclear(res);
            return 0;

        }

        for(i = 0; i < tmp; i++){
            user_id[i] = atoi(PQgetvalue(res, i, 0));
            printf("%d", user_id[i]);
            strncpy(reserve_time[i], PQgetvalue(res, i, 1), 5);
            people_num[i] = atoi(PQgetvalue(res, i, 2));
            desk_num[i] = atoi(PQgetvalue(res, i, 3));
        }

        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);

        for(i = 0; i < tmp; i++){
            sprintf(sql, "SELECT user_phone FROM user_t WHERE user_id = %d", user_id[i]);
            res = PQexec(con, sql);
            printf("%s", sql);
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
            if(resultRows == 0){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                PQclear(res);
                return -1;
            }
            strncpy(user_phone[i], PQgetvalue(res, 0, 0), 10);
        }
        
        sprintf(sendBuf, "%s %d%s", OK_STAT, tmp + 2, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        sprintf(sendBuf, "予約一覧%s　電話番号　時刻　人数　卓番号%s", ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        for(i = 0; i < tmp; i++){
            sprintf(sendBuf, "%s %s %d %d%s", user_phone[i], reserve_time[i], people_num[i], desk_num[i], ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }

        sprintf(sendBuf, "%s", DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }
    

}