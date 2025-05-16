#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\stringcl.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

static TCHAR CommaChar = TEXT( ',' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );

/***********************************************************************
*                             write_olmsted_file                       *
***********************************************************************/
BOOLEAN write_olmsted_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile )
{
FTII_FILE_DATA  ft;
FTII_ARRAY_DATA fa;

if ( ft.get(sorcfile) )
    {
    fa = ft;
    fa.write_csv( destfile.text() );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          WRITE_COUNTS_FILE                           *
***********************************************************************/
BOOLEAN write_counts_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile )
{
FTII_FILE_DATA  ft;

if ( ft.get(sorcfile) )
    {
    ft.write_csv( destfile );
    return TRUE;
    }

return FALSE;
}

