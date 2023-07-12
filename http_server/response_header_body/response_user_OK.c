#include "omos_http.h"

int response_user_OK(char *http_header, char *http_body, int *u_info, int *body_size){
    http_body[0] = '\0';
    *body_size = 0;
    
    sprintf(http_header, "Content-Length: 0\r\n");
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加
    sprintf(http_header + strlen(http_header), "Set-Cookie: id=%s\r\n", u_info[0]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: auth=%s\r\n", u_info[1]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: bsid=%s\r\n", u_info[2]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: pw=%s\r\n", u_info[3]);
    sprintf(http_header + strlen(http_header), "Set-Cookie: csid=%s\r\n", 1);
    sprintf(http_header + strlen(http_header), "Set-Cookie: ctnum=%s\r\n", 1);
    sprintf(http_header + strlen(http_header), "Set-Cookie: layer=%s\r\n", 1);
    sprintf(http_header + strlen(http_header), "Location: /branch.html");

    return 0;
}