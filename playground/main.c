#define _DEFAULT_SOURCE
#define RVLTC_DEFINE_SLEEP 1
#include <revolt/revolt.h>
#include <stdio.h>
#include <pthread.h>

#define HERTZ       60
#define SLEEP_FOR   (int)(1000 / HERTZ)

RevoltErr on_str(
    enum RevoltWSEvent ev,
    revolt_byte *data,
    size_t data_len,
    RevoltWS *ws,
    void *userp
) {
    printf("[RECV] %i %lu %s\n", ev, data_len, data);

    return REVOLTE_OK;
}

void* thread_fc(void *client) {
    revolt_run(client);
    return NULL;
}

int main(void) {
    RevoltErr res;
    Revolt* client;
    const char *token = getenv("REVOLT_TOKEN");
    RevoltcHTTPResponse *user = NULL;
    pthread_t thread;
    char input[64];

    client = revolt_init(
        token,
        revolt_false,
        "https://app.revolt.chat/api",
        "wss://ws.revolt.chat?version=1&format=json"
    );
    if (client == NULL) {
        fprintf(stderr, "[ERROR]: Could not initialize the client\n");
        return 1;
    }

    user = revolt_rest_fetch_me(client->rest);
    printf("\n%s\n", user->body);

    revolt_ws_set_on_str(client->ws, on_str, NULL);
    revolt_ws_send_str(client->ws, "{\"type\":\"Ping\",\"data\":6942069}");

    if (pthread_create(&thread, NULL, thread_fc, client) != 0) {
        fprintf(stderr, "[ERROR]: Could not a thread\n");
        return 1;
    }

    while (1) {
        if (fgets(input, sizeof(input), stdin) != NULL) {
            if (input[0] == 'q') {
                revolt_stop(client);
                break;
            }
        }
    }

    pthread_join(thread, NULL);

    revolt_cleanup(client);
    return 0;
}
