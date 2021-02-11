
#ifndef fileaccess_h
#define fileaccess_h

#include <stdint.h>
#include <fileioc.h>

typedef struct _file_start { char name[9]; uint8_t type; uint8_t archive; uint24_t crc; } file_start_t;

extern bool file_init_error;
extern bool file_checksum_invalid;
extern file_start_t file_data;

#endif
