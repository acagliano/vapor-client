#include <string.h>
#include <stdbool.h>
#include "../fileaccess.h"
#include "content.h"

const char library_var[]="VPRLibr";
const char no_lib_err[]="There are no files in your VAPOR library. You can use the Services tab to the left to search for active servers and their corresponding client software.";

void ui_ShowLibrary(bool show_upd){
    ti_var_t libfile=ti_Open(library_var, "r");
    uint8_t color=gfx_SetTextFGColor(TEXT);
    library_t libinfo;
    uint24_t i=0;
    uint8_t y=10;
    char cs_string[11] = {'\0'};
    if(!libfile){
        text_WrappedString(no_lib_err, 90, 15, 310);
        gfx_BlitRectangle(gfx_buffer, 76, 0, 320-77, 240);
        gfx_SetTextFGColor(color);
        gfx_SetTextFGColor(color);
        return;
    }
    for(; ti_Read(&libinfo, sizeof(library_t), 1, libfile); i++){
        gfx_PrintStringXY(libinfo.name, 85, i*10+y);
        gfx_PrintStringXY("CRC: ", 150, i*10+y);
        sprintf(cs_string, "%xh", libinfo.crc);
        gfx_PrintString(cs_string);
    }
    ti_Close(libfile);
    gfx_SetTextFGColor(color);
    gfx_BlitRectangle(gfx_buffer, 76, 0, 320-77, 240);
}

library_t library_get_entry(const char* string, uint8_t type){
    ti_var_t libf = ti_Open(library_var, "r");
    library_t lib = {0};
    bool found = false;
    while(ti_Read(&lib, sizeof(library_t), 1, libf)){
        if((!strncmp(lib.name, string, 8)) && (lib.type == type)){
            found = true;
            break;
        }
    }
    ti_Close(libf);
    return lib;
}

bool library_set_entry(library_t* libr){
    ti_var_t libf = ti_Open(library_var, "r+");
    library_t lib;
    if(!libf) return false;
    while(ti_Read(&lib, sizeof(library_t), 1, libf)){
        if((!strncmp(lib.name, libr->name, 8)) && (lib.type == libr->type)){
            ti_Seek(-1*sizeof(library_t), SEEK_CUR, libf);
            break;
        }
    }
    ti_Write(libr, sizeof(library_t), 1, libf);
    return true;
}

void lib_Init(void){
    ti_var_t lf=ti_Open(library_var, "r+");
    if(lf && ((ti_GetSize(lf) % sizeof(library_t)) != 0)){ ti_Close(lf); return; }
    else {
        ti_var_t tf=ti_OpenVar("VAPOR", "r", TI_PPRGM_TYPE);
        library_t libinfo = {"VAPOR", TI_PPRGM_TYPE, rc_crc32(0, ti_GetDataPtr(tf), ti_GetSize(tf))};
        ti_Close(tf);
        lf=ti_Open(library_var, "w+");
        ti_Write(&libinfo, sizeof(library_t), 1, lf);
        ti_SetArchiveStatus(true, lf);
        ti_Close(lf);
    }
}

void library_update_entry(file_start_t* lib){
    library_t new_lib;
    strncpy(new_lib.name, file_data.name, 8);
    new_lib.type = file_data.type;
    new_lib.crc = file_data.crc;
    library_set_entry(&new_lib);
}

uint24_t library_get_crc(const char* string, uint8_t type){
    library_t entry = library_get_entry(string, type);
    return entry.crc;
}
