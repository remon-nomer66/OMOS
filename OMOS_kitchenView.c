#include "omos.h"

int kitchenView(PGconn *__con, int __soc, int __menu)
{
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
  int recvLen, sendLen;                    // 送受信データ長
  pthread_t selfId = pthread_self();       // 自スレッドID
  char comm[BUFSIZE];                      // SQLコマンド
  int resultRows;
  int i, cnt;
  char sql[BUFSIZE], buf[BUFSIZE];
  PGresult *res;
  // recipe = recipe_infomation(__soc, menu_id, selfid);

  sprintf(sendBuf, "注文されたメニューのレシピを表示したい場合はRINFO、終了したい場合は\"END\"を押してください。%s%s", ENTER, DATA_END); // 注文されたメニューを表示
  sendLen = strlen(sendBuf);                                                                                                             // 送信データ長
  send(__soc, sendBuf, sendLen, 0);                                                                                                      // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);                                                                                 // 送信データ表示

  recvLen = receive_message(__soc, recvBuf, BUFSIZE); // 受信
  if (recvLen != 0)
    {
      recvBuf[recvLen - 1] = '\0';
      cnt = sscanf(recvBuf, "%s", comm);
      if (cnt == 1)
	{
	  if (strcmp(comm, RINFO) == 0)
	    {
	      printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
	      sprintf(sql, "SELECT menu_id, menu_name, recipe FROM recipe_t WHERE menu_id = %d", __menu);
	      printf("flag\n");
	      res = PQexec(__con, sql);
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
		      sprintf(sendBuf, "商品IDが存在しません %s%s", ENTER, DATA_END);
		    }
		  else
		    {
		      sprintf(sendBuf, "商品ID、商品名、レシピ%s", ENTER); // 注文されたメニューを表示
		      for (i = 0; i < resultRows; i++)
			{
			  sprintf(buf, "%s  %s  %s  %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END);
			  strcat(sendBuf, buf);
			}
		    }
		  sendLen = strlen(sendBuf);                             // 送信データ長
		  send(__soc, sendBuf, sendLen, 0);                      // 送信
		  printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データ表示

		  // 注文されたメニューを表示
		}
	    }
	  else if(strcmp(comm, END) == 0){
	    sendLen = strlen(sendBuf);
	    send(__soc, sendBuf, sendLen, 0);
	    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
	    return 0;
	  }else{
	    sprintf(sendBuf, "RINFOかENDを入力してください%s%s", ENTER, DATA_END);
	    sendLen = strlen(sendBuf);
	    send(__soc, sendBuf, sendLen, 0);
	    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
	  }
	}
    }
}
