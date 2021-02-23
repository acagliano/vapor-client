
#ifndef library_h
#define library_h

typedef struct _library {
    char name[9];
    uint8_t type;
    uint8_t sha1[20];
} library_t;

void ui_ShowLibrary(bool show_upd);
void lib_Init(void);
bool library_set_entry(library_t* libr);
void library_load_sha1(uint8_t* sha1_out, const char* name, uint8_t type);

#endif
