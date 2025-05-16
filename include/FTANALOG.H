#ifndef _FTANALOG_CLASS_
#define _FTANALOG_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

/*
------------
Result types
------------ */
#define SINGLE_POINT_RESULT_TYPE BITSETYPE( 1 )
#define MAX_RESULT_TYPE          BITSETYPE( 2 )
#define MIN_RESULT_TYPE          BITSETYPE( 4 )
#define AVG_RESULT_TYPE          BITSETYPE( 8 )
#define SUB_RESULT_TYPE          BITSETYPE( 16 )

struct FTANALOG_ENTRY
    {
    short        channel;
    BITSETYPE    result_type;
    BITSETYPE    ind_var_type;
    STRING_CLASS ind_value;
    STRING_CLASS end_value;
    FTANALOG_ENTRY() { channel=NO_FT_CHANNEL; ind_var_type=POSITION_VAR; result_type=SINGLE_POINT_RESULT_TYPE; }
    ~FTANALOG_ENTRY(){}
    };

class FTANALOG_CLASS
    {
    public:

    FTANALOG_ENTRY array[MAX_PARMS];
    FTANALOG_CLASS(){}
    ~FTANALOG_CLASS(){}
    void       init( short i );
    BOOLEAN    remove( short i );
    BOOLEAN    load( TCHAR * computer, TCHAR * machine, TCHAR * part );
    BOOLEAN    save( TCHAR * computer, TCHAR * machine, TCHAR * part );
    };

#endif
