#include <windows.h>
static const double MinPosDouble = 1.0e-6;
static const double MinNegDouble = -1.0e-6;

/***********************************************************************
*                            IS_FLOAT_ZERO                             *
***********************************************************************/
BOOLEAN is_float_zero( float x )
{

if ( x < .0001 && x > -.0001 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           NOT_FLOAT_ZERO                             *
***********************************************************************/
BOOLEAN not_float_zero( float x )
{

if ( x < .0001 && x > -.0001 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                               IS_ZERO                                *
***********************************************************************/
bool is_zero( double x )
{
if ( x < MinPosDouble && x > MinNegDouble )
    return true;

return false;
}

/***********************************************************************
*                               NOT_ZERO                               *
***********************************************************************/
bool not_zero( double x )
{
if ( x > MinPosDouble )
    return true;

if ( x < MinNegDouble )
    return true;

return false;
}

/***********************************************************************
*                           DOUBLE_ARE_EQUAL                           *
***********************************************************************/
bool double_are_equal( double x1, double x2 )
{
return is_zero(x2 - x1);
}

