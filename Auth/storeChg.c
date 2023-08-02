#include "omos.h"
#include "store.h"

int storeChg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    char phoneNum[12];  //変更後の電話番号
    char newStoreId[10];

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }

    int auth = u_info[1]; //権限

    if(auth == 2){
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

        //recvBufの文字列をphoneNumにコピー
        strcpy(phoneNum, recvBuf);
        printf("phoneNum: %s\n", phoneNum);

        //電話番号がuser_tのuser_phoneに存在するか確認
        sprintf(sql, "SELECT * FROM user_t WHERE user_phone = '%s'", phoneNum);
        res = PQexec(con, sql);
        printf("%s\n", sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("SELECT failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
        }

        //電話番号が1件存在した場合、
        if(PQntuples(res) == 1){
            //新規店舗を登録する場合は"1",既存店舗を登録する場合は"0"を入力してください
            sprintf(sendBuf, "新規店舗を登録する場合は\"1\",既存店舗を登録する場合は\"0\"を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
            recvBuf[recvLen-1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

            //recvBufの文字列をnewStoreにコピー
            char newStore[2];
            strcpy(newStore, recvBuf);
            printf("newStore: %s\n", newStore);

            //新規店舗を登録する場合
            if(strcmp(newStore,"1") == 0){
                //新規店舗のIDを入力してください
                sprintf(sendBuf, "新規店舗のIDを入力してください(100-)。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen-1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                //recvBufの文字列をnewStoreIdにコピー
                strcpy(newStoreId, recvBuf);
                printf("newStoreId: %s\n", newStoreId);

                //新規店舗のIDがstore_tのstore_idに存在するか確認
                sprintf(sql, "SELECT * FROM store_t WHERE store_id = '%s'", newStoreId);
                res = PQexec(con, sql);
                printf("%s\n", sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("SELECT failed: %s", PQerrorMessage(con));
                    PQclear(res);
                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }

                //新規店舗のIDが0件だった場合、店舗名を入力してください
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "新規店舗名を入力してください。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                    recvBuf[recvLen-1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    //recvBufの文字列をnewStoreNameにコピー
                    char newStoreName[30];
                    strcpy(newStoreName, recvBuf);
                    printf("newStoreName: %s\n", newStoreName);

                    //新規店舗のIDと新規店舗名をstore_tのstore_id,store_nameに登録
                    sprintf(sql, "INSERT INTO store_t VALUES(%s,'%s')", newStoreId, newStoreName);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                        printf("INSERT failed: %s", PQerrorMessage(con));
                        //ロールバック
                        res = PQexec(con, "ROLLBACK");
                        PQclear(res);
                        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }

                    //続いて店舗の所属する地域IDを入力してもらう
                    sprintf(sendBuf, "店舗の所属する地域IDを入力してください。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                    recvBuf[recvLen-1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    //数字以外が入力された場合,
                    int is_valid = 1;
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

                    //recvBufの文字列をnewAreaIdにコピー
                    char newAreaId[10];
                    strcpy(newAreaId, recvBuf);
                    printf("newAreaId: %s\n", newAreaId);

                    //newAreaIDがregion_tのregion_idに存在するかを確かめる
                    sprintf(sql, "SELECT * FROM region_t WHERE region_id = '%s'", newAreaId);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("SELECT failed: %s", PQerrorMessage(con));
                        PQclear(res);
                        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }

                    //newAreaIDが0件だった時、
                    if( PQntuples(res) == 0 ){
                        //入力された地域IDは存在しません
                        sprintf(sendBuf, "地域IDを新規登録します%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        //地域名を入力してもらう
                        sprintf(sendBuf, "地域名を入力してください。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //recvBufの文字列をnewAreaNameにコピー
                        char newAreaName[30];
                        strcpy(newAreaName, recvBuf);
                        printf("newAreaName: %s\n", newAreaName);

                        //region_tのstore_id=newStoreIdに新規地域を登録する
                        sprintf(sql, "INSERT INTO region_t VALUES(%s, %s,'%s')", newStoreId, newAreaId, newAreaName);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_COMMAND_OK){
                            printf("INSERT failed: %s", PQerrorMessage(con));
                            //ロールバック
                            res = PQexec(con, "ROLLBACK");
                            PQclear(res);
                            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                            return -1;
                        }

                        //チェーンIDを入力してください
                        sprintf(sendBuf, "チェーンIDを入力してください。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //recvBufの文字列が数字以外だった場合
                        is_valid = 1;
                        for(int i=0; i<strlen(recvBuf); i++){
                            if(!isdigit(recvBuf[i])){
                                is_valid = 0;
                                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }
                        
                        //recvBufの文字列をnewChainIdにコピー
                        char newChainId[10];
                        strcpy(newChainId, recvBuf);
                        printf("newChainId: %s\n", newChainId);

                        //newChainIdがchain_tのchain_idに存在するかを確かめる
                        sprintf(sql, "SELECT * FROM chain_t WHERE chain_id = '%s'", newChainId);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_TUPLES_OK){
                            printf("SELECT failed: %s", PQerrorMessage(con));
                            PQclear(res);
                            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                            return -1;
                        }

                        // newChainIdが0件だった場合、
                        if( PQntuples(res) == 0){
                            sprintf(sendBuf, "チェーンIDを新規登録します%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            //入力されたチェーンIDで新規登録します.
                            sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", newChainId, newStoreId);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("INSERT failed: %s", PQerrorMessage(con));
                                //ロールバック
                                res = PQexec(con, "ROLLBACK");
                                PQclear(res);
                                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                                return -1;
                            }
                        }else{
                            //newChainIdが1件以上だった場合、
                            sprintf(sendBuf, "チェーンIDが既に存在します。%sで登録します。%s%s", newChainId, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                            //チェーンIDを新規登録します
                            sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", newChainId, newStoreId);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("INSERT failed: %s", PQerrorMessage(con));
                                //ロールバック
                                res = PQexec(con, "ROLLBACK");
                                PQclear(res);
                                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                                return -1;
                            }
                        }

                        //地域ID,地域名、チェーンIDを新規登録しました。
                        sprintf(sendBuf, "地域ID,地域名、チェーンIDを新規登録しました。%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                    //newAreaIDが1件以上存在した場合、
                    else if(PQntuples(res) >= 1){
                        sprintf(sendBuf, "SELECT * FROM region_t WHERE region_id = '%s'", newAreaId);
                        res = PQexec(con, sql);
                        printf("%s\n", sql);
                        if(PQresultStatus(res) != PGRES_TUPLES_OK){
                            printf("SELECT failed: %s", PQerrorMessage(con));
                            PQclear(res);
                            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100 ,ENTER, DATA_END);
                            send(soc, sendBuf, sendLen, 0);
                        }

                        //地域名region_nameを格納する
                        char region_name[20];
                        strcpy(region_name, PQgetvalue(res, 0, 2));
                        printf("region_name: %s\n", region_name);

                        ////入力された地域IDはすでに存在しています
                        sprintf(sendBuf, "入力された地域ID/地域名はすでに存在しています[%s]。この地域IDを登録しますか(y/n)%s%s", region_name, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                        recvBuf[recvLen-1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                        //recvBufの文字列をisRegisterにコピー
                        char isRegister[2];
                        strcpy(isRegister, recvBuf);
                        printf("isRegister: %s\n", isRegister);

                        //isRegisterが"y"の場合、
                        if(strcmp(isRegister, "y") == 0){
                            //入力された地域IDと店舗ID(newStoreId)をregion_tのregion_id、store_id に新規登録する
                            sprintf(sql, "INSERT INTO region_t VALUES(%s, %s,'%s')", newStoreId, newAreaId, region_name);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("INSERT failed: %s", PQerrorMessage(con));
                                //ロールバック
                                res = PQexec(con, "ROLLBACK");
                                PQclear(res);
                                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                                return -1;
                            }

                            //チェーンIDを入力してください
                            sprintf(sendBuf, "チェーンIDを入力してください。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                            recvBuf[recvLen-1] = '\0';
                            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                            //recvBufの文字列が数字以外だった場合
                            is_valid = 1;
                            for(int i=0; i<strlen(recvBuf); i++){
                                if(!isdigit(recvBuf[i])){
                                    is_valid = 0;
                                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                                    sendLen = strlen(sendBuf);
                                    send(soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                    break;
                                }
                            }
                            
                            //recvBufの文字列をnewChainIdにコピー
                            char newChainId[10];
                            strcpy(newChainId, recvBuf);
                            printf("newChainId: %s\n", newChainId);

                            //newChainIdがchain_tのchain_idに存在するかを確かめる
                            sprintf(sql, "SELECT * FROM chain_t WHERE chain_id = '%s'", newChainId);
                            res = PQexec(con, sql);
                            printf("%s\n", sql);
                            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                                printf("SELECT failed: %s", PQerrorMessage(con));
                                PQclear(res);
                                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                send(soc, sendBuf, sendLen, 0);
                                return -1;
                            }

                            // newChainIdが0件だった場合、
                            if( PQntuples(res) == 0){
                                sprintf(sendBuf, "チェーンIDを新規登録します%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                                //入力されたチェーンIDで新規登録します.
                                sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", newChainId, newStoreId);
                                res = PQexec(con, sql);
                                printf("%s\n", sql);
                                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                    printf("INSERT failed: %s", PQerrorMessage(con));
                                    //ロールバック
                                    res = PQexec(con, "ROLLBACK");
                                    PQclear(res);
                                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                    send(soc, sendBuf, sendLen, 0);
                                    return -1;
                                }
                            }else{
                                //newChainIdが1件以上だった場合、
                                sprintf(sendBuf, "チェーンIDが既に存在します。%sで登録します。%s%s", newChainId, ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                                //チェーンIDを新規登録します
                                sprintf(sql, "INSERT INTO chain_t(chain_id, store_id) VALUES(%s, %s)", newChainId, newStoreId);
                                res = PQexec(con, sql);
                                printf("%s\n", sql);
                                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                    printf("INSERT failed: %s", PQerrorMessage(con));
                                    //ロールバック
                                    res = PQexec(con, "ROLLBACK");
                                    PQclear(res);
                                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                    send(soc, sendBuf, sendLen, 0);
                                    return -1;
                                }
                            }
                        }
                        else if(strcmp(isRegister, "n") == 0){
                            sprintf(sendBuf, "地域ID,地域名を登録しません。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                        else{
                            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                    }
                    else{
                        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }
                //新規店舗のIDが1件以上だった場合、
                else if(PQntuples(res)== 1){
                    sprintf(sendBuf, "新規店舗のIDが既に存在します。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                //入力が無効な場合
                else{
                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }

            }
            //既存店舗を登録する場合
            else if(strcmp(newStore, "0")== 0){
                //店舗IDを入力してください
                sprintf(sendBuf, "店舗IDを入力してください(100-)。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen-1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                //recvBufの文字列をnewStoreにコピー
                strcpy(newStore, recvBuf);

                //店舗IDがstore_tのstore_idに存在するか確認
                sprintf(sql, "SELECT * FROM store_t WHERE store_id = '%s'", newStore);
                res = PQexec(con, sql);
                printf("%s\n", sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("SELECT failed: %s", PQerrorMessage(con));
                    PQclear(res);
                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }

                //店舗IDが0件だった場合、
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1305,ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                else if(PQntuples(res)== 1){
                    //phoneNumのuser_idをuser_tから取得する
                    sprintf(sql, "SELECT * FROM user_t WHERE user_phone = '%s'", phoneNum);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("SELECT failed: %s", PQerrorMessage(con));
                        PQclear(res);
                        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }

                    //user_idを格納する
                    char user_id[10];
                    strcpy(user_id, PQgetvalue(res, 0, 0));

                    //user_idのstore_idにnewStoreを格納する
                    sprintf(sql, "UPDATE user_authority_t SET store_id = %s WHERE user_id = '%s'", newStore, user_id);
                    res = PQexec(con, sql);
                    printf("%s\n", sql);
                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                        printf("UPDATE failed: %s", PQerrorMessage(con));
                        //ロールバック
                        res = PQexec(con, "ROLLBACK");
                        PQclear(res);
                        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        send(soc, sendBuf, sendLen, 0);
                        return -1;
                    }

                    //登録完了
                    sprintf(sendBuf, "登録完了 %sの所属店舗を%sにしました。%s%s", phoneNum, newStore, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                }
                else{
                    sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1304, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }
            //新規、既存に対して入力が1,0以外の場合
            else{
                sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1303, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        else{
            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1301, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }
    else{
        sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1302, ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //トランザクション終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;
}