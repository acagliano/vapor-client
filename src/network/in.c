#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "network.h"
#include "controlcodes.h"
#include "../ui/content.h"
#include "srv_types.h"
#include "../fileaccess.h"
#include "../asm/functions.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

const char* vapor_temp_file = "VTMP5100";
srv_list_t* services_arr=NULL;
uint24_t services_arr_block_size=0;
ti_var_t temp_fp;
bool file_init_error=false;
bool file_checksum_invalid=false;
file_start_t file_data;

void conn_HandleInput(packet_t *in_buff, size_t buff_size) {
    size_t data_size = buff_size-1;
    uint8_t ctl = in_buff->control;
    uint8_t response = in_buff->data[0];    // for handlers needing only response codes
    uint8_t* data = &in_buff->data[0];      // for handlers needing arbitrary data
    switch(ctl){
        case CONNECT:
        case DISCONNECT:
            break;
            
        case WELCOME:
            vapor_status=VAPOR_CONNECTED;
            queue_update=true;
            break;
    
        case FETCH_SERVER_LIST:
            if( data_size > services_arr_block_size ){
                services_arr_block_size = data_size;
                services_arr = realloc(services_arr, services_arr_block_size);
            }
            service_count = data_size / sizeof(srv_list_t);
            memcpy(services_arr, data, data_size);
            queue_update=true;
            break;
        case FILE_WRITE_START:
        {
            temp_fp=ti_OpenVar(vapor_temp_file, "w", response);
            if(!temp_fp) file_init_error = true;
            break;
        }
        case FILE_WRITE_DATA:
            if(!temp_fp) break;
            ti_Write(data, data_size, 1, temp_fp);
            break;
        case FILE_WRITE_END:
            {
            file_start_t *packet = (void*)data;
            if(!temp_fp) break;
            if(rc_crc32(0, ti_GetDataPtr(temp_fp), ti_GetSize(temp_fp)) != packet->crc){
                ti_Close(temp_fp);
                ti_DeleteVar(vapor_temp_file, packet->type);
                file_checksum_invalid=true;
                break;
            }
            ti_SetArchiveStatus(packet->archive, temp_fp);
            ti_Close(temp_fp);
            ti_Rename(vapor_temp_file, packet->name);
            library_update_entry(packet);
            break;
            }
        case SRVC_REQ_INFO:
            {
                srv_deps_t *packet = (void*)data;
                uint24_t ct=data_size/sizeof(srv_deps_t), i;
                dep_info_t *info = malloc(ct * sizeof(dep_info_t));
                gfx_FillRectangleColor(76, 0, 320-77, 240, BG_COLOR);
                for(i=0; i<ct; i++){
                    strncpy(info[i].name, packet[i].name, 8);
                    info[i].type = packet[i].type;
                    info[i].crc = library_get_crc(packet[i].name, packet[i].type);
                }
                ntwk_send(SRVC_GET_DLS, PS_PTR(info, ct));
                break;
            }
        case SERVER_ERROR:
        {
            uint8_t color = gfx_SetTextFGColor(231);
            gfx_FillRectangleColor(127, 80, 140, 80, 224);
            gfx_FillRectangleColor(129, 92, 136, 66, 231);
            
            gfx_PrintStringCentered("==SERVER ERROR==", 127, 83, 140);
            gfx_SetTextFGColor(192);
            text_WrappedString(data, 133, 96, 260);
            gfx_PrintStringXY("Any key to dismiss", 131, 148);
            gfx_BlitRectangle(gfx_buffer, 127, 80, 140, 80);
            while(!getKey());
            queue_update=true;
            gfx_SetTextFGColor(color);
            break;
        }
    }
}


