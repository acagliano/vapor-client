
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

char settings_helper_text[9][155] = {
    "Should VAPOR check for an update to itself automatically upon launch?",
    "Should VAPOR update the toolchain libs automatically upon launch?",
    "Should VAPOR update any 3rd party libs automatically upon launch?",
    "Should VAPOR reload itself after updates?",
    "Should VAPOR verify file download correctness using SHA-1 hashes (this will slow download speed)?",
    "Should VAPOR attempt to log you into 3rd party game (or misc) servers?",
    "Universal username token VAPOR will use to authenticate you (see prior).",
    "Universal password token VAPOR will use to authenticate you (see prior).",
    "Should VAPOR use RSA when communicating with VAPOR network and proxied services? RSA is available in 64, 128, and 256-bit modes. Keys expire in 30 days."
};

sr_t setting_yval[] = {
    {85+10, 25, 8},     // autoupd VAPOR
    {85+10, 35, 8},     // autoupd toolchain libs
    {85+10, 45, 8},     // autoupd 3rd party libs
    {90, 60, 8},        // reload after upd
    {90, 70, 8},        // hashing
    {90, 150, 8},       // enable login
    {85+50, 162, 110},  // user
    {85+50, 174, 140},   // pass
    {85+70, 194, 8}     // enable rsa
};

void ui_ShowSettings(void){
    uint24_t x=85, i;
    uint8_t y=10, rect_color;
    gfx_SetColor(33);
    gfx_FillRectangle(setting_yval[setting_selected].x-5, setting_yval[setting_selected].y-1, 320- setting_yval[setting_selected].x, 11);
    gfx_PrintStringXY("OnLaunch Settings:", x, 10);
    gfx_PrintStringXY("autoupdate VAPOR", x+25, 25);
    gfx_PrintStringXY("autoupdate toolchain libs", x+25, 35);
    gfx_PrintStringXY("autoupdate 3rd party libs", x+25, 45);
    gfx_PrintStringXY("reload VAPOR after updates", x+20, 60);
    gfx_PrintStringXY("verify downloads (sha-1)", x+20, 70);
    gfx_PrintStringXY("Conn. Settings:", x, 90);
    gfx_PrintStringXY("host: vapor.cagstech.com", x+5, 105);
    gfx_PrintStringXY("port: 51000", x+5, 115);
    gfx_PrintStringXY("Gaming Login:", x, 135);
    gfx_PrintStringXY("enable VAPOR login", x+20, 150);
    gfx_PrintStringXY("user:", x+5, 162);
    gfx_PrintStringXY("pass:", x+5, 174);
    gfx_PrintStringXY("Security: ", x, 194);
    gfx_SetColor(223);
    gfx_FillRectangle(x+15, 209, 180, 3);
    gfx_SetColor(255);
    for(i=0; i<9; i++){
        uint24_t x = setting_yval[i].x;
        uint8_t y = setting_yval[i].y;
        if(i<6) {
            uint8_t* sett_ptr = (uint8_t*)&settings;
            bool sett_val = (bool)*(sett_ptr+(i - (i==6)));
            gfx_Rectangle(x, y, 8, 8);
            if(settings.flags[i]) {
                uint8_t color = gfx_SetColor(102);
                gfx_FillRectangle(x+1, y+1, 6, 6);
                gfx_SetColor(color);
            }
        }
        else if(i==8){
            uint8_t seclvl = settings.security_level;
            uint8_t color = gfx_SetColor(238);
            gfx_PrintStringXY(seclvl_txt[seclvl], x, y);
            gfx_FillRectangle(85+14+(seclvl*60), y+13, 4, 7);
            gfx_SetColor(color);
        }
        else
        {
            uint24_t w = setting_yval[i].w;
            gfx_Rectangle(x, y, w, 10);
            gfx_PrintStringXY(settings.login_tokens[i - 7], x+1, y+2);
        }
    }
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
    uint8_t oc = gfx_SetColor(128);
    uint8_t tc = gfx_SetTextFGColor(230);
    uint24_t x = 160-120;
    uint8_t y = 120-50, line_ct;
    gfx_FillRectangle(x, y, 240, 100);
    gfx_SetColor(230);
    gfx_PrintStringXY("HELP", x+10, y+4);
    gfx_FillRectangle(x+2, y+18, 236, 100-20);
    x+=4; y+=20;
    gfx_SetTextFGColor(96);
    line_ct = text_WrappedString(settings_helper_text[item], x, y, 265);
    y+=(5+(10*line_ct));
    gfx_PrintStringXY("Press [Clear] to continue.", x, y);
    gfx_SetColor(oc);
    gfx_SetTextFGColor(tc);
    gfx_BlitRectangle(gfx_buffer, 160-120, 120-50, 240, 100);
    while(!(getKey()==sk_Clear));
}
