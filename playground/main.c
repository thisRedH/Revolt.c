#include <revolt/revolt.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    Revolt* c = NULL;
    RevoltResponse *user = NULL;
    //c = revolt_bot_easy_init("TOKEN");
    user = revolt_fetch_me(c);
    printf("\n%s\n%s\n", revolt_response_bytes_str(&(user->header)), revolt_response_bytes_str(&(user->body)));
    revolt_cleanup(c);
    return 0;
}
