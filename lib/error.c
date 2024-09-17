#include "revolt/error.h"
#include <curl/curl.h>

const char *const REVOLT_ERR_STR_OK                 = "No error";
const char *const REVOLT_ERR_STR_UNKNOWN            = "Unknown error";

const char *const REVOLT_ERR_STR_NOT_IMPLEMENTED    = "Function not yet implemented";
const char *const REVOLT_ERR_STR_DEPRECATED         = "Function removed due to deprecation";
const char *const REVOLT_ERR_STR_NOMEM              = "Memory allocation failed";
const char *const REVOLT_ERR_STR_INVAL              = "Invalid arguments passed to function";
const char *const REVOLT_ERR_STR_LIMIT              = "Value/argument too large or too small";
const char *const REVOLT_ERR_STR_OVERFLOW           = "Overflow occurred";
const char *const REVOLT_ERR_STR_UNDERFLOW          = "Underflow occurred";
const char *const REVOLT_ERR_STR_PARSE              = "Parsing error";

const char *const REVOLT_ERR_STR_IO                 = "Generic I/O error";
const char *const REVOLT_ERR_STR_PERMISSION         = "Permission denied";
const char *const REVOLT_ERR_STR_NOT_CONNECTED      = "Not connected to remote host";
const char *const REVOLT_ERR_STR_CONNECT            = "Could not connect to remote host";
const char *const REVOLT_ERR_STR_DISCONNECT         = "Disconnected from remote host";
const char *const REVOLT_ERR_STR_TIMEOUT            = "Connection timed out";
const char *const REVOLT_ERR_STR_SEND               = "Could not send data";
const char *const REVOLT_ERR_STR_RECV               = "Could not receive data";
const char *const REVOLT_ERR_STR_INSUFFICIENT_DATA  = "Insufficient data";

const char *const REVOLT_ERR_STR_DEPENDENCY         = "Dependency error";
const char *const REVOLT_ERR_STR_CURL_INIT          = "CURL initialization failed";

const char *revolt_err_str(RevoltErr err) {
    if ((err & REVOLT_ERR_3RD_PARTY_MASK) > 0) {
        switch (err & REVOLT_ERR_3RD_PARTY_MASK) {
            case REVOLTE_CURL:
                return curl_easy_strerror(err & ~REVOLTE_CURL);
            default:
                return "Unknown 3rd Party RevoltErr value";
        }
    }

    switch (err) {
        case REVOLTE_OK:                return REVOLT_ERR_STR_OK;
        case REVOLTE_UNKNOWN:           return REVOLT_ERR_STR_UNKNOWN;

        case REVOLTE_NOT_IMPLEMENTED:   return REVOLT_ERR_STR_NOT_IMPLEMENTED;
        case REVOLTE_DEPRECATED:        return REVOLT_ERR_STR_DEPRECATED;
        case REVOLTE_NOMEM:             return REVOLT_ERR_STR_NOMEM;
        case REVOLTE_INVAL:             return REVOLT_ERR_STR_INVAL;
        case REVOLTE_LIMIT:             return REVOLT_ERR_STR_LIMIT;
        case REVOLTE_OVERFLOW:          return REVOLT_ERR_STR_OVERFLOW;
        case REVOLTE_UNDERFLOW:         return REVOLT_ERR_STR_UNDERFLOW;
        case REVOLTE_PARSE:             return REVOLT_ERR_STR_PARSE;

        case REVOLTE_IO:                return REVOLT_ERR_STR_IO;
        case REVOLTE_PERMISSION:        return REVOLT_ERR_STR_PERMISSION;
        case REVOLTE_NOT_CONNECTED:     return REVOLT_ERR_STR_NOT_CONNECTED;
        case REVOLTE_CONNECT:           return REVOLT_ERR_STR_CONNECT;
        case REVOLTE_DISCONNECT:        return REVOLT_ERR_STR_DISCONNECT;
        case REVOLTE_TIMEOUT:           return REVOLT_ERR_STR_TIMEOUT;
        case REVOLTE_SEND:              return REVOLT_ERR_STR_SEND;
        case REVOLTE_RECV:              return REVOLT_ERR_STR_RECV;
        case REVOLTE_INSUFFICIENT_DATA: return REVOLT_ERR_STR_INSUFFICIENT_DATA;

        case REVOLTE_DEPENDENCY:        return REVOLT_ERR_STR_DEPENDENCY;
        case REVOLTE_CURL_INIT:         return REVOLT_ERR_STR_CURL_INIT;

        default:                        return "Unknown RevoltErr value";
    }
}
