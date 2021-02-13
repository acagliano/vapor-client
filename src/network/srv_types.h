
#ifndef srvtypes_h
#define srvtypes_h

#include <stddef.h>
#include <stdint.h> 
#include <stdbool.h>

extern uint8_t vapor_status;
extern uint24_t vapor_timeout;
enum _vapor_status {
    VAPOR_OFFLINE,
    VAPOR_WAIT,
    VAPOR_CONNECTED,
};

// Define Package List Type
typedef struct _srv_pkg_list {
    char name[9];
    uint8_t type;
    uint32_t crc;
} srv_pkg_list_t;

// Define Services List Type
typedef struct _srv_list {
    char name[9];
    char host[37];
    uint16_t port;
    uint8_t online;
} srv_list_t;
extern srv_list_t* services_arr;        // externally declare this
extern uint24_t service_selected;       // service selected
extern uint24_t service_count;          // num of services

typedef struct _srv_deps {
    char name[9];
    uint8_t type;
} srv_deps_t;
typedef srv_pkg_list_t dep_info_t;

enum _dl_status {
    DL_WAIT,
    DL_SKIP,
    DL_VERIFY,
    DL_DONE,
    DL_CRC_ERR,
    DL_IO_ERR
};
typedef struct dl_list {
    char name[9];
    uint8_t type;
    uint32_t crc;
    uint8_t status;
} dl_list_t;
extern dl_list_t *dl_list;
extern uint8_t curr_dl;
extern uint8_t curr_total;

extern size_t dl_size;
extern size_t bytes_copied;

#endif
