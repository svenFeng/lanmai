#ifndef LANMAI_ERR_H
#define LANMAI_ERR_H

enum ErrCode : int {
    // IO
    OPEN_FILE_ERROR = 0x0,
    // Config
    PARSE_JSON_ERROR = 0x100,
};

#endif
