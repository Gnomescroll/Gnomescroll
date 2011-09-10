#include "net_time.h"




int LAST_NETPEER_TIME = -1;
int CURRENT_NETPEER_TIME = -1;

void update_current_netpeer_time() {
    LAST_NETPEER_TIME = CURRENT_NETPEER_TIME;
    CURRENT_NETPEER_TIME = _GET_MS_TIME();
    return;
}

void NP_print_delta() {
    printf("NP_print_delta: %i\n", CURRENT_NETPEER_TIME-LAST_NETPEER_TIME); //, CURRENT_NETPEER_TIME, LAST_NETPEER_TIME);
}
