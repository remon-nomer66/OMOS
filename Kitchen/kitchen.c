#include "omos.h"
#include "kitchen.h"

int kitchen(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info, int *u_info, char *reg)
{
  int recvLen, sendLen;                    // 送受信データ長
  char comm[BUFSIZE];                      // SQLコマンド
  int resultRows;
  int i, cnt;
  char sql[BUFSIZE], buf[BUFSIZE];
  PGresult *res;

  while (1)
  {
    sprintf(sendBuf, "注文されたメニューを表示したい場合は\"KINFO\"、フラグを立てる場合は\"KFLAG\"、レシピの表示をする場合は\"RINFO\"終了したい場合は\"KDEL\"を押してください。%s%s", ENTER, DATA_END); // 注文されたメニューを表示
    sendLen = strlen(sendBuf);                                                                                                     // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                                                                              // 送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);                                                                         // 送信データ表示

    recvLen = receive_message(soc, recvBuf, BUFSIZE); // 受信
    if (recvLen != 0)
    {
      recvBuf[recvLen - 1] = '\0';
      cnt = sscanf(recvBuf, "%s", comm);
      if (cnt == 1)
      {
        if (strcmp(comm, KINFO) == 0)
        {
          printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
          sprintf(sql, "SELECT menu_id, order_cnt, order_date, order_time FROM order_t WHERE store_id = %d", u_info[2]);
          printf("flag\n");
          res = PQexec(con, sql);
          if (PQresultStatus(res) != PGRES_TUPLES_OK)
          {
            printf("%s", PQresultErrorMessage(res));
            return -1;
          }
          else
          {
            resultRows = PQntuples(res);
            if (resultRows == 0)
            {
              sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1501, ENTER); // 注文されたメニューを表示
            }
            else
            {
              sprintf(sendBuf, "商品ID、注文数、注文状況、注文時間%s", ENTER); // 注文されたメニューを表示
              for (i = 0; i < resultRows; i++)
              {
                sprintf(buf, "%s %s %s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), ENTER);
                strcat(sendBuf, buf);
              }
            }
            sendLen = strlen(sendBuf);                             // 送信データ長
            send(soc, sendBuf, sendLen, 0);                      // 送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データ表示

            // 注文されたメニューを表示
          }
        }

        else if(strcmp(comm, KFLAG) == 0){
          kitchenFlag(selfId, con, soc, recvBuf, sendBuf, u_info);
        }

        else if(strcmp(comm, RINFO) == 0){
          kitchenView(selfId, con, soc, recvBuf, sendBuf, u_info);
        }

        else if (strcmp(comm, KDEL) == 0)
        {
          sprintf(sql, "SELECT * FROM order_t WHERE kitchen_flag = 0");
          res = PQexec(con, sql);
          if (PQresultStatus(res) != PGRES_TUPLES_OK)
          {
            printf("%s", PQresultErrorMessage(res));
            return -1;
          }
          else
          {
            // キッチンに残っているメニューがある場合,削除できない
            resultRows = PQntuples(res);
            if (resultRows >= 1)
            {
              sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1502, ENTER);
              sendLen = strlen(sendBuf);                             // 送信データ長
              send(soc, sendBuf, sendLen, 0);                      // 送信
              printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データ表示
            }
            else
            {
              sendLen = sprintf(sendBuf, "%s %d%s",OK_STAT, resultRows, ENTER);
              send(soc, sendBuf, sendLen, 0);                      // 送信
              printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データ表示
              return 0;
            }
          }
          return 0;
        }
      }
    }
  }
}