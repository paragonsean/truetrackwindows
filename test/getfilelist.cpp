#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

const TCHAR NullChar         = TEXT( '\0' );
const TCHAR SpaceChar        = TEXT( ' ' );
const TCHAR DecimalPointChar = TEXT( '.' );
const TCHAR ZeroChar         = TEXT( '0' );

/***********************************************************************
*                            GET_FILE_LIST                             *
***********************************************************************/
void get_file_list( TEXT_LIST_CLASS & dest, const TCHAR * rootdir, const TCHAR * file_name_pattern )
{
TCHAR s[MAX_PATH_NAME_LEN+1];
HANDLE fh;
WIN32_FIND_DATA fdata;

dest.empty();

lstrcpy( s, rootdir );
append_filename_to_path( s, file_name_pattern );

fh = FindFirstFile( s, &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( !(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            dest.append( fdata.cFileName );

        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                            GET_FILE_LIST                             *
***********************************************************************/
void get_file_list( TEXT_LIST_CLASS & dest, const TCHAR * rootdir )
{
const TCHAR star_dot_star[] = TEXT( "*.*" );

get_file_list( dest, rootdir, star_dot_star );
}

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
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR * cp;
TEXT_LIST_CLASS fname;


while ( true )
    {
    cprintf( "\n\rInput the root directory " );
    cp = getline();
    if ( !cp )
        break;

    if ( lstrlen(cp) < 1 )
        break;

    get_file_list( fname, cp );
    fname.rewind();
    while ( fname.next() )
        cprintf( "\n\r[%s]", fname.text() );
    }

cprintf( "\n\r" );

return 0;
}

