#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static BOOL    OVERWRITE_EXISTING  = FALSE;
static BOOLEAN NO_FILE_COMPRESSION = FALSE;

static const TCHAR DotChar = TEXT( '.' );

#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, BOOLEAN with_subs, BOOLEAN with_compression, HWND w, int id )
{

TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

int dest_len;
int sorc_len;

WIN32_FIND_DATA fdata;
HANDLE          fh;

/*
-------------------------------------------------------
Copy the sorc and dest dirs and get the length of each.
------------------------------------------------------- */
lstrcpy( sorc, sorc_dir );
lstrcpy( dest, dest_dir );

append_backslash_to_path( sorc );
append_backslash_to_path( dest );

sorc_len = lstrlen( sorc );
dest_len = lstrlen( dest );

lstrcat( sorc, wildname );

fh = FindFirstFile( sorc, &fdata );

/*
---------------------------------------------------------------
If the handle is invalid, I assume (for now) there are no files
--------------------------------------------------------------- */
if ( fh == INVALID_HANDLE_VALUE )
    return TRUE;

while ( TRUE )
    {
    if ( *fdata.cFileName != DotChar )
        {
        lstrcpy( sorc+sorc_len, fdata.cFileName );
        lstrcpy( dest+dest_len, fdata.cFileName );

        if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
            if ( with_subs )
                copy_all_files( dest, sorc, with_subs, NO_FILE_COMPRESSION, w, id );
            }
        else
            {
            if ( w )
                SetDlgItemText( w, id, fdata.cFileName );
            CopyFile( sorc, dest, OVERWRITE_EXISTING );
            }
        }

    if ( !FindNextFile(fh, &fdata) )
        break;
    }

FindClose( fh );
return TRUE;
}

/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, BOOLEAN with_subs, BOOLEAN with_compression, HWND w, int id )
{
return copy_all_files( dest_dir, sorc_dir, TEXT("*.*"), with_subs, with_compression, w, id );
}

/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, HWND w, int id )
{
return copy_all_files( dest_dir, sorc_dir, wildname, WITHOUT_SUBS, WITHOUT_COMPRESSION, w, id );
}
