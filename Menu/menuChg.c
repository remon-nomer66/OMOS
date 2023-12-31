#include "omos.h"
#include "menu.h"

int menuChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    int changeid, changestore, changestore2, changeprice, changelevel, changeseason, changeregion, u_id, u_auth, u_store, i; //変更する商品ID, 変更を加えたい部分の店舗ID, 変更後の値段, 押しかどうか、ユーザID、ユーザの持つ権限、ユーザの所属, ループ用変数
    char response[BUFSIZE], changeitem[BUFSIZE], changename[BUFSIZE], changestar[BUFSIZE];
    PGresult *res; //PGresult型の変数resを宣言

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if(u_auth == AMGR){
        sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        //テーブル名：menu_storage_tからstore_idがu_storeと一致し、かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行したSQL文の結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < 4; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //resの中身をクリア
        //テーブル名：menu_detail_tのmenu_idがchangeidと一致するもののlayerの値が3であるかどうかを確認
        sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer = 3;", changeid); 
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){ //menu_idが存在しない場合
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
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
            //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
            sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", changename);
            res = PQexec(con, sendBuf);
            //存在している場合は、存在していることを伝える。
            if(PQntuples(res) != 0){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
            sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_charge_tのuser_idを抽出する。
            sprintf(sendBuf, "SELECT user_id FROM menu_charge_t;"); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //user_idが0以外だった場合は、テーブル名：menu_charge_tのuser_idにu_idを格納する。
            if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else{
                PQclear(res); //resの中身をクリア
            }
        }else if(strcmp(changeitem, "price") == 0){
            //現在の価格は以下の通りです。と表示
            sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
            sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行結果をクライアントに送信
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
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
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
            //テーブル名：menu_charge_tのuser_idを抽出する。
            sprintf(sendBuf, "SELECT user_id FROM menu_charge_t;"); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //user_idが0以外だった場合は、テーブル名：menu_charge_tのuser_idにu_idを格納する。
            if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else{
                PQclear(res); //resの中身をクリア
            }
        }else if(strcmp(changeitem, "star") == 0){
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
                    //テーブル名：menu_charge_tのuser_idを抽出する。
                    sprintf(sendBuf, "SELECT user_id FROM menu_charge_t;"); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //user_idが0以外だった場合は、テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                        sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else{
                        PQclear(res); //resの中身をクリア
                    }
                }else if(strcmp(changestar, "no") == 0){
                    //何も変更しませんでしたと返す。
                    sprintf(sendBuf, "何も変更しませんでした。%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                }else{
                    //使用不可のコマンドと返す。
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                    //テーブル名：menu_charge_tのuser_idを抽出する。
                    sprintf(sendBuf, "SELECT user_id FROM menu_charge_t;"); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //user_idが0以外だった場合は、テーブル名：menu_charge_tのuser_idにu_idを格納する。
                    if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                        sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE user_id = %d;", u_id, u_id); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else{
                        PQclear(res); //resの中身をクリア
                    }
                }else if(strcmp(changestar, "no") == 0){
                    //何も変更しませんでしたと返す。
                    sprintf(sendBuf, "何も変更しませんでした。%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                }else{
                    //使用不可のコマンドと返す。
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
        }else{
            //打ち込まれたコマンドが使えないことを表示
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        };
    }else if(u_auth == AHQ){
        //changesotre2に0を代入
        changestore2 = 0;
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
            sprintf(sendBuf, "情報を変更したい店舗ID（3桁：半角数字）を入力してください。（例：001）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //3文字以外の場合はエラーを返す
            if(strlen(recvBuf) !=3){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < recvLen-1; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resの中身をクリア
            sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            //情報を変更したい店舗IDが存在する場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致する, かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
            sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changestore);
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < recvLen-1; i++){
                if(!isdigit(recvBuf[i])){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", changeid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_storage_tのstore_idがchengestoreと、menu_idがchangeidと一致するものがあるかどうかを確認
            sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resの中身をクリア
            //テーブル名：menu_detail_tのmenu_idがchangeidと一致するもののlayerの値が3であるかどうかを確認
            sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer = 3;", changeid); 
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
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
                //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
                sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", changename);
                res = PQexec(con, sendBuf);
                //存在している場合は、存在していることを伝える。
                if(PQntuples(res) != 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else if(strcmp(changeitem, "price") == 0){
                //現在の価格は以下の通りです。と表示
                sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
                sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //実行結果をクライアントに送信
                for(int i = 0; i < PQntuples(res); i++){
                    sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                }
                PQclear(res); //resの中身をクリア
                sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //入力された文字が数字以外ならエラーを返す。
                for(i = 0; i < recvLen-1; i++){
                    if(!isdigit(recvBuf[i])){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した変更内容をchangepriceに代入
                sscanf(recvBuf, "%d", &changeprice);
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：menu_price_tのpriceの内容をchangepriceに変更
                sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, changestore); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
            }else if(strcmp(changeitem, "level") == 0){
                //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //入力された文字が数字以外ならエラーを返す。
                for(i = 0; i < recvLen-1; i++){
                    if(!isdigit(recvBuf[i])){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangelevelに代入
                sscanf(recvBuf, "%d", &changelevel);
                //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangeseasonに代入
                    sscanf(recvBuf, "%d", &changeseason);
                    //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                    if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(isdigit(recvBuf[i]) == 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    sscanf(recvBuf, "%d", &changestore2);
                    //テーブル名：store_tのstore_idにchangestore2があるかを確認。なければエラーを返す。
                    sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore2); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //テーブル名：menu_storage_tのstore_idがchangestore2かつmenu_idがchangeidのものがないかを確認。あればエラーを返す。
                    sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE store_id = %d AND menu_id = %d;", changestore2, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //テーブル名：menu_storage_tのstore_idがchangestoreかつmenu_idがchangeidのもののstore_idをchangestore2に変更
                    sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE store_id = %d AND menu_id = %d;", changestore2, changestore, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(changelevel == 4){
                    //どの地域IDに変更しますか。2桁：半角数字で入力してください。と聞く。
                    sprintf(sendBuf, "どの地域IDに変更しますか？%s 2桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //3文字以外の場合はエラーを返す
                    if(strlen(recvBuf) != 2){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(isdigit(recvBuf[i]) == 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    sscanf(recvBuf, "%d", &changeregion);
                    //テーブル名：region_tのregion_idにchangeregionがあるかを確認。なければエラーを返す。
                    sprintf(sendBuf, "SELECT COUNT(*) FROM region_t WHERE region_id = %d;", changeregion); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //テーブル名：menu_detail_tのidの内容をchangeregionに変更
                    sprintf(sendBuf, "UPDATE menu_detail_t SET id = %d WHERE menu_id = %d;", changeregion, changeid); //SQL文作成
                    res = PQexec(con, sendBuf);
                    PQclear(res);
                }
                //テーブル名：menu_detail_tのlayerの内容をchangelevelに変更する。このとき、menu_idがchangeidであるもののみ変更する。
                sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d;", changelevel, changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resの中身をクリア
                if(changelevel == 3){
                    //テーブル名：user_authority_tのstore_idとchangestore2が一致する行のuser_idを取得
                    sprintf(sendBuf, "SELECT user_id FROM user_authority_t WHERE store_id = %d;", changestore2);
                    res = PQexec(con, sendBuf);
                    //user_idをu_idに格納
                    sscanf(PQgetvalue(res, 0, 0), "%d", &u_id);
                    PQclear(res);
                    //テーブル名：menu_charge_tのmenuidがchangeidものを見つけ、user_idをu_idに変更
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                }else{
                    //テーブル名：menu_charge_tのuser_idに0を格納する。
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }
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
                    }else if(strcmp(changestar, "no") == 0){
                        //何も変更しませんでしたと返す。
                        sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                    }else{
                        //使用不可のコマンドと返す。
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                    }else if(strcmp(changestar, "no") == 0){
                        //何も変更しませんでしたと返す。
                        sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                    }else{
                        //使用不可のコマンドと返す。
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                }else{
                //使用不可のコマンドと返す。
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
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
                sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                //テーブル名：menu_detail_tでlayerの値が5のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
                sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 5);"); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //実行結果がなければ、変更できるメニューは存在しないことを表示
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                //実行結果を表示
                for(int i = 0; i < PQntuples(res); i++){
                    sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
                if(strlen(recvBuf) != 4){
                    sendLen = sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }
                //入力された文字が数字以外ならエラーを返す。
                for(int i = 0; i < recvLen-1; i++){
                    if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }
                //クライアントから受信した値をchangeidに代入
                sscanf(recvBuf, "%d", &changeid);
                //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
                sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                if(PQntuples(res) == 0){ //menu_idが存在しない場合
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
                PQclear(res); //resの中身をクリア
                //テーブル名：menu_detail_tのmenu_idがchangeidと一致するもののlayerの値が5であるかどうかを確認
                sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer = 5;", changeid); 
                res = PQexec(con, sendBuf); //SQL文実行
                if(PQntuples(res) == 0){ //menu_idが存在しない場合
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
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
                    //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
                    sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", changename);
                    res = PQexec(con, sendBuf);
                    //存在している場合は、存在していることを伝える。
                    if(PQntuples(res) != 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }
                    //テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                    sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d;", changename, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "price") == 0){
                    //現在の価格は以下の通りです。と表示
                    sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
                    sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //実行結果をクライアントに送信
                    for(int i = 0; i < PQntuples(res); i++){
                        sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                    }
                    PQclear(res); //resの中身をクリア
                    sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangepriceに代入
                    sscanf(recvBuf, "%d", &changeprice);
                    //テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：menu_price_tのpriceの内容をchangepriceに変更
                    sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d;", changeprice, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else if(strcmp(changeitem, "level") == 0){
                    //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                    sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangelevelに代入
                    sscanf(recvBuf, "%d", &changelevel);
                    //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                    if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangeseasonに代入
                        sscanf(recvBuf, "%d", &changeseason);
                        //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                        if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                        sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d;", changeseason, changeid); //SQL文作成
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
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(isdigit(recvBuf[i]) == 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        sscanf(recvBuf, "%d", &changestore2);
                        //テーブル名：store_tのstore_idにchangestore2があるかを確認。なければエラーを返す。
                        sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore2); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_storage_tのstore_idがchangestore2かつmenu_idがchangeidのものがないかを確認。あればエラーを返す。
                        sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE store_id = %d AND menu_id = %d;", changestore2, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_storage_tのstore_idがchangestoreかつmenu_idがchangeidのもののstore_idをchangestore2に変更
                        sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE store_id = %d AND menu_id = %d;", changestore2, changestore, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(changelevel == 4){
                        //どの地域IDに変更しますか。2桁：半角数字で入力してください。と聞く。
                        sprintf(sendBuf, "どの地域IDに変更しますか？%s 2桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //3文字以外の場合はエラーを返す
                        if(strlen(recvBuf) != 2){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(isdigit(recvBuf[i]) == 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        sscanf(recvBuf, "%d", &changeregion);
                        //テーブル名：region_tのregion_idにchangeregionがあるかを確認。なければエラーを返す。
                        sprintf(sendBuf, "SELECT region_id FROM region_t WHERE region_id = %d;", changeregion); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        if(PQntuples(res) == 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        //テーブル名：menu_detail_tのidの内容をchangeregionに変更
                        sprintf(sendBuf, "UPDATE menu_detail_t SET id = %d WHERE menu_id = %d;", changeregion, changeid); //SQL文作成
                        res = PQexec(con, sendBuf);
                        PQclear(res);
                    }
                    //テーブル名：menu_detail_tのlayerの内容をchangelevelに変更する。このとき、menu_idがchangeidであるもののみ変更する。
                    sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d;", changelevel, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                    if(changelevel == 3){
                        //テーブル名：user_authority_tのstore_idとchangestore2が一致する行のuser_idを取得
                    sprintf(sendBuf, "SELECT user_id FROM user_authority_t WHERE store_id = %d;", changestore2);
                    res = PQexec(con, sendBuf);
                    //user_idをu_idに格納
                    sscanf(PQgetvalue(res, 0, 0), "%d", &u_id);
                    PQclear(res);
                    //テーブル名：menu_charge_tのmenuidがchangeidものを見つけ、user_idをu_idに変更
                    sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                    }else{
                        //テーブル名：menu_charge_tのuser_idに0を格納する。
                        sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }
                }else if(strcmp(changeitem, "star") == 0){
                    //menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの値を確認する。
                    sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
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
                        //changestarがyesの場合、テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を1に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                            sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドと返す。
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                        //changestarがyesの場合、テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を0に変更
                        if(strcmp(changestar, "yes") == 0){
                            sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(strcmp(changestar, "no") == 0){
                            //何も変更しませんでしたと返す。
                            sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }else{
                            //使用不可のコマンドと返す。
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                }else if(strcmp(changeitem, "season") == 0){
                    sprintf(sendBuf, "どの季節のメニューにしますか？%s 1：春メニュー、2：夏メニュー、3：秋メニュー、4：冬メニューです。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                    //入力された文字が数字以外ならエラーを返す。
                    for(i = 0; i < recvLen-1; i++){
                        if(!isdigit(recvBuf[i])){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangeseasonに代入
                    sscanf(recvBuf, "%d", &changeseason);
                    //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                    if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                    sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d;", changeseason, changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    PQclear(res); //resの中身をクリア
                }else{
                    //使用不可のコマンドと返す。
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }else if(strcmp(response, "no") == 0){
                //情報を変更したいものがリージョナルメニューかどうかを聞く。
                sprintf(sendBuf, "情報を変更したいものはリージョナルメニューですか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                //受信した内容をresponseに代入
                sscanf(recvBuf, "%s", response);
                if(strcmp(response, "yes") == 0){
                    sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    //テーブル名：menu_detail_tでlayerの値が4のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
                    sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 4);"); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    //実行結果がなければ、変更できるメニューは存在しないことを表示
                    if(PQntuples(res) == 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }

                    //実行結果を表示
                    for(int i = 0; i < PQntuples(res); i++){
                        sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
                    if(strlen(recvBuf) != 4){
                        sendLen = sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }
                    //入力された文字が数字以外ならエラーを返す。
                    for(int i = 0; i < recvLen-1; i++){
                        if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangeidに代入
                    sscanf(recvBuf, "%d", &changeid);
                    //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
                    sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(PQntuples(res) == 0){ //menu_idが存在しない場合
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_detail_tのmenu_idがchangeidと一致するもののlayerの値が4であるかどうかを確認
                    sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer = 4;", changeid); 
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(PQntuples(res) == 0){ //menu_idが存在しない場合
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    PQclear(res); //resの中身をクリア
                    sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, star, regionです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
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
                        //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
                        sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", changename);
                        res = PQexec(con, sendBuf);
                        //存在している場合は、存在していることを伝える。
                        if(PQntuples(res) != 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            return -1;
                        }
                        //テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                        sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d;", changename, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(strcmp(changeitem, "price") == 0){
                        //現在の価格は以下の通りです。と表示
                        sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
                        sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        //実行結果をクライアントに送信
                        for(int i = 0; i < PQntuples(res); i++){
                            sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }
                        PQclear(res); //resの中身をクリア
                        sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangepriceに代入
                        sscanf(recvBuf, "%d", &changeprice);
                        //テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：menu_price_tのpriceの内容をchangepriceに変更
                        sprintf(sendBuf, "UPDATE menu_price_t SET price = %d WHERE menu_id = %d;", changeprice, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(strcmp(changeitem, "level") == 0){
                        //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                        sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangelevelに代入
                        sscanf(recvBuf, "%d", &changelevel);
                        //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                        if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            //クライアントから受信した値をchangeseasonに代入
                            sscanf(recvBuf, "%d", &changeseason);
                            //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                            if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                            sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d;", changeseason, changeid); //SQL文作成
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
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //入力された文字が数字以外ならエラーを返す。
                            for(i = 0; i < recvLen-1; i++){
                                if(isdigit(recvBuf[i]) == 0){
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            sscanf(recvBuf, "%d", &changestore2);
                            //テーブル名：store_tのstore_idにchangestore2があるかを確認。なければエラーを返す。
                            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore2); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_storage_tのstore_idがchangestore2かつmenu_idがchangeidのものがないかを確認。あればエラーを返す。
                            sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE store_id = %d AND menu_id = %d;", changestore2, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                             //テーブル名：menu_storage_tのstore_idがchangestoreかつmenu_idがchangeidのもののstore_idをchangestore2に変更
                            sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE store_id = %d AND menu_id = %d;", changestore2, changestore, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(changelevel == 4){
                            //どの地域IDに変更しますか。2桁：半角数字で入力してください。と聞く。
                            sprintf(sendBuf, "どの地域IDに変更しますか？%s 2桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                            //3文字以外の場合はエラーを返す
                            if(strlen(recvBuf) != 2){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //入力された文字が数字以外ならエラーを返す。
                            for(i = 0; i < recvLen-1; i++){
                                if(isdigit(recvBuf[i]) == 0){
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            sscanf(recvBuf, "%d", &changeregion);
                            //テーブル名：region_tのregion_idにchangeregionがあるかを確認。なければエラーを返す。
                            sprintf(sendBuf, "SELECT region_id FROM region_t WHERE region_id = %d;", changeregion); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(PQntuples(res) == 0){ //region_idが存在しない場合
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_detail_tのidの内容をchangeregionに変更
                            sprintf(sendBuf, "UPDATE menu_detail_t SET id = %d WHERE menu_id = %d;", changeregion, changeid); //SQL文作成
                            res = PQexec(con, sendBuf);
                            PQclear(res);
                        }
                        //テーブル名：menu_detail_tのlayerの内容をchangelevelに変更する。このとき、menu_idがchangeidであるもののみ変更する。
                        sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d;", changelevel, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                        if(changelevel == 3){
                            //テーブル名：user_authority_tのstore_idとchangestore2が一致する行のuser_idを取得
                            sprintf(sendBuf, "SELECT user_id FROM user_authority_t WHERE store_id = %d;", changestore2);
                            res = PQexec(con, sendBuf);
                            //user_idをu_idに格納
                            sscanf(PQgetvalue(res, 0, 0), "%d", &u_id);
                            PQclear(res);
                            //テーブル名：menu_charge_tのmenuidがchangeidものを見つけ、user_idをu_idに変更
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                        }else{
                            //テーブル名：menu_charge_tのuser_idに0を格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }
                    }else if(strcmp(changeitem, "star") == 0){
                        //menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの値を確認する。
                        sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
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
                            //changestarがyesの場合、テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を1に変更
                            if(strcmp(changestar, "yes") == 0){
                                sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d;", changeid); //SQL文作成
                                res = PQexec(con, sendBuf); //SQL文実行
                                PQclear(res); //resの中身をクリア
                            }else if(strcmp(changestar, "no") == 0){
                                //何も変更しませんでしたと返す。
                                sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                            }else{
                                //使用不可のコマンドと返す。
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                            //changestarがyesの場合、テーブル名：menu_storage_tのchangeidと同じmenu_idを持つ、テーブル名：push_tのpush_hqの内容を0に変更
                            if(strcmp(changestar, "yes") == 0){
                                sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                                res = PQexec(con, sendBuf); //SQL文実行
                                PQclear(res); //resの中身をクリア
                            }else if(strcmp(changestar, "no") == 0){
                                //何も変更しませんでしたと返す。
                                sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                            }else{
                                //使用不可のコマンドと返す。
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                    }else if(strcmp(changeitem, "region") == 0){
                        sprintf(sendBuf, "どの地域のメニューにしますか？%s 2桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangeregionに代入
                        sscanf(recvBuf, "%d", &changeregion);
                        //changeregionの値がテーブル名：region_tのregion_idに存在するかどうかを確認
                        sprintf(sendBuf, "SELECT region_id FROM region_t WHERE region_id = %d", changeregion); 
                        res = PQexec(con, sendBuf); //SQL文実行
                        if(PQntuples(res) == 0){ 
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                        PQclear(res); //resの中身をクリア
                        //テーブル名：menu_detail_tのidの内容をchangeregionに変更
                        sprintf(sendBuf, "UPDATE menu_detail_t SET id = %d WHERE menu_id = %d;", changeregion, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else{
                        //使用不可のコマンドと返す。
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }else if(strcmp(response, "no") == 0){
                    sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    //テーブル名：menu_detail_tでlayerの値が1, 2のもののmenu_idとそのmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
                    sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 1 OR layer = 2);"); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(PQntuples(res) == 0){
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //実行結果を表示
                    for(int i = 0; i < PQntuples(res); i++){
                        sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    //入力された文字が数字以外ならエラーを返す。
                    for(int i = 0; i < recvLen-1; i++){
                        if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            return -1;
                        }
                    }
                    //クライアントから受信した値をchangeidに代入
                    sscanf(recvBuf, "%d", &changeid);
                    //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
                    sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", changeid); //SQL文作成
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(PQntuples(res) == 0){ //menu_idが存在しない場合
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                    PQclear(res); //resの中身をクリア
                    //テーブル名：menu_detail_tのmenu_idがchangeidと一致するもののlayerの値が1, 2, 4であるかどうかを確認
                    sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer = 1 OR layer = 2;", changeid); 
                    res = PQexec(con, sendBuf); //SQL文実行
                    if(PQntuples(res) == 0){ //menu_idが存在しない場合
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                        //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
                        sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", changename);
                        res = PQexec(con, sendBuf);
                        //存在している場合は、存在していることを伝える。
                        if(PQntuples(res) != 0){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            return -1;
                        }
                        //changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                        sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d;", changename, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                    }else if(strcmp(changeitem, "price") == 0){
                        //現在の価格は以下の通りです。と表示
                        sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：menu_price_tからmenu_idがchangeidのもののpriceを表示
                        sprintf(sendBuf, "SELECT price FROM menu_price_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        //実行結果をクライアントに送信
                        for(int i = 0; i < PQntuples(res); i++){
                            sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                        }
                        PQclear(res); //resの中身をクリア
                        sprintf(sendBuf, "どんな値段にしますか？%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(int i = 0; i < recvLen; i++){
                            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
                    }else if(strcmp(changeitem, "level") == 0){
                        //どのメニューレベルにするかを聞く。選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューであることも伝える。
                        sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        //入力された文字が数字以外ならエラーを返す。
                        for(i = 0; i < recvLen-1; i++){
                            if(!isdigit(recvBuf[i])){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                        //クライアントから受信した値をchangelevelに代入
                        sscanf(recvBuf, "%d", &changelevel);
                        //changelevelの値が1, 2, 3, 4, 5以外の場合、エラーを返す。
                        if(changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4 && changelevel != 5){
                            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            //クライアントから受信した値をchangeseasonに代入
                            sscanf(recvBuf, "%d", &changeseason);
                            //changeseasonの値が1, 2, 3, 4以外の場合、エラーを返す。
                            if(changeseason != 1 && changeseason != 2 && changeseason != 3 && changeseason != 4){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_detail_tのseasonの内容をchangeseasonに変更
                            sprintf(sendBuf, "UPDATE menu_detail_t SET season = %d WHERE menu_id = %d;", changeseason, changeid); //SQL文作成
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
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //入力された文字が数字以外ならエラーを返す。
                            for(i = 0; i < recvLen-1; i++){
                                if(isdigit(recvBuf[i]) == 0){
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            sscanf(recvBuf, "%d", &changestore2);
                            //テーブル名：store_tのstore_idにchangestore2があるかを確認。なければエラーを返す。
                            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore2); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_storage_tのstore_idがchangestore2かつmenu_idがchangeidのものがないかを確認。あればエラーを返す。
                            sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE store_id = %d AND menu_id = %d;", changestore2, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(strcmp(PQgetvalue(res, 0, 0), "0") != 0){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1705, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_storage_tのstore_idがchangestoreかつmenu_idがchangeidのもののstore_idをchangestore2に変更
                            sprintf(sendBuf, "UPDATE menu_storage_t SET store_id = %d WHERE store_id = %d AND menu_id = %d;", changestore2, changestore, changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }else if(changelevel == 4){
                            //どの地域IDに変更しますか。2桁：半角数字で入力してください。と聞く。
                            sprintf(sendBuf, "どの地域IDに変更しますか？%s 2桁：半角数字で入力してください。%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                            //3文字以外の場合はエラーを返す
                            if(strlen(recvBuf) != 2){
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //入力された文字が数字以外ならエラーを返す。
                            for(i = 0; i < recvLen-1; i++){
                                if(isdigit(recvBuf[i]) == 0){
                                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    return -1;
                                }
                            }
                            sscanf(recvBuf, "%d", &changeregion);
                            //changeregionの値がテーブル名：region_tのregion_idに存在するかどうかを確認
                            sprintf(sendBuf, "SELECT region_id FROM region_t WHERE region_id = %d", changeregion);
                            res = PQexec(con, sendBuf); //SQL文実行
                            if(PQntuples(res) == 0){ 
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                            //テーブル名：menu_detail_tのidの内容をchangeregionに変更
                            sprintf(sendBuf, "UPDATE menu_detail_t SET id = %d WHERE menu_id = %d;", changeregion, changeid); //SQL文作成
                            res = PQexec(con, sendBuf);
                            PQclear(res);
                        }
                        //テーブル名：menu_detail_tのlayerの内容をchangelevelに変更する。このとき、menu_idがchangeidであるもののみ変更する。
                        sprintf(sendBuf, "UPDATE menu_detail_t SET layer = %d WHERE menu_id = %d;", changelevel, changeid); //SQL文作成
                        res = PQexec(con, sendBuf); //SQL文実行
                        PQclear(res); //resの中身をクリア
                        if(changelevel == 3){
                            //テーブル名：user_authority_tのstore_idとchangestore2が一致する行のuser_idを取得
                            sprintf(sendBuf, "SELECT user_id FROM user_authority_t WHERE store_id = %d;", changestore2);
                            res = PQexec(con, sendBuf);
                            //user_idをu_idに格納
                            sscanf(PQgetvalue(res, 0, 0), "%d", &u_id);
                            PQclear(res);
                            //テーブル名：menu_charge_tのmenuidがchangeidものを見つけ、user_idをu_idに変更
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = %d WHERE menu_id = %d;", u_id, changeid); //SQL文作成
                        }else{
                            //テーブル名：menu_charge_tのuser_idに0を格納する。
                            sprintf(sendBuf, "UPDATE menu_charge_t SET user_id = 0 WHERE menu_id = %d;", changeid); //SQL文作成
                            res = PQexec(con, sendBuf); //SQL文実行
                            PQclear(res); //resの中身をクリア
                        }
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
                            }else if(strcmp(changestar, "no") == 0){
                                //何も変更しませんでしたと返す。
                                sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                            }else{
                                //使用不可のコマンドですと返す。
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                            }else if(strcmp(changestar, "no") == 0){
                                //何も変更しませんでしたと返す。
                                    sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                            }else{
                                //使用不可のコマンドですと返す。
                                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                return -1;
                            }
                        }
                    }else{
                        //使用不可のコマンドですと返す。
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        return -1;
                    }
                }else{
                    //使用不可のコマンドですと返す。
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }else{
                //使用不可のコマンドですと返す。
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }else{
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
    }else if(u_auth == ACOR){
        sprintf(sendBuf, "選べる店舗IDです．%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        //テーブル名：region_tからu_storeと同じregion_idを持つstore_idを取得
        sprintf(sendBuf, "SELECT store_id FROM region_t WHERE region_id = %d;", u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1つも無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1706, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //メモリ解放
        sprintf(sendBuf, "メニュー一覧です．%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        //changestoreと同じ値のstore_idを持っているmenu_idを表示、またテーブル名：menu_storage_tから、そのmenu_idのmenu_nameをテーブル名：recipe_tから取得
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name FROM menu_storage_t INNER JOIN recipe_t ON menu_storage_t.menu_id = recipe_t.menu_id WHERE menu_storage_t.store_id = %d;", changestore); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //1つも無ければエラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行結果（menuidとmenuname）を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "menu_id:%s menu_name:%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
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
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
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
                sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else{
                //使用不可のコマンドですと表示
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
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
                sprintf(sendBuf, "何も変更しませんでした．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else{
                //使用不可のコマンドですと表示
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
    }
    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
    sendLen = strlen(sendBuf); //送信データ長
    send(soc, sendBuf, sendLen, 0); //送信
    return 0;
}