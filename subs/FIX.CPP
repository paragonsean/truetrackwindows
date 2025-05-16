#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR ZeroChar  = TEXT( '0' );
static TCHAR CrChar    = TEXT( '\r' );
static TCHAR LfChar    = TEXT( '\n' );
static TCHAR NullChar  = TEXT( '\0' );

/***********************************************************************
*                             FIX_SHOTNAME                             *
***********************************************************************/
void fix_shotname( TCHAR * s )
{
int i;

rjust( s, SHOT_LEN, '0' );

for ( i=0; i<SHOT_LEN; i++ )
    if ( *s == SpaceChar )
        *s = ZeroChar;
}

/***********************************************************************
*                             FIX_DDE_NAME                             *
*                                                                      *
*  Dde names are terminated with a cr/lf. If the name is shorter than  *
*  the max name len then the cr/lf may be included as part of the      *
*  name. This strips them, ending the string at the first cr or lf.    *
***********************************************************************/
void fix_dde_name( TCHAR * s )
{

/*
--------------------------------------------------------
A carriage returns or line feed is the end of the string
-------------------------------------------------------- */
while ( *s != NullChar )
    {
    if ( *s == CrChar || *s == LfChar )
        {
        *s = NullChar;
        break;
        }
    s++;
    }
}

