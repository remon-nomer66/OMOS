#include "omos_http.h"

int response_user_NG(char *http_header, char *http_body, int *u_info, char *pw, int *body_size){
    int size;
    http_body[0] = '\0';

    sprintf(http_body, "<p>電話番号またはパスワード，もしくはその両方が誤っています</p>");
    size = strlen(http_body);
    *body_size = size;
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加
    sprintf(http_header + strlen(http_header), "Set-Cookie: id=0\r\n");
    sprintf(http_header + strlen(http_header), "Set-Cookie: pw=none\r\n");

    return 0;
}
