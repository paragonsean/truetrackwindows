#include <windows.h>
#include <conio.h>
#include <winnetwk.h>   /* link to mpr.lib */

const TCHAR NullChar      = TEXT( '\0' );
const TCHAR BackSlashChar = TEXT( '\\' );

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

/***********************************************************************
*                              ENUMERATE                               *
***********************************************************************/
static BOOLEAN enumerate( NETRESOURCE * resource_to_enumerate )
{
const DWORD BUF_SIZE = 16384;        // 16K is a good size

HANDLE        hEnum;
DWORD         i;
DWORD         nof_bytes;
DWORD         nof_entries;
DWORD         status;
NETRESOURCE * pr;
TCHAR         s[MAX_PATH+1];
bool          havedir;

status = WNetOpenEnum(
    RESOURCE_GLOBALNET,
    RESOURCETYPE_DISK,
    RESOURCEUSAGE_CONNECTABLE | RESOURCEUSAGE_CONTAINER,
    resource_to_enumerate,
    &hEnum
    );

if ( status != NO_ERROR )
    return FALSE;

nof_bytes = BUF_SIZE;
pr = (NETRESOURCE *) GlobalAlloc( GPTR, nof_bytes );
if ( !pr )
    {
    cprintf( "No Memory...\n\r" );
    return FALSE;
    }

do  {
    nof_bytes = BUF_SIZE;
    ZeroMemory( pr, nof_bytes );

    /*
    ----------------------------------------
    I want to enumerate all possible entries
    ---------------------------------------- */
    nof_entries = 0xFFFFFFFF;
    status = WNetEnumResource( hEnum, &nof_entries, pr, &nof_bytes );
    if ( status == NO_ERROR )
        {
        for( i=0; i<nof_entries; i++ )
            {
            if ( pr[i].dwScope == RESOURCE_GLOBALNET )
                {
                /*
                ------------------------------------------------------------------
                If this NETRESOURCE is a container, call the function recursively.
                ------------------------------------------------------------------ */
                if ( (pr[i].dwUsage & RESOURCEUSAGE_CONTAINER) == RESOURCEUSAGE_CONTAINER )
                    {
                    havedir = false;
                    s[0] = NullChar;
                    lstrcpy( s, pr[i].lpRemoteName );
                    if ( s[0] == BackSlashChar )
                        {
                        if ( s[1] == BackSlashChar )
                            {
                            havedir = true;
                            cprintf( ".........computer....[%s]\n\r", s );
                            }
                        }

                    if ( !havedir )
                        {
                        cprintf( "container............[%s]\n\r", s );
                        enumerate( &pr[i] );
                        }
                    }
                }
            else
                {
                cprintf( "Resource not globalnet\n\r" );
                }

            }
        }

    } while( status != ERROR_NO_MORE_ITEMS );

GlobalFree((HGLOBAL) pr);

status = WNetCloseEnum(hEnum);
if ( status != NO_ERROR)
    return FALSE;

return TRUE;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR * cp;

// char  buf[MAX_PATH];
//  DWORD buf_size = MAX_PATH;

enumerate(0);

cprintf( "\n\rPress enter to exit" );
cp = getline();

/*
#define ERROR_FILE_NOT_FOUND             2L
#define ERROR_PATH_NOT_FOUND             3L

cp = getline();
if ( cp )
    {
    if ( lstrlen(cp) > 0 )
        {
        SchemaFile = cp;
        SchemaFile.cat_path( SchemaName );

        get_file_list( cp );

        cprintf( "\n\r" );
        }
    }
*/
return 0;
}

