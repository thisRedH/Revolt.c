#include <revolt/revolt.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    Revolt* c = revolt_bot_easy_init("TOKEN");

    RevoltResponse *user = revolt_fetch_me(c);
    printf("\n%s\n%s\n", revolt_response_bytes_str(&(user->header)), revolt_response_bytes_str(&(user->body)));
    return 0;
}
