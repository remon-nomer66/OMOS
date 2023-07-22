#include "omos.h"

int menuChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    int changeid, changestore, changestore2, changeprice, changelevel, changeseason, u_id, u_auth, u_store, i; //変更する商品ID, 変更を加えたい部分の店舗ID, 変更後の値段, 押しかどうか、ユーザID、ユーザの持つ権限、ユーザの所属, ループ用変数
    char response[BUFSIZE], changeitem[BUFSIZE], changename[BUFSIZE], changestar[BUFSIZE];
    PGresult *res; //PGresult型の変数resを宣言

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if(u_auth == AMGR){
        sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        //テーブル名：menu_charge_tからuser_idがu_idと一致し、かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_charge_t WHERE user_id = %d);", u_id); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "変更できるメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行したSQL文の結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf , sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        }
        PQclear(res); //resの中身をクリア
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        //4文字以外の場合はエラーを返す
        if(strlen(recvBuf) !=4){
            sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < 4; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "商品IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
        //クライアントから受信した値をchangeidに代入
        sscanf(recvBuf, "%d", &changeid);
        //クライアントから受信したchangeidと、menu_idが一致するものかつテーブル名：menu_storage_tのstore_idとu_storeが一致するものを探す。
        sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){ //menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //resの中身をクリア
        //何を変更したいかを聞く。選択肢はname, price, starであることも伝える。
        sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, starです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //クライアントから受信した変更内容をchangeitemに代入
        sscanf(recvBuf, "%s", changeitem);
        //変更内容がnameの場合、どう変更するかを聞く。
        if(strcmp(changeitem, "name") == 0){
            sprintf(sendBuf, "どんな商品名にしますか？%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //クライアントから受信した変更内容をchangenameに代入
            sscanf(recvBuf, "%s", changename);
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
            sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            PQclear(res); //resの中身をクリア
        }else if(strcmp(changeitem, "price") == 0){
            //現在の価格は以下の通りです。と表示
            sprintf(sendBuf, "現在の価格は以下の通りです。%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
            sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行結果をクライアントに送信
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s%s%s", PQgetvalue(res, i, 0), ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            }
            PQclear(res); //resの中身をクリア
            //変更内容がpriceの場合、どう変更するかを聞く。
            sprintf(sendBuf, "値段はいくらに変更しますか？%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //入力された文字列に数字以外が含まれるならエラーを返す。
            for(i = 0; i < recvLen-1; i++){
                if(!isdigit(recvBuf[i])){
                    sprintf(sendBuf, "値段は半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した変更内容をchangepriceに代入
            sscanf(recvBuf, "%d", &changeprice);
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_price_tのpriceの内容をchangepriceに変更
            sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            PQclear(res); //resの中身をクリア
        }else if(strcmp(changeitem, "star") == 0){
            sprintf(sendBuf, "あなたが押し情報を変更できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            //テーブル名：menu_storage_tからstore_idがu_storeと一致する商品のmenu_id, menu_nameを表示する。menu_nameはテーブル名：recipe_tからmenu_idが一致するものを表示。
            sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d;", u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf , sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            }
            PQclear(res); //resの中身をクリア
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //4文字以外の場合はエラーを返す
            if(strlen(recvBuf) !=4){
                sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(int i = 0; i < 4; i++){
                if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                    sprintf(sendBuf, "商品IDは半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したchangeidと、menu_idが一致するものかつテーブル名：menu_storage_tのstore_idとu_storeが一致するものを探す。
            sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
               send(soc, sendBuf, sendLen, 0); //送信
               return -1;
            }
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_mgrの値を確認する。
            sprintf(sendBuf, "SELECT push_mgr FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //push_hqの値が0の場合、押しメニューにしますか？と聞く。
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_mgrの値を1に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_mgr = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changestar, "no") == 0){
                    //何も変更しませんでしたと返す。
                    sprintf(sendBuf, "何も変更しませんでした。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                }else{
                    //使用不可のコマンドと返す。
                    sprintf(sendBuf, "使用不可のコマンドです。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_mgrの値を0に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_mgr = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changestar, "no") == 0){
                    //何も変更しませんでしたと返す。
                    sprintf(sendBuf, "何も変更しませんでした。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                }else{
                    //使用不可のコマンドと返す。
                    sprintf(sendBuf, "使用不可のコマンドです。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
        }else{
            //打ち込まれたコマンドが使えないことを表示
            sprintf(sendBuf, "そのコマンドは使えません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        };
    }else if(u_auth == AHQ){
        //情報を変更したいものがショップメニューかどうかを聞く。
        sprintf(sendBuf, "情報を変更したいものはショップメニューですか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //受信した内容をresponseに代入
        sscanf(recvBuf, "%s", response);
        if(strcmp(response, "yes") == 0){
            //情報を変更したい店舗ID（3桁：半角数字）を聞く。
            sprintf(sendBuf, "情報を変更したい店舗ID（3桁：半角数字）を入力してください。（例：01）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //3文字以外の場合はエラーを返す
            if(strlen(recvBuf) !=3){
                sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 3; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "店舗IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //受信した値をchangestoreに代入
            sscanf(recvBuf, "%d", &changestore);
            //情報を変更したい店舗IDが存在するかどうかをテーブル名store_tから確認
            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //テーブル名store_tから取得した値が0の場合、店舗IDが存在しないことを表示
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "その店舗IDは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resの中身をクリア
            sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //情報を変更したい店舗IDが存在する場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致する, かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
            sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changestore);
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){
                sprintf(sendBuf, "変更できるメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf , sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            }
            PQclear(res); //resの中身をクリア
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //4文字以外の場合はエラーを返す
            if(strlen(recvBuf) !=4){
                sprintf(sendBuf, "商品IDは4文字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < recvLen-1; i++){
                if(!isdigit(recvBuf[i])){
                    sprintf(sendBuf, "商品IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resの中身をクリア
            sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, starです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //受信した内容をchangeitemに代入
            sscanf(recvBuf, "%s", changeitem);
            if(strcmp(changeitem, "name") == 0){
                sprintf(sendBuf, "どんな商品名にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //クライアントから受信した変更内容をchangenameに代入
                sscanf(recvBuf, "%s", changename);
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
                //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else if(strcmp(changeitem, "price") == 0){
                sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //入力された文字が数字以外ならエラーを返す。
                for(i = 0; i < recvLen-1; i++){
                    if(!isdigit(recvBuf[i])){
                        sprintf(sendBuf, "値段は半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangepriceに代入
                sscanf(recvBuf, "%d", &changeprice);
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのpriceの内容をchangepriceに変更
                sprintf(sendBuf, "UPDATE recipe_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
                //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
                //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else if(strcmp(changeitem, "level") == 0){
                //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s", ENTER, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //入力された文字が数字以外ならエラーを返す。
                for(i = 0; i < recvLen-1; i++){
                    if(!isdigit(recvBuf[i])){
                        sprintf(sendBuf, "メニューレベルは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangelevelに代入
                sscanf(recvBuf, "%d", &changelevel);
                //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                    sprintf(sendBuf, "そのメニューレベルは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //changelevelの値が5の場合、どのシーズンメニューにするかを聞く。選択肢は1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューであることも伝える。
                if(changelevel == 5){
                    sprintf(sendBuf, "どの季節のメニューにしますか？%s 1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューです。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "季節メニューは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangeseasonに代入
                    sscanf(recvBuf, "%d", &changeseason);
                    //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                    if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                        sprintf(sendBuf, "その季節は存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                    sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeseason, changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(changelevel == 3){
                    //どの店舗IDに変更しますか。3桁：半角数字で入力してください。と聞く。
                    sprintf(sendBuf, "どの店舗IDに変更しますか？%s 3桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //3文字以外の場合はエラーを返す
                    if(strlen(recvBuf) !=3){
                        sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(isdigit(recvBuf[i]) == 0){
                            sprintf(sendBuf, "店舗IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //テーブル名：menu_storage_tのstore_idの内容をchangestore2に変更
                    sscanf(recvBuf, "%d", &changestore2);
                    sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changestore2, changeid, changestore); //SQL文作成
                    changestore = changestore2;
                }
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_detail_tのlayerの内容をchangelevelに変更
                sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changelevel, changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
                //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else if(strcmp(changeitem, "star") == 0){
                //menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの値を確認する。
                sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //push_hqの値が0の場合、押しメニューにしますか？と聞く。
                if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                    PQclear(res); //resの中身をクリア
                    sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を1に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                        //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                        sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(strcmp(changestar, "no") == 0){
                        //何も変更しませんでしたと返す。
                        sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                    }else{
                        //使用不可のコマンドと返す。
                        sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                //push_hqの値が1の場合、押しメニューをやめますか？と聞く。
                }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                    PQclear(res); //resの中身をクリア
                    sprintf(sendBuf, "押しメニューをやめますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を0に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                        //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                        sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(strcmp(changestar, "no") == 0){
                        //何も変更しませんでしたと返す。
                        sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                    }else{
                        //使用不可のコマンドと返す。
                        sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
            }
        }else if(strcmp(response, "no") == 0){
            //情報を変更したいものがシーズンメニューかどうかを聞く。
            sprintf(sendBuf, "情報を変更したいものはシーズンメニューですか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //受信した内容をresponseに代入
            sscanf(recvBuf, "%s", response);
            if(strcmp(response, "yes") == 0){
                sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                //テーブル名：menu_detail_tでlayerの値が5のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
                sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 5));"); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //実行結果がなければ、変更できるメニューは存在しないことを表示
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "変更できるメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //実行結果を表示
                for(int i = 0; i < PQntuples(res); i++){
                    sprintf(sendBuf, "%s %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf , sendLen, 0); //送信
                }
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0';
                //4文字以外の場合はエラーを返す
                if(strlen(recvBuf) !=4){
                    sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //入力された文字が数字以外ならエラーを返す。
                for(int i = 0; i < recvLen-1; i++){
                    if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                        sprintf(sendBuf, "商品IDは半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangeidに代入
                sscanf(recvBuf, "%d", &changeid);
                //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
                sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                if(PQntuples(res) == 0){ //menu_idが存在しない場合
                    sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, star, seasonです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //受信した内容をchangeitemに代入
                sscanf(recvBuf, "%s", changeitem);
                if(strcmp(changeitem, "name") == 0){
                    sprintf(sendBuf, "どんな商品名にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //クライアントから受信した変更内容をchangenameに代入
                    sscanf(recvBuf, "%s", changename);
                    //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                    sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "price") == 0){
                    sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "値段は半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangepriceに代入
                    sscanf(recvBuf, "%d", &changeprice);
                    //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_price_tのpriceの内容をchangepriceに変更
                    sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "level") == 0){
                    //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                    sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s", ENTER, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "メニューレベルは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangelevelに代入
                    sscanf(recvBuf, "%d", &changelevel);
                    //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                    if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                        sprintf(sendBuf, "そのメニューレベルは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //changelevelの値が5の場合、どのシーズンメニューにするかを聞く。選択肢は1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューであることも伝える。
                    if(changelevel == 5){
                        sprintf(sendBuf, "どの季節のメニューにしますか？%s 1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューです。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "季節メニューは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangeseasonに代入
                        sscanf(recvBuf, "%d", &changeseason);
                        //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                        if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                            sprintf(sendBuf, "その季節は存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                        sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeseason, changeid, changestore); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(changelevel == 3){
                        //どの店舗IDに変更しますか。3桁：半角数字で入力してください。と聞く。
                        sprintf(sendBuf, "どの店舗IDに変更しますか？%s 3桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //3文字以外の場合はエラーを返す
                        if(strlen(recvBuf) !=3){
                            sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(isdigit(recvBuf[i]) == 0){
                                sprintf(sendBuf, "店舗IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //テーブル名：menu_storage_tのstore_idの内容をchangestore2に変更
                        sscanf(recvBuf, "%d", &changestore2);
                        sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changestore2, changeid, changestore); //SQL文作成
                        changestore = changestore2;
                    }
                    //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_detail_tのlayerの内容をchangelevelに変更
                    sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changelevel, changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "star") == 0){
                    //menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの値を確認する。
                    sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //push_hqの値が0の場合、押しメニューにしますか？と聞く。
                    if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                        PQclear(res); //resの中身をクリア
                        sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //クライアントから受信した変更内容をchangestarに代入
                        sscanf(recvBuf, "%s", changestar);
                        //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を1に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                            sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドと返す。
                            sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    //push_hqの値が1の場合、押しメニューをやめますか？と聞く。
                    }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                        PQclear(res); //resの中身をクリア
                        sprintf(sendBuf, "押しメニューをやめますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //クライアントから受信した変更内容をchangestarに代入
                        sscanf(recvBuf, "%s", changestar);
                        //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を0に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                            sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドと返す。
                            sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                }
            }else if(strcmp(response, "no") == 0){
                sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //テーブル名：menu_detail_tでlayerの値が1, 2, 4のもののmenu_idとそのmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
                sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 1 OR layer = 2 OR layer = 4);"); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "変更できるメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //実行結果を表示
                for(int i = 0; i < PQntuples(res); i++){
                    sprintf(sendBuf, "%s %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf , sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                }
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0';
                //4文字以外の場合はエラーを返す
                if(strlen(recvBuf) !=4){
                    sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //入力された文字が数字以外ならエラーを返す。
                for(int i = 0; i < recvLen-1; i++){
                    if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                        sprintf(sendBuf, "商品IDは半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangeidに代入
                sscanf(recvBuf, "%d", &changeid);
                //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
                sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                if(PQntuples(res) == 0){ //menu_idが存在しない場合
                    sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, starです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                sscanf(recvBuf, "%s", changeitem);
                if(strcmp(changeitem, "name") == 0){
                    sprintf(sendBuf, "どんな商品名にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //クライアントから受信した変更内容をchangenameに代入
                    sscanf(recvBuf, "%s", changename);
                    //changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                    sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d;", changename, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "price") == 0){
                    sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(int i = 0; i < recvLen; i++){
                        if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                            sprintf(sendBuf, "値段は半角数字で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangepriceに代入
                    sscanf(recvBuf, "%d", &changeprice);
                    //changeidと同じmenu_idを持つ、テーブル名：price_charge_tのpriceの内容をchangepriceに変更する。
                    sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d;", changeprice, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "level") == 0){
                    //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                    sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、    4：リージョンメニュー、5：シーズンメニューです．%s", ENTER, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "メニューレベルは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangelevelに代入
                    sscanf(recvBuf, "%d", &changelevel);
                    //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                    if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                        sprintf(sendBuf, "そのメニューレベルは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //changelevelの値が5の場合、どのシーズンメニューにするかを聞く。選択肢は1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューであることも伝える。
                    if(changelevel == 5){
                        sprintf(sendBuf, "どの季節のメニューにしますか？%s 1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューです。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "季節メニューは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangeseasonに代入
                        sscanf(recvBuf, "%d", &changeseason);
                        //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                        if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                            sprintf(sendBuf, "その季節は存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                        sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeseason, changeid, changestore); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(changelevel == 3){
                        //どの店舗IDに変更しますか。3桁：半角数字で入力してください。と聞く。
                        sprintf(sendBuf, "どの店舗IDに変更しますか？%s 3桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //3文字以外の場合はエラーを返す
                        if(strlen(recvBuf) !=3){
                            sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(isdigit(recvBuf[i]) == 0){
                                sprintf(sendBuf, "店舗IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //テーブル名：menu_storage_tのstore_idの内容をchangestore2に変更
                        sscanf(recvBuf, "%d", &changestore2);
                        sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM    menu_storage_t WHERE store_id = %d);", changestore2, changeid, changestore); //SQL文作成
                        changestore = changestore2;
                    }
                    //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_detail_tのlayerの内容をchangelevelに変更
                    sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changelevel, changeid, changestore); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "star") == 0){
                    //changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの値を確認する。
                    sprintf(sendBuf, "SELECT push_mgr FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //push_mgrの値が0の場合、押しメニューにしますか？と聞く。
                    if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                        sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //クライアントから受信した変更内容をchangestarに代入
                        sscanf(recvBuf, "%s", changestar);
                        //changestarがyesの場合、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を1に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                            sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドですと返す。
                            sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                        sprintf(sendBuf, "押しメニューをやめますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //クライアントから受信した変更内容をchangestarに代入
                        sscanf(recvBuf, "%s", changestar);
                        //changestarがyesの場合、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの内容を0に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                            //テーブル名：menu_charge_tのuser_idにu_idを格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                                sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドですと返す。
                            sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                }else{
                    //使用不可のコマンドですと返す。
                    sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }else{
                //使用不可のコマンドですと返す。
                sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }else{
            sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
    }else if(u_auth == ACOR){
        sprintf(sendBuf, "選べる店舗IDです．%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //テーブル名：region_tからu_storeと同じregion_idを持つstore_idを取得
        sprintf(sendBuf, "SELECT store_id FROM region_t WHERE region_id = %d;", u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1つも無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "あなたが選べる店舗IDがありません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "store_id:%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
        PQclear(res); //メモリ解放
        //どの店舗IDを選択するかを聞く
        sprintf(sendBuf, "どの店舗IDを選択しますか？（3桁：半角数字）（例：001）%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        //3文字以外の場合はエラーを返す
        if(strlen(recvBuf) !=3){
            sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "店舗IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
        //クライアントから受信した店舗IDをchangestoreに代入
        sscanf(recvBuf, "%d", &changestore);
        //changestoreが選べる店舗IDに含まれているかどうかをテーブル名region_tから確認
        sprintf(sendBuf, "SELECT COUNT(*) FROM region_t WHERE region_id = %d AND store_id = %d;", u_store, changestore); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1行も無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "あなたが選べる店舗IDではありません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //メモリ解放
        sprintf(sendBuf, "メニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //changestoreと同じ値のstore_idを持っているmenu_idを表示、またテーブル名：menu_storage_tから、そのmenu_idのmenu_nameをテーブル名：recipe_tから取得
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name FROM menu_storage_t INNER JOIN recipe_t ON menu_storage_t.menu_id = recipe_t.menu_id WHERE menu_storage_t.store_id = %d;", changestore); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1つも無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "この店舗には選べるメニューがありません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行結果（menuidとmenuname）を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "menu_id:%s%smenu_name:%s%s", PQgetvalue(res, i, 0), ENTER, PQgetvalue(res, i, 1), ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
        PQclear(res); //メモリ解放
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        //入力が4文字以外の場合は商品IDを4桁：半角数字で入力するようにエラーを返す
        if(strlen(recvBuf) !=4){
            sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "商品IDは半角数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
        //クライアントから受信した値をchangeidに代入
        sscanf(recvBuf, "%d", &changeid);
        //changeidがchangestoreと同じ値のstore_idを持つmenu_storage_tに存在するか確認
        sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, changestore); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1つも無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "選べない商品IDです．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //メモリ解放
        //changeidと同じmenu_idを持つテーブル名：push_tのpush_corの値を取得する。このときテーブル名：menu_storage_tを見てstore_idとchangestoreが一致していなければならない。
        sprintf(sendBuf, "SELECT push_cor FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //push_corの値が0の場合、押しメニューにしますか？と聞く。
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            PQclear(res); //メモリ解放
            sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_corの値を1に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実
                PQclear(res); //メモリ解放
            }else if(strcmp(changestar, "no") == 0){
                //何も変更しませんでしたと表示
                sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            }else{
                //使用不可のコマンドですと表示
                sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //push_corの値が0の場合、押しメニューをやめますか？と聞く。
        }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
            PQclear(res); //メモリ解放
            sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_corの値を0に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //メモリ解放
            }else if(strcmp(changestar, "no") == 0){
                //何も変更しませんでしたと表示
                sprintf(sendBuf, "何も変更しませんでした．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            }else{
                //使用不可のコマンドですと表示
                sprintf(sendBuf, "使用不可のコマンドです．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
    }
    return 0;
}