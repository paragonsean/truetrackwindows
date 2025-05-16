#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"

static const TCHAR NullChar        = TEXT( '\0' );
static const TCHAR BackSlashChar   = TEXT( '\\' );
static const TCHAR ColonChar       = TEXT( ':' );
static TCHAR DefaultDirectory[]    = TEXT( "C:\\" );
static STRING_CLASS ExeDirectory;

/***********************************************************************
*                     GET_STATIC_EXE_DIRECTORY                         *
*     This will put a backslash at the end of the directory path.      *
***********************************************************************/
static void get_static_exe_directory( void )
{
TCHAR s[MAX_PATH+1];
TCHAR * cp;
int slen;

if ( GetModuleFileName(NULL, s, MAX_PATH+1) > 0 )
    {
    slen = lstrlen(s);
    if ( slen > 2 )
        {
        cp = s + slen;
        cp--;
        while ( TRUE )
            {
            if ( *cp == BackSlashChar || *cp == ColonChar )
                {
                if ( *cp == ColonChar )
                    {
                    cp++;
                    *(cp) = BackSlashChar;
                    slen++;
                    }
                cp++;
                *cp = NullChar;
                ExeDirectory = s;
                break;
                }
            slen--;
            if ( slen <= 0 )
                break;
            cp--;
            }
        }
    }

if ( ExeDirectory.isempty() )
    ExeDirectory = DefaultDirectory;

}

/***********************************************************************
*                             EXE_DIRECTORY                            *
***********************************************************************/
const TCHAR * exe_directory( void )
{
static bool firstime = true;

if ( firstime )
    {
    firstime = false;
    get_static_exe_directory();
    }

return ExeDirectory.text();
}

/***********************************************************************
*                         GET_EXE_DIRECTORY                            *
***********************************************************************/
BOOLEAN get_exe_directory( TCHAR * dest )
{
if ( dest )
    lstrcpy( dest, exe_directory() );

return TRUE;
}

