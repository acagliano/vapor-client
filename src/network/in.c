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
srv_list_t* services_arr=NULL;
uint24_t services_arr_block_size=0;
ti_var_t temp_fp = 0;
bool file_init_error=false;
bool file_install_error=false;
file_start_t file_data;
uint8_t curr_dl = 0;
uint8_t curr_total = 0;
dl_list_t *dl_list = NULL;
size_t dl_size = 0;
size_t bytes_copied = 0;

void conn_HandleInput(packet_t *in_buff, size_t buff_size) {
    size_t data_size = buff_size-1;
    uint8_t ctl = in_buff->control;
    uint8_t response = in_buff->data[0];    // for handlers needing only response codes
    uint8_t* data = &in_buff->data[0];      // for handlers needing arbitrary data
    switch(ctl){
        case CONNECT:
            break;
        case DISCONNECT:
            vapor_status=false;
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
            struct {
                uint24_t size;
            } *packet = (void*)data;
            temp_fp=ti_OpenVar(vapor_temp_file, "w", dl_list[curr_dl].type);
            if(!temp_fp) {
                dl_list[curr_dl].status = DL_IO_ERR;
                break;
            }
            dl_list[curr_dl].size=packet->size;
            bytes_copied = 0;
            srvc_show_dl_bar();
            break;
        }
        case FILE_WRITE_DATA:
            if(!temp_fp) break;
            ti_Write(data, data_size, 1, temp_fp);
            bytes_copied += data_size;
            srvc_show_dl_bar();
            break;
        case FILE_WRITE_END:
            {
            uint32_t crc=0;
            size_t bytes_written = ti_GetSize(temp_fp);
            file_start_t *packet = (void*)data;
            srvc_show_dl_bar();
            ti_Rewind(temp_fp);
            dl_list[curr_dl].status = DL_VERIFY;
            srvc_show_dl_list();
            crc32(ti_GetDataPtr(temp_fp), bytes_written, &crc);
            if((crc != packet->crc) || (bytes_written != dl_list[curr_dl].size)){
                ti_Close(temp_fp);
                ti_DeleteVar(vapor_temp_file, packet->type);
                dl_list[curr_dl].status = DL_CRC_ERR;
                break;
            }
            ti_SetArchiveStatus(packet->archive, temp_fp);
            ti_Close(temp_fp);
            ti_DeleteVar(packet->name, packet->type);
            ti_RenameVar(vapor_temp_file, packet->name, packet->type);
            library_update_entry(packet);
            dl_list[curr_dl].status = DL_DONE;
            }
        case FILE_WRITE_SKIP:
            if(dl_list[curr_dl].status != DL_DONE)
                dl_list[curr_dl].status = DL_SKIP;
            srvc_show_dl_list();
            curr_dl++;
            if(curr_dl < curr_total)
                srvc_request_file(&dl_list[curr_dl]);
            else free(dl_list);
            break;
        
        case SRVC_REQ_LIST:
            {
                dl_list_t *packet = (void*)data;
                dl_list = malloc(data_size);
                memcpy(dl_list, packet, data_size);
                curr_total = data_size/sizeof(dl_list_t);
                curr_dl = 0;
                srvc_request_file(&dl_list[curr_dl]);
                srvc_show_dl_list();
                break;
            }
        case SERVER_ERROR:
            ui_ErrorWindow("==SERVER ERROR==", data);
            break;
        case SERVER_SUCCESS:
            ui_SuccessWindow("==SERVER ACTION==", data)
            break;
    }
}


