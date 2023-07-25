#include "omos.h"

int kitchenFlag(PGconn *__con, int __soc, int __tableNum)
{
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
    int recvLen, sendLen;                    // 送受信データ長
    pthread_t selfId = pthread_self();       // 自スレッドID
    char comm[BUFSIZE];                      // SQLコマンド
    int resultRows;
    int i, flag;
    char sql[BUFSIZE], buf[BUFSIZE];
    while (1)
    {
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); // 受信
        if (recvLen != 0)
        {
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データ表示
            sscanf(recvBuf, "%s", comm);                           // SQLコマンドを取得
            if (strcmp(comm, ENTER) == 0)
            {
                sendBuf[0] = '\0'; // 送信バッファ初期化
                sprintf(sql, "UPDATE order_t SET order_flag=1 WHERE Table_Info,Store_ID,Food_ID");
                res = PQexec(__con, sql);
                if (PQresultStatus(res) != PGRES_TUPLES_OK)
                {
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1504, ENTER, DATA_END); // 送信データ作成
                    sendLen = strlen(sendBuf);                                            // 送信データ長
                    send(__soc, sendBuf, sendLen, 0);                                       // 送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);                             // 受信
                    recvBuf[recvLen - 1] = '\0';                                          // 受信データにNULLを追加;
                    return -1;
                }
            }
        }
    }
}