#ifndef _SHOT_SAVE_
#define _SHOT_SAVE_

const int   MAX_SHOTSAVE_BUFFERS  = 20;
const DWORD SHOTSAVE_WAIT_TIMEOUT = 1005;

struct SHOT_BUFFER_ENTRY
    {
    bool               save_failed;
    bool               has_alarm;
    bool               has_warning;
    PROFILE_NAME_ENTRY pn;
    FTII_FILE_DATA     f;
    DOUBLE_ARRAY_CLASS param;
    };

#ifdef _MAIN_

SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];
HANDLE              ShotSaveEvent    = 0;

#else

extern HANDLE    ShotSaveEvent;
extern SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];

#endif
bool add_to_shotsave( bool new_has_alarm, bool new_has_warning, PART_NAME_ENTRY & partname, FTII_FILE_DATA & fsorc, DOUBLE_ARRAY_CLASS & psorc );

#endif
