#include "OMOS.h"

int auth_Chg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    char phoneNum[12];  //変更後の電話番号

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }

    int auth = u_info[1];
    printf("auth: %d\n", auth);

    if( auth == 2 ){
        sprintf(sendBuf, "社員の所属する店舗の変更を行います。%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        sprintf(sendBuf, "社員の電話番号を入力してください(09012345678)。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        //入力が数字11桁の場合
        int is_valid = 1;
        for(int i=0; i<strlen(recvBuf); i++){
            if(!isdigit(recvBuf[i])){
                is_valid = 0;
                break;
            }
        }
        if( strlen(recvBuf) == 11 && is_valid ){
            strcpy(phoneNum, recvBuf);
            printf("phoneNum: %s\n", phoneNum);

            //phoneNumの内容をuser_tのuser_phoneに存在するかを確かめる
            sprintf(sql, "SELECT * FROM user_t WHERE user_phone = '%s'", phoneNum);
            res = PQexec(con, sql);
            printf("%s\n", sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("SELECT failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                send(soc, sendBuf, sendLen, 0);
            }

            //検索結果が1件だった場合、
            if(PQntuples(res) == 1){
                //新規店舗を登録する場合は"1",既存店舗を登録する場合は"0"を入力してください
                sprintf(sendBuf, "新規店舗を登録する場合は\"1\",既存店舗を登録する場合は\"0\"を入力してください。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen-1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                //入力が"1"の場合
                if( strcmp(recvBuf, "1") == 0 ){
                    //新規店舗のIDを入力してください
                    sprintf(sendBuf, "新規店舗のIDを入力してください(100-)。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                    recvBuf[recvLen-1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    ////もし数字以外が入力された場合、
                    int is_valid = 1;
                    for(int i=0; i<strlen(recvBuf); i++){
                        if(!isdigit(recvBuf[i])){
                            is_valid = 0;
                            sprintf(sendBuf, "入力が不正です。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            return -1;
                        }
                    }

                    //入力内容をchar storeid[]に格納する
                    char storeid[10];
                    strcpy(storeid, recvBuf);
                    printf("storeid: %s\n", storeid);

                    //storeidの内容をstore_tのstore_idに存在するかを確かめる
                    sprintf(sql, "SELECT * FROM store_t WHERE store_id = '%s'", storeid);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("SELECT failed: %s", PQerrorMessage(con));
                        PQclear(res);
                        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                    }

                    //検索結果が0件だった場合、
                    if( PQntuples(res) == 0 ){
                        //新規店舗の名前を入力してください
                        sprintf(sendBuf, "新規店舗の名前を入力してください。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //入力内容をstorenameに格納する
                        char storename[20];
                        strcpy(storename, recvBuf);
                        printf("storename: %s\n", storename);

                        //store_tに新規店舗を登録する
                        sprintf(sql, "INSERT INTO store_t(store_id, store_name) VALUES(%s, '%s')", storeid, storename);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_COMMAND_OK){
                            printf("INSERT failed: %s", PQerrorMessage(con));
                            PQclear(res);
                            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                        }

                        //新規店舗登録完了
                        sprintf(sendBuf, "新規店舗登録完了%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    }else{
                        //入力された店舗IDはすでに存在しています
                        sprintf(sendBuf, "入力された店舗IDはすでに存在しています。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        return -1;
                    }

                    //店舗の所属する地域ID(3)、地域名、チェーンIDを入力してください
                    sprintf(sendBuf, "店舗の所属する地域ID(ex:3)を入力してください。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                    recvBuf[recvLen-1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    //数字以外が入力された場合,
                    is_valid = 1;
                    for(int i=0; i<strlen(recvBuf); i++){
                        if(!isdigit(recvBuf[i])){
                            is_valid = 0;
                            sprintf(sendBuf, "入力が不正です。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            break;
                        }
                    }

                    //入力内容をchar areaid[]に格納する
                    char areaid[10];
                    strcpy(areaid, recvBuf);
                    printf("areaid: %s\n", areaid);

                    //areaidの内容をregion_tのregion_idに存在するかを確かめる
                    sprintf(sql, "SELECT * FROM region_t WHERE region_id = '%s'", areaid);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("SELECT failed: %s", PQerrorMessage(con));
                        PQclear(res);
                        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                    }

                    //検索結果が0件だった場合、
                    if( PQntuples(res) == 0 ){
                        //入力された地域IDは存在しません
                        sprintf(sendBuf, "地域IDを新規登録しますか？(yes: 1, no: 0)%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //入力が"1"の場合
                        if( strcmp(recvBuf, "1") == 0 ){
                            //地域名を入力してください
                            sprintf(sendBuf, "地域名を入力してください。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                            recvBuf[recvLen-1] = '\0';
                            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                            //入力内容をchar regionname[]に格納する
                            char regionname[20];
                            strcpy(regionname, recvBuf);
                            printf("regionname: %s\n", regionname);

                            //region_tのstore_id=storeidに新規地域を登録する
                            sprintf(sql, "INSERT INTO region_t(store_id, region_id, region_name) VALUES(%s, %s, '%s')", storeid, areaid, regionname);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("INSERT failed: %s", PQerrorMessage(con));
                                PQclear(res);
                                sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                            }

                            //チェーンIDを入力してください
                            sprintf(sendBuf, "チェーンIDを入力してください。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                            recvBuf[recvLen-1] = '\0';
                            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                            //入力内容をchar chainid[]に格納する
                            char chainid[10];
                            strcpy(chainid, recvBuf);
                            printf("chainid: %s\n", chainid);

                            //入力されたチェーンIDがchain_tのchain_idに存在するか
                            sprintf(sql, "SELECT * FROM chain_t WHERE chain_id = '%s'", recvBuf);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                                printf("SELECT failed: %s", PQerrorMessage(con));
                                //ROLLBACK
                                res = PQexec(con, "ROLLBACK");
                                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                    printf("ROLLBACK failed: %s", PQerrorMessage(con));
                                    PQclear(res);
                                    sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                    send(soc, sendBuf, sendLen, 0);
                                }
                                PQclear(res);
                                sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                            }

                            //検索結果が1件以上だった場合、
                            if( PQntuples(res) >= 1 ){
                                //入力されたチェーンIDはすでに存在しています
                                sprintf(sendBuf, "入力されたチェーンIDはすでに存在しています。このチェーンIDを登録しますか(y/n)%s%s", ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                                recvBuf[recvLen-1] = '\0';
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                                //入力が"y"の場合,入力されたチェーンIDと店舗ID(storeid)をchain_tのchain_id、store_id に新規登録する
                                if( strcmp(recvBuf, "y") == 0 ){
                                    sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", chainid, storeid);
                                    res = PQexec(con, sql);
                                    printf("%s\n", sql);
                                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                        printf("INSERT failed: %s", PQerrorMessage(con));
                                        //ROLLBACK
                                        res = PQexec(con, "ROLLBACK");
                                        if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                            printf("ROLLBACK failed: %s", PQerrorMessage(con));
                                            PQclear(res);
                                            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                            send(soc, sendBuf, sendLen, 0);
                                        }
                                        PQclear(res);
                                        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                        send(soc, sendBuf, sendLen, 0);
                                    }

                                    //新規登録完了
                                    sprintf(sendBuf, "新規登録完了%s%s", ENTER, DATA_END);
                                    sendLen = strlen(sendBuf);
                                    send(soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                                    return 0;
                                }else if( strcmp(recvBuf, "n") == 0 ){
                                    sprintf(sendBuf, "入力をやり直してください。%s%s", ENTER, DATA_END);
                                    return -1;  
                                }else{
                                    //入力が"y"でも"n"でもない場合
                                    sprintf(sendBuf, "入力が不正です。%s%s", ENTER, DATA_END);
                                    sendLen = strlen(sendBuf);
                                    send(soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                                    return -1;
                                }
                            }
                            else if( PQntuples(res) == 0 ){
                                //入力されたチェーンIDで新規登録します.
                                sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", recvBuf, storeid);
                                res = PQexec(con, sql);
                                printf("%s\n", sql);

                                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                    printf("INSERT failed: %s", PQerrorMessage(con));
                                    //ROLLBACK
                                    res = PQexec(con, "ROLLBACK");
                                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                        printf("ROLLBACK failed: %s", PQerrorMessage(con));
                                        PQclear(res);
                                        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                        send(soc, sendBuf, sendLen, 0);
                                    }
                                    PQclear(res);
                                    sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                    send(soc, sendBuf, sendLen, 0);
                                }

                                //新規登録完了
                                sprintf(sendBuf, "新規登録完了%s%s", ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            }

                        }else if( strcmp(recvBuf, "0") == 0 ){
                            //入力が"0"の場合,入力をやり直す
                            sprintf(sendBuf, "入力をやり直してください。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            
                            return -1;
                        }else{
                            //入力が"1"でも"0"でもない場合
                            sprintf(sendBuf, "入力が不正です。%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            return -1;
                        }
                    }
                    //検索結果が1件だった場合、
                    else if( PQntuples(res) >= 1 ){
                        sprintf(sendBuf, "SELECT * FROM region_t WHERE region_id = '%s'", areaid);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_TUPLES_OK){
                            printf("SELECT failed: %s", PQerrorMessage(con));
                            PQclear(res);
                            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                        }

                        //地域名region_nameを格納する
                        char region_name[20];
                        strcpy(region_name, PQgetvalue(res, 0, 2));
                        printf("region_name: %s\n", region_name);

                        //入力された地域IDはすでに存在しています
                        sprintf(sendBuf, "入力された地域ID/地域名はすでに存在しています[%s]。この地域IDを登録しますか(y/n)%s%s", region_name, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //入力が"y"の場合,入力された地域IDと店舗ID(storeid)をregion_tのregion_id、store_id に新規登録する
                        if( strcmp(recvBuf, "y") == 0 ){
                            sprintf(sql, "INSERT INTO region_t(store_id, region_id, region_name) VALUES(%s, %s, '%s');", storeid, areaid, region_name);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("INSERT failed: %s", PQerrorMessage(con));
                                //ROLLBACK
                                PQexec(con, "ROLLBACK");
                                PQclear(res);
                                sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);

                                return -1;
                            }

                            //新規登録完了
                            sprintf(sendBuf, "新規登録完了%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            return 0;

                        return 0;

                        }else if( strcmp(recvBuf, "n") == 0 ){
                            //入力が"n"の場合,入力をやり直す
                            sprintf(sendBuf, "入力をやり直してください。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            return -1;
                        }
                        else{
                            //入力が"y"でも"n"でもない場合
                            sprintf(sendBuf, "入力が不正です。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            return -1;
                        }
                    }
                }
                //入力が0の場合
                else if(strcmp(recvBuf, "0") == 0){
                    sprintf(sendBuf, "既存店舗を登録します。店舗IDを入力して下さい。%s%s", ENTER, DATA_END  );
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                    recvBuf[recvLen-1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    //入力内容をchar storeid[]に格納する
                    char storeid[10];
                    strcpy(storeid, recvBuf);
                    printf("storeid: %s\n", storeid);

                    //storeidの内容をstore_tのstore_idに存在するかを確かめる
                    sprintf(sql, "SELECT * FROM store_t WHERE store_id = '%s'", storeid);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);

                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("SELECT failed: %s", PQerrorMessage(con));
                        PQclear(res);
                        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                    }

                    //検索結果が0件だった場合、
                    if( PQntuples(res) == 0 ){
                        //入力された店舗IDは存在しません
                        sprintf(sendBuf, "入力された店舗IDは存在しません。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        return -1;
                    }
                    // user_authority_tとuser_tをuser_idで結合し、phoneNumで絞り込んで、store_id にstoreidを入力する
                    else if( PQntuples(res) == 1 ){
                        sprintf(sql, "UPDATE user_authority_t SET store_id = '%s' FROM user_t WHERE user_authority_t.user_id = user_t.user_id AND user_t.user_phone = '%s'", storeid, phoneNum);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_COMMAND_OK){
                            printf("UPDATE failed: %s", PQerrorMessage(con));
                            PQclear(res);
                            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                        }

                        //既存店舗登録完了
                        sprintf(sendBuf, "既存店舗登録完了%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        return 0;
                    }
                }
                else{
                    //入力が"1"でも"0"でもない場合
                    sprintf(sendBuf, "入力が不正です。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    return -1;
                }
            }
            //検索結果が0件だった場合、
            else if( PQntuples(res) == 0 ){
                //入力された電話番号は存在しません
                sprintf(sendBuf, "入力された電話番号は存在しません。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                return -1;
            }
            else{
                //入力された電話番号はすでに存在しています
                sprintf(sendBuf, "入力された電話番号はすでに存在しています。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                return -1;
            }
        }
        //入力が数字11桁以外の場合
        else{
            //入力された電話番号は不正です
            sprintf(sendBuf, "入力された電話番号は不正です。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            return -1;
        }
    }
    //トランザクション終了
    res = PQexec(con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }
    PQclear(res);

    return 0;
}