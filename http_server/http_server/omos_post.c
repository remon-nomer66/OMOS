#include "omos_http.h"

int httpPostCommand(char *tmpRecvBuf, char command[][BUFSIZE], char param[][BUFSIZE]int *u_info, int *s_info){
    char *cpTmpRecvBuf, *body, *buf;
    int i = 0;

    //cookie用にcp~に格納
    strcpy(cpTmpRecvBuf, tmpRecvBuf);

    //bodyにHTTPリクエストのボディを格納
    body = strstr(tmpRecvBuf, "\n\n");
    if(body == NULL){
        return -1;
    }

    //コマンド解析
    buf = strtok(body, "=");
    if(buf == NULL){
        return -1;
    }
    strcpy(command[0], buf);
    while(1){
        buf = strtok(NULL, "&\n");
        if(buf == NULL){
            return -1;
        }
        strcpy(param[i], buf);
        
        i++;
        buf = strtok(NULL, "=");
        if(buf == NULL){
            break;
        }
        strcpy(command[i], buf);
    }

    //cookieの行を探索
    buf = strstr(cpTmpRecvBuf, "Cookie");
    if(buf == NULL){
        return 0;
    }
    cookieLine = strtok(buf, "\n");

    //user_idの格納
    strtok(cookieLine, "=");
    buf = strtok(NULL, ";")
    strcpy(u_info[0], atoi(buf));

    //権限の格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    strcpy(u_info[1], atoi(buf));

    //所属店舗番号の格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    strcpy(u_info[2], atoi(buf));

    //パスワードの格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    strcpy(u_info[3], atoi(buf));

    //端末店舗番号の格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    strcpy(s_info[0], atoi(buf));

    //端末卓番号の格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    strcpy(s_info[1], atoi(buf));

    //層番号の格納
    strtok(NULL, " ");
    strtok(NULL, "=");
    buf = strtok(NULL, ";")
    *layer = atoi(buf));

    return 0;
}