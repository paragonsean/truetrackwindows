#include <windows.h>

/***********************************************************************
*                              COPYSTRING                              *
*   Copy source string to destination string. If destination           *
*   string is null, allocate memory for it, and then copy to           *
*   it. If destination string is less than source string,              *
*   free memory first, then allocate memory and copy to it.            *
***********************************************************************/
void CopyString( LPTSTR & dest, LPCTSTR sorc )
{
int slen;
int dlen;

if ( !sorc )
    return;

slen = lstrlen(sorc);

if ( dest )
    {
    dlen = lstrlen(dest);
    if ( dlen < slen )
        {
        delete [] dest;
        dest = 0;
        }
    }

if ( !dest )
    dest = new TCHAR[slen + 1];

if ( dest )
    lstrcpy( dest, sorc );
}

/***********************************************************************
*                              COPYSTRING                              *
***********************************************************************/
void CopyString( LPTSTR & dest, LPCTSTR sorc, int maxlen )
{
int slen;
const TCHAR * cp;

if ( !sorc || maxlen < 1 )
    return;

/*
----------------------------
Get the length of the source
---------------------------- */
cp   = sorc;
slen = 0;
while ( slen < maxlen )
    {
    if ( *cp == TEXT('\0') )
        break;
    cp++;
    slen++;
    }

if ( dest )
    {
    if ( lstrlen(dest) < slen )
        {
        delete[] dest;
        dest = 0;
        }
    }

if ( !dest )
    dest = new TCHAR[slen + 1];

if ( dest )
    {
    lstrcpyn( dest, sorc, slen );
    dest[slen] = TEXT('\0');
    }
}

/***********************************************************************
*                             ADDTWOSTRING                             *
***********************************************************************/
void AddTwoString( LPTSTR & dest, LPCTSTR sorc )
{
int slen;
TCHAR * cp;

if ( !sorc )
    return;

slen = lstrlen( sorc );
if ( dest )
    slen += lstrlen( dest );
cp = new TCHAR[slen + 1];
if ( cp )
    {
    if ( dest )
        {
        lstrcpy( cp, dest );
        lstrcat( cp, sorc );
        delete [] dest;
        }
    else
        {
        lstrcpy( cp, sorc );
        }
    dest = cp;
    }
}

