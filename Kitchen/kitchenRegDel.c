#include "omos.h"
#include "kitchen.h"

int kitchenReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, char *reg){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows;
    int sendLen;

    sprintf(sendBuf, "%s 登録完了%s", OK_STAT, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);

    reg[0] = 1;

    return 0;
}

int kitchenDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info, char *reg){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows;
    int sendLen;

    sprintf(sql, "SELECT * FROM order_t WHERE store_id = %d", s_info[0]);
    res = PQexec(con, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s",PQresultErrorMessage(res));
        sprintf(sendBuf, "%s %d%s", ER_STAT, 100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows != 0){
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1505, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);
        return -1;
    }

    sprintf(sendBuf, "%s 削除完了%s", OK_STAT, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld]SEND=>%s\n",selfId,sendBuf);

    reg[0] = 0;

    return 0;
}