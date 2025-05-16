#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

/***********************************************************************
*                            ASCII_DOUBLE                              *
***********************************************************************/
TCHAR * ascii_double( double sorc )
{
static TCHAR s[MAX_DOUBLE_LEN+1];

rounded_double_to_tchar( s, sorc );

return s;
}

/***********************************************************************
*                            ASCII_FLOAT                               *
***********************************************************************/
TCHAR * ascii_float( float sorc )
{
double x;
x = (double) sorc;
return ascii_double( x );
}
