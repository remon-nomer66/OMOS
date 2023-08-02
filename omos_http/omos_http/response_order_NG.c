#include "omos_http.h"

int response_order_NG(char *http_header, char *http_body, int *u_info, int *body_size){
    int size;

    http_body[0] = '\0';

    sprintf(http_body, "<p>注文の際に問題が発生しました</p>");
    size = strlen(http_body);
    *body_size = size;
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}
