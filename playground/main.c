#define _DEFAULT_SOURCE
#define REVOLTC_DEFINE_SLEEP 1
#include <revolt/revolt.h>
#include <revolt/core/util.h>
#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>

RevoltErr on_status(enum RevoltWSEvent ev, revolt_byte *data, size_t data_len, RevoltWS *ws, void *userp) {
    switch (ev) {
        case REVOLT_WS_EV_OPEN:
            printf("[WS]: Opened!\n");
            break;
        case REVOLT_WS_EV_CLOSE:
            printf("[WS]: Closed!\n");
            break;
        case REVOLT_WS_EV_RECV_PING:
            printf("[WS]: Ping!\n");
            break;
        case REVOLT_WS_EV_RECV_PONG:
            printf("[WS]: Pong!\n");
            break;
        default:
            break;
    }
    return REVOLTE_OK;
}

RevoltErr on_recv(enum RevoltWSEvent ev, revolt_byte *data, size_t data_len, RevoltWS *ws, void *userp) {
    if (ev == REVOLT_WS_EV_RECV_STR) {
        printf("[WS] RECV: %i %lu \"%s\"\n", ev, data_len, data);
    } else if (ev == REVOLT_WS_EV_RECV_BIN) {
        printf("[WS] RECV: binary data\n");
    }
    return REVOLTE_OK;
}

void* thread_fc(void *ctx) {
    Revolt *client = ctx;
    RevoltErr res;

    res = revolt_run(client);
    if (res != REVOLTE_OK)
        printf("[ERROR][thread_fc]: res = %s\n", revolt_err_str(res));

    return NULL;
}

int main(void) {
    RevoltErr res;
    Revolt* client;
    const char *token = getenv("REVOLT_TOKEN");
    const RevoltUser *user;
    const RevoltChannel *channel;
    pthread_t thread;
    char input[64];

    client = revolt_init(
        token,
        revolt_false,
        REVOLT_REST_DEFAULT_URL,
        REVOLT_WS_DEFAULT_URL
    );
    if (client == NULL) {
        fprintf(stderr, "[ERROR]: Could not initialize the client\n");
        return 1;
    }

    user = revolt_fetch_self(client, &res);
    channel = revolt_fetch_channel(client, "01J0BVBXCXN63JT3GMBXYW", 0);
    if (res != REVOLTE_OK) {
        fprintf(stderr, "[ERROR]: Could not fetch user err: %s\n", revolt_err_str(res));
    }
    revolt_user_delete((RevoltUser*) user);
    revolt_channel_delete((RevoltChannel*) channel, revolt_false);

    revolt_ws_set_on_open(client->ws, on_status, NULL);
    revolt_ws_set_on_close(client->ws, on_status, NULL);
    revolt_ws_set_on_ping(client->ws, on_status, NULL);
    revolt_ws_set_on_pong(client->ws, on_status, NULL);
    revolt_ws_set_on_str(client->ws, on_recv, NULL);

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
    curl_global_cleanup();
    return 0;
}
