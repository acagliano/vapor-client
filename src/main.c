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
//#include <hashlib.h>

// Game Data Types (Structure Definitions)
#include "ui/content.h"
#include "ui/services.h"
#include "ui/library.h"
#include "ui/settings.h"
#include "flags.h"
#include "network/controlcodes.h"
#include "network/network.h"
#include "asm/functions.h"
#include "network/srv_types.h"
#include "asm/libload_custom.h"

// USB Libraries
#include <usbdrvce.h>
#include <srldrvce.h>

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
bool hashlib_available=false;

int main(void) {
    sk_key_t key=0;
    bool first_loop=true;
   // *(uint8_t*)-1 = 2;
    if(hashlib_available = load_hashlib())
        hashlib_SetMalloc(malloc);
    else {
        settings.hash_files = false;
        settings.rsa_depth = 0;
    }
        
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
        if(key==sk_Down) {menu_item_selected+=(menu_item_selected<MENU_SETTINGS);}
        if(menu_item_selected==MENU_SERVICES){
            if(key==sk_Zoom){
                gfx_FillCircleColor(90, 10*service_selected+13, 3, BG_COLOR );
                service_selected+=(service_selected<(service_count-1));
            }
            if(key==sk_Window){
                gfx_FillCircleColor(90, 10*service_selected+13, 3, BG_COLOR );
                service_selected-=(service_selected>0);
            }
            if((key==sk_Trace || (!services_loaded)) && (vapor_status==VAPOR_CONNECTED)){
                ntwk_send_nodata(FETCH_SERVER_LIST);
                services_loaded = true;
            }
            if(key==sk_Graph){
                ntwk_send(SRVC_GET_REQ, PS_STR(services_arr[service_selected].name));
            }
        }
        if(menu_item_selected==MENU_SETTINGS){
            if(key==sk_Zoom) setting_selected += (setting_selected<6);
            if(key==sk_Window) setting_selected -= (setting_selected>0);
            if(key==sk_Trace) {
                if(setting_selected==5){
                    if(hashlib_available){
                        uint24_t bitw = settings.rsa_bit_width;
                        settings.rsa_bit_width = (bitw==256) ? 0 : (bitw>0) ? bitw*2 : 64;
                    }
                }
                else {
                    uint8_t editthis = setting_selected;
                    if(setting_selected==6) editthis--;
                    settings.flags[editthis] = (!settings.flags[editthis]);
                    if(editthis==HASH_FILES || editthis==ENABLE_VAPOR_LOGIN){
                        if(!hashlib_available)
                            settings.flags[editthis] = false;
                    }
                }
                
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
    library_move_to_archive();
    ti_CloseAll();
    free(services_arr);
    gfx_End();
    usb_Cleanup();
    pgrm_CleanUp();
    return 0;
}

