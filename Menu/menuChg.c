#include "omos.h"

int menuChg(PGconn *__con, int __soc, int *__auth){
    int recvLen, sendLen;　//送受信データ長
    int chengeid; //変更する商品ID
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]　//送受信用バッファ
    char chengeitem, changename, changeprice, changestar;　//変更内容
    pthread_t selfId = pthread_self();  //スレッド
    PGresult *res;　//PGresult型の変数resを宣言

    if(__auth == AMGR){
        sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        //テーブル名：push_tからlayerの値が4のもののmenu_idを取得し、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT menu_id, menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM push_t WHERE layer = 4);"); //SQL文作成
        res = PQexec(__con, sendBuf); //SQL文実行
        //クライアントにresの内容を送信
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf , sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        }
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した値をchengeidに代入
        sscanf(recvBuf, "%d", &chengeid);
        //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
        sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", chengeid);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        if(PQntuples(res) == 0){　//menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        //何を変更したいかを聞く。選択肢はname, price, level, starであることも伝える。
        sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, starです．%s", ENTER, ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した変更内容をchengeitemに代入
        sscanf(recvBuf, "%s", chengeitem);
        //変更内容がnameの場合、どう変更するかを聞く。
        if(strcmp(chengeitem, "name") == 0){
            sprintf(sendBuf, "どんな商品名にしますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchengenameに代入
            sscanf(recvBuf, "%s", changename);
            //テーブル名：recipe_tのmenu_nameの内容をchangenameに変更
            sprintf(sendBuf, "UPDATE recipe_t SET menu_name = %s WHERE menu_id = %d;", changename, chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            return 0;
        }else if(strcmp(chengeitem, "price") == 0){
            //現在の価格は以下の通りです。と表示
            sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //テーブル名：price_charge_tからmenu_idがchengeidのもののpriceを表示
            sprintf(sendBuf, "SELECT price FROM price_charge_t WHERE menu_id = %d;", chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //クライアントにresの内容を送信
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s", PQgetvalue(res, i, 0));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            //変更内容がpriceの場合、どう変更するかを聞く。
            sprintf(sendBuf, "値段はいくらに変更しますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangepriceに代入
            sscanf(recvBuf, "%s", changeprice);
            //テーブル名：recipe_tのpriceの内容をchangepriceに変更
            sprintf(sendBuf, "UPDATE recipe_t SET price = %s WHERE menu_id = %d;", changeprice, chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            return 0;
        }else if(strcmp(chengeitem, "star") == 0){
            sprintf(sendBuf, "メニュー一覧です．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            sprintf(sendBuf, "SELECT menu_id, menu_name FROM recipe_t;");　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //テーブル名：recipe_tのmenu_id, menu_nameを表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
            }
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した値をchengeidに代入
            sscanf(recvBuf, "%d", &chengeid);
            //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            if(PQntuples(res) == 0){　//menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
               send(__soc, sendBuf, sendLen, 0);　//送信
               return -1;
            }
            //テーブル名：push_tのpush_hqの値を確認する。
            sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d;", chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //push_hqの値が0の場合、押しメニューにしますか？と聞く。
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_hqの値を1に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d;", chengeid);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                    return 0;
                }else{
                    return 0;
                }
            }else if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_hqの値を0に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d;", chengeid);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                    return 0;
                }else{
                    return 0;
                }
            }
        }else{
            //打ち込まれたコマンドが使えないことを表示
            sprintf(sendBuf, "そのコマンドは使えません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return 0;
        }
    }else if(__auth == AHQ){
        sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        sprintf(sendBuf, "SELECT menu_id, menu_name FROM recipe_t;");　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //テーブル名：recipe_tのmenu_id, menu_nameを表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
        }
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した値をchengeidに代入
        sscanf(recvBuf, "%d", &chengeid);
        //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
        sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", chengeid);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        if(PQntuples(res) == 0){　//menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        //何を変更したいかを聞く。選択肢はname, price, level, starであることも伝える。
        sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, starです．%s", ENTER, ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した変更内容をchengeitemに代入
        sscanf(recvBuf, "%s", chengeitem);
        //変更内容がnameの場合、どう変更するかを聞く。
        if(strcmp(chengeitem, "name") == 0){
            sprintf(sendBuf, "どんな商品名にしますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchengenameに代入
            sscanf(recvBuf, "%s", changename);
            //テーブル名：recipe_tのmenu_nameの内容をchangenameに変更
            sprintf(sendBuf, "UPDATE recipe_t SET menu_name = %s WHERE menu_id = %d;", changename, chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            return 0;
        }else if(strcmp(chengeitem, "price") == 0){
            //現在の価格は以下の通りです。と表示
            sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //テーブル名：price_charge_tからmenu_idがchengeidのもののpriceを表示
            sprintf(sendBuf, "SELECT price FROM price_charge_t WHERE menu_id = %d;", chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //クライアントにresの内容を送信
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s", PQgetvalue(res, i, 0));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            //変更内容がpriceの場合、どう変更するかを聞く。
            sprintf(sendBuf, "値段はいくらに変更しますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangepriceに代入
            sscanf(recvBuf, "%s", changeprice);
            //テーブル名：recipe_tのpriceの内容をchangepriceに変更
            sprintf(sendBuf, "UPDATE recipe_t SET price = %s WHERE menu_id = %d;", changeprice, chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            return 0;
        }else if(strcmp(chengeitem, "level") == 0){
            //どのメニューレベルにするかを聞く。選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューであることも伝える。
            sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューです．%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //テーブル名：push_tのlayerの内容をrecvBufに変更
            sprintf(sendBuf, "UPDATE recipe_t SET layer = %s WHERE menu_id = %d;", recvBuf, chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            return 0;
        }else if(strcmp(chengeitem, "star") == 0){
            //テーブル名：push_tのpush_mgrの値を確認する。
            sprintf(sendBuf, "SELECT push_mgr FROM push_t WHERE menu_id = %d;", chengeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //push_mgrの値が0の場合、押しメニューにしますか？と聞く。
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_mgrの値を1に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_mgr = 1 WHERE menu_id = %d;", chengeid);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                    return 0;
                }else{
                    return 0;
                }
            }else if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_mgrの値を0に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_mgr = 0 WHERE menu_id = %d;", chengeid);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                    return 0;
                }else{
                    return 0;
                }
            }
        }else{
            //打ち込まれたコマンドが使えないことを表示
            sprintf(sendBuf, "そのコマンドは使えません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return 0;
        }
    }else if(__auth == ACOR){
        sprintf(sendBuf, "メニュー一覧です．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        sprintf(sendBuf, "SELECT menu_id, menu_name FROM recipe_t;");　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //テーブル名：recipe_tのmenu_id, menu_nameを表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
        }
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した値をchengeidに代入
        sscanf(recvBuf, "%d", &chengeid);
        //クライアントから受信したmenu_idがテーブル名：recipe_tに存在するか確認
        sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d;", chengeid);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        if(PQntuples(res) == 0){　//menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        //テーブル名：push_tのpush_corの値を確認する。
        sprintf(sendBuf, "SELECT push_cor FROM push_t WHERE menu_id = %d;", chengeid);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //push_mgrの値が0の場合、押しメニューにしますか？と聞く。
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_corの値を1に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 1 WHERE menu_id = %d;", chengeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
                return 0;
            }else{
                return 0;
            }
        }else if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：push_tのpush_corの値を0に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 0 WHERE menu_id = %d;", chengeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
                return 0;
            }else{
                return 0;
            }
        }
    }
    return 0;
}