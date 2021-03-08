#include <string.h>
#include <stdbool.h>
#include <fileioc.h>
#include <hashlib.h>
#include "content.h"
#include "library.h"
#include "../network/srv_types.h"

const char library_var[]="VPRLibr";
const char no_lib_err[]="There are no files in your VAPOR library. You can use the Services tab to the left to search for active servers and their corresponding client software.";

void ui_ShowLibrary(bool show_upd){
    ti_var_t libfile=ti_Open(library_var, "r");
    uint8_t color=gfx_SetTextFGColor(TEXT);
    library_t libinfo;
    uint24_t i=0;
    uint8_t y=10;
    if(!libfile){
        text_WrappedString(no_lib_err, 90, 15, 310);
        gfx_BlitRectangle(gfx_buffer, 76, 0, 320-77, 240);
        gfx_SetTextFGColor(color);
        gfx_SetTextFGColor(color);
        return;
    }
    for(i=0; ti_Read(&libinfo, sizeof(library_t), 1, libfile); i++){
        uint8_t prior_color = gfx_SetTextFGColor(0);
        gfx_FillRectangleColor(80, i*10+y-1, 50, 9, 239);
        if(libinfo.type==TI_APPVAR_TYPE) gfx_PrintStringXY("appv", 87, i*10+y-1);
        else if(libinfo.type==TI_PRGM_TYPE) gfx_PrintStringXY("prgm", 87, i*10+y-1);
        else if(libinfo.type==TI_PPRGM_TYPE) gfx_PrintStringXY("pprgm", 84, i*10+y-1);
        gfx_SetTextFGColor(prior_color);
        gfx_PrintStringXY(libinfo.name, 140, i*10+y);
        // display SHA-1 somehow?
    }
    ti_Close(libfile);
    gfx_SetTextFGColor(color);
    gfx_BlitRectangle(gfx_buffer, 76, 0, 320-77, 240);
}

library_t* library_get_entry(const char* string, uint8_t type){
    ti_var_t libf = ti_Open(library_var, "r");
    library_t lib = {0}, *ptr=NULL;
    if(!libf) return NULL;
    while(ti_Read(&lib, sizeof(library_t), 1, libf)){
        if((!strncmp(lib.name, string, 8)) && (lib.type == type)){
            ptr = (ti_GetDataPtr(libf)-sizeof(library_t));
            break;
        }
    }
    ti_Close(libf);
    return ptr;
}

bool library_set_entry(library_t* libr){
    ti_var_t libf = ti_Open(library_var, "r+");
    library_t lib = {0};
    if(!libf) return false;
    while(ti_Read(&lib, sizeof(library_t), 1, libf)){
        if((!strncmp(lib.name, libr->name, 8)) && (lib.type == libr->type)){
            ti_Seek(-1*sizeof(library_t), SEEK_CUR, libf);
            break;
        }
    }
    ti_Write(libr, sizeof(library_t), 1, libf);
    ti_Close(libf);
    return true;
}

void lib_Init(void){
    ti_var_t lf=ti_Open(library_var, "r+");
    if(lf){ ti_Close(lf); return; }
    else {
        library_t libinfo = {"VAPOR", TI_PPRGM_TYPE, {0}};
        sha1_ctx context;
        ti_var_t tf=ti_OpenVar("VAPOR", "r", TI_PPRGM_TYPE);
        hashlib_SHA1(&context, ti_GetDataPtr(tf), ti_GetSize(tf), libinfo.sha1);
        ti_Close(tf);
        lf=ti_Open(library_var, "w+");
        ti_Write(&libinfo, sizeof(library_t), 1, lf);
        ti_Close(lf);
    }
}


void library_load_sha1(uint8_t* sha1_out, const char* name, uint8_t type){
    library_t *entry = library_get_entry(name, type);
    if(entry==NULL) return;
    memcpy(sha1_out, entry->sha1, 20);
}

void library_move_to_archive(void) {
    ti_var_t f = ti_Open(library_var, "r");
    if(f){
        ti_SetArchiveStatus(true, f);
        ti_Close(f);
    }
}
