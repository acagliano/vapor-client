
#include <stdint.h>
#include <stddef.h>
#include <tice.h>
#include <string.h>
#include "content.h"
#include "../network/controlcodes.h"
#include "../network/network.h"
#include "../network/srv_types.h"
#include "library.h"


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


void srvc_request_file(dl_list_t* dl){
    file_metadata_t md = {0};
    strncpy(md.name, dl->name, 8);
    md.type=dl->type;
    library_load_sha1(&md.sha1, dl->name, dl->type);
    ntwk_send(FILE_WRITE_START, PS_PTR(&md, sizeof(file_metadata_t)));
}

void srvc_show_dl_list(void){
    uint8_t h = 10 * curr_total + 20;
    uint8_t y_init = 120 - (h / 2);
    uint24_t i;
    uint8_t color = gfx_SetTextFGColor(231);
    gfx_FillRectangleColor(60, y_init - 12, 200, h, 33);
    gfx_FillRectangleColor(62, y_init, 196, h - 14, 231);
    gfx_PrintStringCentered("=DOWNLOADING=", 60, y_init-10, 200);
    gfx_SetTextFGColor(192);
    for(i=0; i<curr_total; i++){
        uint8_t y = (10 * i) + y_init + 4;
        uint8_t status = dl_list[i].status;
        gfx_PrintStringXY(dl_list[i].name, 64, y);
        if(status == DL_WAIT) gfx_PrintStringXY("waiting", 140, y);
        else if(status == DL_SKIP) gfx_PrintStringXY("skipped", 140, y);
        else if(status == DL_IO_ERR) gfx_PrintStringXY("srv.IOErr", 140, y);
        else if(status == DL_VERIFY) gfx_PrintStringXY("validating", 140, y);
        else if(status == DL_DONE) gfx_PrintStringXY("done", 140, y);
        else if(status == DL_VERIF_ERR) gfx_PrintStringXY("dl error", 140, y);
    }
    gfx_SetTextFGColor(color);
    gfx_BlitRectangle(gfx_buffer, 60, y_init-12, 200, h);
    for(i=0; i<100; i++) boot_WaitShort();
    
}

void srvc_show_dl_bar(void){
    uint8_t h = 10 * curr_total + 20;
    uint8_t y_init = 120 - (h / 2);
    uint8_t y = (10 * curr_dl) + y_init + 4;
    uint24_t barw = bytes_copied * 68 / dl_list[curr_dl].size;
    gfx_RectangleColor(138, y, 70, 8, 0);
    gfx_FillRectangleColor(139, y, barw, 8, 228);
    gfx_BlitRectangle(gfx_buffer, 138, y, 70, 10);
}
