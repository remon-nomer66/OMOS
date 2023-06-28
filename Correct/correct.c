<<<<<<< Updated upstream
#include "omos.h"

int correct(PGconn *__con, int __soc, int __auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    
    while(1){
        //期間の指定
        sprintf(sendBuf, "開始年月日（例：20230627）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //recvBufに数字８桁含まれていた場合
        if(!isdigit(recvBuf)){
            //取得した開始年月日を格納
            char start[13];
            strncpy(start, recvBuf, 4);
            start[4] = '-';
            strncpy(start+5, recvBuf+4, 2);
            start[7] = '-';
            strncpy(start+8, recvBuf+6, 2);
            start[10] = '\0';
            break;
        }

        //recvBufに数字以外が含まれていた場合
        else{
            sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
        //期間の指定
        sprintf(sendBuf, "終了年月日（例：20230627）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //recvBufに数字８桁含まれていた場合
        if(!isdigit(recvBuf)){
            //取得した終了年月日を格納
            char end[13];
            strncpy(end, recvBuf, 4);
            end[4] = '-';
            strncpy(end+5, recvBuf+4, 2);
            end[7] = '-';
            strncpy(end+8, recvBuf+6, 2);
            end[10] = '\0';
            break;
        } else if(strcmp(start, end) > 0){
            sprintf(sendBuf, "開始年月日が終了年月日よりも大きいです。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
        //recvBufに数字以外が含まれていた場合
        else{
            sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }

    }


    //時間指定するかどうか
    sprintf(sendBuf, "時間指定しますか？（はい：1、いいえ：0）%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
    recvBuf[recvLen] = '\0';    //受信データにNULLを追加

    //時間指定する場合
    if(recvBuf == 1){
        //時間指定
        while(1){
            sprintf(sendBuf, "対象の開始時間を指定してください（例：2035）。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
            recvBuf[recvLen] = '\0';    //受信データにNULLを追加
            
            //recvBufの値が0000から2359の範囲の場合
            if(recvBuf >= 0000 && recvBuf <= 2359){
                //取得した対象の時間を格納
                char start_target[6];
                strncpy(start_target, recvBuf, 2);
                start_target[2] = ':';
                strncpy(start_target+3, recvBuf+2, 2);
                start_target[5] = '\0';
                break;
            }

            //recvBufに無効な範囲の数字以外が含まれていた場合
            else{
                sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__soc, sendBuf, sendLen, 0);   //送信
            }
        }

        while(1){
            sprintf(sendBuf, "対象の終了時間を指定してください（例：2035）。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
            recvBuf[recvLen] = '\0';    //受信データにNULLを追加
            
            //recvBufの値が0000から2359の範囲の場合
            if(recvBuf >= 0000 && recvBuf <= 2359){
                //取得した対象の時間を格納
                char end_target[6];
                strncpy(end_target, recvBuf, 2);
                end_target[2] = ':';
                strncpy(end_target+3, recvBuf+2, 2);
                end_target[5] = '\0';
                break;
            }

            //recvBufに無効な範囲の数字以外が含まれていた場合
            else{
                sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__soc, sendBuf, sendLen, 0);   //送信
            }
        }  
    }

    //時間指定しない場合
    else{
        //対象の時間をNULLにする
        start_target[6] = NULL;
    }

    //店舗指定するかどうか
    sprintf(sendBuf, "店舗指定しますか？地域指定をしたい場合はいいえを選択してください（はい：1、いいえ：0）%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
    recvBuf[recvLen] = '\0';    //受信データにNULLを追加
    
    //店舗指定する場合
    if(recvBuf == 1){
        //店舗の指定
        //２桁店舗を入力させる
        sprintf(sendBuf, "店舗番号を入力してください（例：01）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //sqlに入力された店舗番号が存在するかどうか確認する
        char sql[BUFSIZE];
        sprintf(sql, "SELECT * FROM store_t WHERE store_id = %s", recvBuf);
        PGresult *res = PQexec(__con, sql);
        if(PQntuples(res) == 1){
            //入力された店舗番号を格納
            char store_id[3];
            strncpy(store_id, recvBuf, 2);
            store_id[2] = '\0';
        }
        //入力された店舗番号が存在しない場合
        else{
            sprintf(sendBuf, "入力された店舗番号は存在しません。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
    }

    //店舗指定しない場合
    else{
        //店舗番号をNULLにする
        store_id[3] = NULL;
    }

    //地域を指定するかどうか
    sprintf(sendBuf, "地域指定しますか？（はい：1、いいえ：0）%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
    recvBuf[recvLen] = '\0';    //受信データにNULLを追加

    //地域指定する場合
    if(recvBuf == 1){
        //地域の指定
        //１桁地域を入力させる
        sprintf(sendBuf, "地域番号を入力してください（例：01）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //sqlに入力された地域番号が存在するかどうか確認する
        char sql[BUFSIZE];
        sprintf(sql, "SELECT * FROM area_t WHERE area_id = %s", recvBuf);
        PGresult *res = PQexec(__con, sql);
        if(PQntuples(res) == 1){
            //入力された地域番号を格納
            char area_id[3];
            strncpy(area_id, recvBuf, 1);
            area_id[2] = '\0';
        }
        //入力された地域番号が存在しない場合
        else{
            sprintf(sendBuf, "入力された地域番号は存在しません。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
    }

    //地域指定しない場合
    else{
        //地域番号をNULLにする
        area_id[3] = NULL;
    }

    //商品指定するかどうか
    sprintf(sendBuf, "商品指定しますか？（はい：1、いいえ：0）%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
    recvBuf[recvLen] = '\0';    //受信データにNULLを追加

    //商品指定する場合
    if(recvBuf == 1){
        //商品番号の指定
        //４桁商品番号を入力させる
        sprintf(sendBuf, "商品番号を入力してください（例：0001）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //sqlに入力された商品番号が存在するかどうか確認する
        char sql[BUFSIZE];
        sprintf(sql, "SELECT * FROM product_t WHERE product_id = %s", recvBuf);
        PGresult *res = PQexec(__con, sql);
        if(PQntuples(res) == 1){
            //入力された商品番号を格納
            char product_id[5];
            strncpy(product_id, recvBuf, 4);
            product_id[4] = '\0';
        }
        //入力された商品番号が存在しない場合
        else{
            sprintf(sendBuf, "入力された商品番号は存在しません。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
    }

    //商品指定しない場合
    else{
        //商品番号をNULLにする
        product_id[5] = NULL;
    }

    //入力された値を一度に全て表示させる
    sprintf(sendBuf, "入力された値を表示します。%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    //入力された値（開始年月日時刻、終了年月日時刻）を一度に表示
    sprintf(sendBuf, "開始年月日時刻：%s %s", start, start_target); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    sprintf(sendBuf, "終了年月日時刻：%s %s", end, end_target); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    //入力された値（店舗番号、地域番号、商品番号）を一度に表示
    sprintf(sendBuf, "店舗番号：%s", store_id); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    sprintf(sendBuf, "地域番号：%s", area_id); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    sprintf(sendBuf, "商品番号：%s", product_id); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信

    //DB接続
    sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
    PGconn *con = PQconnectdb(connInfo);
    if( PQstatus(con) == CONNECTION_BAD ){
        printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
        printf("%s", PQerrorMessage(con));
        con = NULL;
        sendLen = sprintf(sendBuf, "error occured%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);
    }else{
        printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
    }

    //DBから入力された期間の店舗番号、地域番号、商品番号情報を絞り込む
    sprintf(sql, "SELECT * FROM summary WHERE sales_date >= %s AND sales_date <= %s AND store_id = %s AND area_id = %s AND product_id = %s", start, end, store_id, area_id, product_id);
    PGresult *res = PQexec(con, sql);

    //絞り込んだ内容から期間指定の範囲内でOrder_Cntの値を取得し、全て足し算
    int items_sum = 0;
    for(int i = 0; i < PQntuples(res); i++){
        sum += atoi(PQgetvalue(res, i, 3));
    }

    //絞り込んだ内容から商品IDを主キーとし、menu_tから金額を取得
    sprintf(sql, "SELECT * FROM menu_t WHERE menu_id = %s", menu_id);
    PGresult *res = PQexec(con, sql);

    SELCT menu_t.menu_id, menu_t.price, summary.sales_date, summary.order_cnt FROM menu_t INNER JOIN summary ON menu_t.menu_id = summary.menu_id WHERE summary.sales_date >= %s AND summary.sales_date <= %s AND summary.store_id = %s AND summary.area_id = %s AND summary.product_id = %s


    //summaryに金額のみを挿入
    sprintf(sql, "INSERT INTO summary (menu_id, price) VALUES (%s, %s)");


   
    //絞り込んだ内容から期間指定の範囲内でOrder_Cntの値






}

