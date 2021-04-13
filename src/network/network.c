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
    static size_t bytes_read = 0;
    
    bytes_read = srl_Read(&srl, net_buf, net_buf_size);
    if(bytes_read >= sizeof(packet_size)) packet_size = *net_buff;
    if(packet_size)
        if(bytes_read >= (packet_size+3))
            conn_HandleInput(net_buff+3, packet_size);
    
    // now what?
    
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

