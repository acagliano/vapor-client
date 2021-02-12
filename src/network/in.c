#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <debug.h>
#include "network.h"
#include "controlcodes.h"
#include "../ui/content.h"
#include "srv_types.h"
#include "../fileaccess.h"
#include "../asm/functions.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

const char* vapor_temp_file = "VTMP5100";
char err_templ[50] = "invalid checksum error on downloaded file ";
srv_list_t* services_arr=NULL;
uint24_t services_arr_block_size=0;
ti_var_t temp_fp;
bool file_init_error=false;
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
            uint32_t crc=0;
            file_start_t *packet = (void*)data;
            ti_Rewind(temp_fp);
            crc32(ti_GetDataPtr(temp_fp), ti_GetSize(temp_fp), &crc);
            if(crc != packet->crc){
                ti_Close(temp_fp);
                char err_txt[50] = {'\0'};
                strcpy(err_txt, err_templ);
                strcat(err_txt, packet->name);
                ui_ErrorWindow("==SERVER ERROR==", err_txt);
                ti_DeleteVar(vapor_temp_file, packet->type);
                break;
            }
            ti_SetArchiveStatus(packet->archive, temp_fp);
            ti_Close(temp_fp);
            ti_DeleteVar(packet->name, packet->type);
            ti_RenameVar(vapor_temp_file, packet->name, packet->type);
            library_update_entry(packet);
            break;
            }
        case SRVC_REQ_INFO:
            {
                srv_deps_t *packet = (void*)data;
                uint24_t ct=data_size/sizeof(srv_deps_t), i;
                dep_info_t info[16] = {0};
                gfx_FillRectangleColor(76, 0, 320-77, 240, BG_COLOR);
                for(i=0; i<ct; i++){
                    strncpy(info[i].name, packet[i].name, 8);
                    info[i].type = packet[i].type;
                    info[i].crc = library_get_crc(packet[i].name, packet[i].type);
                }
                ntwk_send(SRVC_GET_DLS, PS_PTR(info, ct * sizeof(dep_info_t)));
                break;
            }
        case SERVER_ERROR:
        {
            ui_ErrorWindow("==SERVER ERROR==", data);
            break;
        }
    }
}


