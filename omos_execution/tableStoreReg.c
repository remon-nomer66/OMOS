#include "omos.h"

int tableStoreReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;
    int cnt, resultRows;
    char sql[BUFSIZE];
    PGresult *res;
    int tableNum, capacity;
    char comm[BUFSIZE];

    sprintf(sql, "SELECT * FROM store_t WHERE store_id = %d", u_info[2]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows != 1){
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }

    while(1){
        sprintf(sendBuf, "登録する卓番号と人数を半角空白明けで入力してください%s登録を終了する場合は\"END\"と入力してください%s%s", ENTER, ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){   //文字が入力されたら
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
    
            cnt = sscanf(recvBuf, "%d %d", &tableNum, &capacity);

            if(cnt == 2){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(cnt == 2){
                sprintf(sql, "INSERT INTO store_table_t (store_id, desk_num, desk_max, desk_use)VALUES (%d, %d, %d, 0)", u_info[2], tableNum, capacity);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "%s %d%s", OK_STAT, 3, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                sprintf(sendBuf, "%d %d%s", tableNum, capacity, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(cnt == 0){
                cnt = sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, END) == 0){
                    PQclear(res);
                    return 0;
                }
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_200, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }else{
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_200, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }

    
}