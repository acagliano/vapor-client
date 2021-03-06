
#ifndef services_h
#define services_h

#include <stdbool.h>
#include "../network/srv_types.h"

extern bool services_loaded;


void ui_RenderServicesContent(void);
void srvc_show_dl_list(void);
void srvc_request_file(dl_list_t* dl);
void srvc_show_dl_bar(void);


#endif
