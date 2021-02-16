#include <string.h>
#include <stdbool.h>
#include "../fileaccess.h"
#include "content.h"
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
        gfx_PrintStringXY(libinfo.name, 80, i*10+y);
        if(libinfo.type==TI_APPVAR_TYPE) gfx_PrintStringXY("appv", 150, i*10+y);
        else gfx_PrintStringXY("prgm", 150, i*10+y);
        gfx_SetTextXY(225, i*10+y);
        gfx_PrintUInt(libinfo.date.year, 4);
        gfx_PrintString("-");
        gfx_PrintUInt(libinfo.date.month, 2);
        gfx_PrintString("-");
        gfx_PrintUInt(libinfo.date.day, 2);
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
        ti_var_t tf=ti_OpenVar("VAPOR", "r", TI_PPRGM_TYPE);
        ti_Close(tf);
        library_t libinfo = {"VAPOR", TI_PPRGM_TYPE, {0}};
        lf=ti_Open(library_var, "w+");
        ti_Write(&libinfo, sizeof(library_t), 1, lf);
        ti_Close(lf);
    }
}

void library_load_date(dl_list_t* dl){
    ti_var_t tf = ti_OpenVar(dl->name, "r", dl->type);
    library_t* lib;
    if(!tf) return;
    ti_Close(tf);
    lib = library_get_entry(dl->name, dl->type);
    if(lib) memcpy(&dl->date, &lib->date, sizeof(date_t));
}
