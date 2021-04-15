#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <fileioc.h>
#include <usbdrvce.h>
//#include <hashlib.h>
#include "network.h"
#include "controlcodes.h"
#include "../ui/content.h"
#include "../ui/library.h"
#include "../ui/services.h"
#include "../ui/settings.h"
#include "srv_types.h"
#include "../asm/functions.h"
#include "../asm/libload_custom.h"
#undef NDEBUG
#include <debug.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

const char* vapor_temp_file = "VTMP5100";
srv_list_t* services_arr=NULL;
uint24_t services_arr_block_size=0;
ti_var_t temp_fp = 0;
uint8_t curr_dl = 0;
uint8_t curr_total = 0;
dl_list_t *dl_list = NULL;
size_t dl_size = 0;
size_t bytes_copied = 0;
sha1_ctx ctx;

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
            if(settings.flags[UPD_SELF]){
                dl_list=malloc(sizeof(dl_list_t));
                strncpy(dl_list->name, "VAPOR", 8);
                dl_list->type=TI_PPRGM_TYPE;
                curr_total = 1;
                curr_dl = 0;
                srvc_request_file(&dl_list[curr_dl]);
            }
            break;
    
        case FETCH_SERVER_LIST:
            if( data_size > services_arr_block_size ){
                services_arr_block_size = data_size;
                services_arr = realloc(services_arr, services_arr_block_size);
            }
            services_loaded = true;
            service_count = data_size / sizeof(srv_list_t);
            memcpy(services_arr, data, data_size);
            queue_update=true;
            break;
        case FILE_WRITE_START:
        {
            struct {
                uint24_t size;
                uint8_t type;
            } *packet = (void*)data;
            temp_fp=ti_OpenVar(vapor_temp_file, "w", packet->type);
            if(!temp_fp) {
                dl_list[curr_dl].status = DL_IO_ERR;
                break;
            }
            if(settings.flags[HASH_FILES]) hashlib_Sha1Init(&ctx);
            dl_list[curr_dl].size=packet->size;
            bytes_copied = 0;
            if(strncmp(dl_list[curr_dl].name, "VAPOR", 8)){
                srvc_show_dl_list();
                srvc_show_dl_bar();
            }
            break;
        }
        case FILE_WRITE_DATA:
            if(!temp_fp) break;
            {
            uint8_t h = 10 * curr_total + 20;
            dl_list[curr_dl].status = DL_DOWNLOADING;
            ti_Write(data, data_size, 1, temp_fp);
            bytes_copied += data_size;
            if(settings.flags[HASH_FILES]) hashlib_Sha1Update(&ctx, data, data_size);
            srvc_show_dl_list();
            srvc_show_dl_bar();
            gfx_BlitRectangle(gfx_buffer, 60, 120 - (h / 2) - 12, 200, h);
            ntwk_send_nodata(FILE_WRITE_NEXT);
            break;
            }
        case FILE_WRITE_END:
        {
            file_metadata_t *packet = (void*)data;
            if(temp_fp){
                bool file_error = false;
                uint8_t sha1[20];
                srvc_show_dl_bar();
                ti_Rewind(temp_fp);
                
                if(settings.flags[HASH_FILES]) {
                    dl_list[curr_dl].status = DL_VERIFY;
                    srvc_show_dl_list();
                    hashlib_Sha1Final(&ctx, sha1);
                    if(memcmp(sha1, packet->sha1, 20)){
                        file_error = true;
                    }
                }
                
                if(ti_GetSize(temp_fp) != dl_list[curr_dl].size)
                    file_error = true;
                
                if(!file_error){
                    library_t addme;
                    memcpy(&addme, packet, sizeof(library_t));
                    library_set_entry(&addme);
                    ti_DeleteVar(packet->name, packet->type);
                    ti_SetArchiveStatus(true, temp_fp);
                    ti_Close(temp_fp);
                    ti_RenameVar(vapor_temp_file, packet->name, packet->type);
                    dl_list[curr_dl].status = DL_DONE;
                    if((!strncmp(dl_list[curr_dl].name, "VAPOR", 8)) && (dl_list[curr_dl].type==TI_PPRGM_TYPE)) {
                        ti_CloseAll();
                        usb_Cleanup();
                        update_program();
                    }
                }
                else {
                    ti_Close(temp_fp);
                    ti_DeleteVar(vapor_temp_file, packet->type);
                    dl_list[curr_dl].status = DL_VERIF_ERR;
                }
            }
        }
        case FILE_WRITE_SKIP:
            if(dl_list[curr_dl].status != DL_DONE)
                dl_list[curr_dl].status = response;
            if(strncmp(dl_list[curr_dl].name, "VAPOR", 8))
                srvc_show_dl_list();
            curr_dl++;
            if(curr_dl < curr_total)
                srvc_request_file(&dl_list[curr_dl]);
            else {
                free(dl_list);
                queue_update=true;
            }
            break;
        
        case SRVC_GET_REQ:
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
            
        case SRVC_RUN_FILE:
            {
                struct {
                    uint8_t name[9];
                    uint8_t type;
                } *packet = (void*)data;
                run_program(packet->name, packet->type);
                break;
            }
    }
}


