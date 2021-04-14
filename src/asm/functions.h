#ifndef functions_h
#define functions_h

#include <stdbool.h>

uint8_t getKey(void);
void update_program(void);
void run_program(const char *name, uint8_t type);
bool load_library(const char *libname);


#endif
