#include "omos_http.h"

int response_user_NG(char *http_header, char *http_body, int *u_info, int *body_size){
    http_body[0] = '\0';

    sprintf(http_body, "<p>予約可能な数を超えています</p>");
    body_size = strlen(http_body);
    
    sprintf(http_header, "Content-Length: %d\r\n", body_size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}