#include "omos_http.h"

int response_user_OK(char *http_header, char *http_body, int *u_info, char *pw, int *body_size){
    http_body[0] = '\0';
    int size;

    sprintf(http_body, "<a href=\"./branch.html\">選択画面に移動</a>");

    size = strlen(http_body);
    *body_size = size;
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加
    sprintf(http_header + strlen(http_header), "Set-Cookie: id=%d\r\n", u_info[0]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: auth=%d\r\n", u_info[1]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: bsid=%d\r\n", u_info[2]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: pw=%s\r\n", pw);
    sprintf(http_header + strlen(http_header), "Set-Cookie: csid=%d\r\n", 1);
    sprintf(http_header + strlen(http_header), "Set-Cookie: ctnum=%d\r\n", 1);
    sprintf(http_header + strlen(http_header), "Set-Cookie: layer=%d\r\n", 1);

    return 0;
}
