#include <stdarg.h>
#undef NDEBUG
#include <debug.h>
#include "network.h"

net_mode_t *mode;
uint24_t ntwk_inactive_clock;

bool ntwk_init(void) {
    if(cemu_check()) {
        dbg_sprintf(dbgout, "Running in CEmu pipe mode\n");
        mode = &mode_pipe;
    }
    else { mode = &mode_srl; }

    return mode->init();
}

void ntwk_process(void) {
    uint8_t i;
    static size_t packet_size = 0;

    for(i = 0; i < 2 * 1; i++ ) {
        if(mode->process) mode->process();

        /* If the device was disconnected, exit */
        if(!netflags.network_up) return;

        /* Handle input */
        if(packet_size) {
            if(mode->read_to_size(packet_size)) {
                ntwk_inactive_clock = 0;
                conn_HandleInput((packet_t *) &net_buf, packet_size);
                packet_size = 0;
            } else break;
        } else {
            if(mode->read_to_size(sizeof(packet_size))) packet_size = *(size_t*)net_buf;
            else break;
        }
    }
}

bool ntwk_send_(uint8_t num_parts, uint8_t ctrl, ...) {
    uint8_t i;
    va_list ap;
    size_t total_size = 1;

    if(!netflags.network_up) return false;

    if(num_parts) {
        va_start(ap, num_parts);
        for(i = 0; i < num_parts; i++) {
            va_arg(ap, void*);
            total_size += va_arg(ap, size_t);
        }
        va_end(ap);
    }

    dbg_sprintf(dbgout, "Total size: %u\n", total_size);

    mode->write(&total_size, sizeof(total_size));

    mode->write(&ctrl, sizeof(ctrl));

    if(num_parts) {
        va_start(ap, num_parts);
        for(i = 0; i < num_parts; i++) {
            void *data = va_arg(ap, void*);
            size_t size = va_arg(ap, size_t);

            mode->write(data, size);
        }
        va_end(ap);
    }

    return true;
}

