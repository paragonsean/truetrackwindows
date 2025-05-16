#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\evclass.h"
#include "..\include\subs.h"

const static int32 NewMachStatMsgLen = MACHINE_NAME_LEN + (2*BOARD_DATA_HEX_LEN) + 4;
const static int32 DownDataMsgLen    = MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN + 1) + OPERATOR_NUMBER_LEN;

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
BOOLEAN NEW_MACHINE_STATE_CLASS::set( HDDEDATA edata )
{
DWORD   bytes_copied;
int32   slen;
TCHAR   buf[NewMachStatMsgLen+1];
TCHAR * cp;

if ( !edata )
    return FALSE;

if ( edata == (HDDEDATA) TRUE )
    return FALSE;

slen = NewMachStatMsgLen + 1;
slen *= sizeof( TCHAR );

bytes_copied = DdeGetData( edata, (LPBYTE) buf, slen, 0 );
if ( bytes_copied > 0 )
    {
    fix_dde_name( buf );
    if ( replace_tab_with_null(buf) )
        {
        lstrcpy( name, buf );
        cp = nextstring( buf );
        old_status_value = (BOARD_DATA) extlong( cp, BOARD_DATA_HEX_LEN, HEX_RADIX );
        cp += BOARD_DATA_HEX_LEN + 1;
        new_status_value = (BOARD_DATA) extlong( cp, BOARD_DATA_HEX_LEN, HEX_RADIX );
        return TRUE;
        }

    }

return FALSE;
}

/***********************************************************************
*                             DOWN_DATA_CLASS                          *
***********************************************************************/
DOWN_DATA_CLASS::DOWN_DATA_CLASS()
{
lstrcpy( machine,    NO_MACHINE               );
lstrcpy( part,       NO_PART                  );
lstrcpy( cat,        DOWNCAT_SYSTEM_CAT       );
lstrcpy( subcat,     DOWNCAT_UNSP_DOWN_SUBCAT );
lstrcpy( catname,    NO_DOWNCAT_NAME          );
lstrcpy( subcatname, NO_DOWNCAT_NAME          );
lstrcpy( opnumber,   NO_OPERATOR_NUMBER       );
init_systemtime_struct( st );
}
 
/***********************************************************************
*                             DOWN_DATA_CLASS                          *
*                                   =                                  *
***********************************************************************/
void DOWN_DATA_CLASS::operator=(const DOWN_DATA_CLASS & sorc )
{
lstrcpy( machine,    sorc.machine    );
lstrcpy( part,       sorc.part       );
lstrcpy( cat,        sorc.cat        );
lstrcpy( subcat,     sorc.subcat     );
lstrcpy( catname,    sorc.catname    );
lstrcpy( subcatname, sorc.subcatname );
lstrcpy( opnumber,   sorc.opnumber   );
st = sorc.st;
}
 
/***********************************************************************
*                                 SET                                  *
***********************************************************************/
BOOLEAN DOWN_DATA_CLASS::set( HDDEDATA edata )
{
DWORD      bytes_copied;
int32      slen;
TCHAR      buf[DownDataMsgLen+1];
TCHAR    * cp;
SYSTEMTIME st;

if ( !edata )
    return FALSE;

if ( edata == (HDDEDATA) TRUE )
    return FALSE;

slen = DownDataMsgLen + 1;
slen *= sizeof( TCHAR );

bytes_copied = DdeGetData( edata, (LPBYTE) buf, slen, 0 );
if ( bytes_copied > 0 )
    {
    fix_dde_name( buf );
    cp = buf;

    /*
    ------------
    Machine name
    ------------ */
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( machine, cp );

    /*
    ---------
    Part name
    --------- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( part, cp );

    /*
    ----
    Date
    ---- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    extmdy( st, cp );

    /*
    ----
    Time
    ---- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    exthms( st, cp );

    /*
    ---
    Cat
    --- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( cat, cp );

    /*
    ------
    Subcat
    ------ */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( subcat, cp );

    /*
    -------------
    Category name
    ------------- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( catname, cp );

    /*
    ----------------
    Subcategory name
    ---------------- */
    cp = nextstring( cp );
    if ( !replace_tab_with_null(cp) )
        return FALSE;
    lstrcpy( subcatname, cp );

    /*
    ---------------
    Operator number
    --------------- */
    cp = nextstring( cp );
    lstrcpy( opnumber, cp );

    return TRUE;
    }

return FALSE;
}


