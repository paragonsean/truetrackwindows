#include <windows.h>

#include "..\include\visiparm.h"

static TCHAR YChar = TEXT( 'Y' );
static TCHAR LowYChar = TEXT( 'y' );
static TCHAR s[] = UNKNOWN;
const static TCHAR NullChar = TEXT( '\0' );
static TCHAR YString[] = TEXT( "Y" );
static TCHAR NString[] = TEXT( "N" );

/***********************************************************************
*                           ASCII_TO_BOOLEAN                           *
***********************************************************************/
BOOLEAN ascii_to_boolean( TCHAR * cp )
{
if ( *cp == YChar || *cp == LowYChar )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           BOOLEAN_TO_ASCII                           *
***********************************************************************/
TCHAR * boolean_to_ascii( BOOLEAN b )
{
if ( b )
    return YString;

return NString;
}

/***********************************************************************
*                                 UNKNOWN                              *
***********************************************************************/
BOOLEAN unknown( TCHAR * cp )
{
if ( lstrcmpi(s, cp) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             UNKNOWN_STRING                           *
***********************************************************************/
TCHAR * unknown_string()
{
return s;
}

/***********************************************************************
*                                ROUND                                 *
***********************************************************************/
double round( double sorc, double smallest_increment )
{
const double ZERO    = 0.0;
const double POINT_5 = 0.5;
double x;

x  = sorc / smallest_increment;
if ( x >= ZERO )
    x += POINT_5;
else
    x -= POINT_5;

x = (double) ( (int) x );

return x * smallest_increment;
}

/***********************************************************************
*                            MASK_FROM_WIRE                            *
* Convert a wire number to a bit mask, assuming wire 1 is bit 0, etc   *
***********************************************************************/
unsigned mask_from_wire( unsigned wire )
{
unsigned mask;

mask = 1;

if ( wire > 1 )
    {
    wire--;
    mask <<= wire;
    }

return mask;
}

/***********************************************************************
*                            WIRE_FROM_MASK                            *
* Convert a bit mask to a wire number, assuming wire 1 is bit 0, etc   *
* Wire 0 is returned if the mask is 0                                  *
***********************************************************************/
unsigned wire_from_mask( unsigned mask )
{
const unsigned BIT_0_MASK = 1;
unsigned wire;

wire = 0;
while ( mask )
    {
    wire++;
    if ( mask & BIT_0_MASK )
        break;
    mask >>= 1;
    }

return wire;
}
