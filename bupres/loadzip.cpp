#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"

static TCHAR   NullChar     = TEXT( '\0' );
static TCHAR   LowerAChar   = TEXT( 'a' );
static TCHAR   UpperAChar   = TEXT( 'A' );
static TCHAR   FilterEnd[]  = TEXT( "*.ZIP\0" );
static TCHAR   DefaultZipDir[] = TEXT( "\\" );
static TCHAR * Filter       = 0;                   /* TEXT( "Zip Files\0*.ZIP\0" ); */
static TCHAR * ChooseString = 0;

/*
-----------------------------------
Const strings defined in bupres.cpp
----------------------------------- */
extern TCHAR VisiTrakIniFile[];
extern TCHAR BackupSection[];

/***********************************************************************
*                             INIT_STRINGS                             *
***********************************************************************/
static void init_strings( void )
{
int32   slen;
TCHAR * s;

slen = 2 + lstrlen( FilterEnd );
s = resource_string( MainInstance, ZIP_FILES_STRING );
Filter = maketext( s, slen );
if ( Filter )
    {
    s = Filter;
    s = nextstring( Filter );
    lstrcpy( s, FilterEnd );
    s = nextstring( s );
    *s = NullChar;
    }

s = resource_string( MainInstance, CHOOSE_ZIP_FILE_STRING );
ChooseString = maketext( s );
}

/***********************************************************************
*                          LOAD_ZIP_FILE                               *
***********************************************************************/
void load_zip_file( void )
{
HCURSOR      old_cursor;
OPENFILENAME fh;
TCHAR      * cp;
TCHAR        fname[MAX_PATH+1];
TCHAR        buf[MAX_PATH+1];
TCHAR        oridir[MAX_PATH+1];
TCHAR        newdir[MAX_PATH+1];

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow;
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = oridir;
fh.lpstrTitle        = ChooseString;
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( !Filter )
    init_strings();

cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BDZipName") );
if ( cp )
    {
    if ( *cp == LowerAChar || *cp == UpperAChar )
        cp = 0;
    }

if ( cp )
    {
    lstrcpy( oridir, cp );
    if ( dir_from_filename(oridir) )
        append_backslash_to_path( oridir );
    else
        cp = 0;
    }

if ( !cp )
    lstrcpy( oridir, DefaultZipDir );

if ( GetOpenFileName( &fh ) )
    {
    old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );
    if ( directory_exists( BackupDir) )
        kill_directory( BackupDir );

    lstrcpy( newdir, BackupDir );
    remove_backslash_from_path( newdir );

    if ( CreateDirectory(newdir, 0) )
        {

        if ( GetCurrentDirectory(sizeof(oridir), oridir) == 0 )
            *oridir = NullChar;

        SetCurrentDirectory( newdir );

        /*
        -------------------------------------------
        Assume pkunzip.exe is is the exes directory
        ------------------------------------------- */
        get_exe_directory( buf );
        lstrcat( buf, TEXT("pkzip25 -ext -dir \"") );
        lstrcat( buf, fname );
        lstrcat( buf, TEXT( "\"" ) );

        execute_and_wait( buf, newdir );

        if ( *oridir )
            SetCurrentDirectory( oridir );
        }

    SetCursor( old_cursor );
    }

}
