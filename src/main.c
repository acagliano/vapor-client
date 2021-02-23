//--------------------------------------
// Program Name: VAPOR
// Author: Anthony Cagliano
// License:
// Description:
//--------------------------------------

/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <intce.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Other available headers */
// stdarg.h, setjmp.h, assert.h, ctype.h, float.h, iso646.h, limits.h, errno.h, debug.h
#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>

// Game Data Types (Structure Definitions)
#include "ui/content.h"
#include "ui/library.h"
#include "flags.h"
#include "network/controlcodes.h"
#include "network/network.h"
#include "asm/functions.h"
#include "network/srv_types.h"

// USB Libraries
#include <usbdrvce.h>

#define DEBUG
#undef NDEBUG
#include <debug.h>

net_flags_t netflags = {0};
uint24_t service_selected=0;
uint24_t service_count=0;
bool exit_game=false;

bool prefer_ssl=false;
const char server[]="vapor.cagstech.com:51000";

uint8_t getKey(void);   // from asm/getkey.asm

void MainMenu(void);
uint8_t vapor_status=false;
#define VAPOR_TIMEOUT 2000
uint24_t vapor_timeout=0;
bool queue_update=true;


int main(void) {
    sk_key_t key=0;
    bool first_loop=true;
    
    lib_Init();
    gfx_Begin();
   
    ntwk_init();
    
    ti_SetGCBehavior(&gfx_End, &gfx_Begin);
    gfx_SetDefaultPalette(gfx_8bpp);
    gfx_SetDrawBuffer();
    gfx_SetTextTransparentColor(1);
    gfx_SetTextBGColor(1);
    gfx_FillScreen(255);
    gfx_RestoreCanvas();
    gfx_BlitBuffer();
    ui_RenderContent();
    do {
        key=getKey();
        if(key==sk_Yequ) {
            ntwk_send(CONNECT, PS_VAL(prefer_ssl), PS_STR(server));
            vapor_status=VAPOR_WAIT;
            vapor_timeout=VAPOR_TIMEOUT;
            queue_update=true;
        }
        if(key==sk_Up) {menu_item_selected-=(menu_item_selected>1);}
        if(key==sk_Down) {menu_item_selected+=(menu_item_selected<MENU_SERVICES);}
        if(menu_item_selected==MENU_SERVICES){
            if(key==sk_Zoom){
                gfx_FillCircleColor(90, 10*service_selected+13, 3, BG_COLOR );
                service_selected+=(service_selected<(service_count-1));
            }
            if(key==sk_Window){
                gfx_FillCircleColor(90, 10*service_selected+13, 3, BG_COLOR );
                service_selected-=(service_selected>0);
            }
            if(key==sk_Trace){
                ntwk_send_nodata(FETCH_SERVER_LIST);
            }
            if(key==sk_Graph){
                ntwk_send(SRVC_GET_REQ, PS_STR(services_arr[service_selected].name));
            }
        }
        if(key==sk_Clear){
            ntwk_send_nodata(DISCONNECT);
            break;
        }
        if(key) queue_update=true;
        ntwk_process();
        if((vapor_status!=VAPOR_CONNECTED) && vapor_timeout){
            vapor_timeout--;
            if(vapor_timeout==0){
                vapor_status=false;
                queue_update=true;
            }
        }
        if(queue_update) {ui_RenderContent(); queue_update=false;}
    } while(1);
    //cache_purge();
    ti_CloseAll();
    free(services_arr);
    gfx_End();
    usb_Cleanup();
    pgrm_CleanUp();
    return 0;
}

