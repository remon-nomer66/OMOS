#include "omos.h"
#include "test.h"
#include "correct.h"

int correct(pthread_t selfId, PGconn *con, int soc, int *u_info)
{
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
  char sql[LONG_BUFSIZE];
  int recvLen, sendLen; // 送受信データ長
  char start[13];
  char end[13];
  char start_target[9];
  char end_target[9];
  char store_id[4];
  char region_id[3];
  char product_id[5];
  char chain_id[3];
  int sum = 0;
  int items_sum = 0;
  PGresult *res;
  int tmp, recvBuf_tmp;
  int auth = u_info[1];
  int cnt, date;
  int dateStr;

  while(1){
    // 期間の指定
    sprintf(sendBuf, "開始年月日（例：20230627）%s%s", ENTER, DATA_END); // 送信データ作成
    sendLen = strlen(sendBuf);                                           // 送信データ長
    send(soc, sendBuf, sendLen, 0);        
    printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
    recvLen = receive_message(soc, recvBuf, BUFSIZE); // 受信
    if (recvLen > 0)
    {
      recvBuf[recvLen - 1] = '\0';
      // 受信データにNULLを追加

      dateStr = atoi(recvBuf);
      printf("%d\n", dateStr);

      // recvBufに数字８桁含まれていた場合
      if (20220101 <= dateStr && dateStr <= 21001231)
      {
        // 取得した開始年月日を格納
        // char start[13];
        strncpy(start, recvBuf, 4);
        start[4] = '-';
        strncpy(start + 5, recvBuf + 4, 2);
        start[7] = '-';
        strncpy(start + 8, recvBuf + 6, 2);
        start[10] = '\0';
        break;
      }

      // recvBufに数字以外が含まれていた場合
      else
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2101, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                         // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                    // 送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf)); 
      }
    }
    
  }
  while(1){
    // 期間の指定
    sprintf(sendBuf, "終了年月日（例：20230627）%s%s", ENTER, DATA_END); // 送信データ作成
    sendLen = strlen(sendBuf);                                           // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                      // 送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
    recvLen = recv(soc, recvBuf, BUFSIZE, 0);                            // 受信
    recvBuf[recvLen-1] = '\0';                                             // 受信データにNULLを追加

    dateStr = atoi(recvBuf);

    // recvBufに数字８桁含まれていた場合
    if (20220101 <= dateStr && dateStr <= 21001231)
    {
      // 取得した終了年月日を格納
      // char end[13];
      date = dateStr;
      strncpy(end, recvBuf, 4);
      end[4] = '-';
      strncpy(end + 5, recvBuf + 4, 2);
      end[7] = '-';
      strncpy(end + 8, recvBuf + 6, 2);
      end[10] = '\0';
      break;
    }
    // recvBufに数字以外が含まれていた場合
    else
    {
      sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2102, ENTER); // 送信データ作成
      sendLen = strlen(sendBuf);                                         // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                    // 送信
      //recvBufの中身を空にする
      memset(recvBuf, '\0', sizeof(recvBuf));
    }
  }

  if(strcmp(start, end) > 0){
    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2103, ENTER); // 送信データ作成
    sendLen = strlen(sendBuf);                                                         // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                                    // 送信
    return -1;
  }

  // 時間指定するかどうか
  sprintf(sendBuf, "時間指定しますか？（はい：1、いいえ：0）%s%s", ENTER, DATA_END); // 送信データ作成
  sendLen = strlen(sendBuf);                                                         // 送信データ長
  send(soc, sendBuf, sendLen, 0);                                                    // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
  recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                          // 受信
  recvBuf[recvLen-1] = '\0';                                                           // 受信データにNULLを追加

  tmp = atoi(recvBuf);

  // 時間指定する場合
  if (tmp == 1)
  {
    // 時間指定
    while (1)
    {
      sprintf(sendBuf, "対象の開始時間を指定してください（例：2035）。%s%s", ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                                               // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                                          // 送信
      printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
      recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                                // 受信
      recvBuf[recvLen-1] = '\0';                                                                 // 受信データにNULLを追加
      recvBuf_tmp = atoi(recvBuf);

      // recvBufが４文字ではない場合
      if (strlen(recvBuf) != 4)
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2104, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                         // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                    // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
      // recvBufの値が0000から2359の範囲の場合
      else if (recvBuf_tmp >= 0000 && recvBuf_tmp <= 2359)
      {
        // 取得した対象の時間を格納
        // char start_target[6];
        strncpy(start_target, recvBuf, 2);
        start_target[2] = ':';
        strncpy(start_target + 3, recvBuf + 2, 2);
        start_target[5] = ':';
        start_target[6] = '0';
        start_target[7] = '0';
        start_target[8] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        break;
      }

      // recvBufに無効な範囲の数字以外が含まれていた場合
      else
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2105, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                         // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                    // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
    }

    while (1)
    {
      sprintf(sendBuf, "対象の終了時間を指定してください（例：2035）。%s%s", ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                                               // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                                          // 送信
      printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
      recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                                // 受信
      recvBuf[recvLen-1] = '\0';                                                                 // 受信データにNULLを追加
      recvBuf_tmp = atoi(recvBuf);

      // recvBufが４文字ではない場合
      if (strlen(recvBuf) != 4)
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2106, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                         // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                    // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
      // recvBufの値が0000から2359の範囲の場合
      else if (recvBuf_tmp >= 0000 && recvBuf_tmp <= 2359)
      {
        // 取得した対象の時間を格納
        // char end_target[6];
        strncpy(end_target, recvBuf, 2);
        end_target[2] = ':';
        strncpy(end_target + 3, recvBuf + 2, 2);
        end_target[5] = ':';
        end_target[6] = '0';
        end_target[7] = '0';
        end_target[8] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        break;
      }

      // recvBufに無効な範囲の数字以外が含まれていた場合
      else
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2107, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                         // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                    // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
    }
  }

  // 時間指定しない場合
  else
  {
    // 対象の時間をNULLにする
    start_target[0] = '\0';
    end_target[0] = '\0';
  }

  //開始年月日と終了年月日が同じ時で対象終了時間が対象開始時間よりも早かった場合、関数から出ていく
  if(strcmp(start, end) == 0 && strcmp(start_target, end_target) > 0){
    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2108, ENTER); // 送信データ作成
    sendLen = strlen(sendBuf);                                                         // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                                    // 送信
    return -1;
  }

  if(u_info[1] == 2 || u_info[1] == 3){
    // 地域を指定するかどうか
    sprintf(sendBuf, "地域指定しますか？（はい：1、いいえ：0）%s%s", ENTER, DATA_END); // 送信データ作成
    sendLen = strlen(sendBuf);                                                         // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                                    // 送信
    recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                          // 受信
    recvBuf[recvLen] = '\0';                                                           // 受信データにNULLを追加

    tmp = atoi(recvBuf);

    // 地域指定する場合
    if (tmp == 1)
    {
      while(1){
        // 地域の指定
        // １桁地域を入力させる
        sprintf(sendBuf, "地域番号を入力してください（例：10）%s%s", ENTER, DATA_END); // 送信データ作成
        sendLen = strlen(sendBuf);                                                     // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                                // 送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                      // 受信
        recvBuf[recvLen-1] = '\0';                                                       // 受信データにNULLを追加

        int area_num = atoi(recvBuf);
        printf("%d\n", area_num);

        // sqlに入力された地域番号が存在するかどうか確認する
        char sql[BUFSIZE];
        sprintf(sql, "SELECT * FROM region_t WHERE region_id = %d", area_num);
        res = PQexec(con, sql);
        int resultRows = PQntuples(res);
        //printf("rr: %d\n", resultRows);

        if (resultRows != 0)
        {
          // 入力された地域番号を格納
          // char region_id[3];
          strncpy(region_id, recvBuf, 1);
          region_id[2] = '\0';
          break;
        }
        // 入力された地域番号が存在しない場合
        else
        {
          sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2109, ENTER); // 送信データ作成
          sendLen = strlen(sendBuf);                                       // 送信データ長
          send(soc, sendBuf, sendLen, 0);                                  // 送信
          //recvBufの中身を空にする
          memset(recvBuf, '\0', sizeof(recvBuf));
        }
      }
      }

    // 地域指定しない場合
    else
    {
      // 地域番号をNULLにする
      region_id[0] = '\0';
    }
  }

  if(u_info[1] == 4){
    //チェーン指定するかどうか
    sprintf(sendBuf, "チェーン指定しますか？（はい：1、いいえ：0）%s%s", ENTER, DATA_END); // 送信データ作成
    sendLen = strlen(sendBuf);                                                         // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                                    // 送信
    recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                          // 受信
    recvBuf[recvLen] = '\0';                                                           // 受信データにNULLを追加

    tmp = atoi(recvBuf);

    // チェーン指定する場合
    if (tmp == 1)
    {
      while(1){
        // チェーンの指定
        // １桁チェーンを入力させる
        sprintf(sendBuf, "チェーン番号を入力してください（例：60*60~69）%s%s", ENTER, DATA_END); // 送信データ作成
        sendLen = strlen(sendBuf);                                                     // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                                // 送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                      // 受信
        recvBuf[recvLen-1] = '\0';                                                       // 受信データにNULLを追加

        int chain_num = atoi(recvBuf);
        printf("%d\n", chain_num);

        // sqlに入力されたチェーン番号が存在するかどうか確認する
        char sql[BUFSIZE];
        sprintf(sql, "SELECT * FROM chain_t WHERE chain_id = %d", chain_num);
        res = PQexec(con, sql);
        int resultRows = PQntuples(res);

        if (resultRows != 0)
        {
          // 入力されたチェーン番号を格納
          // char chain_id[3];
          strncpy(chain_id, recvBuf, 1);
          chain_id[2] = '\0';
          break;
        }
        // 入力されたチェーン番号が存在しない場合
        else
        {
          sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2110, ENTER); // 送信データ作成
          sendLen = strlen(sendBuf);                                       // 送信データ長
          send(soc, sendBuf, sendLen, 0);                                  // 送信
          //recvBufの中身を空にする
          memset(recvBuf, '\0', sizeof(recvBuf));
        }
      }
    }
    else {
      // チェーン番号をNULLにする
      chain_id[0] = '\0';
    }
  }

  // 店舗指定するかどうか
  sprintf(sendBuf, "店舗指定しますか？（はい：1、いいえ：0）%s%s", ENTER, DATA_END); // 送信データ作成
  sendLen = strlen(sendBuf);                                                                                                       // 送信データ長
  send(soc, sendBuf, sendLen, 0);                                                                                                  // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
  recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                                                                        // 受信
  recvBuf[recvLen-1] = '\0';                                                                                                         // 受信データにNULLを追加

  tmp = atoi(recvBuf);

  // 店舗指定する場合
  if (tmp == 1)
  {
    while(1){
      // 店舗の指定
      // ３桁店舗を入力させる
      sprintf(sendBuf, "店舗番号を入力してください（例：101＊100以降の入力のみ）%s%s", ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                                     // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                                // 送信
      printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
      recvLen = recv(soc, recvBuf, BUFSIZE, 0);                                      // 受信
      recvBuf[recvLen-1] = '\0';                 // 受信データにNULLを追加
      printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

      // recvBufのもじれつを数字に変換
      int store_num = atoi(recvBuf);

      // sqlに入力された店舗番号が存在するかどうか,sotre_tテーブルから取得する
      char sql[BUFSIZE];
      sprintf(sql, "SELECT * FROM summary_t WHERE store_id = %d", store_num);
      res = PQexec(con, sql);
      int resultRows = PQntuples(res);
      //printf("rr: %d\n", resultRows);
      // PGresult *res = PQexec(con, sql);
      if (resultRows != 0)
      {
        // 入力された店舗番号を格納
        // char store_id[3];
        strncpy(store_id, recvBuf, 3);
        store_id[3] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        break;
      }
      // 入力された店舗番号が存在しない場合
      else
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2111, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                                   // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                              // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
    }
    
  }
  // 店舗指定しない場合
  else
  {
    // 店舗番号をNULLにする
    store_id[0] = '\0';
  }

  // 商品指定するかどうか
  sprintf(sendBuf, "商品指定しますか？（はい：1、いいえ：0）%s%s", ENTER, DATA_END); // 送信データ作成
  sendLen = strlen(sendBuf);                                             // 送信データ長
  send(soc, sendBuf, sendLen, 0);                                        // 送信
  recvLen = recv(soc, recvBuf, BUFSIZE, 0);                              // 受信
  recvBuf[recvLen] = '\0';                                               // 受信データにNULLを追加

  tmp = atoi(recvBuf);

  // 商品指定する場合
  if (tmp == 1)
  {
    while(1){
      // 商品番号の指定
      // ４桁商品番号を入力させる
      sprintf(sendBuf, "商品番号を入力してください（1~9999までの値）%s%s", ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                           // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                      // 送信
      recvLen = recv(soc, recvBuf, BUFSIZE, 0);                            // 受信
      recvBuf[recvLen] = '\0';                                             // 受信データにNULLを追加

      int product_num = atoi(recvBuf);

      // sqlに入力された商品番号が存在するかどうか確認する
      char sql[BUFSIZE];
      sprintf(sql, "SELECT * FROM summary_t WHERE menu_id = %d", product_num);
      res = PQexec(con, sql);
        int resultRows = PQntuples(res);
        //printf("rr: %d\n", resultRows);

      // PGresult *res = PQexec(con, sql);
      if (resultRows != 0)
      {
        // 入力された商品番号を格納
        // char product_id[5];
        strncpy(product_id, recvBuf, 4);
        product_id[4] = '\0';
        break;
      }
      // 入力された商品番号が存在しない場合
      else
      {
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2112, ENTER); // 送信データ作成
        sendLen = strlen(sendBuf);                                       // 送信データ長
        send(soc, sendBuf, sendLen, 0);                                  // 送信
        //recvBufの中身を空にする
        memset(recvBuf, '\0', sizeof(recvBuf));
      }
    }
    
  }

  // 商品指定しない場合
  else
  {
    // 商品番号をNULLにする
    product_id[0] = '\0';
  }

  // 入力された値を一度に全て表示させる
  sprintf(sendBuf, "入力された値を表示します。%s", ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                               // 送信データ長
  send(soc, sendBuf, sendLen, 0);                          // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  // 入力された値（開始年月日時刻、終了年月日時刻）を一度に表示
  sprintf(sendBuf, "開始年月日時刻：%s %s%s", start, start_target, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                                      // 送信データ長
  send(soc, sendBuf, sendLen, 0);                                 // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  sprintf(sendBuf, "終了年月日時刻：%s %s%s", end, end_target, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                                  // 送信データ長
  send(soc, sendBuf, sendLen, 0);                             // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  sprintf(sendBuf, "地域番号：%s%s", region_id, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                 // 送信データ長
  send(soc, sendBuf, sendLen, 0);            // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  sprintf(sendBuf, "チェーン番号：%s%s", chain_id, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                 // 送信データ長
  send(soc, sendBuf, sendLen, 0);            // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  // 入力された値（店舗番号、地域番号、商品番号）を一度に表示
  sprintf(sendBuf, "店舗番号：%s%s", store_id, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                  // 送信データ長
  send(soc, sendBuf, sendLen, 0);             // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  sprintf(sendBuf, "商品番号：%s%s", product_id, ENTER); // 送信データ作成
  sendLen = strlen(sendBuf);                    // 送信データ長
  send(soc, sendBuf, sendLen, 0);               // 送信
  printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信

  char searchArray[BUFSIZE];

  // 時間指定、地域指定、店舗指定、商品指定がない場合
  if (strlen(start_target) == 0 && strlen(end_target) == 0 && strlen(region_id) == 0 && strlen(store_id) == 0 && strlen(product_id) == 0 && strlen(chain_id) == 0){
    searchArray[0] = '\0';
  }

  if (strlen(start_target) != 0 && strlen(end_target) != 0){
    sprintf(searchArray, "%s AND CAST(summary.order_time AS time) BETWEEN '%s' AND '%s'", searchArray, start_target, end_target);
  }

  if (strlen(region_id) != 0){
    sprintf(searchArray + strlen(searchArray), "%s AND region.region_id = %s", searchArray, region_id);
  }

  if (strlen(chain_id) != 0)
  {
    sprintf(searchArray + strlen(searchArray), "%s AND chain.chain_id = %s", searchArray, chain_id);
  }

  if (strlen(store_id) != 0){
    sprintf(searchArray + strlen(searchArray), "%s AND summary.store_id = %s", searchArray, store_id);
  }

  if (strlen(product_id) != 0){
    sprintf(searchArray + strlen(searchArray), "%s AND summary.menu_id = %s", searchArray, product_id);
  }

  //データベース結合
  sprintf(sql, "SELECT recipe.menu_name, SUM(summary.order_cnt * menu.price) AS item_calc FROM summary_t AS summary INNER JOIN menu_price_t AS menu ON summary.menu_id = menu.menu_id INNER JOIN region_t AS region ON summary.store_id = region.store_id INNER JOIN chain_t AS chain ON summary.store_id = chain.store_id INNER JOIN recipe_t AS recipe ON summary.menu_id = recipe.menu_id WHERE summary.order_date BETWEEN '%s' AND '%s' %s GROUP BY recipe.menu_name ORDER BY item_calc DESC", start, end, searchArray);
  printf("%s\n", sql);
  res = PQexec(con, sql);
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
      sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_2113, ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                       // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                  // 送信
      printf("No data retrieved\n");
      printf("%s\n", PQerrorMessage(con));
      PQclear(res);
      return -1;
  }

  //結合したデータベースの表示
  // 結果の取得と表示
  cnt = PQntuples(res);
  for (int i = 0; i < cnt; i++)
  {
    sprintf(sendBuf, "%s %s%s",PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); // 送信データ作成
    sendLen = strlen(sendBuf);                                        // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                   // 送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
  }

  //商品番号の指定がなかった場合
  if(strlen(product_id) == 0){
    sprintf(sql, "SELECT SUM(summary.order_cnt * menu.price) AS total_calc FROM summary_t AS summary INNER JOIN menu_price_t AS menu ON summary.menu_id = menu.menu_id INNER JOIN region_t AS region ON summary.store_id = region.store_id INNER JOIN chain_t AS chain ON summary.store_id = chain.store_id WHERE summary.order_date BETWEEN '%s' AND '%s' %s", start, end, searchArray);
    printf("%s\n", sql);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
      sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_2113, ENTER, DATA_END); // 送信データ作成
      sendLen = strlen(sendBuf);                                       // 送信データ長
      send(soc, sendBuf, sendLen, 0);                                  // 送信
      printf("No data retrieved\n");
      printf("%s\n", PQerrorMessage(con));
      PQclear(res);
      return -1;
    }
  }

  //結合したデータベースの表示
  cnt = PQntuples(res);
  for (int i = 0; i < cnt; i++)
  {
    sprintf(sendBuf, "%s 合計金額：%s%s", OK_STAT, PQgetvalue(res, i, 0), ENTER); // 送信データ作成
    sendLen = strlen(sendBuf);                                        // 送信データ長
    send(soc, sendBuf, sendLen, 0);                                   // 送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId,sendBuf);                              // 送信
  }

  return 0;
}
