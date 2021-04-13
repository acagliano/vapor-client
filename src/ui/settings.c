
#include <fileioc.h>
#include <graphx.h>
#include <string.h>
#include <stdbool.h>
#include "settings.h"
#include "content.h"
#include "../asm/functions.h"

#define SETTINGSV 1   // increment whenever the settings layout changes
char settings_appv[] = "VaporSav";
settings_t settings;
/*
struct {
            version
            bool autoupd_self;
            bool autoupd_canonical;
            bool autoupd_noncanonical;
            bool reload_after_upd;
            bool hash_files;
            bool enable_rsa;
            bool enable_vapor_login;
            char username[STR_WIDTHS];
            char password[STR_WIDTHS];
        };
 */

uint8_t setting_selected = 0;
bool settings_help_up = false;

#define CHECKBOX 0
#define INPUT 1
typedef struct _settingsrender {
    uint24_t x;
    uint8_t y, w;
} sr_t;

char seclvl_txt[4][30] = {
    "Off", "LOW | 64-bit RSA", "MED | 128-bit RSA", "HIGH | 256-bit RSA"
};

char settings_update_vapor[] = "Upon program load (and network up) attempt to update program VAPOR?";
char settings_update_toolchain[] = "Upon program load (and network up) attempt to update toolchain libs?";
char settings_update_3rdparty[] = "Upon program load (and network up) attempt to update 3rd party libs?";
char settings_reload_after_upd[] = "Should VAPOR reload itself after a self-update or a library update?";
char settings_verify_dls[] = "Should VAPOR verify file downloads using SHA-1? This will slow your download. Requires HASHLIB.";
char settings_rsa_enable[] = "Use RSA? 64, 128, and 256 bit mode avail. Changing mode requires key regen. Keys expire after 30 days. Requires HASHLIB.";
char settings_login_token[] = "Generate keypair to auth. with VAPOR-proxied servers? Replaces standard login where supported. Requires HASHLIB.";

char* settings_helper_text[] = {
    settings_update_vapor,
    settings_update_toolchain,
    settings_update_3rdparty,
    settings_reload_after_upd,
    settings_verify_dls,
    settings_rsa_enable,
    settings_login_token
};

sr_t setting_yval[] = {
    {80+45-15, 25, 8},     // autoupd VAPOR
    {80+45-15, 35, 8},     // autoupd toolchain libs
    {80+45-15, 45, 8},     // autoupd 3rd party libs
    {80+20, 60, 8},        // reload after upd
    {80+20, 70, 8},        // hashing
    {80+30-15, 120, 8},     // RSA
    {80+30-15, 145, 8},       // enable login
};

void settings_RenderValueUI(uint8_t opt_id, uint24_t x, uint8_t y){
    bool sett_val = settings.flags[opt_id];
    switch(opt_id){
        case 5:
            {
                uint24_t rsa_depth = settings.rsa_bit_width;
                uint24_t box_x = (rsa_depth==0) ? 85 : (rsa_depth==64) ? 125 : (rsa_depth==128) ? 185 : 245;
                uint24_t box_w = (rsa_depth==0) ? 30 : (rsa_depth==64) ? 55 : (rsa_depth==128) ? 62 : 62;
                uint8_t color = gfx_SetColor(107);
                gfx_FillRectangle(box_x, y+12-1, box_w, 9+2);
                gfx_PrintStringXY("Off", 90, y+12);
                gfx_PrintStringXY("64-bit", 90+40, y+12);
                gfx_PrintStringXY("128-bit", 90+100, y+12);
                gfx_PrintStringXY("256-bit", 90+160, y+12);
                gfx_SetColor(color);
            }
            break;
            
        case 6:
            sett_val = settings.flags[opt_id-1];
        default:
            {
                gfx_Rectangle(x, y, 8, 8);
                if(sett_val) {
                    uint8_t color = gfx_SetColor(102);
                    gfx_FillRectangle(x+1, y+1, 6, 6);
                    gfx_SetColor(color);
                }
            }
            break;
    }

}

void ui_ShowSettings(void){
    uint24_t x=85, i;
    uint8_t y=10, rect_color;
    gfx_SetColor(33);
    gfx_FillRectangle(setting_yval[setting_selected].x-5, setting_yval[setting_selected].y-1, 320- setting_yval[setting_selected].x, 11);
    
    gfx_PrintStringXY("__GENERAL SETTINGS__", x, 5);
    gfx_PrintStringXY("AutoUpdate on ProgStart:", x+15, 15);
    gfx_PrintStringXY("VAPOR", x+45, 25);
    gfx_PrintStringXY("Toolchain Libs", x+45, 35);
    gfx_PrintStringXY("3rd party Libs", x+45, 45);
    
    gfx_PrintStringXY("relaunch after autoupdate", x+30, 60);
    gfx_PrintStringXY("hash downloads (sha-1)", x+30, 70);
    gfx_PrintStringXY("__CONNECTION SETTINGS__", x, 90);
    gfx_PrintStringXY("Host: vapor.cagstech.com:51000", x+5, 105);
    gfx_PrintStringXY("Enable RSA", x+5, 120);
    gfx_PrintStringXY("Enable login key", x+30, 145);
    gfx_PrintStringXY("Help:", x+5, 162);
    gfx_SetColor(255);
    for(i=0; i<7; i++){
        uint24_t x = setting_yval[i].x;
        uint8_t y = setting_yval[i].y;
        settings_RenderValueUI(i, x, y);
    }
    settings_HelpUI(setting_selected);
}

void settings_Load(void){
    ti_var_t sfp = ti_Open(settings_appv, "r");
    settings_t s;
    if(!sfp) {
        memset(&settings, 0, sizeof(settings));
        settings.version=SETTINGSV;
        return;
    }
    ti_Read(&s, sizeof(s), 1, sfp);
    ti_Close(sfp);
    if(s.version != settings.version) return;
    memcpy(&settings, &s, sizeof(settings));
}

void settings_Save(void){
    ti_var_t sfp = ti_Open(settings_appv, "r+");
    if(!sfp) return;
    ti_Write(&settings, sizeof(settings), 1, sfp);
    ti_SetArchiveStatus(true, sfp);
    ti_Close(sfp);
}

void settings_HelpUI(uint8_t item){
    uint8_t y_start=162+10;
    text_WrappedString(settings_helper_text[item], 85, y_start, 310);
    gfx_BlitRectangle(gfx_buffer, 85, y_start, 320-85, 240-y_start);
}
