#include "omos.h"

int kitchenView(PGconn *__con, int __soc, int __menu)
{
	char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
	int recvLen, sendLen;					 // 送受信データ長
	pthread_t selfId = pthread_self();		 // 自スレッドID
	char comm[BUFSIZE];						 // SQLコマンド
	int resultRows;
	int i, cnt;
	char sql[BUFSIZE], buf[BUFSIZE];
	PGresult *res;

	sprintf(sendBuf, "注文されたメニューのレシピを表示したい場合はRINFO、終了したい場合は\"END\"を押してください。%s%s", ENTER, DATA_END); // 注文されたメニューを表示
	sendLen = strlen(sendBuf);																											   // 送信データ長
	send(__soc, sendBuf, sendLen, 0);																									   // 送信
	printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);																				   // 送信データ表示

	recvLen = receive_message(__soc, recvBuf, BUFSIZE); // 受信
	if (recvLen != 0)
	{
		recvBuf[recvLen - 1] = '\0';
		cnt = sscanf(recvBuf, "%s", comm);
		if (cnt == 1)
		{
			if (strcmp(comm, RINFO) == 0)
			{
				printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
				sprintf(sql, "SELECT menu_id, menu_name, recipe FROM recipe_t WHERE menu_id = %d", __menu);
				printf("flag\n");
				res = PQexec(__con, sql);
				if (PQresultStatus(res) != PGRES_TUPLES_OK)
				{
					printf("%s", PQresultErrorMessage(res));
					return -1;
				}#include "omos.h"
#include "kitchen.h"

int kitchenView(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info)
{
	int recvLen, sendLen;					 // 送受信データ長
	char comm[BUFSIZE];						 // SQLコマンド
	int resultRows;
	int i, cnt, tmp;
	char sql[BUFSIZE], buf[BUFSIZE];
	PGresult *res;
	int menu_id[BUFSIZE];
	int menu_len;
	char menu_name[BUFSIZE][30];
	int param;
	char recipe[LONG_BUFSIZE];
	int chain_id, region_id, season;
	int flag;

	while(1){
		time_t t = time(NULL);
		struct tm *local = localtime(&t);

		//IDの取得
		sprintf(sql, "SELECT chain_t.chain_id, region_t.region_id FROM chain_t JOIN region_t ON chain_t.store_id = region_t.store_id WHERE region_t.store_id = %d", u_info[2]);
		res = PQexec(con, sql);
		if(PQresultStatus(res) != PGRES_TUPLES_OK){
			printf("%s", PQresultErrorMessage(res));
			sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
			sendLen = strlen(sendBuf);
			send(soc, sendBuf, sendLen, 0);
			printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			return -1;
		}
		resultRows = PQntuples(res);
		if(resultRows != 1){
			sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
			sendLen = strlen(sendBuf);
			send(soc, sendBuf, sendLen, 0);
			printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			return -1;
		}
		chain_id = atoi(PQgetvalue(res, 0, 0));
		region_id = atoi(PQgetvalue(res, 0, 1));

		//season番号の取得
		if(3 <= local->tm_mon && local->tm_mon <= 5){
			season = 1;
		}else if(6 <= local->tm_mon && local->tm_mon <= 8){
			season = 2;
		}else if(9 <= local->tm_mon && local->tm_mon <= 11){
			season = 3;
		}else if(12 == local->tm_mon || 1 == local->tm_mon || local->tm_mon == 2){
			season = 4;
		} printf("tmp: %d\n", tmp);

		sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id IN (0, %d, %d, %d) AND (season = 0 OR season = %d)", chain_id, region_id, u_info[2], season);
		res = PQexec(con, sql);
		if(PQresultStatus(res) != PGRES_TUPLES_OK){
			printf("%s", PQresultErrorMessage(res));
			sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
			sendLen = strlen(sendBuf);
			send(soc, sendBuf, sendLen, 0);
			printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			return -1;
		}
		menu_len = resultRows = PQntuples(res);
		printf("menu_len: %d\n", menu_len);
		if(resultRows == 0){
			sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
			sendLen = strlen(sendBuf);
			send(soc, sendBuf, sendLen, 0);
			printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			return -1;
		}
		for(i = 0; i < menu_len; i++){
			menu_id[i] = atoi(PQgetvalue(res, i, 0));
			printf("menu_info: %d\n", menu_id[i]);
		}

		for(i = 0; i < menu_len; i++){
			sprintf(sql, "SELECT menu_name FROM recipe_t WHERE menu_id = %d", menu_id[i]);
			res = PQexec(con, sql);
			if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows == 0){
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                PQclear(res);
                return -1;
            }
			strncpy(menu_name[i], PQgetvalue(res, 0, 0), strlen(PQgetvalue(res, 0, 0)));
			printf("menu_name: %s\n", menu_name[i]);
		}

		printf("flag\n");

		

		sprintf(sendBuf, "レシピを表示したい商品番号を入力してください%s終了する際は\"END\"と入力してください%s商品番号　商品名%s", ENTER, ENTER, ENTER);
		sendLen = strlen(sendBuf);
		send(soc, sendBuf, sendLen, 0);
		printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

		for(i = 0; i < menu_len; i++){
			sprintf(sendBuf, "%d %s%s", menu_id[i], menu_name[i], ENTER);
			sendLen = strlen(sendBuf);
			send(soc, sendBuf, sendLen, 0);
			printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);	
		}

		sprintf(sendBuf, "%s", DATA_END);
		sendLen = strlen(sendBuf);
		send(soc, sendBuf, sendLen, 0);
		printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
		
		recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%d", &param);
			if(cnt == 1){
				for(i = 0; i < menu_len; i++){
					if(menu_id[i] == param){
						flag = EXIST;
						break;
					}
				}
				if(flag == EXIST){
					sprintf(sql, "SELECT recipe FROM recipe_t WHERE menu_id = %d", param);
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
					if(resultRows != 1){
						sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
						sendLen = strlen(sendBuf);
						send(soc, sendBuf, sendLen, 0);
						printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

						PQclear(res);
						return -1;
					}
					sprintf(sendBuf, "%s%s", PQgetvalue(res, 0, 0), ENTER);
					sendLen = strlen(sendBuf);
					send(soc, sendBuf, sendLen, 0);
					printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
				}
			}else{
				cnt = sscanf(recvBuf, "%s", comm);
				if(cnt == 1 && strcmp(comm, END) == 0){
					PQclear(res);
					return 0;
				}
			}
		}

	}
	
}
				else
				{
					resultRows = PQntuples(res);
					if (resultRows == 0)
					{
						sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1503, ENTER, DATA_END);
					}
					else
					{
						sprintf(sendBuf, "商品ID、商品名、レシピ%s", ENTER); // 注文されたメニューを表示
						for (i = 0; i < resultRows; i++)
						{
							sprintf(buf, "%s  %s  %s  %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END);
							strcat(sendBuf, buf);
						}
					}
					sendLen = strlen(sendBuf);							   // 送信データ長
					send(__soc, sendBuf, sendLen, 0);					   // 送信
					printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データ表示

					// 注文されたメニューを表示
				}
			}
			else if (strcmp(comm, END) == 0)
			{
				sendLen = strlen(sendBuf);
				send(__soc, sendBuf, sendLen, 0);
				printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
				return 0;
			}
			else
			{
				sprintf(sendBuf, "RINFOかENDを入力してください%s%s", ENTER, DATA_END);
				sendLen = strlen(sendBuf);
				send(__soc, sendBuf, sendLen, 0);
				printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			}
		}
	}
}
