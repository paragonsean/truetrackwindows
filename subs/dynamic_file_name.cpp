#include <windows.h>

#include "..\include\visiparm.h"

#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\stringcl.h"
#include "..\include\timeclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static const TCHAR ShiftTag[]       = TEXT( "{s}" );
static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR LeftCurlyBraket  = TEXT( '{' );
static const TCHAR RightCurlyBraket = TEXT( '}' );

/***********************************************************************
*                          DYNAMIC_FILE_NAME                           *
* You must call shifts_startup() before using this if the shift tag    *
* might be used.                                                       *
***********************************************************************/
TCHAR * dynamic_file_name( TIME_CLASS & tsorc, PART_NAME_ENTRY & pn, const TCHAR * sorc )
{
static TCHAR name[MAX_PATH+1];
TCHAR * cp;
TCHAR * dest;
short   shift_number;
TIME_CLASS t;
SYSTEMTIME st;
TCHAR mysorc[MAX_PATH+1];
name[0] = NullChar;

if ( !sorc )
    return name;

copystring( mysorc, sorc );


shift_number = 0;

t = tsorc;

/*
------------------------------------------------------------------------
If this name uses the shift number then I replace the true date with the
shift date.
------------------------------------------------------------------------ */
if ( findstring(ShiftTag, mysorc) )
    {
    get_shift_number_and_date( &shift_number, &st, t.system_time() );
    t.set( st );
    }

cp = mysorc;
dest = name;
while ( replace_char_with_null(cp, LeftCurlyBraket) )
    {
    dest = copystring( dest, cp );
    cp = nextstring( cp );
    if ( replace_char_with_null(cp, RightCurlyBraket) )
        {
        if (strings_are_equal(cp, TEXT("dd"), 2) )
            {
            dest = copystring( dest, t.dd() );
            }
        else if (strings_are_equal(cp, TEXT("mmm"), 3) )
            {
            dest = copystring( dest, t.mmm() );
            }
        else if (strings_are_equal(cp, TEXT("mm"), 2) )
            {
            dest = copystring( dest, t.mm() );
            }
        else if (strings_are_equal(cp, TEXT("hh"), 2) )
            {
            dest = copystring( dest, t.hh() );
            }
        else if (strings_are_equal(cp, TEXT("mi"), 2) )
            {
            dest = copystring( dest, t.mi() );
            }
        else if (strings_are_equal(cp, TEXT("m"), 1) )
            {
            dest = copystring( dest, pn.machine );
            }
        else if (strings_are_equal(cp, TEXT("p"), 1) )
            {
            dest = copystring( dest, pn.part );
            }
        else if (strings_are_equal(cp, TEXT("c"), 1) )
            {
            dest = copystring( dest, pn.computer );
            }
        else if (strings_are_equal(cp, TEXT("yyyy"), 4) )
            {
            dest = copystring( dest, t.yyyy() );
            }
        else if (strings_are_equal(cp, TEXT("yy"), 2) )
            {
            dest = copystring( dest, t.yy() );
            }
        else if (strings_are_equal(cp, TEXT("s"), 1) )
            {
            dest = copystring( dest, int32toasc((int32)shift_number) );
            }
        else
            {
            dest = copystring( dest, TEXT("??") );
            }

        cp = nextstring( cp );
        }

    }

if ( *cp != NullChar )
    copystring( dest, cp );

return name;
}
