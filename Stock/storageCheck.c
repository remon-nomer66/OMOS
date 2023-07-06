#include "omos.h"

int storageCheck(PGconn *__con, int __soc, int __auth){
    int recvLen, sendLen; //送受信データ長
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; //送受信用バッファ
    pthread_t selfId = pthread_self(); // スレッド
    PGresult *res; //PGresult型の変数resを宣言
    int OD1, OD2, check; //OD1はフード・ドリンクの商品ID、OD2はフード・ドリンクの発注数、check1は発注票作成の際、動作続行のチェック用
    char check2[BUFSIZE], char OD3[BUFSIZE]; //check2は本部による在庫確認作業を行うための変数, OD3は発注票作成の際、動作続行のチェック用

    u_id = __u_info[0]; //ユーザID
    u_auth = __u_info[1]; //ユーザの持つ権限
    u_store = __u_info[2]; //ユーザの所属

    if (u_auth == ACLERK){ //店員
        //現在の在庫です。と表示
        sprintf(sendBuf, "現在の在庫です。");
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        // 在庫一斉表示
        //テーブル名：menu_storage_tからstore_idがu_storeのものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d", u_store);　//SQL文を作成
        res = PQexec(__con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(__soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf{recvLen} = '\0'; //受信データにNULLを追加
        }
        //在庫切れが近い商品です。と表示
        sprintf(sendBuf, "在庫切れが近い商品です。");
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        // テーブル名：menu_storage_tからstore_idがu_storeと一致し、かつstorageの値がmin_storageよりも小さいものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d AND menu_storage_t.storage < menu_storage_t.min_storage", u_store);　//SQL文を作成
        res = PQexec(__con, sendBuf);　//送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(__soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf{recvLen} = '\0'; //受信データにNULLを追加
        }
    }else if (u_auth == AMGR){ //店長
        // 在庫一斉表示
        //テーブル名：menu_storage_tからstore_idがu_storeのものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d", u_store);　//SQL文を作成
        res = PQexec(__con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(__soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf{recvLen} = '\0'; //受信データにNULLを追加
        }
        //在庫切れが近い商品です。と表示
        sprintf(sendBuf, "在庫切れが近い商品です。");
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        // テーブル名：menu_storage_tからstore_idがu_storeと一致し、かつstorageの値がmin_storageよりも小さいものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d AND menu_storage_t.storage < menu_storage_t.min_storage", u_store);　//SQL文を作成
        res = PQexec(__con, sendBuf);　//送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(__soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf{recvLen} = '\0'; //受信データにNULLを追加
        }
        while (1){
            sprintf(sendBuf, "発注しますか？ (y/n) %s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(__soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf{recvLen} = '\0'; //受信データにNULLを追加
            if (strcmp(recvBuf, "y") == 0){
                sprintf(sendBuf, "フードとドリンク、どちらを発注しますか？(フード/ドリンク)%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(__soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf{recvLen} = '\0'; //受信データにNULLを追加
                if (strcmp(recvBuf, "フード") == 0){
                while (1){
                    check = 0;
                    sprintf(sendBuf, "どのフード注文しますか？商品ID（4桁）を打ち込んでください。（例：0001） %s操作を終了したい場合は exit と入力してください．%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf);//送信データ長
                    send(__soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf{recvLen} = '\0';//受信データにNULLを追加
                    if (strcmp(recvBuf, "exit") == 0){
                        break;
                    }else{
                        //4文字以外の場合はエラーを返す
                        if (strlen(recvBuf) != 4){
                            sprintf(sendBuf, "商品IDは4桁で入力してください。%s", ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(__soc, sendBuf, sendLen, 0); //送信
                            



                                // テーブル名：recipe_tからOD1と同じmenu_idを持つmenu_nameを取得してOD3に代入
                                sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id = %s", OD1);
                                　 //SQL文作成
                                    res = PQexec(__con, sendBuf);
                                　 //実行
                                    // もしうまくいかなければエラーを表示する
                                    if (PQresultStatus(res) != PGRES_TUPLES_OK)
                                {
                                    sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s", ENTER);
                                    　 //送信データ作成
                                        sendLen = strlen(sendBuf);
                                    　 //送信データ長
                                        send(__soc, sendBuf, sendLen, 0);
                                    　 //送信
                                        check1 = 1;
                                }
                                if (check1 != 1)
                                {
                                    OD3 = PQgetvalue(res, 0, 0);
                                    　 //実行結果をOD3に代入
                                        // テーブル名：store_order_tのmenu_idにOD1を挿入、store_orderにOD2を挿入、menu_nameにOD3を挿入
                                        sprintf(sendBuf, "INSERT INTO store_order_t VALUES(%s, %d, %s)", OD1, OD3, OD2);
                                    　 //SQL文作成
                                        res = PQexec(__con, sendBuf);
                                    　 //実行
                                }
                            }
                        }
                    }
                    // テーブル名：store_order_tの中身をcsvファイルとして出力
                    sprintf(sendBuf, "COPY store_order_t TO 'store_order_t.csv' WITH CSV");
                    　 //SQL文作成
                        res = PQexec(__con, sendBuf);
                    　 //実行
                }
                else if (strcmp(recvBuf, "ドリンク") == 0)
                {
                    while (1)
                    {
                        check1 = 0;
                        sprintf(sendBuf, "どのドリンクをどれだけ注文しますか？半角数字で menu_id（4桁）+ 半角スペース + 発注量（3桁） を打ち込んでください。（例：アサヒスーパードライ（menu_id = 0002）を5個発注する時　0002 005） %s操作を終了したい場合は exit と入力してください．%s", ENTER);
                        　 //送信データ作成
                            sendLen = strlen(sendBuf);
                        　 //送信データ長
                            send(__soc, sendBuf, sendLen, 0);
                        　 //送信
                            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
                        　 //受信
                            recvBuf{recvLen} = '\0';
                        　 //受信データにNULLを追加
                            if (strcmp(recvBuf, "exit") == 0)
                        {
                            break;
                        }
                        else
                        {
                            if (strlen(recvBuf) >= 8)
                            {
                                　 //8桁以上の値がrecvBufに入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が長すぎます。%s", ENTER);
                                　 //送信データ作成
                                    sendLen = strlen(sendBuf);
                                　 //送信データ長
                                    send(__soc, sendBuf, sendLen, 0);
                                　 //送信
                            }
                            else if (isdigit(recvBuf) == 0)
                            {
                                　 //数字以外の文字が入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                                　 //送信データ作成
                                    sendLen = strlen(sendBuf);
                                　 //送信データ長
                                    send(__soc, sendBuf, sendLen, 0);
                                　 //送信
                            }
                            else
                            {
                                // recvBufをsscanf()を用いてOD1とOD2に分ける
                                sscanf(recvBuf, "%s %s", OD1, OD2);
                                // テーブル名：recipe_tからOD1と同じmenu_idを持つmenu_nameを取得してOD3に代入
                                sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id = %s", OD1);
                                　 //SQL文作成
                                    res = PQexec(__con, sendBuf);
                                　 //実行
                                    // もしうまくいかなければエラーを表示する
                                    if (PQresultStatus(res) != PGRES_TUPLES_OK)
                                {
                                    sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s", ENTER);
                                    　 //送信データ作成
                                        sendLen = strlen(sendBuf);
                                    　 //送信データ長
                                        send(__soc, sendBuf, sendLen, 0);
                                    　 //送信
                                        check1 = 1;
                                }
                                if (check != 1)
                                {
                                    OD3 = PQgetvalue(res, 0, 0);
                                    　 //実行結果をOD3に代入
                                        // テーブル名：store_order_tのmenu_idにOD1を挿入、store_orderにOD2を挿入、menu_nameにOD3を挿入
                                        sprintf(sendBuf, "INSERT INTO store_order_t VALUES(%s, %d, %s)", OD1, OD3, OD2);
                                    　 //SQL文作成
                                        res = PQexec(__con, sendBuf);
                                    　 //実行
                                }
                            }
                        }
                    }
                    // テーブル名：store_order_tの中身をcsvファイルとして出力
                    sprintf(sendBuf, "COPY store_order_t TO 'store_order_t.csv' WITH CSV");
                    　 //SQL文作成
                        res = PQexec(__con, sendBuf);
                    　 //実行
                }
                else
                {
                    // 入力されている文字が不正であることを伝える。
                    sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                    　 //送信データ作成
                        sendLen = strlen(sendBuf);
                    　 //送信データ長
                        send(__soc, sendBuf, sendLen, 0);
                    　 //送信
                }
            }
            else if (strcmp(recvBuf, "n") == 0)
            {
                break;
            }
        }
    }
    else if (__auth == ACOR)
    {
        　 //COR
            while (1)
        {
            check1 = 0;
            // どの店舗IDを選ぶかを確認する。
            sprintf(sendBuf, "どの店舗IDの在庫を確認しますか？2桁で入力してください。（例：店舗IDが3の時 03 のように入力する。）%s操作を終了したい場合は exit と入力してください．%s", ENTER);
            　 //送信データ作成
                sendLen = strlen(sendBuf);
            　 //送信データ長
                send(__soc, sendBuf, sendLen, 0);
            　 //送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
            　 //受信
                recvBuf{recvLen} = '\0';
            　 //受信データにNULLを追加
                if (strcmp(recvBuf, "exit") == 0)
            {
                break;
            }
            else
            {
                // recvBufの中身を確認
                if (strlen(recvBuf) != 2)
                {
                    　 //2桁でなければエラーを表示する。
                        sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                    　 //送信データ作成
                        sendLen = strlen(sendBuf);
                    　 //送信データ長
                        send(__soc, sendBuf, sendLen, 0);
                    　 //送信
                }
                else if (isdigit(recvBuf) == 0)
                {
                    　 //数字以外の文字が入っていたら、エラーを表示する。
                        sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                    　 //送信データ作成
                        sendLen = strlen(sendBuf);
                    　 //送信データ長
                        send(__soc, sendBuf, sendLen, 0);
                    　 //送信
                }
                else
                {
                    // テーブル名：summary_tからstore_idがrecvBufと同じものを取得して表示
                    sprintf(sendBuf, "SELECT * FROM summary_t WHERE store_id = %s", recvBuf);
                    　 //SQL文作成
                        res = PQexec(__con, sendBuf);
                    　 //実行
                        // もしうまくいかなければエラーを表示する
                        if (PQresultStatus(res) != PGRES_TUPLES_OK)
                    {
                        sprintf(sendBuf, "選択した店舗IDは存在しません。%s", ENTER);
                        　 //送信データ作成
                            sendLen = strlen(sendBuf);
                        　 //送信データ長
                            send(__soc, sendBuf, sendLen, 0);
                        　 //送信
                            check1 = 1;
                    }
                    if (check1 != 1)
                    {
                        // check2にrecvBufを挿入
                        sprintf(check2, "%s", recvBuf);
                        // 閲覧したい商品の商品IDを入力させる。
                        sprintf(sendBuf, "閲覧したい商品の商品IDを入力してください（4桁　例：商品IDが1の商品なら　0001と入力する。）。%s", ENTER);
                        　 //送信データ作成
                            sendLen = strlen(sendBuf);
                        　 //送信データ長
                            send(__soc, sendBuf, sendLen, 0);
                        　 //送信
                            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
                        　 //受信
                            recvBuf{recvLen} = '\0';
                        　 //受信データにNULLを追加
                            if (strlen(recvBuf) != 4)
                        {
                            　 //4桁でなければエラーを表示する。
                                sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                            　 //送信データ作成
                                sendLen = strlen(sendBuf);
                            　 //送信データ長
                                send(__soc, sendBuf, sendLen, 0);
                            　 //送信
                        }
                        else if (isdigit(recvBuf) == 0)
                        {
                            　 //数字以外の文字が入っていたら、エラーを表示する。
                                sprintf(sendBuf, "入力された値が不正です。%s", ENTER);
                            　 //送信データ作成
                                sendLen = strlen(sendBuf);
                            　 //送信データ長
                                send(__soc, sendBuf, sendLen, 0);
                            　 //送信
                        }
                        else
                        {
                            // テーブル名：order_tからstore_idがcheck2と同じもの、かつをmenu_idがrecvBufと同じものを取得して表示
                            sprintf(sendBuf, "SELECT * FROM order_t WHERE store_id = %s AND menu_id = %s", check2, recvBuf);
                            　 //SQL文作成
                                res = PQexec(__con, sendBuf);
                            　 //実行
                                // もしうまくいかなければその店舗に指定された商品IDが存在しないことを示すエラーを表示する
                                if (PQresultStatus(res) != PGRES_TUPLES_OK)
                            {
                                sprintf(sendBuf, "選択した店舗には指定された商品は存在しません。%s", ENTER);
                                　 //送信データ作成
                                    sendLen = strlen(sendBuf);
                                　 //送信データ長
                                    send(__soc, sendBuf, sendLen, 0);
                                　 //送信
                                    check1 = 1;
                            }
                            if (check1 != 1)
                            {
                                sprintf(sendBuf, "%s", PQgetvalue(res, 0, 0));
                                　 //送信データ作成
                                    sendLen = strlen(sendBuf);
                                　 //送信データ長
                                    send(__soc, sendBuf, sendLen, 0);
                                　 //送信
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}