#ifndef _GPVAR_CLASS_
#define _GPVAR_CLASS_

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#define MIN_GPVAR_NUMBER 78
#define MAX_GPVAR_NUMBER 89
#define NOF_GPVARS       12

#define GPVAR_POS_TYPE   0
#define GPVAR_VEL_TYPE   1
#define GPVAR_VOLT_TYPE  2
#define GPVAR_PRES_TYPE  3
#define GPVAR_OTHER_TYPE 4

#define GPVAR_NAME_LEN    25
#define GPVAR_NUMBER_LEN   2
#define NO_GPVAR_NAME  TEXT( "NONE" )

class GPVAR_ENTRY
    {
    public:
    TCHAR number[GPVAR_NUMBER_LEN+1];
    TCHAR value[MAX_FLOAT_LEN+1];
    TCHAR sub[MAX_FLOAT_LEN+1];
    TCHAR mul[MAX_FLOAT_LEN+1];
    TCHAR div[MAX_FLOAT_LEN+1];
    TCHAR add[MAX_FLOAT_LEN+1];
    int32 type;
    TCHAR name[GPVAR_NAME_LEN+1];

    GPVAR_ENTRY();
    ~GPVAR_ENTRY();
    void operator=( const GPVAR_ENTRY & sorc );

    TCHAR    * record( void );
    BOOLEAN    read( TCHAR * sorc );
    BOARD_DATA variable_number( void );
    BOARD_DATA variable_value( void );
    };

class GPVAR_CLASS
    {
    private:

    GPVAR_ENTRY varlist[NOF_GPVARS];

    public:

    GPVAR_CLASS();
    ~GPVAR_CLASS();
    GPVAR_ENTRY & operator[]( int32 i );
    void          operator=( const GPVAR_CLASS & sorc );

    BOOLEAN save( TCHAR * computer, TCHAR * machine, TCHAR * part );
    BOOLEAN load( TCHAR * computer, TCHAR * machine, TCHAR * part );
    };

#endif

