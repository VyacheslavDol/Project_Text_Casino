#ifndef MAIL_SERVICE
#define MAIL_SERVICE

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "../db/db.h"

#define PAYLOAD_SIZE 1024

#define MAIL_USERNAME getenv("MAIL_USERNAME")
#define MAIL_PASSWORD getenv("MAIL_PASSWORD")
#define MAIL_SMTP_SERVER getenv("MAIL_SMTP_SERVER")

int send_confirmation_email(const char* recipient_mail, const char* uuid);
void gen_uuid_mail(char* uuid_mail);
void db_add_link(const char* uuid_user, const char* uuid_mail);
int acc_conf(const char* uuid);

#endif