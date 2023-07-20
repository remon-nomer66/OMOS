#include "OMOS.h"

int auth_Chg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    int auth;

    //トランザクションの開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //権限情報をmain_authに格納
    int main_auth = u_info[1];
    printf("main_auth = %d\n", main_auth);  //デバッグ用, 後で消す,main_auth=3

    //権限を変更したい人の電話番号を入力してもらう.
    sprintf(sendBuf, "権限を変更したい人の電話番号を入力してください.%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //電話番号を受信
    recvLen = recv(soc, recvBuf, BUFSIZE, 0);
    recvBuf[recvLen-1] = '\0';  //改行コードを削除
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
    printf("recvBuf = %s\n", recvBuf);

    //入力した電話番号が存在するかどうか, user_tテーブルから検索する
    sprintf(sql, "SELECT user_phone FROM user_t WHERE user_phone = %s", recvBuf);
    printf("sql = %s\n", sql);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        return -1;
    }

    //電話番号が存在しない場合
    if(PQntuples(res) == 0){
        sprintf(sendBuf, "電話番号が存在しません!!.%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        PQclear(res);
        return -1;
    }

    //recvBufに入っている電話番号から,user_authority_tのuser_idを取得する.get_user_idにuser_idを格納する
    sprintf(sql, "SELECT user_id FROM user_t WHERE user_phone = %s", recvBuf);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        return -1;
    }
    int get_user_id = atoi(PQgetvalue(res, 0, 0));
    printf("get_user_id = %d\n", get_user_id);


    //user_tのuser_idをキーとして、user_authority_tと結合する。その際に、user_authorityを取得する。
    sprintf(sql, "SELECT user_authority FROM user_authority_t WHERE user_id = (SELECT user_id FROM user_t WHERE user_phone = %s)", recvBuf);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        return -1;
    }
    auth = atoi(PQgetvalue(res, 0, 0));
    printf("auth = %d\n", auth);

    //main_authが4だった場合、
    if(main_auth == 4){
        //authが1の時、権限を5に変える。
        if(auth == 1){
            sprintf(sql, "UPDATE user_authority_t SET user_authority = 5 WHERE user_id = %d", get_user_id);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                //ロールバック
                res = PQexec(con, "ROLLBACK");
                PQclear(res);
                return -1;
            }
            sprintf(sendBuf, "権限を店員に変更しました.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
        else if(auth == 5){
            //権限をお客にするか店長にするかを問う
            sprintf(sendBuf, "権限をお客にする場合は1、店長にする場合は4を入力してください.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //もし、1か4以外の数字が入力された場合、再度入力させる
            while(1){
                //権限を受信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                //1か4以外の数字が入力された場合、再度入力させる
                if(atoi(recvBuf) != 1 && atoi(recvBuf) != 4){
                    sprintf(sendBuf, "1か4を入力してください.%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                else{
                    break;
                }
            }
            //1が入力された場合、権限を1に変更する
            if(atoi(recvBuf) == 1){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 1 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限をお客に変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
            //4が入力された場合、権限を4に変更する
            else if(atoi(recvBuf) == 4){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 4 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限を店長に変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        else{
            sprintf(sendBuf, "権限を変更することができません.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }

        return 0;
    }

    //main_authが3だった場合、
    if(main_auth == 3){
        //authが4の時、権限をCORにするか、店員に変更するかを問う
        if(auth == 4){
            sprintf(sendBuf, "権限をCORにする場合は3、店員にする場合は5を入力してください.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //もし、3か5以外の数字が入力された場合、再度入力させる
            while(1){
                //権限を受信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                //3か5以外の数字が入力された場合、再度入力させる
                if(atoi(recvBuf) != 3 && atoi(recvBuf) != 5){
                    sprintf(sendBuf, "3か5を入力してください.%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                else{
                    break;
                }
            }
            //3が入力された場合、権限を3に変更する
            if(atoi(recvBuf) == 3){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 3 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限をCORに変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
            //5が入力された場合、権限を5に変更する
            else if(atoi(recvBuf) == 5){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 5 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限を店員に変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
            
        }
        else{
            sprintf(sendBuf, "権限を変更することができません.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }

    //main_authが2だった場合、
    if(main_auth == 2){
        //authが3だった場合、権限を本部にするか、店長にするかを問う
        if(auth == 2){
            sprintf(sendBuf, "権限を本部にする場合は2、店長にする場合は4を入力してください.%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //もし、2か4以外の数字が入力された場合、再度入力させる
            while(1){
                //権限を受信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0);
                recvBuf[recvLen] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                //2か4以外の数字が入力された場合、再度入力させる
                if(atoi(recvBuf) != 2 && atoi(recvBuf) != 4){
                    sprintf(sendBuf, "2か4を入力してください.%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                else{
                    break;
                }
            }
            //2が入力された場合、権限を2に変更する
            if(atoi(recvBuf) == 2){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 2 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限を本部に変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
            //4が入力された場合、権限を4に変更する
            else if(atoi(recvBuf) == 4){
                sprintf(sql, "UPDATE user_authority_t SET user_authority = 4 WHERE user_id = %d", get_user_id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    return -1;
                }
                sprintf(sendBuf, "権限を店長に変更しました.%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
            else{
                printf("権限を変更することができません.");
                //sprintf("権限を変更することができません.%s%s", ENTER, DATA_END);
                //sendLen = strlen(sendBuf);
                //send(soc, sendBuf, sendLen, 0);
                //printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }

    //トランザクション終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;
    
}