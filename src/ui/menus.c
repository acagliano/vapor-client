#include <stdint.h>
#include <stdbool.h>
#include "content.h"
#include "../gfx/graphics.h"
#include "../network/srv_types.h"


uint8_t menu_item_selected=MENU_NULL;

char menu_text[][10]={
    " ",
    "LIBRARY",
    "SERVICES"
};

char nav_bar_text[][10]={
    " Connect",
    "\30",
    "\31"
};

char nav_opts_btm[][10]={
    "Pkg Upd",
    "Plchld",
    "Srv List",
    "Srv Join"
};


void ui_button_indic(const char* string, const char c, uint24_t x, uint8_t y, uint24_t w, uint8_t h, bool center_text, bool active){
    gfx_RectangleColor(x, y, w, h, 0);
    gfx_SetColor(NAV_COLOR);
    if(active) gfx_SetColor(NAV_SELECTED);
    gfx_FillRectangle(x+1, y+1, w-2, h-2);
    if(center_text) x=text_CenterText(string, c, x, w);
    if(string!=NULL) gfx_PrintStringXY(string, x, y+6);
    else if(c) { gfx_SetTextXY(x, 228); gfx_PrintChar(c); }
}

void ui_render_vapor_status(void){
    if(vapor_status==VAPOR_OFFLINE) gfx_SetColor(192);
    else if(vapor_status==VAPOR_WAIT) gfx_SetColor(231);
    else if(vapor_status==VAPOR_CONNECTED) gfx_SetColor(36);
    gfx_FillRectangle(73, 240-20+1, 20-2, 20-2);
    gfx_RLETSprite(vapor_indic, 73, 240-20+1);
}

void ui_RenderNavBar(void){
    uint8_t i, color=gfx_SetTextFGColor(0);
    gfx_RLETSprite(main_icon, 12, 0);
    for(i=1; i<3; i++)
        ui_button_indic(menu_text[i], 0, 0, (i-1)*20+50, 76, 20, true, (menu_item_selected==i));
    ui_button_indic("  Connect", 0, 0, 240-20, 95, 20, false, false);
    ui_render_vapor_status();
    if(menu_item_selected>0){
        ui_button_indic(NULL, 30, 96, 240-20, 32, 20, true, false);
        ui_button_indic(NULL, 31, 128, 240-20, 32, 20, true, false);
        ui_button_indic(nav_opts_btm[(menu_item_selected-1)*2], 0, 160, 240-20, 80, 20, true, false);
        ui_button_indic(nav_opts_btm[(menu_item_selected-1)*2+1], 0, 240, 240-20, 80, 20, true, false);
    }
    gfx_SetTextFGColor(color);
}
