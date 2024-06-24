#include <revolt/common.h>
#include <revolt/client.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    Revolt* c = revolt_bot_easy_init("TOKEN");
    RevoltResponse *user = revolt_http_user_me(c->http);
    printf("Hello World! %s\n%s", revolt_response_bytes_str(&(user->header)), revolt_response_bytes_str(&(user->body)));
    return 0;
}
