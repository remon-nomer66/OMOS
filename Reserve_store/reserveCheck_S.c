#include "omos.h"
#include "reserve.h"

int reserveCheck_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info, int reg_chg_flag, int reserve_no){
    int recvLen, sendLen;   //送受信データ長
    char r_date[11], r_time[9];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp;
    char storeBuf[BUFSIZE];
    char zero[3] = "0";
    int a_table[20] = {0}; //利用可能な卓番号
    char user_phone[12];              //予約希望店舗番号
    int user_id;
    char *buf;
    char r_year[5], r_month[3], r_day[3], r_hour[3], r_min[3];

    if(reg_chg_flag == 0){
        sprintf(sendBuf, "登録する電話番号、日時，人数を\"02012345678 2023-06-18 12:15 5\"のように半角空白を挿入した形で入力してください%s%s", ENTER, DATA_END);
    }else{
        sprintf(sendBuf, "変更後の日時，人数を\"2023-06-18 12:15 5\"のように半角空白を挿入した形で入力してください%s%s", ENTER, DATA_END);
    }
    
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //予約内容受け取り
    recvLen = receive_message(soc, recvBuf, BUFSIZE);
    if(recvLen > 0){
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        if(reg_chg_flag == 0)
        {
            cnt = sscanf(recvBuf, "%s %s %s %d", user_phone, r_date, r_time, &p_num);
            if(cnt == 4){
                sprintf(sql, "SELECT user_id FROM user_t WHERE user_phone = %s", user_phone);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
                user_id = atoi(PQgetvalue(res, 0, 0));
            }else{
                return -1;
            }
        }else{
            cnt = sscanf(recvBuf, "%s %s %d", r_date, r_time, &p_num);
            if(cnt != 3){
                return -1;
            }
        }
	    
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
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1204, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }else{
            if(strlen(r_year) != 4){
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1205, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                PQclear(res);
                return -1;
            }
            if(strlen(r_month) != 2){
                if(strlen(r_month) == 1){
                    strcat(zero, r_month);
                    strcpy(r_month, zero);
                    zero[0] = '0';
                    zero[1] = '\0';
                }else{
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1206, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
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
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1207, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    PQclear(res);
                    return -1;
                }
            }
        }

        printf("1\n");

        time_t t = time(NULL);
        struct tm *local = localtime(&t);

        //現在の年以前の予約でないか
        if(atoi(r_year) < (local->tm_year + 1900)){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1208, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }

        //現在の月以前の予約でないか
        if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) < (local->tm_mon + 1))){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1209, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }

        //現在の日以前の予約でないか
        if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) < local->tm_mday)){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1210, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
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
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1211, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                PQclear(res);
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
                sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1212, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                PQclear(res);
                return -1;
            }
        }

        //現在の時間以前の予約でないか
        if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) == local->tm_mday) && (atoi(r_hour) < local->tm_hour)){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1213, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }

        //現在の分以前の予約でないか
        if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) == local->tm_mday) && (atoi(r_hour) == local->tm_hour) && atoi(r_min) < local->tm_min){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1214, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }
        
        //予約可能時刻の以前でないか(30分前であるか)
        if((atoi(r_year) == (local->tm_year + 1900)) && (atoi(r_month) == (local->tm_mon + 1)) && (atoi(r_day) == local->tm_mday) && (atoi(r_hour) * 60 + atoi(r_min) <= local->tm_hour * 60 + local->tm_min + 30)){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1215, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }

        sprintf(r_date, "%s-%s-%s", r_year, r_month, r_day);
        sprintf(r_time, "%s:%s:00", r_hour, r_min);

        //ここまで日付時間チェック

        //当該店舗の卓について，人数的に許容できる卓の番号を格納
        sprintf(sql, "SELECT * FROM store_table_t WHERE store_id = %d AND desk_max >= %d ORDER BY desk_max ASC", s_info[0], p_num);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }
        tmp = resultRows = PQntuples(res);
        if(resultRows <= 0){
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1217, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            PQclear(res);
            return -1;
        }
        for(i = 0; i < resultRows; i++){
            a_table[i] = atoi(PQgetvalue(res, i, 1));
        }

        //当該時間前後に卓の空きがあるのか，あるなら予約テーブルに情報追加
        //reg_chg_flag = 0: 登録
        //reg_chg_flag = 1: 変更
        if(reg_chg_flag == 0){
            sprintf(sql, "BEGIN TRANSACTION");
            PQexec(con, sql);
            for(i = 0; i < tmp; i++){
                sprintf(sql, "SET search_path to reserve");
                PQexec(con, sql);
                sprintf(sql, "SELECT * FROM reserve_t WHERE store_id = %d AND desk_num = %d AND reserve_date = '%s' AND reserve_time BETWEEN '%d:%s:00' AND '%d:%s:00'", s_info[0], a_table[i], r_date, atoi(r_hour) - 1, r_min, atoi(r_hour) + 1, r_min);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);

                    sprintf(sql, "ROLLBACK");
                    PQexec(con, sql);

                    PQclear(res);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sql, "INSERT INTO reserve_t (reserve_no, user_id, people_num, reserve_date, reserve_time, store_id, desk_num) VALUES (nextval('reserve_seq'), %d, %d, '%s', '%s', %d, %d)", user_id, p_num, r_date, r_time, s_info[0], a_table[i]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        sprintf(sql, "SET search_path to public");
                        PQexec(con, sql);

                        sprintf(sql, "ROLLBACK");
                        PQexec(con, sql);

                        PQclear(res);
                        return -1;
                    }
                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);
                    
                    sprintf(sql, "COMMIT");
                    PQexec(con, sql);
                    flag = 1;
                    break;
                }
            }
        }else{
            sprintf(sql, "BEGIN TRANSACTION");
            PQexec(con, sql);
            for(i = 0; i < tmp; i++){
                sprintf(sql, "SET search_path to reserve");
                PQexec(con, sql);
                sprintf(sql, "SELECT * FROM reserve_t WHERE store_id = %d AND desk_num = %d AND reserve_date = '%s' AND reserve_time BETWEEN '%d:%s:00' AND '%d:%s:00'", s_info[0], a_table[i], r_date, atoi(r_hour) - 1, r_min, atoi(r_hour) + 1, r_min);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);

                    sprintf(sql, "ROLLBACK");
                    PQexec(con, sql);

                    PQclear(res);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sql, "UPDATE reserve_t SET people_num = %d, reserve_date = '%s', reserve_time = '%s', store_id = %d, desk_num = %d WHERE reserve_no = %d", p_num, r_date, r_time, s_info[0], a_table[i], reserve_no);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_COMMAND_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        sprintf(sql, "SET search_path to public");
                        PQexec(con, sql);

                        sprintf(sql, "ROLLBACK");
                        PQexec(con, sql);

                        PQclear(res);
                        return -1;
                    }
                    sprintf(sql, "SET search_path to public");
                    PQexec(con, sql);
        
                    sprintf(sql, "COMMIT");
                    PQexec(con, sql);
                    flag = 1;
                    break;
                }
            }
        }
        if(flag != 1){                    
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1218, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }else{
            sprintf(sendBuf, "%s %d%s", OK_STAT, 1, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            
            sprintf(sendBuf, "%s %s:%s %d%s", r_date, r_hour, r_min, p_num, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }else{
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_200, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        PQclear(res);
        return -1;
    }
    
    PQclear(res);
    return 0;
}