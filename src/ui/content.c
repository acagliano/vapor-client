#include <stdbool.h>
#include "content.h"
#include "../gfx/graphics.h"
#include "../network/srv_types.h"



const char main_text[]="VAPOR is a networking infrastructure that makes it easy to connect with hosted calculator game servers. It also maintains and auto-updates a library of Client software and dependencies needed to play said games.";



void ui_RenderNullContent(void){
    uint8_t color=gfx_SetTextFGColor(TEXT);
    gfx_PrintStringXY("http://cagstech.com/vapor", 90, 13*text_WrappedString(main_text, 90, 15, 310)+15);
    gfx_SetTextFGColor(color);
}

void ui_RenderContent(void){
    gfx_FillScreen(NAV_COLOR);
    gfx_FillRectangleColor(76, 0, 320-77, 240, BG_COLOR);
    ui_RenderNavBar();
    switch(menu_item_selected){
        case MENU_NULL:
            ui_RenderNullContent();
            break;
        case MENU_SERVICES:
            ui_RenderServicesContent();
            break;
        case MENU_LIBRARY:
            ui_ShowLibrary(false);
            break;
    }
    gfx_BlitBuffer();
}

