#include "omos.h"
#include "menu.h"

int menuDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    int u_id, u_auth, u_store, delid, changestore, changeid, i; //ユーザID、ユーザの持つ権限、ユーザの所属、削除したいメニューID、情報を変更したい店舗ID、情報を変更したいメニューID、ループカウンタ
    char delname[BUFSIZE], response[BUFSIZE]; //クライアントからの返答
    PGresult *res; //PGresult型の変数resを宣言

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if(u_auth == AMGR){
        //あなたが削除できるメニュー一覧です。と表示
        sprintf(sendBuf, "あなたが削除できるメニュー一覧です．%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        //テーブル名：menu_detail_tからidがu_storeと一致し、かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE id = %d);", u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s: %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
        PQclear(res); //resのメモリを解放
        //削除したい商品IDを入力してください。と表示
        sprintf(sendBuf, "削除したい商品ID（4桁：半角数字）を入力してください．（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
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
        //受信した値をdelidに代入
        sscanf(recvBuf, "%d", &delid);
        //テーブル名：menu_storage_tに存在するか確認。その際store_idの値はu_storeと一致するものを確認
        sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", delid, u_store); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){ //menu_idが存在しない場合
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        PQclear(res); //resのメモリを解放
        //テーブル名：recipe_tからmenu_idがdelidと一致するもののmenu_nameを取得して表示
        sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //実行結果をdelnameに格納
        sscanf(PQgetvalue(res, i, 0), "%s", delname);
        //実行しようとしている商品名が正しいか確認
        sprintf(sendBuf, "本当に%sを削除しますか？(y/n)%s%s", delname, ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //受信した内容をresponseに入れる
        sscanf(recvBuf, "%s", response);
        if(strcmp(response, "y") == 0){ //削除する場合
            //テーブル名：menu_charge_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_charge_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：recipe_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //テーブル名：push_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM push_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_price_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_price_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_detail_tからidがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_detail_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_storage_tからstore_idがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_storage_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                sprintf(sendBuf, "削除しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
        }else if(strcmp(response, "n") == 0){ //削除しない場合
            sprintf(sendBuf, "削除を中止しました．%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }else{
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
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
            //削除を実行したい店舗ID（3桁：半角数字）を聞く。
            sprintf(sendBuf, "削除を実行したい店舗ID（3桁：半角数字）を入力してください。（例：001）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //2文字以外の場合はエラーを返す
            if(strlen(recvBuf) !=3){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1702, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 3; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //受信した値をchangestoreに代入
            sscanf(recvBuf, "%d", &changestore);
            //削除を実行したい店舗IDが存在するかどうかをテーブル名store_tから確認
            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //テーブル名store_tから取得した値が0の場合、店舗IDが存在しないことを表示
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resのメモリを解放
            sprintf(sendBuf, "あなたが削除できるメニュー一覧です．%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            //テーブル名：menu_storage_tからstore_idがchangestoreと一致し、かつテーブル名：menu_detail_tでlayerの値が3のもののmenu_idを取得し表示、また、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
            sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 3) AND recipe_t.menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s: %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }
            PQclear(res); //resのメモリを解放
            sprintf(sendBuf, "どのメニューを削除しますか？商品ID（4桁：半角数字）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
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
            for(i = 0; i < 4; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1703, ENTER); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した値をdelidに代入
            sscanf(recvBuf, "%d", &delid);
            //テーブル名：menu_storage_tに存在するか確認。その際store_idの値はchangestoreと一致するものを確認
            sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", delid, changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            PQclear(res); //resのメモリを解放
            //テーブル名：recipe_tからmenu_idがdelidと一致するもののmenu_nameを取得して表示
            sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行結果をdelnameに格納
            sscanf(PQgetvalue(res, 0, 0), "%s", delname);
            //実行しようとしている商品名が正しいか確認
            sprintf(sendBuf, "本当に%sを削除しますか？(y/n)%s%s", delname, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //クライアントから受信した値をresponseに代入
            sscanf(recvBuf, "%s", response);
            if(strcmp(response, "y") == 0){ //削除する場合
                //テーブル名：menu_charge_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_charge_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：recipe_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //テーブル名：push_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM push_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_price_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_price_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_detail_tからidがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_detail_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_storage_tからstore_idがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_storage_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                sprintf(sendBuf, "削除しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else if(strcmp(response, "n") == 0){ //削除しない場合
                sprintf(sendBuf, "削除を中止しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1701, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }else if(strcmp(response, "no") == 0){
            //選べるメニュー一覧を表示
            sprintf(sendBuf, "あなたが削除できるメニュー一覧です．%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            //テーブル名：menu_detail_tでlayerの値が1, 2, 4, 5のもののmenu_idとそのmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
            sprintf(sendBuf, "SELECT recipe_t.menu_id, recipe_t.menu_name FROM recipe_t WHERE recipe_t.menu_id IN (SELECT menu_id FROM menu_detail_t WHERE layer = 1 OR layer = 2 OR layer = 4 OR layer = 5);"); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s: %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }
            PQclear(res); //resのメモリを解放
            //削除したい商品IDを入力してください。と表示
            sprintf(sendBuf, "削除したい商品ID（4桁：半角数字）を入力してください．（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
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

            //受信した値をdelidに代入
            sscanf(recvBuf, "%d", &delid);
            //テーブル名：menu_detail_tに存在するか確認。その際layerの値は1, 2, 4, 5のものを確認
            sprintf(sendBuf, "SELECT menu_id FROM menu_detail_t WHERE menu_id = %d AND layer IN (1, 2, 4, 5);", delid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1704, ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }

            PQclear(res); //resのメモリを解放
            //テーブル名：recipe_tからmenu_idがdelidと一致するもののmenu_nameを取得して表示
            sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行結果をdelnameに格納
            sscanf(PQgetvalue(res, i, 0), "%s", delname);
            //実行しようとしている商品名が正しいか確認
            sprintf(sendBuf, "本当に%sを削除しますか？(y/n)%s%s", delname, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //受信した内容をresponseに入れる
            sscanf(recvBuf, "%s", response);
            if(strcmp(response, "y") == 0){ //削除する場合
                //テーブル名：menu_charge_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_charge_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：recipe_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM recipe_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                //テーブル名：push_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM push_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_price_tからmenu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_price_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_detail_tからidがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_detail_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                //テーブル名：menu_storage_tからstore_idがu_storeと、menu_idがdelidと一致するものを削除
                sprintf(sendBuf, "DELETE FROM menu_storage_t WHERE menu_id = %d;", delid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                PQclear(res); //resのメモリを解放
                sprintf(sendBuf, "削除しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else if(strcmp(response, "n") == 0){ //削除しない場合
                sprintf(sendBuf, "削除を中止しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else{
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
    }
    return 0;
}