#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <microhttpd.h>
#include <stdio.h>
#include <string.h>

#include "../error/error.h"
#include "../service/mail_service.h"

#define HTTP_PORT 8888

void http_server();

#endif