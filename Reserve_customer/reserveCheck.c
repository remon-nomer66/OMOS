#include "omos.h"

int reserveCheck(PGconn *__con, int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    char r_date[BUFSIZE], r_time[BUFSIZE];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows;
    int s_num;
    char r_year[5], r_month[3], r_day[3], r_hour[3], r_min[3], *buf;
    char storeBuf[BUFSIZE];
    char zero[3] = "0";
    int a_table[10] = {0}; //利用可能な卓番号

    sprintf(sendBuf, "登録する店舗番号，日時(時間は15分刻み)，人数を\"110 2023-06-18 12:15 5\"のように半角空白を挿入した形で入力してください%s店舗番号 店舗名%s", ENTER, ENTER);

    //店舗番号と店舗名の対応関係を表示
    sprintf(sql, "SELECT store_id, store_name FROM store_t");
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows == 0){
        sprintf(sendBuf, "データベースエラー%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    for(i = 0; i < resultRows; i++){
        sprintf(storeBuf, "%5d %s", atoi(PQgetvalue(res, i, 0)), PQgetvalue(res, i, 1));
        strcat(sendBuf, storeBuf);
    }
    sendLen = strlen(sendBuf);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //予約内容受け取り
    recvLen = receive_message(__soc, recvBuf, BUFSIZE);
    if(recvLen > 0){
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        cnt = sscanf(recvBuf, "%d %s %s %d", &s_num, r_date, r_time, &p_num);
        if(cnt == 4){
            //ここから日付時間チェック
            buf = strtok(r_date, "-");
            strncpy(r_year, buf, sizeof(r_year) - 1);
            r_year[sizeof(r_year) - 1] = '\0';
            buf = strtok(NULL, "-");
            strncpy(r_month, buf, sizeof(r_month) - 1);
            r_month[sizeof(r_month) - 1] = '\0';
            buf = strtok(NULL, " ");
            strncpy(r_day, buf, sizeof(r_day) - 1);
            r_day[sizeof(r_day) - 1] = '\0';

            if(r_year == NULL || r_month == NULL || r_day == NULL){
                sprintf(sendBuf, "文字列に問題があります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                return -1;
            }else{
                if(strlen(r_year) != 4){
                    sprintf(sendBuf, "%sは不適切な年です%s", r_year, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    return -1;
                }
                if(strlen(r_month) != 2){
                    if(strlen(r_month) == 1){
                        strcat(zero, r_month);
                        strcpy(r_month, zero);
                        zero[0] = '0';
                        zero[1] = '\0';
                    }else{
                        sprintf(sendBuf, "%sは不適切な月です%s", r_month, ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        
                        return -1;
                    }
                }
                if(strlen(r_day) != 2){
                    if(strlen(r_day) == 1){
                        strcat(zero, r_day);
                        strcpy(r_day, zero);
                        zero[0] = '0';
                        zero[1] = '\0';
                    }else{
                        sprintf(sendBuf, "%sは不適切な日です%s", r_day, ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        
                        return -1;
                    }
                }
            }

            time_t t = time(NULL);
            struct tm *local = localtime(&t);
            if(atoi(r_year) < (local->tm_year + 1900)){
                sprintf(sendBuf, "%sは不適切な年です%s", r_year, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                return -1;
            }
            if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) < (local->tm_mon + 1))){
                sprintf(sendBuf, "%sは不適切な月です%s", r_month, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                return -1;
            }
            if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) < local->tm_mday)){
                sprintf(sendBuf, "%sは不適切な月です%s", r_day, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                return -1;
            }

            buf = strtok(r_time, ":");
            strncpy(r_hour, buf, sizeof(r_hour) - 1);
            r_hour[sizeof(r_hour) - 1] = '\0';
            buf = strtok(NULL, " ");
            strncpy(r_min, buf, sizeof(r_min) - 1);
            r_min[sizeof(r_min) - 1] = '\0';
            
            if(strlen(r_hour) != 2){
                if(strlen(r_hour) == 1){
                    strcat(zero, r_hour);
                    strcpy(r_hour, zero);
                    zero[0] = '0';
                    zero[1] = '\0';
                }else{
                    sprintf(sendBuf, "%sは不適切な時間です%s", r_hour, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    
                    return -1;
                }
            }
            if(strlen(r_min) != 2){
                if(strlen(r_min) == 1){
                    strcat(zero, r_min);
                    strcpy(r_min, zero);
                    zero[0] = '0';
                    zero[1] = '\0';
                }else{
                    sprintf(sendBuf, "%sは不適切な分です%s", r_min, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    
                    return -1;
                }
            }

            if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) == local->tm_mday) && (atoi(r_hour) < local->tm_hour)){
                sprintf(sendBuf, "%sは不適切な時間です%s", r_hour, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                
                return -1;
            }
            if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) == local->tm_mday) && ((atoi(r_hour) == local->tm_hour) && (atoi(r_hour) == local->tm_hour) && atoi(r_min) < local->tm_min)){
                sprintf(sendBuf, "%sは不適切な分です%s", r_min, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                return -1;
            }

            if(!((strcmp(r_hour, "17") >= 0) && (strcmp(r_hour, "22") <= 0) && ((strcmp(r_min, "00") == 0) || (strcmp(r_min, "15") == 0) || (strcmp(r_min, "30") == 0) || (strcmp(r_min, "45") == 0)))){
                sprintf(sendBuf, "15分刻みで入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }

            sprintf(r_date, "%s-%s-%s%d", r_year, r_month, r_day);
            sprintf(r_time, "%s:%s:00%d", r_hour, r_min);
            printf("%s\n", r_date);
            printf("%s\n", r_time);
            //ここまで日付時間チェック

            //=========================================================
            //=========================================================

            //当該店舗の卓について，人数的に許容できる卓の番号を格納
            sprintf(sql, "SELECT * FROM store_table_t WHERE store_id = %d AND desk_max >= %d ORDER BY desk_max ASC", s_num, p_num);
            res = PQexec(__con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows <= 0){
                sprintf(sendBuf, "人数エラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                return -1;
            }
            for(i = 0; i < resultRows; i++){
                a_table[i] = atoi(PQgetvalue(res, i, 1));
            }

            //当該時間前後に卓の空きがあるのか，あるなら予約テーブルに情報追加
            sprintf(sql, "BEGIN TRANSACTION");
            PQexec(__con, sql);
            for(i = 0; i < resultRows; i++){
                sprintf(sql, "SELECT * FROM reserve_t WHERE store_id = %d AND desk_num = %d AND reserve_date = '%s' AND reserve_time <= '%d:%s:00' AND reserve_time <= '%d:%s:00'", s_num, a_table[i], r_date, atoi(r_hour) + 1, r_min, atoi(r_hour) - 1, r_min);
                res = PQexec(__con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sql, "ROLLBACK");
                    PQexec(__con, sql);

                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sql, "INSERT INTO reserve_t (account_id, people_num, reserve_date, reserve_time, store_id, desk_num) VALUES (%d, %d, '%s', '%s:00', %d, %d)", __auth[1], p_num, r_date, r_time, s_num, a_table[i]);
                    res = PQexec(__con, sql);
                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "データベースエラー%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        sprintf(sql, "ROLLBACK");
                        PQexec(__con, sql);

                        return -1;
                    }
                    sprintf(sql, "COMMIT");
                    PQexec(__con, sql);
                    flag = 1;
                }
            }
            if(flag != 1){                    
                sprintf(sendBuf, "当該時間の予約は満杯です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else{
                sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", s_num);
                res = PQexec(__con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                sprintf(sendBuf, "登録された内容は以下のとおりです%s店舗名：%s%s日時：%s %s%s人数：%d%s", ENTER, PQgetvalue(res, 0, 0), ENTER, r_date, r_time, ENTER, p_num, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }
    PQclear(res);
    return 0;
}