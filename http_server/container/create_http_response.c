#include "omos_http.h"

int createResponseMessage(char *http_response, int status, char *http_header, char *http_body, int body_size){
    unsigned int no_body_len, body_len;

    response[0] = '\0';

    if(status == 200){
        //レスポンス行とヘッダフィールド
        sprintf(http_response, "HTTP/1.1 200 OK\r\n%s\r\n", http_header);
        
        no_body_len = strlen(http_response);
        body_len = body_size;

        //ヘッダフィールドの後ろにボディをコピー
        memcpy(&http_response[no_body_len], http_body, body_len);
    }else if(status == 301){
        //レスポンス行とヘッダフィールド
        sprintf(http_response,"HTTP/1.1 301 Moved Permanently\r\n%s\r\n", http_header);

        no_body_len = strlen(http_response);
        body_len = body_size;

        //ヘッダフィールドの後ろにボディをコピー
        memcpy(&http_response[no_body_len], http_body, body_len);
    }else if(status == 400){
        //レスポンス行とヘッダフィールド
        sprintf(http_response, "HTTP/1.1 400 Bad Request\r\n%s\r\n", http_header);
        
        no_body_len = strlen(http_response);
        body_len = body_size;

        //ヘッダフィールドの後ろにボディをコピー
        memcpy(&http_response[no_body_len], http_body, body_len);
    }else if(status == 404){
        //レスポンス行とヘッダフィールド
        sprintf(http_response, "HTTP/1.1 404 Not Found\r\n%s\r\n", http_header);
        
        no_body_len = strlen(http_response);
        body_len = 0;
    }

    return no_body_len + body_len;
}