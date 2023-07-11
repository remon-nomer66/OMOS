#include "omos.h"

int kitchenFlag(PGconn *__con, int __soc, int __tableNum)
{
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
    int recvLen, sendLen;                    // 送受信データ長
    pthread_t selfId = pthread_self();       // 自スレッドID
    char comm[BUFSIZE];                      // SQLコマンド
    int resultRows;
    int i;
    char sql[BUFSIZE], buf[BUFSIZE];
    // recipe = recipe_infomation(__soc, menu_id, selfid);
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
                    printf("%s", PQresultErrorMessage(res));
                    return -1;
                }
            }
        }
    }
}