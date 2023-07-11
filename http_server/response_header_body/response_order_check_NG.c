#include "omos_http.h"

int response_order_check_NG(char *http_header, char *http_body, int *u_info, int *body_size){
    http_body[0] = '\0';
    *body_size = 0;
    
    sprintf(http_header, "Content-Length: 0\r\n");
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加
    
    return 0;
}