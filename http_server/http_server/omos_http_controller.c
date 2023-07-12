#include "omos_http.h"

/**
 * OMOSメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *omos_http_controller(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    pthread_t selfId;
    int cnt, kitchen_y_n;
    int table_num;
    int reg[2];
    int flag, f_login;
    int store_id;
    int u_info[4]; //[0]: アカウントID(user_id)，[1]: 権限, [2]: 店舗番号(store_id), [3]: p/w
    int s_info[3] = {10, 2, 0};  //[0]: 店舗番号，[1]: 卓番号, [2]: kitchen(y/n)

    char command[5][BUFSIZE];
    char tmpRecvBuf[BUFSIZE];
    char param[5][BUFSIZE];
    int branch_result;  //omos_http_branchからの結果(0 or -1)
    char http_header[BUFSIZE], http_body[BUFSIZE], http_response[BUFSIZE];
    int body_size, response_size;
    int layer;

    recvLen = flag = f_login = 0;
    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){

        //リクエストメッセージ受信
        recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
        if(recvLen == 0 || recvLen == -1){
            break;
        }

        //サーバにHTTPリクエストの内容を表示
        http_rq_re(selfId, threadParam->soc, recvBuf, recvLen, 0);

        //method判定
        if(strncmp(recvBuf, "GET", 3) == 0){
            strcpy(tmpRecvBuf, recvBuf);
            httpGetCommand(tmpRecvBuf, command, param, u_info, s_info, layer);
        }else if(strncmp(recvBuf, "POST", 4) == 0){
            strcpy(tmpRecvBuf, recvBuf);
            httpPostCommand(tmpRecvBuf, command, param, u_info, s_info, layer);
        }

        //コマンド判定，操作
        branch_result = omos_http_branch(selfId, theradParam->con, threadPram->soc, u_info, s_info, command, param, http_header, http_body, &body_size, layer);

        //レスポンスメッセージの作成
        response_size = create_http_response(http_response, branch_result, http_header, http_body, body_size);
        if(response_size == -1){
            printf("レスポンスメッセージの作成に失敗\n");
            break;
        }

        //サーバにHTTPレスポンスの内容を表示
        http_rq_re(selfId, threadParam->soc, http_response, response_size, 1);

        //レスポンスメッセージ送信
        sendLen = send(threadParam->soc, http_response, response_size, 0);
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
