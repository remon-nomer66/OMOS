/**
 * OMOSサーバ(端末版)
 * omos_server_main.c
*/

#include "omos.h"

void service_start(int __lsoc);

/**
 * メイン関数
 * [ARGUMENT]
 *      argc    : コマンドライン引数の個数
 *      argv    : コマンドライン引数の配列へのポインタ
 * [RETURN]
 *      1       : エラー発生
 */
int main(int argc, char *argv[]){
    int listenSoc;
    unsigned short port;

    if(argc == 2){
        port = atoi(argv[1]);
    }else{
        port = PORT;
    }

    if((listenSoc = setup_listen(port)) < 0){
        exit(1);
    }

    service_start(listenSoc);

    return 1;
}

/**
 * スレッド起動ベース
 * [ARGUMENT]
 *      __lsoc  : LISTENソケットディスクリプタ
 * [RETURN]
 *      NONE
 */
void service_start(int __lsoc){
    char *dbHost = "kite.cs.miyazaki-u.ac.jp";
    char *dbPort = "5432";
    char *dbName = "db13";      //接続先を正しく入力
    char *dbLogin = "dbuser13";
    char *dbPwd = "dbpass13";
    char connInfo[BUFSIZE];
    char sendBuf[BUFSIZE];
    pthread_t worker;
    ThreadParameter *threadParam;
    struct sockaddr_in client;
    unsigned int client_len;
    int s_new, sendLen;

    while(1){
        client_len = sizeof(client);
        printf("[MAIN THREAD] Accept Call...\n");
        s_new = accept(__lsoc, (struct sockaddr *)&client, &client_len);

        //スレッド引数構造体に値セット
        threadParam = (ThreadParameter *)malloc(sizeof(ThreadParameter));
        threadParam->c_ipaddr = client.sin_addr;
        threadParam->c_port = client.sin_port;
        threadParam->soc = s_new;

        //DB接続
        sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
        threadParam->con = PQconnectdb(connInfo);
    
        //接続確認
        if( PQstatus(threadParam->con) == CONNECTION_BAD ){
            printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
            printf("%s", PQerrorMessage(threadParam->con));
            threadParam->con = NULL;
            sendLen = sprintf(sendBuf, "error occured%s", ENTER);
            send(s_new, sendBuf, sendLen, 0);
        }else{
            printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
            pthread_create(&worker, NULL, (void *)omos_service, (void *)threadParam);
            printf("[MAIN THREAD] Created thread ID: %ld\n", worker);
            pthread_detach(worker);
        }
    }
}
