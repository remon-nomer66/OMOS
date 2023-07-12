#include "omos.h"

int reserveChg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, i, cnt, param;
    int reserve_no[RSRVMAX];
    char reserve_date[RSRVMAX][10];
    char reserve_time[RSRVMAX][8];
    int reserve_store_id[RSRVMAX];
    int reserve_desk_num[RSRVMAX];
    char reserve_store_name[RSRVMAX][30];
    char buf[BUFSIZE], comm[BUFSIZE];
    char store_name[BUFSIZE];
    int reg_chg_flag = 1;   //chg: 1
    int tmp = 0;

    while(1){
        //予約変更する対象があるかチェック
        sprintf(sql, "SET search_path to reserve");
        res = PQexec(con, sql);
        sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d", u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            sprintf(sql, "SET search_path to public");
	        PQexec(con, sql);
            PQclear(res);
            return -1;
        }
        tmp = resultRows = PQntuples(res);
        if(resultRows <= 0){
            sprintf(sendBuf, "予約変更の対象がありません%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            sprintf(sql, "SET search_path to public");
	        PQexec(con, sql);
            PQclear(res);
            return -1;
        }
        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);

        //予約変更可能な時
        for(i = 0; i < resultRows; i++){
            reserve_no[i] = PQgetvalue(res, i, 0);
            strcpy(reserve_date[i], PQgetvalue(res, i, 2));
            strcpy(reserve_time[i], PQgetvalue(res, i, 3));
            reserve_store_id[i] = atoi(PQgetvalue(res, i, 4));
            reserve_desk_num[i] = atoi(PQgetvalue(res, i, 5));
        }

        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);
        
        for(i = 0; i < tmp; i++){
            sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", reserve_store_id[i]);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            strcpy(reserve_store_name[i], PQgetvalue(res, 0, 0));
        }
        sprintf(sendBuf, "変更する予約番号を入力してください%s予約番号 店舗名 予約日 予約時間%s予約変更から抜ける場合は\"END\"と入力してください%s", ENTER, ENTER, ENTER, DATA_END);
        for(i = 0; i < tmp; i++){
            sprintf(buf, "%d %s %s %s%s", i + 1, reserve_store_name[i], reserve_date
            [i], reserve_time[i], ENTER);
            strcat(sendBuf, buf);
        }
        sprintf(buf, "%s", DATA_END);
	    strcat(sendBuf, buf);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        //変更対象を受け取り
        while(1){
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen > 0){   //文字が入力されたら
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                cnt = sscanf(recvBuf, "%d", &param);

                //削除対象の確認
                if(cnt == 1 && 1 <= param && param <= RSRVMAX){
                    sprintf(sendBuf, "変更する対象は%s%3d %s %s %s%sでよろしいですか？よろしい場合は\"YES\"と入力してください%s%s", ENTER, &param, reserve_store_name[param - 1], reserve_date[param - 1], reserve_time[param - 1], ENTER, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    recvLen = receive_message(soc, recvBuf, BUFSIZE);
                    if(recvLen != 0){
                        cnt = sscanf(recvBuf, "%s", comm);
                        if((cnt == 1) && (strcmp(comm, YES) == 0)){
                            reserveCheck(selfId, con, soc, recvBuf, sendBuf, u_info, reg_chg_flag, reserve_no[param - 1]);
                        }
                    }
                }else{
                    cnt = sscanf(recvBuf, "%s", comm);
                    if(strcmp(comm, END) == 0){
                        sprintf(sendBuf, "ユーザ画面に戻ります%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        PQclear(res);
                        return 0;
                    }
                }
            }
        }
    }
    PQclear(res);
    return -1;
}