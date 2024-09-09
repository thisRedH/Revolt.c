#include <revolt/revolt.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *token = getenv("REVOLT_TOKEN");
    Revolt* c = revolt_init(
        token,
        revolt_false,
        "https://app.revolt.chat/api",
        "wss://ws.revolt.chat?version=1&format=json"
    );
    RevoltResponse *user = NULL;

    if (c == NULL)
        fprintf(stderr, "[ERROR]: Could not initialize the client\n");

    user = revolt_fetch_me(c);
    printf("\n%s\n%s\n", revolt_response_bytes_str(&(user->header)), revolt_response_bytes_str(&(user->body)));
    revolt_cleanup(c);
    return 0;
}
