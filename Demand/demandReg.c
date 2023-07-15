#include "omos.h"

int demandReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int store_id;

    //トランジション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //store_idを取得する
    store_id = u_info[2];

    //menu_storage_tのstore_idと同様のものかつ、menu_storage_tのstorage_flag == 1のものを表示する
    sprintf(sendBuf, "SELECT * FROM menu_storage_t WHERE store_id = %d AND storage_flag = 1", store_id);
    res = PQexec(con, sendBuf);

    //検索結果が0件の場合、エラーを返す
    if(PQntuples(res) == 0){
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }

    //検索結果が1件以上だった場合、商品番号を入力してもらう
    if(PQntuples(res) >= 1){
        while(1){
            sprintf(sendBuf, "商品番号を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(isdigit(comm)){
                    break;
                }else{
                    sprintf(sendBuf, "商品番号が不正です。%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }
        }
        //入力してもらった商品番号をmenu_storage_tから検索する
        sprintf(sendBuf, "SELECT * FROM menu_storage_t WHERE store_id = %d AND storage_flag = 1 AND menu_id = %d", store_id, comm);
        res = PQexec(con, sendBuf);
        //検索結果が0件の場合、エラーを返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return -1;
        }

        //検索結果が1件の場合、menu_storage_tのstore_flagを0にする
        if(PQntuples(res) == 1){
            sprintf(sendBuf, "UPDATE menu_storage_t SET storage_flag = 0 WHERE store_id = %d AND storage_flag = 1 AND menu_id = %d", store_id, comm);
            res = PQexec(con, sendBuf);
            //UPDATEが正常に完了した場合、COMMITする
            if(PQresultStatus(res) == PGRES_COMMAND_OK){
                res = PQexec(con, "COMMIT");
                sprintf(sendBuf, "商品を削除しました。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                //対応する店舗IDと商品番号をstore_order_tから探し出し、store_order_cntを取得する
                sprintf(sendBuf, "SELECT * FROM store_order_t WHERE store_id = %d AND menu_id = %d", store_id, comm);
                res = PQexec(con, sendBuf);
                //検索結果が0件の場合、エラーを返す
                if(PQntuples(res) == 0){
                    sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                //検索結果が1件の場合、store_order_cntを取得する
                if(PQntuples(res) == 1){
                    sprintf(sendBuf, "SELECT store_order_cnt FROM store_order_t WHERE store_id = %d AND menu_id = %d", store_id, comm);
                    res = PQexec(con, sendBuf);
                    //store_order_cntを取得する
                    int store_order_cnt = PQgetvalue(res, 0, 0);
                    
                    //store_order_cntが0の場合、store_order_tから削除する
                    if(store_order_cnt == 0){
                        sprintf(sendBuf, "DELETE FROM store_order_t WHERE store_id = %d AND menu_id = %d", store_id, comm);
                        res = PQexec(con, sendBuf);
                        //DELETEが正常に完了した場合、COMMITする
                        if(PQresultStatus(res) == PGRES_COMMAND_OK){
                            res = PQexec(con, "COMMIT");
                            sprintf(sendBuf, "商品を削除しました。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            return 0;
                        }
                    //store_order_cntが0でない場合、menu_storage_tの在庫個数(storage)にstore_order_cntを足す
                    }else{
                        sprintf(sendBuf, "UPDATE menu_storage_t SET storage = storage + %d WHERE store_id = %d AND menu_id = %d", store_order_cnt, store_id, comm);
                        res = PQexec(con, sendBuf);
                        //UPDATEが正常に完了した場合、COMMITする
                        if(PQresultStatus(res) == PGRES_COMMAND_OK){
                            res = PQexec(con, "COMMIT");
                            sprintf(sendBuf, "商品の発注確認完了です。%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            return 0;
                        }
                    }

                //検索結果が2件以上の場合、エラーを返す
                }else{
                    sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
            }else{
                //UPDATEが正常に完了しなかった場合、ROLLBACKする
                res = PQexec(con, "ROLLBACK");
                sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }
    }

    return 0;

}