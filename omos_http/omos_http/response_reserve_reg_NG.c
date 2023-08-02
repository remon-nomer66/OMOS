#include "omos_http.h"

int response_reserve_reg_NG(char *http_header, char *http_body, int *u_info, int *body_size){
    int size;
    http_body[0] = '\0';

    sprintf(http_body, "<h5>予約可能な数を超えています</h5><br><a href=\"./reserve.html\">OK</a>");
    size = strlen(http_body);
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}
