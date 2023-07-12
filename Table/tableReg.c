#include "omos.h"

int tableReg(PGconn *__con, int __soc, int *__u_info, int *__s_info){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int cnt;
    char sql[BUFSIZE];
    int num;
    int store_id = __userinfo[2];
    int resultRows;

    while (1)
    {
        // ２桁のテーブル番号の登録
        sprintf(sendBuf, "テーブル番号を入力してください%s", ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                   // 送信データ長
        send(__soc, sendBuf, sendLen, 0);                            // 送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);                  // 受信
        recvBuf[recvLen] = '\0';                                     // 受信データにNULLを追加
        cnt = sscanf(recvBuf, "%d", &cnt);                           // 受信データをint型に変換
        // sqlに入力された卓番号が存在するかどうか確認する
        sprintf(sql, "SELECT * FROM table_list WHERE table_id = %d;", cnt);
        PGresult *res = PQexec(__con, sql);

        if (PQntuples(res) == 0)
        {
            if (cnt < 100 && cnt > 0)
            { // 入力された値が２桁の数字だった場合
                sprintf(sql, "SELECT * FROM table WHERE t_num = %d AND t_flag = 0", num);
                PQexec(__con, sql);
                resultRows = PQntuples(res);

                sprintf(sql, "UPDATE table SET t_flag = 1 WHERE t_num = %d;", num);
                PQexec(__con, sql);
                resultRows = PQntuples(res);
                sprintf(sendBuf, "テーブル番号%dを登録しました%s", cnt, ENTER); // 送信データ作成
                sendLen = strlen(sendBuf);                                      // 送信データ長
                send(__soc, sendBuf, sendLen, 0);                               // 送信
            }
            else if (cnt == 0)
            {                                                   // 入力された値が0だった場合
                sprintf(sendBuf, "0は入力できません%s", ENTER); // 送信データ作成
                sendLen = strlen(sendBuf);                      // 送信データ長
                send(__soc, sendBuf, sendLen, 0);               // 送信
            }
            else if (num > 99)
            {                                                             // 入力された値が２桁の数字以外だった場合
                sprintf(sendBuf, "2桁の数字を入力してください%s", ENTER); // 送信データ作成
                sendLen = strlen(sendBuf);                                // 送信データ長
                send(__soc, sendBuf, sendLen, 0);                         // 送信
            }
        }

        else
        {                                                                          // 入力された値がすでに存在する場合
            sprintf(sendBuf, "そのテーブル番号はすでに登録されています%s", ENTER); // 送信データ作成
            sendLen = strlen(sendBuf);                                             // 送信データ長
            send(__soc, sendBuf, sendLen, 0);                                      // 送信
        }
    }
  
    s_info[0] = u_info[2];
    s_info[1] = num;

    return 0;

}