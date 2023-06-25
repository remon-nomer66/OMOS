#include "omos.h"

\* お会計処理 *\
int pay(int __soc, int user_id){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int num;    //割り勘人数
    int remainder;  //割り勘の余り
    double price;   //合計金額
    int sharePrice;  //割り勘金額

    char *dbHost = "kite.cs.miyazaki-u.ac.jp";
    char *dbPort = "5432";
    char *dbName = "db42";      //接続先を正しく入力
    char *dbLogin = "dbuser42";
    char *dbPwd = "dbpass42";
    
    sprintf(sendBuf, "お会計を行います．\n お会計卓番号を入力ください。%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長を取得
    send(__soc, sendBuf, sendLen, 0);   //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

    if(recvLen > 0){    //受信データがある場合
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);  //受信データを表示
        sscanf(recvBuf, "%s", comm);    //受信データからコマンドを取得
        //コマンドが数字かどうか判定
        if(isdigit(comm[0[]){
            \\数字ならばkitchenDBに接続する
            sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
            conn = PQconnectdb(connInfo);   //DBに接続
            if( PQstatus(conn) == CONNECTION_BAD ){  //接続確認
                printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
                printf("%s", PQerrorMessage(conn));
                conn = NULL;
                sprintf(sendBuf, "error occured%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
            }
        }else{
            //数字でなければエラーを返す
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        //kitchenDBに接続できているか確認
        sprintf(query, "SELECT * FROM kitchen WHERE table_info LIKE '%s%%' AND kitchen_flag = 1", comm);    \\SQL文を作成
        res = PQexec(conn, query);  //SQL文を実行
        if(PQresultStatus(res) != PGRES_TUPLES_OK){ //実行結果の確認
            printf("SELECT failed: %s", PQerrorMessage(conn));
            PQclear(res);
            PQfinish(conn);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        //結果の確認
        int rows = PQntuples(res);
        if(rows == 0){  //レコードが存在しない場合
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        
        // レコードが存在する場合
        double totalPrice = 0.0;    // 合計金額の初期化
        for(int i = 0; i < rows; i++) {  // 絞り込まれたレコードをループで処理
            char *productID = PQgetvalue(res, i, 0);  // 商品IDの取得
            int quantity = atoi(PQgetvalue(res, i, 1));  // 注文個数の取得
            
            // menuテーブルから単価を取得するクエリの作成
            char priceQuery[256];   // クエリ用バッファ
            sprintf(priceQuery, "SELECT price FROM menu WHERE menu_id = %s", menuID);
            
            // クエリの実行
            PGresult *priceRes = PQexec(conn, priceQuery);
            if(PQresultStatus(priceRes) != PGRES_TUPLES_OK){
                printf("SELECT failed: %s", PQerrorMessage(conn));
                PQclear(priceRes);
                PQfinish(conn);
                sprintf(sendBuf, "error occured%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
            }
            
            // 単価の取得
            price = atof(PQgetvalue(priceRes, 0, 0));
            
            // 合計金額の計算
            totalPrice += price * quantity;
            
            PQclear(priceRes);  // クエリ結果の解放
        }

        //ポイントを使用するかどうか問う
        sprintf(sendBuf, "ポイントを使用しますか？(y/n)%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

        // ポイントを使用する場合、pointUse関数を呼び出し、socket・合計金額・ユーザIDを渡す
        if(recvBuf[0] == 'y'){
            totalPrice = pointUse(__soc, totalPrice, userID);
        }

        // お会計処理
        sprintf(sendBuf, "お会計は%d円です。%s", totalPrice, ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

        // 割り勘の有無を問う
        sprintf(sendBuf, "割り勘しますか？(y/n)%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

        // 割り勘する場合
        if(recvBuf[0] == 'y'){
            // 割り勘人数の入力を求める
            sprintf(sendBuf, "割り勘人数を入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

            // 割り勘人数の取得
            num = atoi(recvBuf);

            // 割り勘金額の計算
            sharePrice = totalPrice / num;
            // totalPrice/numの余りを計算
            remainder = (int)totalPrice % num;

            // 割り勘金額の送信
            sprintf(sendBuf, "一人あたり%d円です。%s", sharePrice, ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

            // 余りがある場合,○人○円/○人○円と表示する
            if(remainder != 0){
                sprintf(sendBuf, "%d人%d円/%d人%d円です。%s", num - remainder, sharePrice, remainder, sharePrice + 1, ENTER);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
                recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信
            }

        // 割り勘しない場合
        }else if(recvBuf[0] == 'n'){
            // お会計完了のメッセージを送信
            sprintf(sendBuf, "お会計完了です。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信
        }

        // 会計完了したかどうかを聞く
        sprintf(sendBuf, "会計完了しましたか？(y/n)%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信
        

        // 会計完了していない場合
        if(recvBuf[0] == 'n'){
            // 会計処理を繰り返す
            continue;
        // 会計完了した場合
        }else if(recvBuf[0] == 'y'){
            // 会計完了のメッセージを送信
            sprintf(sendBuf, "会計完了です。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信
            break;
        }

        
    }


}