#include <windows.h>

/********************************************************************
*                              REVERSE                              *
********************************************************************/
void reverse( LPTSTR s )
{

TCHAR  c;
LPTSTR cp1;
LPTSTR cp2;

cp1 = s;
cp2 = s + lstrlen(s) - 1;
while ( cp1 < cp2 )
    {
    c = *cp2;
    *cp2-- = *cp1;
    *cp1++ = c;
    }
}

