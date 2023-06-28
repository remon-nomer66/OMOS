    #include "omos.h"

    int storageCheck(PGconn *__con, int __soc, int __auth){
        int recvLen, sendLen;　//送受信データ長
        char recvBuf[BUFSIZE], sendBuf[BUFSIZE]　//送受信用バッファ
        pthread_t selfId = pthread_self();  //スレッド
        PGresult *res;　//PGresult型の変数resを宣言
        int OD1, OD2, check;
        char OD3[BUFSIZE];

        if(__auth == ACLERK){　//店員
            //在庫一斉表示
            sprintf(sendBuf, "SELECT menu_storage.menu_id, menu_storage.storage, omos_recipe.menu_name FROM menu_storage, omos_recipe WHERE menu_storage.menu_id = omos_recipe.menu_number");　//送信データ作成
            res = PQexec(__con, sendBuf);　//送信データを実行
            //実行結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            //在庫切れ間近のメニュー一覧表示
            sprintf(sendBuf, "SELECT menu_storage.menu_id, menu_storage.storage, omos_recipe.menu_name FROM menu_storage, omos_recipe WHERE menu_storage.menu_id = omos_recipe.menu_number, menu_storage.storage < menu_storage.min_storage");　//送信データ作成
            res = PQexec(__con, sendBuf);　//送信データを実行
            //実行結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
        }else if(__auth == AMGR){　//店長
            //在庫一斉表示
            sprintf(sendBuf, "SELECT menu_storage.menu_id, menu_storage.storage, omos_recipe.menu_name FROM menu_storage, omos_recipe WHERE menu_storage.menu_id = omos_recipe.menu_number");　//送信データ作成
            res = PQexec(__con, sendBuf);　//送信データを実行
            //実行結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            //在庫切れ間近のメニュー一覧表示
            sprintf(sendBuf, "SELECT menu_storage.menu_id, menu_storage.storage, omos_recipe.menu_name FROM menu_storage, omos_recipe WHERE menu_storage.menu_id = omos_recipe.menu_number, menu_storage.storage < menu_storage.min_storage");　//送信データ作成
            res = PQexec(__con, sendBuf);　//送信データを実行
            //実行結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            while(1){
                sprintf(sendBuf, "発注しますか？ (y/n) %s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                if(strcmp(recvBuf, "y") == 0){
                    sprintf(sendBuf, "フードとドリンク、どちらを発注しますか？(フード/ドリンク)%s", ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf , sendLen, 0);　//送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                    if(strcmp(recvBuf, "フード") == 0){
                        while(1){
                            check = 0;
                            sprintf(sendBuf, "どのフードをどれだけ注文しますか？半角数字で menu_id（4桁）+ 半角スペース + 発注量（3桁） を打ち込んでください。（例：アサヒスーパードライ（menu_id = 0002）を5個発注する時　0002 005） %s操作を終了したい場合は exit と入力してください．%s", ENTER);　//送信データ作成
                            sendLen = strlen(sendBuf);　//送信データ長
                            send(__soc, sendBuf , sendLen, 0);　//送信
                            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                            if(strcmp(recvBuf, "exit") == 0){
                                break;
                            }else{
                                if(strlen(recvBuf) >= 8){　//8桁以上の値がrecvBufに入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が長すぎます。%s", ENTER);　//送信データ作成
                                    sendLen = strlen(sendBuf);　//送信データ長
                                    send(__soc, sendBuf , sendLen, 0);　//送信
                                }else if(isdigit(recvBuf) == 0){　//数字以外の文字が入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が不正です。%s", ENTER);　//送信データ作成
                                    sendLen = strlen(sendBuf);　//送信データ長
                                    send(__soc, sendBuf , sendLen, 0);　//送信
                                }else{
                                    //recvBufをsscanf()を用いてOD1とOD2に分ける
                                    sscanf(recvBuf, "%s %s", OD1, OD2);
                                    //テーブル名：omos_recipeからOD1と同じmenu_numberを持つmenu_nameを取得してOD3に代入
                                    sprintf(sendBuf, "SELECT menu_name FROM omos_recipe WHERE menu_number = %s", OD1);　//SQL文作成
                                    res = PQexec(__con, sendBuf);　//実行
                                    //もしうまくいかなければエラーを表示する
                                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                                        sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s", ENTER);　//送信データ作成
                                        sendLen = strlen(sendBuf);　//送信データ長
                                        send(__soc, sendBuf , sendLen, 0);　//送信
                                        check = 1;
                                    }
                                    if(check == 1){
                                        OD3 = PQgetvalue(res, 0, 0);　//実行結果をOD3に代入
                                        //テーブル名：menu_storage_orderのmenu_idにOD1を挿入、store_orderにOD2を挿入、menu_nameにOD3を挿入
                                        sprintf(sendBuf, "INSERT INTO menu_storage_order VALUES(%s, %d, %s)", OD1, OD3, OD2);　//SQL文作成
                                        res = PQexec(__con, sendBuf);　//実行
                                        //テーブル名：menu_storage_oederの中身をcsvファイルとして出力
                                        sprintf(sendBuf, "COPY menu_storage_order TO 'menu_storage_order.csv' WITH CSV");　//SQL文作成
                                        res = PQexec(__con, sendBuf);　//実行
                                    }
                                }
                            }
                        }
                    }else if(strcmp(recvBuf, "ドリンク") == 0){
                        while(1){
                            check = 0;
                            sprintf(sendBuf, "どのドリンクをどれだけ注文しますか？半角数字で menu_id（4桁）+ 半角スペース + 発注量（3桁） を打ち込んでください。（例：アサヒスーパードライ（menu_id = 0002）を5個発注する時　0002 005） %s操作を終了したい場合は exit と入力してください．%s", ENTER);　//送信データ作成
                            sendLen = strlen(sendBuf);　//送信データ長
                            send(__soc, sendBuf , sendLen, 0);　//送信
                            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                            if(strcmp(recvBuf, "exit") == 0){
                                break;
                            }else{
                                if(strlen(recvBuf) >= 8){　//8桁以上の値がrecvBufに入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が長すぎます。%s", ENTER);　//送信データ作成
                                    sendLen = strlen(sendBuf);　//送信データ長
                                    send(__soc, sendBuf , sendLen, 0);　//送信
                                }else if(isdigit(recvBuf) == 0){　//数字以外の文字が入っていたら、エラーを表示する。
                                    sprintf(sendBuf, "入力された値が不正です。%s", ENTER);　//送信データ作成
                                    sendLen = strlen(sendBuf);　//送信データ長
                                    send(__soc, sendBuf , sendLen, 0);　//送信
                                }else{
                                    //recvBufをsscanf()を用いてOD1とOD2に分ける
                                    sscanf(recvBuf, "%s %s", OD1, OD2);
                                    //テーブル名：omos_recipeからOD1と同じmenu_numberを持つmenu_nameを取得してOD3に代入
                                    sprintf(sendBuf, "SELECT menu_name FROM omos_recipe WHERE menu_number = %s", OD1);　//SQL文作成
                                    res = PQexec(__con, sendBuf);　//実行
                                    //もしうまくいかなければエラーを表示する
                                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                                        sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s", ENTER);　//送信データ作成
                                        sendLen = strlen(sendBuf);　//送信データ長
                                        send(__soc, sendBuf , sendLen, 0);　//送信
                                        check = 1;
                                    }
                                    if(check == 1){
                                        OD3 = PQgetvalue(res, 0, 0);　//実行結果をOD3に代入
                                        //テーブル名：menu_storage_orderのmenu_idにOD1を挿入、store_orderにOD2を挿入、menu_nameにOD3を挿入
                                        sprintf(sendBuf, "INSERT INTO menu_storage_order VALUES(%s, %d, %s)", OD1, OD3, OD2);　//SQL文作成
                                        res = PQexec(__con, sendBuf);　//実行
                                        //テーブル名：menu_storage_oederの中身をcsvファイルとして出力
                                        sprintf(sendBuf, "COPY menu_storage_order TO 'menu_storage_order.csv' WITH CSV");　//SQL文作成
                                        res = PQexec(__con, sendBuf);　//実行
                                    }
                                }
                            }
                        }else{
                            //入力されている文字が不正であることを伝える。
                            sprintf(sendBuf, "入力された値が不正です。%s", ENTER);　//送信データ作成
                            sendLen = strlen(sendBuf);　//送信データ長
                            send(__soc, sendBuf , sendLen, 0);　//送信
                        }
                    }
                }else if(strcmp(recvBuf, "n") == 0){
                    break;
                }
            }
        }else if(__auth == ACOR){　//COR
            sprintf(sendBuf, "");　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf , sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        }
        return 0;
    }