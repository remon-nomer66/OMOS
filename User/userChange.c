#include "OMOS.h"

int userChange(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    PGresult *res;
    int changeNum;  //変更したい内容を[0:電話番号, 1:パスワード, 2:氏名]から選択する
    int newPhoneNum, newPass, newuserName;  //変更後の電話番号、パスワード、氏名

    //トランザクション開始
    res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    while(1){
        //変更したい内容を[0:電話番号, 1:パスワード, 2:氏名]から選択する
        sprintf(sendBuf, "変更したい内容を選択してください[0:電話番号, 1:パスワード, 2:氏名]:%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        //入力が0,1,2の場合、変更したい内容として扱う
        if( strcmp(recvBuf, "0") == 0 || strcmp(recvBuf, "1") == 0 || strcmp(recvBuf, "2") == 0 ){
            changeNum = atoi(recvBuf);
            break;
        }else{
            sprintf(sendBuf, "入力が不正です。0,1,2のいずれかを入力してください。%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
    }

    //changeNumによって処理を分岐
    //changeNum==0:電話番号を変更する場合、
    if(changeNum == 0){
        while(1){
            //電話番号を入力してもらう
            sprintf(sendBuf, "新しい電話番号を入力してください(09024681234)。%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            
            //入力が数字11桁の場合、電話番号として扱う
            if( strlen(recvBuf) == 11 && isdigit(recvBuf) ){
                newPhoneNum = atoi(recvBuf);  //文字列を数値に変換
                //user_tテーブルに同一の電話番号がないか確認する
                sprintf(sql, "SELECT * FROM user_t WHERE phone_num = '%s'", newPhoneNum);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("SELECT failed: %s", PQerrorMessage(con));
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    sprintf(sendBuf, "error occured%s", ENTER);
                    send(soc, sendBuf, sendLen, 0);
                }
                //同一の電話番号がない場合、break
                if(PQntuples(res) == 0){
                    break;
                }else{
                    sprintf(sendBuf, "既に登録されている電話番号です。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf);  //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
                break;
            }else{
                sprintf(sendBuf, "電話番号が不正です。%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        //user_tテーブルの電話番号を変更する
        sprintf(sql, "UPDATE user_t SET phone_num = '%s' WHERE user_id = %d", newPhoneNum, u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
        }

        //変更がうまく行った場合、変更完了と送信する
        sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //changeNum==1:パスワードを変更する場合、
    if(changeNum == 1){
        while(1){
            //パスワードを入力してもらう
            sprintf(sendBuf, "新しいパスワードを入力してください。%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

            //入力が8文字以上16文字以下の英数字の場合、パスワードとして扱い、breakする
            if( 8 <= strlen(recvBuf) && strlen(recvBuf) <= 16 ){
                newPass = recvBuf;
                break;
            }else{
                sprintf(sendBuf, "パスワードが不正です。%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        //user_tテーブルのパスワードを変更する
            sprintf(sql, "UPDATE user_t SET user_pass = '%s' WHERE user_id = %d", newPass, u_info[0]);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("UPDATE failed: %s", PQerrorMessage(con));
                //ロールバック
                res = PQexec(con, "ROLLBACK");
                PQclear(res);
                sprintf(sendBuf, "error occured%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
            }

        //変更がうまく行った場合、変更完了と送信する
        sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //changeNum==2:氏名を変更する場合、
    if(changeNum == 2){
        while(1){
            //氏名を入力してもらう
            sprintf(sendBuf, "新しい氏名を入力してください。%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

            //入力が30文字以下の場合、氏名として扱う
            if( strlen(recvBuf) <= 30 ){
                newuserName = recvBuf;  //文字列を数値に変換
            break;
            }else{
                sprintf(sendBuf, "氏名が不正です。%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //user_tテーブルの氏名を変更する
        sprintf(sql, "UPDATE user_t SET user_name = '%s' WHERE user_id = %d", newuserName, u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
        //変更がうまく行った場合、変更完了と送信する
        sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    }

    return 0;  //正常終了

}
