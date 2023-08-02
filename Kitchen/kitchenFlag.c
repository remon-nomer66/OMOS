#include "omos.h"
#include "kitchen.h"

int kitchenFlag(pthread_t selfId, PGconn *con, int soc, char *sendBuf, char *recvBuf, int *u_info){
    char sql[BUFSIZE];
    PGresult *res;
    int tmp, resultRows, i, param, cnt;
    int menu_id[100];
    char menu_name[100][30];
    int desk_num[100];
    int order_cnt[100];
    char order_time[100][9];
    char comm[BUFSIZE];
    int recvLen, sendLen;

    printf("u[2]: %d\n", u_info[2]);

    while(1){
        sprintf(sql, "SELECT * FROM order_t WHERE store_id = %d AND kitchen_flag = 0", u_info[2]);
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
        tmp = resultRows = PQntuples(res);
        printf("tmp: %d\n", tmp);
        if(resultRows < 0){
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            PQclear(res);
            return -1;
        }
        for(i = 0; i < tmp; i++){
            menu_id[i] = atoi(PQgetvalue(res, i, 2));
            desk_num[i] = atoi(PQgetvalue(res, i, 1));
            order_cnt[i] = atoi(PQgetvalue(res, i, 3));
            strncpy(order_time[i], PQgetvalue(res, i, 6), 8);
            printf("id: %d\n", menu_id[i]);
            printf("desk: %d\n", desk_num[i]);
            printf("cnt: %d", order_cnt[i]);
            printf("time: %s\n", order_time[i]);
        }

        for(i = 0; i < tmp; i++){
            sprintf(sql, "SELECT menu_name FROM recipe_t WHERE menu_id = %d", menu_id[i]);
            res = PQexec(con, sql);
            printf("%s\n", sql);
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
            if(resultRows < 0){
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            strcpy(menu_name[i], PQgetvalue(res, 0, 0));
        }

        sprintf(sendBuf, "現在の注文状況%s終了する際は\"END\"と入力してください%s番号　商品名　卓番号　個数　注文時間%s", ENTER, ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        for(i = 0; i < tmp; i++){
            sprintf(sendBuf, "%d %s %d %d %s%s", i + 1, menu_name[i], desk_num[i], order_cnt[i], order_time[i], ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }

        sprintf(sendBuf, "%s", DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%d", &param);
            printf("param: %d\n", param);
            if(cnt == 1 && param <= tmp){
                sprintf(sql, "UPDATE order_t SET kitchen_flag = 1 WHERE store_id = %d AND desk_num = %d AND menu_id = %d AND order_time = '%s'", u_info[2], desk_num[param - 1], menu_id[param - 1], order_time[param - 1]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "%s %s% d%s", OK_STAT, menu_name[param - 1], order_cnt[param - 1], ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(cnt == 1){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1504, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(cnt == 0){
                cnt = sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, END) == 0){
                    break;
                }
            }
        }
    }
    PQclear(res);
    return 0;  
}