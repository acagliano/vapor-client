#ifndef content_h
#define content_h

#include <stddef.h>
#include <stdint.h>
#include <graphx.h>
#include <stdbool.h>
#include "../fileaccess.h"
#include "../network/srv_types.h"

#define gfx_RestoreCanvas() \
        gfx_FillScreen(BG_COLOR)

#define gfx_FillRectangleColor(x, y, w, h, color) \
        gfx_SetColor((color)); \
        gfx_FillRectangle((x), (y), (w), (h));
        
#define gfx_RectangleColor(x, y, w, h, color) \
        gfx_SetColor((color)); \
        gfx_Rectangle((x), (y), (w), (h));
        
#define gfx_PrintUIntXY(int, x, y) \
        gfx_SetTextXY((x), (y)); \
        gfx_PrintUInt((int), num_len(int));
        
#define gfx_PrintUIntSize(int) \
        gfx_PrintUInt((int), num_len(int));
        
#define gfx_FillCircleColor(x, y, r, color) \
        gfx_SetColor(color); \
        gfx_FillCircle((x), (y), (r));

#define BG_COLOR 74
#define IND_COLOR 159
#define NAV_COLOR 181
#define NAV_SELECTED 148
#define TEXT 223
#define ONLINE_COLOR 38
#define OFFLINE_COLOR 224

extern bool queue_update;

extern uint8_t menu_item_selected;
enum _menu_items {
    MENU_NULL,
    MENU_LIBRARY,
    MENU_SERVICES
};

typedef struct _library {
    char name[9];
    uint8_t type;
    date_t date;
} library_t;


void ui_RenderBackground(void);
void ui_ShowLibrary(bool show_upd);
void ui_RenderContent(void);
void lib_Init(void);
void ui_RenderNavBar(void);
void ui_RenderServicesContent(void);
void ui_ContentWindow(const char* error_t, const char* error_m, uint8_t border_color);
#define ui_ErrorWindow(title, msg)      ui_ContentWindow((title), (msg), (224));
#define ui_SuccessWindow(title, msg)    ui_ContentWindow((title), (msg), (36));

uint8_t num_len(uint24_t num);
uint8_t text_WrappedString(const char *str, uint24_t left_margin, uint8_t top_margin, uint24_t right_margin);
uint24_t text_CenterText(const char *string, const char c, uint24_t x, uint24_t w);
void gfx_PrintStringCentered(const char* string, uint24_t x, uint8_t y, uint24_t w);

uint32_t crc32(const void *data, size_t n_bytes, uint32_t* crc);
bool library_set_entry(library_t* libr);
void library_load_date(dl_list_t* dl);

void srvc_show_dl_list(void);
void srvc_request_file(dl_list_t* dl);
void srvc_show_dl_bar(void);

#endif
