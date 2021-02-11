
#include "content.h"
#include "../network/srv_types.h"


void ui_RenderServicesContent(void){
    uint24_t x=85, i;
    uint8_t y, rect_color;
    uint8_t color=gfx_SetTextFGColor(TEXT);
    if(services_arr==NULL) return;
    for(i=0; i<service_count; i++){
        gfx_SetTextFGColor(TEXT);
        y=i*10+5;
        if(service_selected==i){
            gfx_FillRectangleColor(x-5, y-1, 315-x, 10, NAV_SELECTED);
            gfx_SetTextFGColor(0);
        }
        rect_color = (services_arr[i].online) ? ONLINE_COLOR : OFFLINE_COLOR;
        gfx_FillRectangleColor(x, y, 8, 8, rect_color);
        gfx_PrintStringXY(services_arr[i].name, x+14, y);
        gfx_PrintStringXY("@", x+85, y);
        gfx_PrintString(services_arr[i].host);
        gfx_PrintString(":");
        gfx_PrintUIntSize(services_arr[i].port);
    }
    gfx_SetTextFGColor(color);
}
