
#ifndef fileaccess_h
#define fileaccess_h

#include <stdint.h>
#include <fileioc.h>

typedef struct _file_start { char name[9]; uint8_t type; uint32_t crc; uint8_t archive; } file_start_t;

extern bool file_init_error;
extern file_start_t file_data;

#endif
