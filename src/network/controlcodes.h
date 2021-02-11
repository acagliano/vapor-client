#ifndef controlcodes_h
#define controlcodes_h

enum ControlCodes {
    
    CONNECT,
    DISCONNECT,
    
    FETCH_SOFTWARE_LIST = 0x10,
    FETCH_SERVER_LIST,
    FETCH_SOFTWARE_INFO,
    
    SRVC_GET_REQ = 0x20,
    SRVC_REQ_INFO,
    SRVC_GET_DLS,
    
    FILE_WRITE_START = 0x40,
    FILE_WRITE_DATA,
    FILE_WRITE_END,
    FILE_CHECK_UP_TO_DATE,
    
    LIBRARY_CHECK_CRC = 0x50,
    LIBRARY_UPDATE_ITEM,
    
    // DEBUG: 0xf*
    WELCOME = 0xd0,
    MESSAGE = 0xf0,
    BRIDGE_ERROR = 0xf1,
    SERVER_ERROR = 0xf2,
    PING = 0xfc,
};

//returned from some routines
enum _player_status {
    IN_LOBBY,
    WAITING,
    IN_GAME
};

enum _error_codes {
    SUCCESS,
    NO_GAME_FOUND,
    GAME_IN_PROGRESS,
    PLAYER_LIMIT
};


#endif
