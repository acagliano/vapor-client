#ifndef functions_h
#define functions_h

#include <stdbool.h>

uint8_t getKey(void);
void update_program(void);
void run_program(const char *name, uint8_t type);
bool load_library(const uint8_t *data);
bool load_hashlib(void);
bool load_bigint(void);

void _hashlib_init(void); /* might wanna replace this with an actual function */
void _bigint_init(void); /* might wanna replace this with an actual function */

#endif
