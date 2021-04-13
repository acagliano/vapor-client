
#ifndef settings_h
#define settings_h

#include <stdbool.h>


// This structure is not for VAPOR specifically, but for potentially allowing VAPOR
// to handle logging into "game" accounts

enum _settingsFlags {
    UPD_SELF,
    UPD_CANON,
    UPD_NONCANON,
    RELOAD_AFT_UPD,
    HASH_FILES,
    ENABLE_VAPOR_LOGIN
};
enum _settingsTokens {
    USERNAME,
    PASSWORD
};
enum _securityLevel {
    OFF,
    LOW,
    MED,
    HIGH
};

#define STR_WIDTHS 25
typedef struct _settings {
    uint8_t version;
    union {
        struct {
            bool flags[6];
            uint24_t rsa_bit_width;
            bool enable_login;
        };
        struct {
            bool autoupd_self;
            bool autoupd_canonical;
            bool autoupd_noncanonical;
            bool reload_after_upd;
            bool hash_files;
            bool enable_vapor_token;
            uint24_t rsa_depth;
        };
    };
} settings_t;


void ui_ShowSettings(void);
void settings_HelpUI(uint8_t item);

extern settings_t settings;
extern uint8_t setting_selected;
extern bool settings_help_up;

#endif
