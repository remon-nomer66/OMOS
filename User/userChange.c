#include "OMOS.h"
#include "user.h"

int userChange(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];
    int changeNum;  //変更したい内容を[0:電話番号, 1:パスワード, 2:氏名]から選択する
    char newPhoneNum[12], newPass[20], newuserName[30];  //変更後の電話番号、パスワード、氏名

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
    printf("BEGIN failed: %s", PQerrorMessage(con));
    PQclear(res);
    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
    send(soc, sendBuf, sendLen, 0);
    }

    int user_id = u_info[0];
    printf("user_id: %d\n", user_id);

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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1108, ENTER); //送信データ作成
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
            
            //入力が数字11桁の場合
            int is_valid = 1;
            for(int i=0; i<strlen(recvBuf); i++){
                if(!isdigit(recvBuf[i])){
                    is_valid = 0;
                    break;
                }
            }
            if( strlen(recvBuf) == 11 && is_valid ){
                strcpy(newPhoneNum, recvBuf);
                //user_tテーブルに同一の電話番号がないか確認する
                sprintf(sql, "SELECT * FROM user_t WHERE user_phone = %s", newPhoneNum);
                res = PQexec(con, sql);
                printf("good[1]");
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    //ロールバック
                    res = PQexec(con, "ROLLBACK");
                    PQclear(res);
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    send(soc, sendBuf, sendLen, 0);
                }
                //同一の電話番号がない場合、break
                if(PQntuples(res) == 0){
                    printf("good[2]");
                    break;
                }else{
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1109 ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf);  //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    printf("good[3]");
                    return -1;
                }
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1101, ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                printf("good[4]");
            }
        }
        //user_tテーブルの電話番号を変更する
        sprintf(sql, "UPDATE user_t SET user_phone = %s WHERE user_id = %d;", newPhoneNum, u_info[0]);
        res = PQexec(con, sql);
        printf("%s", sql);
        printf("good[5]");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1110 ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
        }
        else{
            //変更がうまく行った場合、変更完了と送信する
            sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            printf("good[6]");
        }
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
                strcpy(newPass, recvBuf);
                printf("newPass: %s\n", newPass);
                break;
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1102, ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        //user_tテーブルのパスワードを変更する
        sprintf(sql, "UPDATE user_t SET user_pass = '%s' WHERE user_id = %d;", newPass, u_info[0]);
        res = PQexec(con, sql);
        printf("%s", sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1110, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
        else{
            //変更がうまく行った場合、変更完了と送信する
            sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
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
                strcpy(newuserName, recvBuf);
            break;
            }else{
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1103, ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //user_tテーブルの氏名を変更する
        sprintf(sql, "UPDATE user_t SET user_name = '%s' WHERE user_id = %d", newuserName, u_info[0]);
        res = PQexec(con, sql);
        printf("UPDATE: %s", PQerrorMessage(con));
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1110, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
        else{
            //変更がうまく行った場合、変更完了と送信する
            sprintf(sendBuf, "変更完了%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }

    //コミット
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;  //正常終了

}