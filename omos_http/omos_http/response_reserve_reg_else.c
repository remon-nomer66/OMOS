#include "omos_http.h"

int response_reserve_reg_else(char *http_header, char *http_body, int *u_info, int *body_size){
    int size;
    http_body[0] = '\0';

    sprintf(http_body, "<h5>データベースエラーが発生しました</h5><p>管理者にお問い合わせください</p><br><a href=\"./reserve.html\">OK</a>");
    size = strlen(http_body);
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}