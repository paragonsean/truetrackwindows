#include <windows.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\machname.h"
#include "..\include\nameclas.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\machine.h"
#include "..\include\wclass.h"

#include "resource.h"

const DWORD NEW_SYSTEM_HELP      = 0x0001;
const DWORD UPDATE_SYSTEM_HELP   = 0x0002;
const DWORD CONTENTS_HELP        = 0x0003;
const DWORD CREATE_FLOPPIES_HELP = 0X0004;

const COLORREF RedColor    = RGB( 255,   0,   0 );

static HBRUSH WhiteBrush = 0;

#define MY_MAX_INI_STRING_LEN 255

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS CurrentDialogWindow;
HWND         StatusTextWindow    = 0;

COMPUTER_CLASS Computer;

TCHAR BackSlashChar     = TEXT('\\');
TCHAR CChar             = TEXT( 'C' );
TCHAR CommaChar         = TEXT( ',' );
TCHAR CrChar            = TEXT( '\r' );
TCHAR DotChar           = TEXT( '.' );
TCHAR SpaceChar         = TEXT( ' ' );
TCHAR YChar             = TEXT( 'Y' );
TCHAR NString[]         = TEXT( "N" );
TCHAR YString[]         = TEXT( "Y" );
TCHAR NullChar          = TEXT( '\0' );
TCHAR TempName[]        = TEMP_GRAPHLST_DB;
TCHAR NewEnd[]          = TEXT( ";0" );
TCHAR CrLfString[]      = TEXT( "\015\012" );
TCHAR UnknownString[]   = UNKNOWN;
TCHAR MyName[]          = TEXT( "VisiSetup" );
TCHAR LevelZeroString[] = TEXT( "0" );
TCHAR LevelTenString[]  = TEXT( "10" );
TCHAR ChannelMode7String[] = TEXT( "7" );
TCHAR OriginalComputerName[COMPUTER_NAME_LEN+1] = TEXT( "C01" );
TCHAR NewComputerName[COMPUTER_NAME_LEN+1]      = TEXT( "C01" );
TCHAR DataArchiverComputerName[]                = TEXT( "DS"  );

TCHAR DataString[] = TEXT( "data" );
TCHAR ExesString[] = TEXT( "exes" );

TCHAR BackupDirectoryName[MAX_PATH+1] = TEXT( "" );
TCHAR DataDirectoryName[MAX_PATH+1]   = TEXT( "" );
TCHAR ExeDirectoryName[MAX_PATH+1]    = TEXT( "" );
TCHAR RootDirectoryName[MAX_PATH+1]   = TEXT( "" );
TCHAR V4DataDirectoryName[] = TEXT( "C:\\V4" );

TCHAR DisplayIniFile[]       = TEXT( "display.ini" );
TCHAR EventmanExeFile[]      = TEXT( "eventman.exe" );
TCHAR EditPartIniFileName[]  = TEXT( "editpart.ini" );
TCHAR EditPartExeFile[]      = TEXT( "editpart.exe" );
TCHAR FasTrak2ExeFile[]      = TEXT( "ft2.exe"     );
TCHAR FasTrak2IniFileName[]  = TEXT( "ftii.ini" );
TCHAR FtiiBoardsFileName[]   = TEXT( "Ftii_boards.txt" );
TCHAR FtiiMessageFileName[]  = TEXT( "ftii_control.msg" );   /* ftii control.msg in the install directory, copy to \v5\exes\control.msg */
TCHAR ProfileExeFile[]       = TEXT( "profile.exe" );
TCHAR MonEditExeFile[]       = TEXT( "monedit.exe" );
TCHAR VisiTrakIniFileName[]  = TEXT( "visitrak.ini" );
TCHAR VisiSuFileName[]       = TEXT( "Su.exe" );
TCHAR MonallIniFileName[]    = TEXT( "monall.ini" );
TCHAR MonallExeFile[]        = TEXT( "monall.exe" );
TCHAR MonallNTExeFile[]      = TEXT( "monallnt.exe" );
TCHAR PkzipExeFile[]         = TEXT( "pkzip25.exe" );
TCHAR VisiportSysFile[]      = TEXT( "visiport.sys" );
TCHAR NewProfileMenuFile[]   = TEXT( "newmenu.mnu" );
TCHAR NTDriverDir[]          = TEXT( "drivers" );
TCHAR ProfileMenuFile[]      = TEXT( "profile.mnu" );
TCHAR StartupListFile[]      = TEXT( "startup.lst" );
TCHAR SureTrakExeFile[]      = TEXT( "suretrak.exe" );
TCHAR SureTrakNTExeFile[]    = TEXT( "suretrnt.exe" );
TCHAR GermanProfileMenuFile[] = TEXT( "profileg.mnu" );
TCHAR MexProfileMenuFile[]   = TEXT( "profiles.mnu" );
TCHAR DefaultSetupFile[]     = SETUP_SHEET_FILE;
TCHAR StatusScreenConfigFile[] = TEXT( "status_screen_config.dat" );

TCHAR ConfigSection[]       = TEXT( "Config" );
TCHAR ButtonLevelsSection[] = TEXT( "ButtonLevels" );

TCHAR CurrentChannelModeKey[]  = TEXT( "CurrentChannelMode" );
TCHAR DowntimePurgeButtonKey[] = TEXT( "DowntimePurgeButton" );
TCHAR EditOperatorsButtonKey[] = TEXT( "EditOperatorsButton" );
TCHAR EditControlButtonKey[]   = TEXT( "EditControlButton" );
TCHAR NoNetworkKey[]           = TEXT( "NoNetwork" );
TCHAR ParameterTabLevelKey[]   = TEXT( "ParameterTabEditLevel" );
TCHAR RootDirKey[]             = TEXT( "RootDir" );

TCHAR HaveSureTrakKey[]        = TEXT( "HaveSureTrak" );
TCHAR StartSureTrakKey[]       = TEXT( "StartSureTrak" );
TCHAR ThisComputerKey[]        = TEXT( "ThisComputer" );
TCHAR VersionKey[]             = TEXT( "Version" );

TCHAR ChanPreConfigKey[]  = TEXT( "ChanPreConfig" );
TCHAR ChanPostConfigKey[] = TEXT( "ChanPostConfig" );
TCHAR Ft1ChanConfig[]     = TEXT( "0180" );
TCHAR Ft2ChanConfig[]     = TEXT( "1400" );

BOOLEAN NeedPartEditor            = FALSE;
BOOLEAN NeedMonitorConfig         = FALSE;
BOOLEAN NeedMonitorStartup        = FALSE;
BOOLEAN NeedStartMenuEntry        = FALSE;
BOOLEAN NeedNTDrivers             = FALSE;
BOOLEAN NeedToCopyFt2ControlFiles = FALSE;
BOOLEAN HaveSureTrak              = FALSE;
BOOLEAN NeedSureTrakStartup       = FALSE;
BOOLEAN IsNT                      = FALSE;
BOOLEAN IsFasTrak2                = FALSE;
bool    NeedToCopy                = false;
BOOLEAN IsOfficeWorkstation       = FALSE;
BOOLEAN IsNoNetwork               = FALSE;
BOOLEAN Ft2ControlFilesAreMissing = FALSE;
BOOLEAN Ft2ControlFilesAreOld     = FALSE;

static TCHAR * SorcControlFiles[] = {
    { TEXT( "e20.prg" ) },
    { TEXT( "m2.prg" ) },
    { TEXT( "m20.prg" ) }
    };

static TCHAR * SorcMessageFiles[] = {
    { TEXT( "e20.msg" ) },
    { TEXT( "m2.msg" ) },
    { TEXT( "m20.msg" ) }
    };

static TCHAR * SorcParamFiles[] = {
    { TEXT( "e20.dat" ) },
    { TEXT( "m2.dat" ) },
    { TEXT( "m20.dat" ) }
    };

static TCHAR * SorcMachsetFiles[] = {
    { TEXT( "e20.txt" ) },
    { TEXT( "m2.txt" ) },
    { TEXT( "m20.txt" ) }
    };

static TCHAR * SorcDirNames[] = {
    { TEXT( "e20" ) },
    { TEXT( "m2" ) },
    { TEXT( "m20" ) }
    };

static int32 StSorcIndex = 0;
static const int32 NofStSorcFiles = 3;

static TCHAR * Ft2ControlFile[] = {
    { TEXT("control4m.txt") },
    { TEXT("control4c.txt") },
    { TEXT("control5m.txt") },
    { TEXT("control5c.txt") },
    { TEXT("control5p.txt") },
    { TEXT("control5e.txt") },
    { TEXT("control_all.txt") },
    { TEXT("control_all.msg") }
    };
static int NofFt2ControlFiles = 8;

/*
---------------------------------
Main Version Number of old system
--------------------------------- */
int OldVersion = 5;

/*
--------------------
Copy floppies thread
-------------------- */
const int NO_FLOPPY_THREAD = 0;
const int FLOPPY_1_THREAD  = 1;
const int FLOPPY_2_THREAD  = 2;
const int FLOPPY_3_THREAD  = 3;
const int FLOPPY_4_THREAD  = 4;

static int CurrentFloppyThread        = NO_FLOPPY_THREAD;
static bool WantToAbortFloppyCreation = false;

static TCHAR LinkDesc[] = TEXT( "TrueTrak 2020 Monitoring System" );
static TCHAR LinkFile[] = TEXT( "TrueTrak 2020 Monitoring System.lnk" );

BOOLEAN menu_update( TCHAR * sorc_file_name, TCHAR * dest_file_name );

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          RESOURCE_MESSAGE_BOX                        *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                            WAIT_FOR_FILE                             *
*        Wait for a file to appear (e.g. after CopyFile).              *
***********************************************************************/
boolean wait_for_file( TCHAR * filename )
{
int count;

count = 10;
while ( true )
    {
    if ( file_exists(filename) )
        break;

    count--;
    if ( count == 0 )
        return false;

    Sleep( 500 );
    }

return true;
}

/***********************************************************************
*                                                                      *
*                             CREATE_LINK                              *
*                                                                      *
*  Uses the shell's IShellLink and IPersistFile interfaces             *
*  to create and store a shortcut to the specified object.             *
*                                                                      *
*  Returns the result of calling the member functions of the interfaces*
*                                                                      *
*  lpszPathObj - address of a buffer containing the path of the object *
*  lpszPathLink - address of a buffer containing the path where the    *
*    shell link is to be stored                                        *

*  lpszDesc - address of a buffer containing the description of the    *
*    shell link                                                        *
*                                                                      *
***********************************************************************/
HRESULT create_link( TCHAR * exe_file_name, TCHAR * link_file_name, TCHAR * link_description )
{
HRESULT        hres;
IPersistFile * ppf;
IShellLink   * psl;
WCHAR          ws[MAX_PATH];

/*
------------------------------------------
Get a pointer to the IShellLink interface.
------------------------------------------ */
hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl );
if ( SUCCEEDED(hres) )
    {

    /*
    -------------------------------------------------------------
    Set the path to the shortcut target, and add the description.
    ------------------------------------------------------------- */
    psl->SetPath( exe_file_name );
    psl->SetDescription( link_description );

    /*
    --------------------------------------------------------------
    Query IShellLink for the IPersistFile interface for saving the
    shortcut in persistent storage.
    -------------------------------------------------------------- */
    hres = psl->QueryInterface( IID_IPersistFile, (void**) &ppf );

    if ( SUCCEEDED(hres) )
        {

        /*
        -------------------------------
        Ensure that the string is ANSI.
        ------------------------------- */
        #ifdef UNICODE
            lstrcpy( ws, link_file_name );
        #else
            MultiByteToWideChar(CP_ACP, 0, link_file_name, -1, ws, MAX_PATH);
        #endif

        /*
        --------------------------------------------
        Save the link by calling IPersistFile::Save.
        -------------------------------------------- */
        hres = ppf->Save( ws, TRUE );
        ppf->Release();
        }

    psl->Release();
    }

return hres;
}

/***********************************************************************
*                             GET_OS_TYPE                              *
***********************************************************************/
void get_os_type( void )
{
OSVERSIONINFO vi;

vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

GetVersionEx( &vi );

if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT )
    IsNT = TRUE;
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
void show_status( UINT id )
{
HWND w;

w = CurrentDialogWindow.handle();
if ( w )
    {
    set_text( w, STATUS_MSG_TEXTBOX, resource_string(id) );
    }
}

/***********************************************************************
*                          SHOW_UPDATE_STATUS                          *
***********************************************************************/
static void show_update_status( UINT id )
{
set_text( StatusTextWindow, resource_string(id) );
}

/***********************************************************************
*                          SHOW_UPDATE_STATUS                          *
***********************************************************************/
static void show_update_status( TCHAR * s )
{
set_text( StatusTextWindow, s );
}

/***********************************************************************
*                         MY_GET_INI_STRING                            *
***********************************************************************/
TCHAR * my_get_ini_string( TCHAR * fname, TCHAR * section, TCHAR * key )
{
static TCHAR Buf[MY_MAX_INI_STRING_LEN+1];

TCHAR path[MAX_PATH+1];

*Buf = 0;

lstrcpy( path, ExeDirectoryName );
lstrcat( path, fname );

GetPrivateProfileString( section, key, UnknownString, Buf, MY_MAX_INI_STRING_LEN, path );

return Buf;
}

/***********************************************************************
*                          MY_PUT_INI_STRING                           *
***********************************************************************/
void my_put_ini_string( TCHAR * fname, TCHAR * section, TCHAR * key, TCHAR * sorc )
{

TCHAR path[MAX_PATH+1];

lstrcpy( path, ExeDirectoryName );
append_filename_to_path( path, fname );

WritePrivateProfileString( section, key, sorc, path );
}

/***********************************************************************
*                           IS_EXE_DIRECTORY                           *
***********************************************************************/
static BOOL is_exe_directory( TCHAR * exe_dir )
{
TCHAR s[MAX_PATH+1];

lstrcpy( s, exe_dir );
append_filename_to_path( s, VisiTrakIniFileName );

if ( file_exists(s) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            FIX_ONE_GRAPH                             *
*                                                                      *
* Add the save status to the end of each line of the GRAPHLST.TXT      *
* file.                                                                *
*            Old line = 000001;04/12/1995;23:59:00                     *
*            New line = 000001;04/12/1995;23:59:00;1                   *
*                                                                      *
***********************************************************************/
static void fix_one_graph( TCHAR * fname, int dirlen )
{
TCHAR   buf[GRAPHLST_RECLEN+1];
TCHAR   newfile[MAX_PATH+1];
TCHAR * cp;

BOOLEAN have_new_file;
FILE_CLASS dest;
FILE_CLASS sorc;

have_new_file = FALSE;

/*
--------------------------------------
Make a temporary name for the new file
-------------------------------------- */
lstrcpy( newfile, fname );
lstrcpy( (newfile+dirlen), TempName );

/*
----------------------
Open the existing file
---------------------- */
if ( !sorc.open_for_read(fname) )
    return;

/*
-----------------
Create a new file
----------------- */
if ( !dest.open_for_write(fname) )
    {
    sorc.close();
    return;
    }

while ( TRUE )
    {
    cp = sorc.readline();
    if ( !cp )
        break;

    /*
    -------------------------------------------------------
    If the line is not the right length, something is wrong
    ------------------------------------------------------- */
    if ( lstrlen(cp) != 26 )
        break;

    /*
    --------------------------------------------------------------
    All I'm doing is adding the save status to the end of the line
    -------------------------------------------------------------- */
    lstrcpy( buf, cp );
    lstrcat( buf, NewEnd );

    dest.writeline( buf );

    have_new_file = TRUE;
    }

sorc.close();
dest.close();

if ( have_new_file )
    {
    DeleteFile( fname );
    MoveFile( newfile, fname );
    }
else
    {
    DeleteFile( newfile );
    }
}

/***********************************************************************
*                           FIX_GRAPH_FILES                            *
***********************************************************************/
void fix_graph_files( TCHAR * path )
{

TCHAR s[MAX_PATH+1];
int   slen;

WIN32_FIND_DATA fdata;
HANDLE          fh;

lstrcpy( s, path );
append_backslash_to_path( s );

slen = lstrlen( s );

lstrcat( s, TEXT("*.*") );

fh = FindFirstFile( s, &fdata );

/*
---------------------------------------------------------------
If the handle is invalid, I assume (for now) there are no files
--------------------------------------------------------------- */
if ( fh == INVALID_HANDLE_VALUE )
    return;

while ( TRUE )
    {
    if ( *fdata.cFileName != DotChar )
        {
        lstrcpy( s+slen, fdata.cFileName );

        if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
            fix_graph_files( s );
            }
        else
            {
            SetDlgItemText( CurrentDialogWindow.handle(), FILENAME_TEXTBOX, s );
            if ( lstrcmpi(fdata.cFileName, GRAPHLST_DB) == 0 )
                fix_one_graph( s, slen );
            }
        }

    if ( !FindNextFile(fh, &fdata) )
        break;
    }

FindClose( fh );
return;
}

/***********************************************************************
*                            UPDATE_DCURVE                             *
*   3. If there is no alumax work equation, add it to DCURVE.TXT.      *
***********************************************************************/
void update_dcurve( void )
{
DB_TABLE t;
int      highest_curve_number;
BOOLEAN  have_work;
TCHAR    desc[DCURVE_DESC_LEN+1];

have_work = FALSE;
highest_curve_number = 0;

if ( t.open( diffcurve_dbname(), DCURVE_RECLEN, PWL ) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        maxint( highest_curve_number, (int) t.get_short(DCURVE_CURVE_NUMBER_OFFSET) );
        t.get_alpha( desc, DCURVE_DESC_OFFSET, DCURVE_DESC_LEN );
        if ( lstrcmp(desc, resource_string(PRE_IMPACT_WORK_STRING)) == 0 )
            {
            have_work = TRUE;
            break;
            }
        }

    if ( !have_work )
        {
        t.put_short( DCURVE_CURVE_NUMBER_OFFSET, (short) (highest_curve_number + 1), DCURVE_NUMBER_LEN );
        t.put_alpha( DCURVE_DESC_OFFSET, resource_string(PRE_IMPACT_WORK_STRING), DCURVE_DESC_LEN );
        t.put_float( DCURVE_HEAD_PRES_OFFSET, (float) 1.0, DCURVE_COEF_LEN );
        t.put_float( DCURVE_ROD_PRES_OFFSET,  (float) 1.0, DCURVE_COEF_LEN );
        t.rec_append();
        }

    t.close();
    }
}

/***********************************************************************
*                           UPDATE_PARMNAME                            *
*      If there is no PreImpact Work in PARMNAME.TXT, add it.          *
***********************************************************************/
void update_parmname( void )
{
DB_TABLE t;
short    parameter_number;

parameter_number = PRE_IMPACT_WORK + 1;

if ( t.open( parmname_dbname(), PARMNAME_RECLEN, PWL ) )
    {
    t.put_short( PARMNAME_NUMBER_OFFSET, parameter_number, PARAMETER_NUMBER_LEN );
    if ( !t.get_next_key_match( 1, NO_LOCK) )
        {
        t.put_long( PARMNAME_MACH_TYPE_OFFSET, long(COLD_CHAMBER_TYPE) | long(HOT_CHAMBER_TYPE), BITSETYPE_LEN );
        t.put_alpha( PARMNAME_NAME_OFFSET,     resource_string(PRE_IMPACT_WORK_STRING),          PARAMETER_NAME_LEN );
        t.put_long( PARMNAME_TYPE_OFFSET,      WORK_VAR,                                         BITSETYPE_LEN );
        t.rec_append();
        }

    t.close();
    }
}

/***********************************************************************
*                             UPDATE_UNITS                             *
*      If there are no work units in UNITS.TXT add them.               *
***********************************************************************/
void update_units( TCHAR * floppydir )
{
DB_TABLE t;
DB_TABLE sorc;
STRING_CLASS sorcfile;

int highest_id;
int id;

/*
---------------------------------------
Copy any new units to the existing file
--------------------------------------- */
highest_id = 0;

if ( t.open( units_dbname(), UNITS_RECLEN, PWL ) )
    {
    while ( t.get_next_record(FALSE) )
        {
        id = (int) t.get_long( UNITS_ID_OFFSET );
        if ( id > highest_id )
            highest_id = id;
        }

    /*
    -------------------------------------------
    Open the new units db in the sorc directory
    ------------------------------------------- */
    sorcfile = floppydir;
    sorcfile.cat_path( UNITS_DB );
    if ( sorc.open(sorcfile.text(), UNITS_RECLEN, PWL) )
        {
        /*
        ------------------------------------------
        Copy any records not in the existing table
        ------------------------------------------ */
        while ( sorc.get_next_record(FALSE) )
            {
            id = (int) sorc.get_long( UNITS_ID_OFFSET );
            if ( id > highest_id )
                {
                t.copy_rec( sorc );
                t.rec_append();
                }
            }
        sorc.close();
        }
    t.close();
    }

/*
----------------------------------------------
Make sure work is defined in the vartype table
---------------------------------------------- */
if ( t.open( vartype_dbname(), VARTYPE_RECLEN, PWL ) )
    {
    while ( t.get_next_record(FALSE) )
        {
        id = (int) t.get_long( VARTYPE_VALUE_OFFSET );
        if ( id > highest_id )
            highest_id = id;
        }
    /*
    -------------------------------------------
    Open the new units db in the sorc directory
    ------------------------------------------- */
    sorcfile = floppydir;
    sorcfile.cat_path( VARTYPE_DB );
    if ( sorc.open(sorcfile.text(), UNITS_RECLEN, PWL) )
        {
        /*
        ------------------------------------------
        Copy any records not in the existing table
        ------------------------------------------ */
        while ( sorc.get_next_record(FALSE) )
            {
            id = (int) sorc.get_long( UNITS_ID_OFFSET );
            if ( id > highest_id )
                {
                t.copy_rec( sorc );
                t.rec_append();
                }
            }
        sorc.close();
        }
    t.close();
    }
}

/***********************************************************************
*                           UPDATE_RODPITCHES                          *
* This used to just copy the new pitches but now I copy any record     *
* that is in the sorc file and not in the dest file. (V6.38)           *
***********************************************************************/
static void update_rodpitches( TCHAR * floppydir )
{
static TCHAR newpitch[] = TEXT( "1.6" );
static TCHAR newpitch1[] = TEXT( "100" );

DB_TABLE     dest;
DB_TABLE     sorc;
STRING_CLASS sorcfile;
BOOLEAN      need_to_insert;

if ( dest.open( rodpitch_dbname(), RODPITCH_RECLEN, FL ) )
    {
    sorcfile = floppydir;
    sorcfile.cat_path( RODPITCH_DB );
    if ( sorc.open(sorcfile.text(), RODPITCH_RECLEN, FL) )
        {
        while ( sorc.get_next_record(FALSE) )
            {
            dest.copy_rec( sorc );
            dest.rewind();
            if ( !dest.get_next_key_match(1, FALSE) )
                {
                dest.rewind();
                need_to_insert = dest.goto_first_greater_than_record( 1 );
                dest.copy_rec( sorc );
                if ( need_to_insert )
                    dest.rec_insert();
                else
                    dest.rec_append();
                }
            }
        }
    dest.close();
    }

if ( dest.open( psensor_dbname(), PSENSOR_RECLEN, FL ) )
    {
    sorcfile = floppydir;
    sorcfile.cat_path( PSENSOR_DB );
    if ( sorc.open(sorcfile.text(), PSENSOR_RECLEN, FL) )
        {
        /*
        ------------------------------------------
        Copy any records not in the existing table
        ------------------------------------------ */
        while ( sorc.get_next_record(FALSE) )
            {
            dest.copy_rec( sorc );
            dest.rewind();
            if ( !dest.get_next_key_match(4, FALSE) )
                {
                dest.rewind();
                need_to_insert = dest.goto_first_greater_than_record( 4 );
                /*
                -----------------------------------------------------------------
                If there is a record I load it so I need to recopy the new record
                ----------------------------------------------------------------- */
                dest.copy_rec( sorc );
                if ( need_to_insert )
                    dest.rec_insert();
                else
                    dest.rec_append();
                }
            }
        sorc.close();
        }
    dest.close();
    }
}

/***********************************************************************
*                       CREATE_STARTUP_LIST_FILE                       *
***********************************************************************/
static void create_startup_list_file( void )
{
static TCHAR ft2_delay_string[] = TEXT( "2000" );
static TCHAR ms_delay_string[]  = TEXT( "1000" );

TCHAR   sm1[MAX_PATH+1];
TCHAR * s;
TCHAR * fname;
FILE_CLASS f;

/*
------------------------------------------------------
A minus at the start of the line means start minimized
------------------------------------------------------ */
*sm1 = TEXT( '-' );
s = sm1 + 1;

lstrcpy( s, ExeDirectoryName );
append_backslash_to_path( s );
fname = s + lstrlen( s );

lstrcpy( fname, StartupListFile );

if ( f.open_for_write(s) )
    {
    lstrcpy( fname, EventmanExeFile );
    f.writeline( s );

    f.writeline( ms_delay_string );

    lstrcpy( fname, ProfileExeFile );
    f.writeline( s );

    if ( NeedPartEditor )
        {
        lstrcpy( fname, EditPartExeFile );
        f.writeline( s );
        }

    if ( NeedMonitorConfig )
        {
        lstrcpy( fname, MonEditExeFile );
        f.writeline( s );
        }

    if ( HaveSureTrak && NeedSureTrakStartup && !IsOfficeWorkstation )
        {
        lstrcpy( fname, SureTrakExeFile );
        f.writeline( sm1 );
        }

    if ( NeedMonitorStartup && !IsOfficeWorkstation )
        {
        /*
        -------------------------------
        Give the FasTrak2 time to start
        ------------------------------- */
        if ( IsFasTrak2 )
            f.writeline( ft2_delay_string );

        lstrcpy( fname, MonallExeFile );
        f.writeline( sm1 );
        }

    f.close();
    }

}

/***********************************************************************
*                  CHANGE_TO_CONTROL_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK change_to_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 1 );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        COPY_P42_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK copy_p42_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 1 );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                               COPY_P42                               *
***********************************************************************/
static void copy_p42()
{
static TCHAR P42Dir[] = TEXT( "c:\\p42" );
static TCHAR CcFile[] = TEXT( "c:\\p42\\cc.exe" );
static TCHAR PTestFile[] = TEXT( "c:\\p42\\ptest.exe" );
static TCHAR SciFile[]   = TEXT( "c:\\p42\\sci.exe" );

TCHAR * cp;
TCHAR   s[MAX_PATH+1];
bool    need_to_copy = false;

if ( IsNT )
    return;

if ( IsFasTrak2 )
    return;

if ( !directory_exists(P42Dir) )
    need_to_copy = true;
else if ( !file_exists(CcFile) )
    need_to_copy = true;

if ( need_to_copy )
    {
    if ( DialogBox(MainInstance, TEXT("COPY_P42_DIALOG"), MainWindow.handle(), (DLGPROC) copy_p42_dialog_proc) == 0 )
        return;
    }

if ( need_to_copy )
    {
    show_update_status( COPY_P42_STRING );
    if ( !directory_exists(P42Dir) )
        create_directory( P42Dir );
    copystring( s, ExeDirectoryName );
    append_backslash_to_path( s );
    cp = s + lstrlen(s);
    cp = copystring( cp, TEXT("pkzip25 -ext -dir ") );
    get_exe_directory( cp );
    lstrcat( s, TEXT("p42files") );
    execute_and_wait( s, P42Dir );

    /*
    ----------------------------------------
    Sci.exe is the same program as ptest.exe
    ---------------------------------------- */
    CopyFile( PTestFile, SciFile, FALSE );
    }
}

/***********************************************************************
*                         UPDATE_ANALOG_SENSORS                        *
* The FT2 systems need two new sensors, -10 to +10 volts               *
* and 0 to 5 volts. I look to see if they have the old table with 23   *
* entries. If so, I append my new entries on the end.                  *
***********************************************************************/
static void update_analog_sensors()
{
DB_TABLE t;
DB_TABLE sorc;
STRING_CLASS sorcfile;
int highest_id;
int id;

/*
-----------------------------------------
Copy any new sensors to the existing file
----------------------------------------- */
highest_id = 0;

if ( t.open( asensor_dbname(), ASENSOR_RECLEN, PWL ) )
    {
    while ( t.get_next_record(FALSE) )
        {
        id = (int) t.get_long( ASENSOR_NUMBER_OFFSET );
        if ( id > highest_id )
            highest_id = id;
        }

    /*
    -------------------------------------------
    Open the new units db in the sorc directory
    ------------------------------------------- */
    sorcfile = exe_directory();
    sorcfile.cat_path( ASENSOR_DB );
    if ( sorc.open(sorcfile.text(), ASENSOR_RECLEN, PWL) )
        {
        /*
        ------------------------------------------
        Copy any records not in the existing table
        ------------------------------------------ */
        while ( sorc.get_next_record(FALSE) )
            {
            id = (int) sorc.get_long( ASENSOR_NUMBER_OFFSET );
            if ( id > highest_id )
                {
                t.copy_rec( sorc );
                t.rec_append();
                }
            }
        sorc.close();
        }
    t.close();
    }
}

/***********************************************************************
*                       CHECK_CONTROL_FILE_DATES                       *
* If any of the current control files are missing or older than the    *
* new ones NeedTocopyFt2ControlFiles is set to TRUE.                   *
***********************************************************************/
static void check_control_file_dates()
{
FILETIME current_file_time;
FILETIME new_file_time;
FILE_CLASS f;
int i;
TCHAR           sorc[MAX_PATH+1];
TCHAR           dest[MAX_PATH+1];
TCHAR         * sorcfile;
TCHAR         * destfile;

get_exe_directory( sorc );
sorcfile = sorc + lstrlen(sorc);

destfile = copystring( dest, ExeDirectoryName );

Ft2ControlFilesAreMissing = FALSE;
Ft2ControlFilesAreOld     = FALSE;

for ( i=0; i<NofFt2ControlFiles; i++ )
    {
    copystring( sorcfile, Ft2ControlFile[i] );
    copystring( destfile, Ft2ControlFile[i] );
    if ( f.open_for_read(dest) )
        {
        f.get_creation_time( &current_file_time );
        f.close();
        if ( f.open_for_read(sorc) )
            {
            f.get_creation_time( &new_file_time );
            f.close();
            if ( CompareFileTime(&current_file_time, &new_file_time) < 0 )
                Ft2ControlFilesAreOld = TRUE;
            }
        }
    else
        {
        Ft2ControlFilesAreMissing = TRUE;
        }

    if ( Ft2ControlFilesAreMissing && Ft2ControlFilesAreOld )
        break;
    }

NeedToCopyFt2ControlFiles = FALSE;
if ( Ft2ControlFilesAreMissing || Ft2ControlFilesAreOld )
    NeedToCopyFt2ControlFiles = TRUE;
}

/***********************************************************************
*                             COPY_NEW_DBS                             *
*              This is only called by the update thread.               *
***********************************************************************/
void copy_new_dbs( int subversion )
{

TCHAR           sorc[MAX_PATH+1];
TCHAR           dest[MAX_PATH+1];
TCHAR         * cp;
TCHAR         * fname;
TCHAR         * destfile;
BOOLEAN         need_downtime_db;
int             i;
int             nof_machines;
INT_ARRAY_CLASS ft2boards;

TCHAR computer[COMPUTER_NAME_LEN+1];
TCHAR machine[MACHINE_NAME_LEN+1];
TCHAR part[PART_NAME_LEN+1];
DB_TABLE t;
DB_TABLE p;

if ( !get_exe_directory(sorc) )
    return;

fname = sorc + lstrlen(sorc);

/*
----------------------------------
Make sure there is a password file
---------------------------------- */
lstrcpy( dest, password_dbname() );
if ( !file_exists(dest) )
    {
    lstrcpy( fname, PASSWORD_DB );
    CopyFile( sorc, dest, FALSE );
    }

/*
----------------
And a density db
---------------- */
lstrcpy( dest, density_dbname() );
if ( !file_exists(dest) )
    {
    lstrcpy( fname, DENSITY_DB );
    CopyFile( sorc, dest, FALSE );
    }

/*
---------------------------------------------------------
Version 5.05 was the first with operators, downtimes, etc
--------------------------------------------------------- */
if ( OldVersion==5 && subversion < 5 )
    {
    lstrcpy( fname, OPERATOR_DB );
    CopyFile( sorc, operator_dbname(), FALSE );

    lstrcpy( fname, SHIFT_DB );
    CopyFile( sorc, shift_dbname(), FALSE );

    lstrcpy( fname, DOWNCAT_DB );
    CopyFile( sorc, downcat_dbname(), FALSE );

    lstrcpy( fname, DOWNSCAT_DB );
    CopyFile( sorc, downscat_dbname(), FALSE );
    }

/*
-----------------------------------------------------
Copy the FT2 Control Program Files (there are 5 now )
----------------------------------------------------- */
if ( IsFasTrak2 && NeedToCopyFt2ControlFiles )
    {
    destfile = copystring( dest, ExeDirectoryName );
    for ( i=0; i<NofFt2ControlFiles; i++ )
        {
        copystring( fname, Ft2ControlFile[i] );
        copystring( destfile, Ft2ControlFile[i] );
        CopyFile( sorc, dest, FALSE );
        }
    }

/*
--------------------------------------------------------------------------------------------
Make sure there is a SureTrak2 message file
The actual message file has the same name as the old message file but is in the exes folder.
I use a different name on the install directory because the old one is there too.
I always copy this now (as of V6.23).
-------------------------------------------------------------------------------------------- */
lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, CONTROL_MESSAGE_FILE );
lstrcpy( fname, FtiiMessageFileName );
CopyFile( sorc, dest, FALSE );

/*
--------------------------------------
Make sure there is a FasTrak2 ini file
I always copy this now.
-------------------------------------- */
if ( HaveSureTrak )
    lstrcpy( fname, TEXT("ftii_with_control.ini") );
else
    lstrcpy( fname, TEXT("ftii_mon_only.ini") );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, FasTrak2IniFileName );
CopyFile( sorc, dest, FALSE );

lstrcpy( computer, my_get_ini_string(VisiTrakIniFileName, ConfigSection, ThisComputerKey) );

/*
--------------------------------------------------------------
Copy the default suretrak control messages if there is no file
-------------------------------------------------------------- */
lstrcpy( dest, control_message_name(computer) );
if ( !file_exists(dest) )
    {
    lstrcpy( fname, CONTROL_MESSAGE_FILE );
    CopyFile( sorc, dest, FALSE );
    }

/*
------------------------------------------------
Copy the default control.prg if there is no file
------------------------------------------------ */
lstrcpy( dest, control_program_name(computer) );
if ( !file_exists(dest) )
    {
    lstrcpy( fname, CONTROL_PROGRAM_FILE );
    CopyFile( sorc, dest, FALSE );
    }

/*
---------------------------------------------------
Make sure there is a global suretrak parameter file
--------------------------------------------------- */
lstrcpy( dest, stparam_filename(computer) );
if ( !file_exists(dest) )
    {
    lstrcpy( fname, STPARAM_FILE );
    CopyFile( sorc, dest, FALSE );
    }
/*
----------------------------------
Make a list of the fastrak2 boards
---------------------------------- */
lstrcpy( sorc, ftii_boards_dbname(computer) );
if ( file_exists(sorc) )
    {
    if ( t.open(sorc, FTII_BOARDS_RECLEN, PFL) )
        {
        i = 0;
        while ( t.get_next_record(FALSE) )
            {
            ft2boards[i] = t.get_short( FTII_BOARDS_NUMBER_OFFSET );
            i++;
            }
        t.close();
        }
    }

if ( t.open( machset_dbname(computer), MACHSET_RECLEN, PWL) )
    {
    nof_machines = t.nof_recs();

    while ( t.get_next_record(FALSE) )
        {
        need_downtime_db = FALSE;

        /*
        -----------------------------------------------------
        Create a machine results directory if there isn't one
        ----------------------------------------------------- */
        t.get_alpha( machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
        cp = mrdir_name( computer, machine, 0 );
        if ( !directory_exists(cp) )
            {
            CreateDirectory( cp, NULL );
            need_downtime_db = TRUE;
            }

        if ( OldVersion==5 && subversion < 5 )
            {
            lstrcpy( fname, DSTAT_DB );
            CopyFile( sorc, dstat_dbname(computer, machine), FALSE );

            lstrcpy( fname, SEMAPHOR_DB );
            CopyFile( sorc, madir_name(computer, machine, SEMAPHOR_DB), FALSE );
            need_downtime_db = TRUE;
            }

        if ( need_downtime_db )
            {
            lstrcpy( fname, DOWNTIME_DB );
            CopyFile( sorc, downtime_dbname(computer, machine), FALSE );
            }

        /*
        ------------------------------------
        Make sure each part has suretrak dbs
        ------------------------------------ */
        if ( p.open( parts_dbname(computer, machine), PARTS_RECLEN, PFL) )
            {
            while ( p.get_next_record(FALSE) )
                {
                p.get_alpha( part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );

                /*
                ----------------------
                SureTrak profile steps
                ---------------------- */
                cp = ststeps_dbname( computer, machine, part );
                if ( !file_exists(cp) )
                    {
                    lstrcpy( fname, STSTEPS_DB );
                    CopyFile( sorc, cp, FALSE );
                    }

                /*
                -------------------------------
                SureTrak limit switch positions
                ------------------------------- */
                cp = stlimits_dbname( computer, machine, part );
                if ( !file_exists(cp) )
                    {
                    lstrcpy( fname, STLIMITS_DB );
                    CopyFile( sorc, stlimits_dbname(computer, machine, part), FALSE );
                    }
                }
            p.close();
            }
        }
    t.close();
    }

create_startup_list_file();
}

/***********************************************************************
*                      UPDATE_SURETRAK_AND_VERSION                     *
***********************************************************************/
void update_suretrak_and_version( BOOLEAN updating )
{
BOOLEAN                 b;
INI_CLASS               ini;
INI_CLASS               esini;
MACHINE_NAME_LIST_CLASS mn;
MACHINE_CLASS           m;
NAME_CLASS              d;
NAME_CLASS              s;
FILE_CLASS              f;

s = ExeDirectoryName;
s.cat_path( VisiTrakIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_string( VersionKey, resource_string(CURRENT_VERSION_STRING) );

/*
---------------------------------------------------------------------------------------------
If I am updating from a monitor to a control I should change the current machine to a control
--------------------------------------------------------------------------------------------- */
if ( updating )
    {
    b = ini.get_boolean( HaveSureTrakKey );
    if ( HaveSureTrak & !b )
        {
        mn.add_computer( Computer.whoami() );
        mn.rewind();
        while ( mn.next() )
            {
            if ( m.find(mn.computer_name(), mn.name()) )
                {
                if ( (m.monitor_flags & MA_MONITORING_ON) && m.is_ftii )
                    {
                    if ( DialogBox(MainInstance, TEXT("CHANGE_TO_CONTROL_DIALOG"), MainWindow.handle(), (DLGPROC) change_to_control_dialog_proc) == 1 )
                        {
                        m.suretrak_controlled = TRUE;
                        m.monitor_flags |= MA_HAS_DIGITAL_SERVO;
                        m.save();
                        s.get_ft2_editor_settings_file_name( m.computer, m.name );
                        esini.set_file( s.text() );
                        esini.set_section( ConfigSection );
                        esini.put_string( CurrentChannelModeKey, ChannelMode7String );
                        s.get_auto_save_all_file_name();
                        f.open_for_write( s );
                        f.writeline( TEXT("This file will be deleted by Part Setup the next time it runs.") );
                        f.close();
                        }
                    /*
                    --------------------------------------------------------------------------------------
                    Copy the following files in any case as they won't hurt anything if it is monitor only
                    -------------------------------------------------------------------------------------- */
                    d.get_ft2_editor_settings_file_name( m.computer, m.name );
                    if ( !d.file_exists() )
                        {
                        s = exe_directory();
                        s.cat_path( FTII_EDITOR_SETTINGS_FILE );
                        CopyFile( s.text(), d.text(), FALSE );
                        }
                    d.get_ft2_global_settings_file_name( m.computer, m.name );
                    if ( !d.file_exists() )
                        {
                        s = exe_directory();
                        s.cat_path( FTII_GLOBAL_SETTINGS_FILE );
                        CopyFile( s.text(), d.text(), FALSE );
                        }
                    }
                }
            }
        }
    }

ini.put_boolean( HaveSureTrakKey, HaveSureTrak );
ini.put_boolean( StartSureTrakKey, NeedSureTrakStartup );
}

/***********************************************************************
*                         BOOT_DRIVE_LETTER                            *
***********************************************************************/
TCHAR boot_drive_letter( void )
{
TCHAR path[MAX_PATH+1];
int   slen;
TCHAR c;

c    = CChar;
slen = GetSystemDirectory( path, MAX_PATH );
if ( slen > 0 )
    c = *path;

return c;
}

/***********************************************************************
*                        FULL_VISIPORT_SYSTEM_NAME                     *
***********************************************************************/
TCHAR * full_visiport_system_name( void )
{
static TCHAR path[MAX_PATH+1];
int32        slen;

slen = GetSystemDirectory( path, MAX_PATH );
if ( slen > 0 && slen <= MAX_PATH )
    {
    append_filename_to_path(  path, NTDriverDir );
    append_filename_to_path(  path, VisiportSysFile );
    }
else
    *path = NullChar;

return path;
}

/***********************************************************************
*                        COPY_FILE_DIALOG_PROC                         *
*                This is used for three dialog boxes                   *
***********************************************************************/
BOOL CALLBACK copy_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                NeedToCopy = true;
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          COPY_HELP_FILE                              *
***********************************************************************/
static void copy_help_file( void )
{
const TCHAR help_file_name[]       = TEXT( "v5help" );
const TCHAR cnt_file_name[]        = TEXT( "v5help.cnt" );
const TCHAR gid_file_name[]        = TEXT( "v5help.gid" );
const TCHAR zip_extension[]        = TEXT( ".zip" );

TCHAR dest[MAX_PATH+1];
TCHAR s[MAX_PATH+1];
TCHAR * cp;


get_exe_directory( s );

lstrcat( s, help_file_name );

lstrcat( s, zip_extension );

NeedToCopy = true;

if ( is_removable(s) )
    {
    NeedToCopy = false;
    DialogBox( MainInstance, TEXT("COPY_HELP_DIALOG"), MainWindow.handle(), (DLGPROC) copy_dialog_proc );
    }

if ( !NeedToCopy )
    return;

show_status( COPY_HELP_FILE_STRING );

if ( !file_exists(s) )
    {
    MessageBox( 0, s, resource_string(FILE_NOT_FOUND_STRING), MB_OK | MB_SYSTEMMODAL );
    return;
    }

/*
--------------------------------
Delete the old cnt and gid files
-------------------------------- */
lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, cnt_file_name );
DeleteFile( s );

lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, gid_file_name );
DeleteFile( s );

/*
-----------------------
Unzip the new help file
----------------------- */
lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, TEXT("pkzip25 -ext -over=all ") );
cp = s + lstrlen(s);
get_exe_directory( cp );

lstrcat( s, help_file_name );

lstrcpy( dest, ExeDirectoryName );
remove_backslash_from_path( dest );

execute_and_wait( s, dest );
}

/***********************************************************************
*                          KILL_MONITOR_FILES                          *
* If this is an office workstation then I don't want the monitor exes  *
* at all.                                                              *
***********************************************************************/
static void kill_monitor_files()
{
TCHAR s[MAX_PATH+1];
TCHAR * cp;

copystring( s, ExeDirectoryName );
append_backslash_to_path( s );

cp = s + lstrlen(s);

copystring( cp, MonallNTExeFile );
DeleteFile( s );

copystring( cp, SureTrakNTExeFile );
DeleteFile( s );

copystring( cp, SureTrakExeFile );
DeleteFile( s );

copystring( cp, MonallExeFile );
DeleteFile( s );

copystring( cp, FasTrak2ExeFile );
DeleteFile( s );
}

/***********************************************************************
*                            CHOOSE_FTII                               *
***********************************************************************/
static void choose_ftii()
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * cp;

copystring( dest, ExeDirectoryName );
append_backslash_to_path( dest );

cp = dest + lstrlen(dest);

copystring( cp, MonallNTExeFile );
DeleteFile( dest );

copystring( cp, SureTrakNTExeFile );
DeleteFile( dest );

copystring( cp, SureTrakExeFile );
DeleteFile( dest );

copystring( cp, MonallExeFile );
DeleteFile( dest );

copystring( sorc, ExeDirectoryName );
append_filename_to_path( sorc, FasTrak2ExeFile );
MoveFile( sorc, dest );
}

/***********************************************************************
*                         CHOOSE_FILE_BY_OS                            *
***********************************************************************/
static void choose_file_by_os( TCHAR * file95, TCHAR * filent )
{
TCHAR path95[MAX_PATH+1];
TCHAR pathnt[MAX_PATH+1];

lstrcpy( path95, ExeDirectoryName );
append_filename_to_path( path95, file95 );

lstrcpy( pathnt, ExeDirectoryName );
append_filename_to_path( pathnt, filent );

if ( IsNT )
    {
    DeleteFile( path95 );

    if ( file_exists(full_visiport_system_name()) )
        MoveFile( pathnt, path95 );
    else
        DeleteFile( pathnt );
    }
else
    {
    DeleteFile( pathnt );
    }

}

/***********************************************************************
*                           GET_FOLDER_PATH                            *
*     It is assumed the dest size is MAX_PATH * sizeof(TCHAR)          *
***********************************************************************/
bool get_folder_path( TCHAR * dest, TCHAR * local_machine_key, TCHAR * current_user_key )
{
static TCHAR shell_folder_key[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

HKEY  rh;
DWORD type;
ULONG buf_size;
bool  have_key;

have_key = false;

if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( rh, local_machine_key, NULL, &type, (unsigned char *) dest, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(rh);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( rh, current_user_key, NULL, &type, (unsigned char *) dest, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(rh);
        }
    }

return have_key;
}

/***********************************************************************
*                            UPDATE_THREAD                             *
***********************************************************************/
DWORD update_thread( int * notused )
{
TCHAR hide_menu_string[]        = TEXT( "HideMenu" );
TCHAR dde_param_delim_string[]  = TEXT( "DdeParamDelimiter" );
TCHAR dde_show_machine_string[] = TEXT( "DdeParamShowMachine" );

TCHAR    * cp;
TCHAR      dest[MAX_PATH+1];
TCHAR      s[MAX_PATH+1];
TCHAR      floppydir[MAX_PATH+1];
SYSTEMTIME t0;
SYSTEMTIME t;
BOOLEAN    need_fixgraph;
BOOLEAN    have_profile_menu;
BOOLEAN    installing_from_floppy;
int        subversion;

/*
---------------------------------------
See if this update has been done before
--------------------------------------- */
show_update_status( BEGIN_UPDATE_STRING );
cp = my_get_ini_string( VisiTrakIniFileName, ConfigSection, VersionKey );

OldVersion = asctoint32( cp );

cp = findchar( DotChar, cp, lstrlen(cp) );
subversion = 0;
if ( cp )
    {
    cp++;
    subversion = asctoint32( cp );
    }

/*
----------------------------------------------------------------------
If the profile.exe is older than 21-Jan-97, fix the graphlst.txt files
---------------------------------------------------------------------- */
need_fixgraph = TRUE;
init_systemtime_struct( t0 );
exthms( t0, TEXT("00:01:00")   );
extmdy( t0, TEXT("01/21/1997") );

lstrcpy( s, ExeDirectoryName );
lstrcat( s, ProfileExeFile );
if ( get_file_write_time(t, s) )
    {
    if ( seconds_difference(t, t0) >= 0 )
        need_fixgraph = FALSE;
    }

if ( need_fixgraph )
    {
    if ( get_root_directory(s) )
        {
        show_update_status( UPDATE_SAVE_STATUS_STRING );
        fix_graph_files( s );
        }
    }

show_update_status( UPDATE_DIFF_CURVES_STRING );
update_dcurve();

show_update_status( UPDATE_PARAM_NAMES_STRING );
update_parmname();

show_update_status( COPY_NEW_DBS_STRING );
copy_new_dbs( subversion );
update_analog_sensors();

cp = my_get_ini_string( DisplayIniFile, ConfigSection, hide_menu_string );
if ( lstrcmp(cp, UnknownString) == 0 )
    {
    my_put_ini_string( DisplayIniFile, ConfigSection, hide_menu_string, YString );
    }

cp = my_get_ini_string( MonallIniFileName, ConfigSection, dde_param_delim_string );
if ( lstrcmp(cp, UnknownString) == 0 )
    {
    my_put_ini_string( MonallIniFileName, ConfigSection, dde_param_delim_string, TEXT("\\t") );
    }

cp = my_get_ini_string( MonallIniFileName, ConfigSection, dde_show_machine_string );
if ( lstrcmp(cp, UnknownString) == 0 )
    {
    my_put_ini_string( MonallIniFileName, ConfigSection, dde_show_machine_string,     YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowPart"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowDate"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowTime"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowShot"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamParmList"),    TEXT("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60") );
    }

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, DowntimePurgeButtonKey );
if ( lstrcmp(cp, UnknownString) == 0 )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, DowntimePurgeButtonKey, LevelTenString );

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditOperatorsButtonKey );
if ( lstrcmp(cp, UnknownString) == 0 )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditOperatorsButtonKey, LevelTenString );

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditControlButtonKey );
if ( unknown(cp) )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditControlButtonKey, LevelTenString );

/*
---------------------------------------------------------------------------------------------
If this is the local install on a Data Archiver I want to tell eventman to ignore the network
--------------------------------------------------------------------------------------------- */
cp = NString;
if ( IsNoNetwork )
    cp = YString;
my_put_ini_string( VisiTrakIniFileName, ConfigSection, NoNetworkKey, cp );

cp = my_get_ini_string( EditPartIniFileName, ConfigSection, ParameterTabLevelKey );
if ( unknown(cp) )
     my_put_ini_string( EditPartIniFileName, ConfigSection, ParameterTabLevelKey, LevelZeroString );

/*
--------------------------
Copy the new exes and mnus
-------------------------- */
lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, ProfileMenuFile );
have_profile_menu = file_exists( dest );

show_update_status( UNZIP_EXES_STRING );
get_exe_directory( floppydir );

/*
---------------------------
Copy the pkzip25 executable
--------------------------- */
lstrcpy( s, floppydir );
lstrcat( s, PkzipExeFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, PkzipExeFile );

if ( !file_exists(dest) )
    {
    show_update_status( COPYING_PKZIP_STRING );
    CopyFile( s, dest, FALSE );

    if ( !wait_for_file(dest) )
         resource_message_box( FILE_NOT_FOUND_STRING, PKZIP_COPY_FAIL_STRING );
    show_update_status( UNZIP_EXES_STRING );
    }

/*
---------------------
Update the units file
--------------------- */
show_update_status( UPDATE_UNITS_STRING );
update_units( floppydir );

/*
-------------------------------------
Update the rodpitch and psensor files
------------------------------------- */
show_update_status( UPDATE_RODPITCH_STRING );
update_rodpitches( floppydir );

/*
-------------------
Copy the Setup file
------------------- */
lstrcpy( s, floppydir );
lstrcat( s, DefaultSetupFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, DefaultSetupFile );

if ( !file_exists(dest) )
    {
    CopyFile( s, dest, FALSE );
    wait_for_file(dest);
    }

/*
----------------------------------
Copy the Status Screen Config file
---------------------------------- */
lstrcpy( s, floppydir );
lstrcat( s, StatusScreenConfigFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, StatusScreenConfigFile );

if ( !file_exists(dest) )
    {
    CopyFile( s, dest, FALSE );
    wait_for_file(dest);
    }

/*
---------------------------
Update the profile.mnu file
--------------------------- */
lstrcpy( s, floppydir );

append_filename_to_path( s, NewProfileMenuFile );
lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, ProfileMenuFile );

menu_update( s, dest );


/*
-----------------------------------
The exes are now on a separate disk
----------------------------------- */
NeedToCopy             = true;
installing_from_floppy = is_removable( s );
if ( installing_from_floppy )
    {
    NeedToCopy = false;
    DialogBox( MainInstance, TEXT("COPY_EXES_DIALOG"), MainWindow.handle(), (DLGPROC) copy_dialog_proc );
    }

if ( !NeedToCopy )
    return 1;

lstrcpy( s, ExeDirectoryName );
append_backslash_to_path( s );
lstrcat( s, TEXT("pkzip25 -ext -over=all ") );
lstrcat( s, floppydir );
lstrcat( s, TEXT("v5exes *.exe *.hlp *.pif *.swf *.bmp ft2help.*") );
if ( !have_profile_menu )
    {
    lstrcat( s, TEXT(" ") );
    lstrcat( s, ProfileMenuFile );
    }

lstrcpy( dest, ExeDirectoryName );
remove_backslash_from_path( dest );

execute_and_wait( s, dest );

if ( IsOfficeWorkstation || IsNoNetwork )
    {
    kill_monitor_files();
    }
else if ( IsFasTrak2 )
    {
    choose_ftii();
    }
else
    {
    choose_file_by_os( MonallExeFile, MonallNTExeFile );
    choose_file_by_os( SureTrakExeFile, SureTrakNTExeFile );
    }

update_suretrak_and_version( TRUE );

/*
-----------------------------------
Create a P42 directory if necessary
copy_p42();
----------------------------------- */
/*
---------------------------------------------------
Copy the help file (prompt for disk 2 if necessary)
--------------------------------------------------- */
copy_help_file();

/*
----------------------------------------
The ft2help files are on a separate disk
---------------------------------------- */
NeedToCopy = true;
lstrcpy( s, floppydir );
installing_from_floppy = is_removable( s );
if ( installing_from_floppy )
    {
    NeedToCopy = false;
    DialogBox( MainInstance, TEXT("COPY_FT2HELP_DIALOG"), MainWindow.handle(), (DLGPROC) copy_dialog_proc );
    }

if ( NeedToCopy )
    {
    lstrcpy( s, ExeDirectoryName );
    append_backslash_to_path( s );
    lstrcat( s, TEXT("pkzip25 -ext -over=all ") );
    lstrcat( s, floppydir );
    lstrcat( s, TEXT("ft2help *.*") );

    lstrcpy( dest, ExeDirectoryName );
    remove_backslash_from_path( dest );

    execute_and_wait( s, dest );
    }

if ( NeedStartMenuEntry )
    {
    if ( CoInitialize(0) == S_OK )
        {
        /*
        -----------------------------------------
        Make the full path to the startup program
        ----------------------------------------- */
        lstrcpy( s, ExeDirectoryName );
        append_filename_to_path(  s, VisiSuFileName );

        /*
        ----------------------------
        Put a link in the start menu
        ---------------------------- */
        if ( get_folder_path(dest, TEXT("Common Startup"), TEXT("Startup")) )
            {
            append_filename_to_path( dest, LinkFile );
            create_link( s, dest, LinkDesc );
            }
        }

    CoUninitialize();
    }

CurrentDialogWindow.post( WM_DBCLOSE );
return 0;
}

/***********************************************************************
*                         START_UPDATE_THREAD                          *
***********************************************************************/
static void start_update_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) update_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    resource_message_box( CREATE_FAILED_STRING, UPDATE_THREAD_STRING );

}

/***********************************************************************
*                           FIND_DIRECTORIES                           *
***********************************************************************/
static void find_directories( HWND w )
{
static TCHAR firstry[]   = TEXT( "C:\\V5\\EXES" );
static TCHAR secondtry[] = TEXT( "C:\\V5EXES" );
TCHAR        s[MAX_PATH+1];
TCHAR        drivelistc[] = TEXT( "CDEFGH" );
TCHAR        drivelistd[] = TEXT( "DCEFGH" );
TCHAR      * cp;
TCHAR        c;

lstrcpy( s, firstry );
append_backslash_to_path( s );

/*
---------------------------------------------------------
Start looking on the boot drive (for now I assume C or D)
--------------------------------------------------------- */
c = boot_drive_letter();
if ( c == *drivelistc )
    cp = drivelistc;
else
    cp = drivelistd;

while ( TRUE )
    {
    if ( !(*cp) )
        break;

    *firstry   = *cp;
    *secondtry = *cp;
    cp++;

    if ( drive_exists(firstry) )
        {
        if ( directory_exists(firstry) )
            lstrcpy( s, firstry);
        else if ( directory_exists(secondtry) )
            lstrcpy( s, secondtry);
        else
            continue;
        }

    append_backslash_to_path( s );

    if ( is_exe_directory(s) )
        break;
    }

if ( !is_exe_directory(s) )
    {
    resource_message_box( COULD_NOT_FIND_EXE_STRING, CANNOT_CONTINUE_STRING );
    return;
    }

copystring( ExeDirectoryName, s );
set_text( w, EXE_DIR_EDITBOX, s );
}

/***********************************************************************
*                     REMOVE_SURETRAK_CONTROL_MENU                     *
***********************************************************************/
static void remove_suretrak_control_menu()
{
static TCHAR SURETRAK_CONTROL_MENU[] = TEXT( ",710," );
static TCHAR TEMP_MENU_FILE[] = TEXT( "tempmnu.txt" );

STRING_CLASS dfn;
STRING_CLASS sfn;
FILE_CLASS dest;
FILE_CLASS sorc;
TCHAR * cp;

sfn = ExeDirectoryName;
sfn.cat_path( ProfileMenuFile );

dfn = ExeDirectoryName;
dfn.cat_path( TEMP_MENU_FILE );

if ( sorc.open_for_read(sfn.text()) )
    {
    if ( dest.open_for_write(dfn.text()) )
        {
        while ( true )
            {
            cp = sorc.readline();
            if ( !cp )
                break;
            if ( !findstring(SURETRAK_CONTROL_MENU, cp) )
                dest.writeline( cp );
            }
        dest.close();
        sorc.close();
        DeleteFile( sfn.text() );
        MoveFile( dfn.text(), sfn.text() );
        }
    }
}

/***********************************************************************
*                           INSTALL_THREAD                             *
***********************************************************************/
DWORD install_thread( int * notused )
{
static TCHAR backup_section[]          = TEXT( "Backup" );
static TCHAR backup_dir_key[]          = TEXT( "BackupDir" );
static TCHAR backup_zip_key[]          = TEXT( "BDZipName" );
static TCHAR * ftchanpath[] = {
    TEXT( "\\E20\\NEWPART\\" ),
    TEXT( "\\E20\\VISITRAK\\" ),
    TEXT( "\\M20\\NEWPART\\" ),
    TEXT( "\\M20\\VISITRAK\\" ),
    TEXT( "\\M2\\NEWPART\\" ),
    TEXT( "\\M2\\VISITRAK\\" )
    };
const int nof_ftchanpaths = 6;

static TCHAR default_backup_zip_file[] = TEXT( "backup.zip" );

TCHAR    floppydir[MAX_PATH+1];
TCHAR    dest[MAX_PATH+1];
TCHAR    sorc[MAX_PATH+1];
TCHAR    ini_file_name[MAX_PATH+1];

TCHAR  * dest_file;
TCHAR  * sorc_file;
TCHAR  * cp;
int32    i;
WORD     lang_id;
BOOLEAN  installing_from_floppy;
long     monitor_flags;

DB_TABLE t;

show_status( CREATING_DIRS_STRING );

append_backslash_to_path( RootDirectoryName );

lstrcpy( ExeDirectoryName,    RootDirectoryName );
lstrcpy( DataDirectoryName,   RootDirectoryName );
lstrcpy( BackupDirectoryName, RootDirectoryName );

lstrcat( ExeDirectoryName,    TEXT( "EXES"  )   );
lstrcat( DataDirectoryName,   TEXT( "DATA"  )   );
lstrcat( BackupDirectoryName, TEXT( "BACKUP")   );

create_directory( ExeDirectoryName    );
create_directory( DataDirectoryName   );
create_directory( BackupDirectoryName );
get_exe_directory( floppydir );

if ( NeedNTDrivers )
    {
    /*
    -----------------------------------------------
    Copy the port driver to the NT driver directory
    ----------------------------------------------- */
    lstrcpy( sorc, floppydir );
    append_filename_to_path(  sorc, VisiportSysFile );

    lstrcpy( dest, full_visiport_system_name() );
    if ( lstrlen(dest) > 0 )
        CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc, floppydir );
    if ( SetCurrentDirectory(sorc) )
        WinExec( "regini visiport.ini", SW_SHOWNORMAL );
    else
        resource_message_box( NO_PORT_DEV_INSTALL_STRING, REGINI_FAIL_STRING );
    }

show_status( COPYING_PKZIP_STRING  );

/*
---------------------------
Copy the pkzip25 executable
--------------------------- */
lstrcpy( sorc, floppydir );
lstrcat( sorc, PkzipExeFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, PkzipExeFile );

CopyFile( sorc, dest, FALSE );

if ( !wait_for_file(dest) )
    {
    resource_message_box( FILE_NOT_FOUND_STRING, PKZIP_COPY_FAIL_STRING );
    MessageBox( 0, sorc, dest, MB_OK );
    }

/*
-------------------
Copy the Setup file
------------------- */
lstrcpy( sorc, floppydir );
lstrcat( sorc, DefaultSetupFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, DefaultSetupFile );

if ( !file_exists(dest) )
    {
    CopyFile( sorc, dest, FALSE );
    wait_for_file(dest);
    }

/*
----------------------------------
Copy the Status Screen Config file
---------------------------------- */
lstrcpy( sorc, floppydir );
lstrcat( sorc, StatusScreenConfigFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, StatusScreenConfigFile );

if ( !file_exists(dest) )
    {
    CopyFile( sorc, dest, FALSE );
    wait_for_file(dest);
    }

/*
---------------------------------
Copy the FasTrak2 control program
--------------------------------- */
lstrcpy( sorc, floppydir );

if ( HaveSureTrak )
    lstrcat( sorc, FTII_CONTROL_PROGRAM_FILE );
else
    lstrcat( sorc, TEXT("ftii_control_program_mon_only.txt") );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, FTII_CONTROL_PROGRAM_FILE );
CopyFile( sorc, dest, FALSE );
wait_for_file(dest);

/*
--------------------------------------------------------------------------------------------
Make sure there is a SureTrak2 message file
The actual message file has the same name as the old message file but is in the exes folder.
I use a different name on the install directory because the old one is there too.
-------------------------------------------------------------------------------------------- */
lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, CONTROL_MESSAGE_FILE );
if ( !file_exists(dest) )
    {
    lstrcpy( sorc, floppydir );
    lstrcat( sorc,  FtiiMessageFileName );
    CopyFile( sorc, dest, FALSE );
    }

/*
--------------------------
Copy the FasTrak2 ini file
-------------------------- */
lstrcpy( sorc, floppydir );
if ( HaveSureTrak )
    lstrcat( sorc, TEXT("ftii_with_control.ini") );
else
    lstrcat( sorc, TEXT("ftii_mon_only.ini") );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, FasTrak2IniFileName );
CopyFile( sorc, dest, FALSE );
wait_for_file(dest);

/*
---------------------------------------
Copy the german or spanish profile menu
--------------------------------------- */
lang_id = PRIMARYLANGID( GetUserDefaultLangID() );

if ( lang_id==LANG_SPANISH || lang_id== LANG_GERMAN )
    {
    lstrcpy( sorc, floppydir );
    if ( lang_id == LANG_GERMAN )
        append_filename_to_path(  sorc, GermanProfileMenuFile );
    else
        append_filename_to_path(  sorc, MexProfileMenuFile );
    lstrcpy( dest, ExeDirectoryName );
    append_filename_to_path(  dest, ProfileMenuFile );
    CopyFile( sorc, dest, FALSE );
    }

/*
-------------------------
Unzip the v5data.zip file
------------------------- */
show_status( COPYING_DATA_STRING  );

lstrcpy( sorc, ExeDirectoryName );
append_backslash_to_path( sorc );
lstrcat( sorc, TEXT("pkzip25 -ext -dir ") );
lstrcat( sorc, floppydir );
lstrcat( sorc, TEXT("v5data") );

execute_and_wait( sorc, DataDirectoryName );

show_status( UPDATING_DATA_STRING  );

/*
--------------------------------------------------------------------
If I am installing a new FT2 system, replace the old boards.txt with
an empty boards.txt and new ftii_boards.txt
-------------------------------------------------------------------- */
if ( IsFasTrak2 )
    {
    lstrcpy( sorc, floppydir );
    append_filename_to_path(  sorc, BOARDS_DB );

    lstrcpy( dest, DataDirectoryName );
    append_filename_to_path( dest, OriginalComputerName );
    append_filename_to_path( dest, BOARDS_DB );
    CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc, floppydir );
    append_filename_to_path(  sorc, FTII_BOARDS_DB );

    lstrcpy( dest, DataDirectoryName );
    append_filename_to_path( dest, OriginalComputerName );
    append_filename_to_path( dest, FTII_BOARDS_DB );
    CopyFile( sorc, dest, FALSE );
    }

/*
-------------------------------------------------------------------------------------
If this is a FasTrak 1 system I need to copy the old ftchan.txt file to all the parts
------------------------------------------------------------------------------------- */
if ( !IsFasTrak2 )
    {
    lstrcpy( sorc, floppydir );
    append_filename_to_path(  sorc, TEXT("ft1chan.txt") );

    lstrcpy( dest, DataDirectoryName );
    append_filename_to_path( dest, OriginalComputerName );
    cp = dest + lstrlen(dest);

    for ( i=0; i<nof_ftchanpaths; i++ )
        {
        copystring( cp, ftchanpath[i] );
        append_filename_to_path( dest, FTCHAN_DB );
        if ( file_exists(dest) )
            CopyFile( sorc, dest, FALSE );
        }
    }

/*
-----------------------------------
The exes are now on a separate disk
----------------------------------- */
NeedToCopy             = true;
installing_from_floppy = is_removable( floppydir );
if ( installing_from_floppy )
    {
    NeedToCopy = false;
    DialogBox( MainInstance, TEXT("COPY_EXES_DIALOG"), MainWindow.handle(), (DLGPROC) copy_dialog_proc );
    }

if ( !NeedToCopy )
    return 1;

show_status( COPYING_EXES_STRING  );

/*
--------------
Unzip the exes
-------------- */
lstrcpy( sorc, ExeDirectoryName );
append_backslash_to_path( sorc );
lstrcat( sorc, TEXT("pkzip25 -ext ") );
lstrcat( sorc, floppydir );
lstrcat( sorc, TEXT("v5exes") );

execute_and_wait( sorc, ExeDirectoryName );
if ( IsFasTrak2 )
    remove_suretrak_control_menu();

/*
--------------------------------------------------------------------------------------------
If this is a ft2, default the channel display to 7 and 8. If it is a ft1, default to 3 and 4
-------------------------------------------------------------------------------------------- */
cp = Ft1ChanConfig;
if ( IsFasTrak2 )
    cp = Ft2ChanConfig;

my_put_ini_string( DisplayIniFile, ConfigSection, ChanPreConfigKey,  cp );
my_put_ini_string( DisplayIniFile, ConfigSection, ChanPostConfigKey, cp );

/*
-----------------------------------
Create a P42 directory if necessary
copy_p42();
----------------------------------- */

lstrcpy( ini_file_name, ExeDirectoryName );
append_filename_to_path(  ini_file_name, VisiTrakIniFileName );

/*
------------------------
Fix the backup directory
[Backup]
BackupDir=e:\v5\backup\
------------------------ */
lstrcpy( sorc, BackupDirectoryName );
append_backslash_to_path( sorc );
WritePrivateProfileString( backup_section, backup_dir_key, sorc, ini_file_name );

/*
---------------------------------
[Backup]
BDZipName=e:\v5\backup.zip
--------------------------------- */
lstrcpy( sorc, RootDirectoryName );
append_filename_to_path( sorc, default_backup_zip_file );
WritePrivateProfileString( backup_section, backup_zip_key, sorc, ini_file_name );

/*
-------------------
[Config]
 RootDir=e:\v5\data\
------------------- */
lstrcpy( sorc, DataDirectoryName );
append_backslash_to_path( sorc );
WritePrivateProfileString( ConfigSection, RootDirKey, sorc, ini_file_name );

cp = NString;
if ( IsNoNetwork )
    cp = YString;
WritePrivateProfileString( ConfigSection, NoNetworkKey, cp, ini_file_name );

update_suretrak_and_version( FALSE );

/*
---------------------------------------------------------
If there is a suretrak control, the installer should have
chosen the type [E20, M2, M20]. Copy the corresponding
control, message, and parameter files (note, the new
computer name has not been set yet so I have to use
the original (c01).
--------------------------------------------------------- */
lstrcpy( sorc, DataDirectoryName );
append_filename_to_path( sorc, OriginalComputerName );
append_backslash_to_path( sorc );

lstrcpy( dest, sorc );

dest_file = dest + lstrlen(dest);
sorc_file = sorc + lstrlen(sorc);

if ( HaveSureTrak )
    {
    lstrcpy( sorc_file, SorcControlFiles[StSorcIndex] );
    lstrcpy( dest_file, CONTROL_PROGRAM_FILE );
    CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc_file, SorcMessageFiles[StSorcIndex] );
    lstrcpy( dest_file, CONTROL_MESSAGE_FILE );
    CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc_file, SorcParamFiles[StSorcIndex] );
    lstrcpy( dest_file, STPARAM_FILE );
    CopyFile( sorc, dest, FALSE );

    /*
    -----------------------------------------------------
    Suretrak controls may only have one machine, copy the
    appropriate machset file
    ----------------------------------------------------- */
    lstrcpy( sorc_file, SorcMachsetFiles[StSorcIndex] );
    lstrcpy( dest_file, MACHSET_DB );
    CopyFile( sorc, dest, FALSE );

    /*
    ------------------------------------------------
    Delete the directories of the other two machines
    Delete this one as well if this is an office
    workstation.
    ------------------------------------------------ */
    for ( i=0; i<NofStSorcFiles; i++ )
        {
        if ( i != StSorcIndex || IsOfficeWorkstation )
            {
            lstrcpy( sorc_file, SorcDirNames[i] );
            kill_directory( sorc );
            }
        }
    }
else if ( IsOfficeWorkstation )
    {
    /*
    ----------------------------------
    Remove all the machine directories
    ---------------------------------- */
    for ( i=0; i<NofStSorcFiles; i++ )
        {
        lstrcpy( sorc_file, SorcDirNames[i] );
        kill_directory( sorc );
        }
    }

lstrcpy( sorc_file, MACHSET_DB );
if ( IsOfficeWorkstation )
    {
    if ( t.open( sorc, MACHSET_RECLEN, FL) )
        {
        t.empty();
        t.close();
        }
    }
else
    {
    /*
    -----------------------------------------------
    Mark the machines as suretrak controlled or not
    ----------------------------------------------- */
    if ( t.open( sorc, MACHSET_RECLEN, FL) )
        {
        while ( t.get_next_record(FALSE) )
            {
            t.put_boolean( MACHSET_ST_BOARD_OFFSET, HaveSureTrak );
            if ( HaveSureTrak && !IsFasTrak2 )
                {
                monitor_flags = t.get_long( MACHSET_MONITOR_FLAGS_OFFSET );
                monitor_flags &= ~MA_HAS_DIGITAL_SERVO;
                t.put_long( MACHSET_MONITOR_FLAGS_OFFSET, monitor_flags, BITSETYPE_LEN );
                }
            t.rec_update();
            }
        t.close();
        }
    }
/*
-----------------------------------------------------------------------
There are three machset files (E20.TXT, etc) that are no longer needed.
Remove them.
----------------------------------------------------------------------- */
for ( i=0; i<NofStSorcFiles; i++ )
    {
    lstrcpy( sorc_file, SorcMachsetFiles[i] );
    DeleteFile( sorc );
    }

create_startup_list_file();

lstrcpy( sorc, DataDirectoryName );
append_filename_to_path( sorc, OriginalComputerName );

lstrcpy( dest, DataDirectoryName );
append_filename_to_path( dest, NewComputerName );

/*
----------------------------------------------------------------------------------------
Fix the name of this computer I know the original name is C01, and rename the directory.
---------------------------------------------------------------------------------------- */
if ( lstrcmp(OriginalComputerName, NewComputerName) != 0 )
    {
    WritePrivateProfileString( ConfigSection, ThisComputerKey, NewComputerName, ini_file_name );
    MoveFile( sorc, dest );
    }

/*
-------------------------------------------------
In the computer db, I need a backslash at the end
------------------------------------------------- */
append_backslash_to_path( dest );

/*
----------------------
Update the computer db
---------------------- */
lstrcpy( sorc, DataDirectoryName );
append_filename_to_path( sorc, COMPUTER_DB );

if ( t.open( sorc, COMPUTER_RECLEN, FL) )
    {
    t.empty();
    t.put_alpha( COMPUTER_NAME_OFFSET, NewComputerName, COMPUTER_NAME_LEN );
    t.put_alpha( COMPUTER_DIR_OFFSET,  dest,            OLD_COMPUTER_DIR_LEN );
    t.rec_append();
    t.close();
    }

if ( IsOfficeWorkstation || IsNoNetwork )
    {
    kill_monitor_files();
    }
else if ( IsFasTrak2 )
    {
    choose_ftii();
    }
else
    {
    choose_file_by_os( MonallExeFile, MonallNTExeFile );
    choose_file_by_os( SureTrakExeFile, SureTrakNTExeFile );
    }

if ( CoInitialize(0) == S_OK )
    {
    /*
    -----------------------------------------
    Make the full path to the startup program
    ----------------------------------------- */
    lstrcpy( sorc, ExeDirectoryName );
    append_filename_to_path(  sorc, VisiSuFileName );

    if ( !strings_are_equal(NewComputerName, DataArchiverComputerName) )
        {
        /*
        --------------------------
        Put an icon on the desktop
        -------------------------- */
        if ( get_folder_path(dest, TEXT("Common Desktop"), TEXT("Desktop")) )
            {
            append_filename_to_path( dest, LinkFile );
            create_link( sorc, dest, LinkDesc );
            }

        if ( NeedStartMenuEntry )
            {
            /*
            ----------------------------
            Put a link in the start menu
            ---------------------------- */
            if ( get_folder_path(dest, TEXT("Common Startup"), TEXT("Startup")) )
                {
                append_filename_to_path( dest, LinkFile );
                create_link( sorc, dest, LinkDesc );
                }
            }
        }

    CoUninitialize();
    }

copy_help_file();

/*
----------------------------------------
The ft2help files are on a separate disk
---------------------------------------- */
NeedToCopy = true;
installing_from_floppy = is_removable( floppydir );
if ( installing_from_floppy )
    {
    NeedToCopy = false;
    DialogBox( MainInstance, TEXT("COPY_FT2HELP_DIALOG"), MainWindow.handle(), (DLGPROC) copy_dialog_proc );
    }

if ( NeedToCopy )
    {
    lstrcpy( sorc, ExeDirectoryName );
    append_backslash_to_path( sorc );
    lstrcat( sorc, TEXT("pkzip25 -ext -over=all ") );
    lstrcat( sorc, floppydir );
    lstrcat( sorc, TEXT("ft2help *.*") );

    lstrcpy( dest, ExeDirectoryName );
    remove_backslash_from_path( dest );

    execute_and_wait( sorc, dest );
    }

CurrentDialogWindow.post( WM_DBCLOSE );
return 0;
}

/***********************************************************************
*                         START_INSTALL_THREAD                         *
***********************************************************************/
static void start_install_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) install_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    resource_message_box( CREATE_FAILED_STRING, INSTALL_THREAD_STRING );

}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( const DWORD context )
{
TCHAR fname[MAX_PATH+1];


if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("setup.hlp") );
    WinHelp( MainWindow.handle(), fname, HELP_CONTEXT, context );
    }

}

/***********************************************************************
*                            FINISH_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK finish_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        UPDATE_STATUS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK update_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        StatusTextWindow = GetDlgItem( hWnd, STATUS_MSG_TEXTBOX );
        start_update_thread();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_DBCLOSE:
        StatusTextWindow = 0;
        CurrentDialogWindow = 0;
        CreateDialog(
            MainInstance,
            TEXT("FINISH_DIALOG"),
            MainWindow.handle(),
            (DLGPROC) finish_dialog_proc );
        DestroyWindow( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          FILENAME_IN_STRING                          *
***********************************************************************/
static BOOLEAN filename_in_string( TCHAR * s, TCHAR * filename )
{
int32 slen;
int32 filename_len;

filename_len = lstrlen( filename );
slen         = lstrlen( s );

if ( slen >= filename_len )
    {
    s += slen;
    s -= filename_len;
    if ( lstrcmpi(s, filename) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    THIS_IS_AN_OFFICE_WORKSTATION                     *
***********************************************************************/
static BOOLEAN this_is_an_office_workstation( TCHAR * visitrak_ini_file )
{
TCHAR   buf[MAX_INI_STRING_LEN+1];
TCHAR   name[COMPUTER_NAME_LEN+1];
TCHAR   path[MAX_PATH+1];
TCHAR   this_computer[COMPUTER_NAME_LEN+1];
TCHAR * cp;
STRING_CLASS s;
FILE_CLASS f;
DB_TABLE   t;
BOOLEAN have_path      = FALSE;
BOOLEAN is_workstation = FALSE;

GetPrivateProfileString( ConfigSection, ThisComputerKey, UNKNOWN, buf, MAX_INI_STRING_LEN, visitrak_ini_file );
if ( !unknown(buf) )
    {
    copystring( this_computer, buf );
    GetPrivateProfileString( ConfigSection, RootDirKey, UNKNOWN, path, MAX_INI_STRING_LEN, visitrak_ini_file );
    if ( !unknown(path) )
        {
        s = path;
        s.cat_path( COMPUTER_CSV );
        if ( file_exists(s.text()) )
            {
            /*
            ---------------------------------------------------------------------------
            The old computer.txt file has been replaced with a new free format csv file
            --------------------------------------------------------------------------- */
            f.open_for_read( s.text() );
            while ( true )
                {
                s = f.readline();
                if ( s.isempty() )
                    break;
                cp = findchar( CommaChar, s.text() );
                if ( cp )
                    {
                    *cp = NullChar;
                    if ( s.len() <= COMPUTER_NAME_LEN )
                        {
                        copystring( name, s.text() );
                        if ( strings_are_equal(this_computer, name) )
                            {
                            cp++;
                            copystring( path, cp );
                            have_path = TRUE;
                            break;
                            }
                        }
                    }
                }
            f.close();
            }
        else
            {
            append_filename_to_path( path, COMPUTER_DB );
            if ( t.open( path, COMPUTER_RECLEN, PFL) )
                {
                while ( t.get_next_record(NO_LOCK) )
                    {
                    t.get_alpha( name, COMPUTER_NAME_OFFSET, COMPUTER_NAME_LEN );
                    if ( strings_are_equal(this_computer, name) )
                        {
                        t.get_alpha( path, COMPUTER_DIR_OFFSET,  OLD_COMPUTER_DIR_LEN );
                        have_path = TRUE;
                        break;
                        }
                    }
                t.close();
                }
            }
        }
    }

if ( have_path )
    {
    append_filename_to_path( path, MACHSET_DB );
    if ( t.open( path, MACHSET_RECLEN, PFL) )
        {
        if ( t.nof_recs() < 1 )
            is_workstation = TRUE;
        t.close();
        }
    }

return is_workstation;
}

/***********************************************************************
*                       INIT_UPDATE_CHECKBOXES                         *
***********************************************************************/
static void init_update_checkboxes( HWND w )
{
TCHAR buf[MAX_INI_STRING_LEN+1];
TCHAR path[MAX_PATH+1];
TCHAR * fname;
TCHAR * cp;
BOOLEAN is_suretrak;
BOOLEAN need_check;
BOOLEAN need_part_editor;
BOOLEAN need_monitor_setup;
BOOLEAN need_board_monitor;
FILE_CLASS   f;
STRING_CLASS s;
DB_TABLE     t;
HWND         cw;

need_part_editor   = FALSE;
need_monitor_setup = FALSE;
need_board_monitor = FALSE;

if ( get_text( path, w, EXE_DIR_EDITBOX, MAX_PATH) )
    {
    IsFasTrak2 = FALSE;

    s = path;
    s.replace( ExesString, DataString );
    s.cat_path( my_get_ini_string(VisiTrakIniFileName, ConfigSection, ThisComputerKey) );
    s.cat_path( FTII_BOARDS_DB );
    if ( file_exists(s.text()) )
        {
        if ( t.open(s.text(), FTII_BOARDS_RECLEN, PFL) )
            {
            if ( t.get_next_record(FALSE) )
                IsFasTrak2 = TRUE;
            t.close();
            }
        }

    append_backslash_to_path( path );
    fname = path + lstrlen(path);

    lstrcpy( fname, VisiTrakIniFileName );

    GetPrivateProfileString( ConfigSection, HaveSureTrakKey, UNKNOWN, buf, MAX_INI_STRING_LEN, path );
    if ( *buf == YChar )
        is_suretrak = TRUE;
    else
        is_suretrak = FALSE;

    set_checkbox( w, SURETRAK_CONTROL_CHECKBOX, is_suretrak );

    NeedSureTrakStartup = FALSE;
    if ( is_suretrak )
        {
        GetPrivateProfileString( ConfigSection, StartSureTrakKey, UNKNOWN, buf, MAX_INI_STRING_LEN, path );
        if ( *buf == YChar )
            NeedSureTrakStartup = TRUE;
        }

    /*
    --------------------------------------------------------------------------
    If this is a suretrak control with auto start then it isn't a fastrak2 yet
    -------------------------------------------------------------------------- */
    if ( NeedSureTrakStartup )
        IsFasTrak2 = FALSE;

    set_checkbox( w, FASTRAK2_XBOX, IsFasTrak2 );

    if ( IsFasTrak2 )
        check_control_file_dates();
    set_checkbox( w, COPY_FT2_CONTROL_FILES_XBOX, NeedToCopyFt2ControlFiles );

    /*
    ---------------------------------------------------------------------------
    See if this is an local install on a data archiver. If so, NoNetwork=Y will
    be in the visitrak.ini file.
    ---------------------------------------------------------------------------   */
    GetPrivateProfileString( ConfigSection, NoNetworkKey, UNKNOWN, buf, MAX_INI_STRING_LEN, path );
    if ( *buf == YChar )
        {
        need_check = TRUE;
        }
    else
        {
        need_check = FALSE;
        NeedSureTrakStartup = FALSE;
        }

    set_checkbox( w, NO_NETWORK_XBOX, need_check );

    need_check = FALSE;
    if ( this_is_an_office_workstation(path) )
        {
        need_check = TRUE;
        NeedSureTrakStartup = FALSE;
        }
    set_checkbox( w, OFFICE_WORKSTATION_XBOX, need_check );

    cw = GetDlgItem( w, START_SURETRAK_CONTROL_CHECKBOX );
    if ( IsFasTrak2 )
        EnableWindow( cw, FALSE );
    else
        set_checkbox( cw, NeedSureTrakStartup );

    lstrcpy( fname, StartupListFile );
    f.open_for_read( path );
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        if ( !need_part_editor && filename_in_string(cp, EditPartExeFile) )
            need_part_editor = TRUE;

        if ( !need_monitor_setup && filename_in_string(cp, MonEditExeFile) )
            need_monitor_setup = TRUE;

        if ( !need_board_monitor && filename_in_string(cp, MonallExeFile) )
            need_board_monitor = TRUE;
        }

    f.close();

    set_checkbox( w, PART_EDITOR_CHECKBOX,    need_part_editor   );
    set_checkbox( w, AUTO_MONITOR_CHECKBOX,   need_board_monitor );
    set_checkbox( w, MONITOR_SETUPS_CHECKBOX, need_monitor_setup );
    }

if ( CoInitialize(0) == S_OK )
    {
    if ( get_folder_path(path, TEXT("Common Startup"), TEXT("Startup")) )
        {
        append_filename_to_path( path, LinkFile );
        if ( file_exists(path) )
            {
            NeedStartMenuEntry = TRUE;
            set_checkbox( w, START_AT_BOOT_XBOX, TRUE );
            }
        }

    CoUninitialize();
    }

}

/***********************************************************************
*                  DO_UPDATE_SURETRAK_CONTROL_CHANGE                   *
***********************************************************************/
static void do_update_suretrak_control_change()
{
BOOL is_enabled;
HWND w;

w = CurrentDialogWindow.handle();

if ( is_checked( w, SURETRAK_CONTROL_CHECKBOX) )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

if ( is_checked( w, FASTRAK2_XBOX) )
    is_enabled = FALSE;

w = GetDlgItem( w, START_SURETRAK_CONTROL_CHECKBOX );

EnableWindow( w, is_enabled );

if ( is_enabled && NeedSureTrakStartup )
    set_checkbox( w, TRUE );
else
    set_checkbox( w, FALSE );
}

/***********************************************************************
*                   DO_START_SURETRAK_CONTROL_CHANGE                   *
***********************************************************************/
static void do_start_suretrak_control_change()
{
HWND w;

w = CurrentDialogWindow.handle();

NeedSureTrakStartup = is_checked( w, START_SURETRAK_CONTROL_CHECKBOX );
}

/***********************************************************************
*                     DO_UPDATE_WORKSTATION_TOGGLE                     *
***********************************************************************/
static void do_update_workstation_xbox()
{
HWND w;
HWND monitor_setups;
HWND monitor_startup;
HWND control_startup;
BOOL is_enabled;

w = CurrentDialogWindow.handle();
monitor_setups  = GetDlgItem( w, MONITOR_SETUPS_CHECKBOX );
monitor_startup = GetDlgItem( w, AUTO_MONITOR_CHECKBOX );
control_startup = GetDlgItem( w, START_SURETRAK_CONTROL_CHECKBOX );

is_enabled = TRUE;
if ( is_checked(w, NO_NETWORK_XBOX) || is_checked(w, OFFICE_WORKSTATION_XBOX) )
    {
    is_enabled = FALSE;
    set_checkbox( monitor_setups,  FALSE );
    set_checkbox( monitor_startup, FALSE );
    set_checkbox( control_startup, FALSE );
    }

EnableWindow( monitor_setups, is_enabled );
EnableWindow( monitor_startup, is_enabled );

if ( is_enabled )
    do_update_suretrak_control_change();
else
    EnableWindow( control_startup, is_enabled );
}

/***********************************************************************
*                       FIND_DIRECTORY_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK find_directory_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        find_directories( hWnd );
        init_update_checkboxes( hWnd );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SURETRAK_CONTROL_CHECKBOX:
            case FASTRAK2_XBOX:
                do_update_suretrak_control_change();
                return TRUE;

            case START_SURETRAK_CONTROL_CHECKBOX:
                do_start_suretrak_control_change();
                return TRUE;

            case OFFICE_WORKSTATION_XBOX:
            case NO_NETWORK_XBOX:
                do_update_workstation_xbox();
                return TRUE;

            case COPY_FT2_CONTROL_FILES_XBOX:
                if ( !is_checked(hWnd, COPY_FT2_CONTROL_FILES_XBOX) )
                    {
                    if ( Ft2ControlFilesAreMissing )
                        resource_message_box( COPY_BY_HAND_STRING, MISSING_CONTROL_FILES_STRING );
                    }
                return TRUE;

            case IDOK:
                if ( is_checked(hWnd, PART_EDITOR_CHECKBOX) )
                    NeedPartEditor = TRUE;

                if ( is_checked(hWnd, MONITOR_SETUPS_CHECKBOX) )
                    NeedMonitorConfig = TRUE;

                if ( is_checked(hWnd, AUTO_MONITOR_CHECKBOX) )
                    NeedMonitorStartup = TRUE;

                if ( is_checked(hWnd, START_AT_BOOT_XBOX) )
                    NeedStartMenuEntry = TRUE;

                NeedSureTrakStartup = FALSE;
                if ( is_checked(hWnd, SURETRAK_CONTROL_CHECKBOX) )
                    {
                    HaveSureTrak = TRUE;
                    if ( is_checked(hWnd, START_SURETRAK_CONTROL_CHECKBOX) )
                        NeedSureTrakStartup = TRUE;
                    }

                if ( is_checked(hWnd, FASTRAK2_XBOX) )
                    IsFasTrak2 = TRUE;
                else
                    IsFasTrak2 = FALSE;

                NeedToCopyFt2ControlFiles = FALSE;
                if ( is_checked(hWnd, COPY_FT2_CONTROL_FILES_XBOX) )
                    NeedToCopyFt2ControlFiles = TRUE;

                IsNoNetwork = FALSE;
                if ( is_checked(hWnd, NO_NETWORK_XBOX) )
                    IsNoNetwork = TRUE;

                IsOfficeWorkstation = FALSE;
                if ( is_checked(hWnd, OFFICE_WORKSTATION_XBOX) )
                    IsOfficeWorkstation = TRUE;

                if ( IsNoNetwork || IsOfficeWorkstation )
                    {
                    NeedSureTrakStartup = FALSE;
                    NeedMonitorStartup  = FALSE;
                    NeedMonitorConfig   = FALSE;
                    }

                get_text( ExeDirectoryName, hWnd, EXE_DIR_EDITBOX, MAX_PATH+1 );
                append_backslash_to_path( ExeDirectoryName );
                if ( !is_exe_directory(ExeDirectoryName) )
                    {
                    resource_message_box( NO_VISITRAK_INI_STRING, CANNOT_CONTINUE_STRING );
                    return TRUE;
                    }
                SetCurrentDirectory( ExeDirectoryName );
                if ( names_startup(ExeDirectoryName) )
                    Computer.startup( ExeDirectoryName );

                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("UPDATE_STATUS_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) update_status_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       INSTALL_STATUS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK install_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        CurrentDialogWindow.set_focus( IDCANCEL );
        CurrentDialogWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        show_status( BEGIN_INSTALL_STRING );
        start_install_thread();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( NEW_SYSTEM_HELP );
         return TRUE;

    case WM_DBCLOSE:
        CurrentDialogWindow = 0;
        CreateDialog(
            MainInstance,
            TEXT("FINISH_DIALOG"),
            MainWindow.handle(),
            (DLGPROC) finish_dialog_proc );
        DestroyWindow( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      DO_SURETRAK_CONTROL_CHANGE                      *
***********************************************************************/
static void do_suretrak_control_change()
{
int  i;
BOOL is_enabled;
HWND w;

int  id[] = { RADIO_GROUPBOX, E20_RADIO, M2_RADIO, M20_RADIO,
              DIST_UNITS_TBOX, VEL_UNITS_TBOX, ROD_PITCH_TBOX };

const int n = sizeof(id)/sizeof(int);

w = CurrentDialogWindow.handle();

if ( is_checked( w, SURETRAK_CONTROL_CHECKBOX) )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

for ( i=0; i<n; i++ )
    EnableWindow( GetDlgItem(w, id[i]), is_enabled );

if ( is_checked( w, FASTRAK2_XBOX) )
    is_enabled = FALSE;

w = GetDlgItem( w, START_SURETRAK_CONTROL_CHECKBOX );

EnableWindow( w, is_enabled );

if ( is_enabled && NeedSureTrakStartup )
    set_checkbox( w, TRUE );
else
    set_checkbox( w, FALSE );
}

/***********************************************************************
*                        RENAME_ROOT_DIRECTORY                         *
***********************************************************************/
static boolean rename_root_directory( HWND w )
{
static TCHAR oldname[] = TEXT( "OldV5" );

int     i;
TCHAR   sorc[MAX_PATH+1];
TCHAR   dest[MAX_PATH+1];
TCHAR * cp;


lstrcpy( sorc, RootDirectoryName );
remove_backslash_from_path( sorc );

i = 1;
while ( true )
    {
    lstrcpy( dest, sorc );
    cp = dest + lstrlen(dest) - 1;
    /*
    ---------------------
    Replace V5 with OldV5
    --------------------- */
    while ( cp > dest )
        {
        if ( *cp == BackSlashChar )
            {
            cp++;
            lstrcpy( cp, oldname );
            break;
            }
        cp--;
        }
    /*
    ------------
    Add a number
    ------------ */
    lstrcat( dest, int32toasc(i) );

    if ( !directory_exists(dest) )
        break;
    i++;
    }

cp = maketext( resource_string(WILL_BE_RENAMED_STRING), lstrlen(dest) );
if ( cp )
    {
    lstrcat( cp, dest );
    i = MessageBox( w, cp, resource_string(INSTALL_DIR_EXISTS_STRING), MB_OKCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL );
    delete[] cp;
    cp = 0;
    }

if ( i != IDOK )
    return false;

if ( !MoveFile(sorc, dest) )
    {
    resource_message_box( UNABLE_TO_RENAME_DIR_STRING, UNABLE_TO_COMPLY_STRING );
    return false;
    }

return true;
}

/***********************************************************************
*                          INIT_ROOT_DIR_EBOX                          *
***********************************************************************/
static void init_root_dir_ebox( HWND w )
{
TCHAR path[MAX_PATH+1];

lstrcpy( path, resource_string(DEFAULT_INSTALL_DIRECTORY_STRING) );
*path = boot_drive_letter();

set_text( w, path );
}

/***********************************************************************
*                       MAKE_SURE_I_AM_ON_SCREEN                       *
***********************************************************************/
static void make_sure_i_am_on_screen( void )
{
RECT dr;
RECT r;
long dy;

dr = desktop_client_rect();
if ( MainWindow.getrect(r) )
    {
    if ( r.bottom > dr.bottom )
        {
        dy = dr.bottom - r.bottom;
        MainWindow.offset( 0, dy );
        }
    }
}

/***********************************************************************
*                     DO_INSTALL_WORKSTATION_TOGGLE                    *
***********************************************************************/
static void do_install_workstation_xbox()
{
HWND w;
HWND monitor_setups;
HWND monitor_startup;
HWND control_startup;
BOOL is_enabled;

w = CurrentDialogWindow.handle();
monitor_setups  = GetDlgItem( w, MONITOR_SETUPS_CHECKBOX );
monitor_startup = GetDlgItem( w, AUTO_MONITOR_CHECKBOX );
control_startup = GetDlgItem( w, START_SURETRAK_CONTROL_CHECKBOX );

is_enabled = TRUE;
if ( is_checked(w, NO_NETWORK_XBOX) || is_checked(w, OFFICE_WORKSTATION_XBOX) )
    {
    is_enabled = FALSE;
    set_checkbox( monitor_setups,  FALSE );
    set_checkbox( monitor_startup, FALSE );
    set_checkbox( control_startup, FALSE );
    }

EnableWindow( monitor_setups, is_enabled );
EnableWindow( monitor_startup, is_enabled );

if ( is_enabled )
    do_suretrak_control_change();
else
    EnableWindow( control_startup, is_enabled );
}

/***********************************************************************
*                         STARTUP_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK startup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
static HWND root_dir_ebox = 0;
static HWND computer_ebox = 0;


id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        make_sure_i_am_on_screen();
        MainWindow.set_title( resource_string(STARTUP_TITLE) );
        CurrentDialogWindow.set_focus( IDOK );
        CurrentDialogWindow.post( WM_DBINIT );
        root_dir_ebox = GetDlgItem( hWnd, ROOT_DIRECTORY_EBOX );
        computer_ebox = GetDlgItem( hWnd, THIS_COMPUTER_EBOX );
        CheckRadioButton( hWnd, E20_RADIO, M20_RADIO, E20_RADIO );
        set_checkbox( hWnd, FASTRAK2_XBOX, TRUE );
        do_suretrak_control_change();
        if ( !IsNT )
            EnableWindow( GetDlgItem( hWnd, NT_DRIVERS_CHECKBOX), FALSE );
        return TRUE;

    case WM_DBINIT:
        init_root_dir_ebox( root_dir_ebox );
        limit_text( computer_ebox, COMPUTER_NAME_LEN );
        set_text( computer_ebox, OriginalComputerName );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( NEW_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SURETRAK_CONTROL_CHECKBOX:
            case FASTRAK2_XBOX:
                do_suretrak_control_change();
                return TRUE;

            case START_SURETRAK_CONTROL_CHECKBOX:
                do_start_suretrak_control_change();
                return TRUE;

            case OFFICE_WORKSTATION_XBOX:
            case NO_NETWORK_XBOX:
                do_install_workstation_xbox();
                return TRUE;

            case IDOK:
                get_text( NewComputerName, computer_ebox, COMPUTER_NAME_LEN );
                if ( lstrlen(NewComputerName) < 1 )
                    {
                    resource_message_box( hWnd, MainInstance, MUST_HAVE_COMPUTER_STRING, CANNOT_CONTINUE_STRING, MB_OK );
                    set_text( computer_ebox, OriginalComputerName );
                    return TRUE;
                    }

                if ( is_checked(hWnd, PART_EDITOR_CHECKBOX) )
                    NeedPartEditor = TRUE;

                if ( is_checked(hWnd, NO_NETWORK_XBOX) )
                    IsNoNetwork = TRUE;

                if ( is_checked(hWnd, OFFICE_WORKSTATION_XBOX) )
                    IsOfficeWorkstation = TRUE;

                if ( is_checked(hWnd, START_AT_BOOT_XBOX) )
                    NeedStartMenuEntry = TRUE;

                if ( is_checked(hWnd, MONITOR_SETUPS_CHECKBOX) )
                    NeedMonitorConfig = TRUE;

                NeedSureTrakStartup = FALSE;
                if ( is_checked(hWnd, SURETRAK_CONTROL_CHECKBOX) )
                    {
                    HaveSureTrak = TRUE;
                    if ( is_checked(hWnd, START_SURETRAK_CONTROL_CHECKBOX) )
                        NeedSureTrakStartup = TRUE;
                    }

                /*
                ---------------------------
                Get the suretrak units type
                --------------------------- */
                StSorcIndex = 0;
                if ( is_checked(hWnd, M2_RADIO) )
                    StSorcIndex = 1;
                if ( is_checked(hWnd, M20_RADIO) )
                    StSorcIndex = 2;

                if ( is_checked(hWnd, START_MONITOR_CHECKBOX) )
                    NeedMonitorStartup = TRUE;

                if ( is_checked(hWnd, NT_DRIVERS_CHECKBOX) )
                    NeedNTDrivers = TRUE;

                if ( is_checked(hWnd, FASTRAK2_XBOX) )
                    IsFasTrak2 = TRUE;
                else
                    IsFasTrak2 = FALSE;

                if ( IsFasTrak2 )
                    {
                    NeedSureTrakStartup = FALSE;
                    }

                if ( IsNoNetwork || IsOfficeWorkstation )
                    {
                    NeedSureTrakStartup = FALSE;
                    NeedMonitorStartup  = FALSE;
                    NeedMonitorConfig   = FALSE;
                    }

                get_text( RootDirectoryName, root_dir_ebox, MAX_PATH );
                append_backslash_to_path( RootDirectoryName );
                if ( directory_exists(RootDirectoryName) )
                    {
                    if ( !rename_root_directory(hWnd) )
                        return TRUE;
                    }

                if ( !create_directory(RootDirectoryName) )
                    {
                    resource_message_box( CANT_CREATE_DIR_STRING, CANNOT_CONTINUE_STRING );
                    return TRUE;
                    }

                CurrentDialogWindow = 0;

                CreateDialog(
                    MainInstance,
                    TEXT("INSTALL_STATUS_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) install_status_dialog_proc );

                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  UPDATE_STARTUP_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK update_startup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HWND   MyStaticControl;

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MyStaticControl= GetDlgItem( hWnd, UPDATE_PW_NOTE_TBOX );
        MainWindow.shrinkwrap( hWnd );
        MainWindow.set_title( resource_string(UPDATE_STARTUP_TITLE) );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CTLCOLORSTATIC:
        if ( MyStaticControl == (HWND) lParam )
            {
            SetTextColor( (HDC) wParam, RedColor );
            SetBkMode( (HDC) wParam, TRANSPARENT );
            return (long) WhiteBrush;
            }
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;

            case IDOK:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("FIND_DIRECTORY_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) find_directory_dialog_proc );

                DestroyWindow( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      CREATE_FLOPPIES_THREAD                          *
***********************************************************************/
DWORD create_floppies_thread( int * notused )
{
const TCHAR * flist1[] = {
    { TEXT("asensor.txt")   },
    { TEXT("boards.txt")   },
    { TEXT("control.msg")  },
    { TEXT("control.prg")  },
    { TEXT("density.txt")  },
    { TEXT("downcat.txt")  },
    { TEXT("downscat.txt") },
    { TEXT("downtime.txt") },
    { TEXT("dstat.txt")    },
    { TEXT("ft1chan.txt")  },
    { TEXT("ftii_control.msg")  },
    { TEXT("ftii_mon_only.ini") },
    { TEXT("ftii_with_control.ini") },
    { TEXT("ftii_boards.txt") },
    { TEXT("ftii_control_program.txt") },
    { TEXT("ftii_control_program_mon_only.txt") },
    { TEXT("newmenu.mnu")  },
    { TEXT("newmenug.mnu") },
    { TEXT("newmenus.mnu") },
    { TEXT("operator.txt") },
    { TEXT("parmlist.txt") },
    { TEXT("parmname.txt") },
    { TEXT("psensor.txt")   },
    { TEXT("password.txt") },
    { TEXT("pkzip25.exe")  },
    { TEXT("profileg.mnu") },
    { TEXT("profiles.mnu") },
    { TEXT("regini.exe")   },
    { TEXT("rodpitch.txt") },
    { TEXT("semaphor.txt") },
    { TEXT("setup.exe")    },
    { TEXT("setup.hlp")    },
    { TEXT("SetupSheet.txt") },
    { TEXT("shift.txt")    },
    { TEXT("status_screen_config.dat") },
    { TEXT("stlimits.txt") },
    { TEXT("stparam.dat")  },
    { TEXT("ststeps.txt")  },
    { TEXT("units.txt")    },
    { TEXT("v5data.zip")   },
    { TEXT("v5dataj.zip")  },
    { TEXT("vartype.txt")  },
    { TEXT("visiport.ini") },
    { TEXT("visiport.sys") }
    };

const int nofiles1 = sizeof(flist1)/sizeof(TCHAR *);

const TCHAR * flist2[] = {
    { TEXT("v5exes.zip") }
    };
const int nofiles2 = sizeof(flist2)/sizeof(TCHAR *);

const TCHAR * flist3[] = {
    { TEXT("v5help.zip") }
    };
const int nofiles3 = sizeof(flist3)/sizeof(TCHAR *);

const TCHAR * flist4[] = {
    { TEXT("ft2help.zip") }
    };
const int nofiles4 = sizeof(flist4)/sizeof(TCHAR *);

const TCHAR floppyroot[] = TEXT( "A:\\" );

TCHAR sorc[MAX_PATH+1];
TCHAR dest[MAX_PATH+1];
TCHAR prompt[MAX_PATH+1];

HWND  w;
TCHAR * sorcname;
TCHAR * destname;

const TCHAR ** mylist;
int i;
int n;
int radio_id;

w = CurrentDialogWindow.handle();

switch( CurrentFloppyThread )
    {
    case FLOPPY_1_THREAD:
        mylist   = flist1;
        n        = nofiles1;
        radio_id = FLOPPY_2_RADIO;
        break;

    case FLOPPY_2_THREAD:
        mylist   = flist2;
        n        = nofiles2;
        radio_id = FLOPPY_3_RADIO;
        break;

    case FLOPPY_3_THREAD:
        mylist   = flist3;
        n        = nofiles3;
        radio_id = 0; // Was FLOPPY_4_RADIO;
        break;

    case FLOPPY_4_THREAD:
        mylist   = flist4;
        n        = nofiles4;
        radio_id = 0;
        break;

    default:
        return 1;
    }

if ( !get_exe_directory(sorc) )
    return 1;

append_backslash_to_path( sorc );
sorcname = sorc + lstrlen(sorc);

lstrcpy( dest, floppyroot );
destname = dest + lstrlen(dest);

for( i=0; i<n; i++ )
    {
    if ( WantToAbortFloppyCreation )
        {
        INT answer;
        answer = resource_message_box( MainInstance, WANT_TO_ABORT_STRING, ABORT_RECEIVED_STRING, MB_YESNO | MB_SYSTEMMODAL );
        if ( answer == IDYES )
            {
            set_text( w, INSERT_FLOPPY_TBOX, resource_string(FLOPPY_IN_DRIVE_STRING) );
            CurrentFloppyThread = NO_FLOPPY_THREAD;
            return 1;
            }
        WantToAbortFloppyCreation = false;
        }

    lstrcpy( sorcname, mylist[i] );
    lstrcpy( destname, mylist[i] );

    if ( file_exists(sorc) )
        {
        lstrcpy( prompt, resource_string(COPYING_STRING) );
        lstrcat( prompt, dest );
        lstrcat( prompt, TEXT("...") );
        set_text( w, INSERT_FLOPPY_TBOX, prompt );
        if ( !CopyFile(sorc, dest, FALSE) )
            {
            MessageBox( w, dest, resource_string(UNABLE_TO_COPY_STRING), MB_OK | MB_SYSTEMMODAL );
            CurrentFloppyThread = NO_FLOPPY_THREAD;
            return 1;
            }
        }
    }


if ( radio_id )
    {
    CurrentFloppyThread = NO_FLOPPY_THREAD;
    set_text( w, INSERT_FLOPPY_TBOX, resource_string(FLOPPY_IN_DRIVE_STRING) );
    CheckRadioButton( w, FLOPPY_1_RADIO, FLOPPY_4_RADIO, radio_id );
    }
else
    {
    resource_message_box( HOW_TO_INSTALL_STRING, COPY_COMPLETE_STRING );
    MainWindow.post( WM_CLOSE );
    }

return 0;
}

/***********************************************************************
*                      START_CREATE_FLOPPIES_THREAD                    *
***********************************************************************/
static void start_create_floppies_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) create_floppies_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    resource_message_box( CREATE_FAILED_STRING, CREATE_FLOPPIES_THREAD_STRING );

}

/***********************************************************************
*                      CREATE_FLOPPIES_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK create_floppies_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        make_sure_i_am_on_screen();
        MainWindow.set_title( resource_string(CREATE_FLOPPIES_TITLE) );
        CheckRadioButton( hWnd, FLOPPY_1_RADIO, FLOPPY_3_RADIO, FLOPPY_1_RADIO );
        set_text( hWnd, INSERT_FLOPPY_TBOX, resource_string(FLOPPY_IN_DRIVE_STRING) );
        CurrentDialogWindow.set_focus( BEGIN_COPYING_BUTTON );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CREATE_FLOPPIES_HELP );
         return TRUE;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            switch ( id )
                {
                case FLOPPY_1_RADIO:
                case FLOPPY_2_RADIO:
                case FLOPPY_3_RADIO:
                case FLOPPY_4_RADIO:
                    if ( is_checked(hWnd, id) )
                        set_text( hWnd, INSERT_FLOPPY_TBOX, resource_string(FLOPPY_IN_DRIVE_STRING) );
                    return TRUE;

                case BEGIN_COPYING_BUTTON:
                    if ( CurrentFloppyThread != NO_FLOPPY_THREAD )
                        {
                        resource_message_box( UNABLE_TO_COMPLY_STRING, COPY_IN_PROGRESS_STRING );
                        return TRUE;
                        }

                    if ( is_checked(hWnd, FLOPPY_1_RADIO) )
                        {
                        CurrentFloppyThread = FLOPPY_1_THREAD;
                        }
                    else if ( is_checked(hWnd, FLOPPY_2_RADIO) )
                        {
                        CurrentFloppyThread = FLOPPY_2_THREAD;
                        }
                    else if ( is_checked(hWnd, FLOPPY_3_RADIO) )
                        {
                        CurrentFloppyThread = FLOPPY_3_THREAD;
                        }
                    else
                        {
                        CurrentFloppyThread = FLOPPY_4_THREAD;
                        }
                    WantToAbortFloppyCreation = false;
                    start_create_floppies_thread();
                    return TRUE;

                case STOP_COPYING_BUTTON:
                    if ( CurrentFloppyThread != NO_FLOPPY_THREAD )
                         {
                         WantToAbortFloppyCreation = true;
                         set_text( hWnd, INSERT_FLOPPY_TBOX, resource_string(STOPPING_STRING) );
                         }
                    return TRUE;

                case WM_CLOSE:
                    if ( CurrentFloppyThread != NO_FLOPPY_THREAD )
                        resource_message_box( WAIT_FOR_COPY_STRING, UNABLE_TO_COMPLY_STRING );
                    else
                        MainWindow.post( WM_CLOSE );
                    return TRUE;
                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        SOURCE_IS_ON_FLOPPIES                         *
***********************************************************************/
boolean source_is_on_floppies( void )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
if ( is_removable(s) )
    return true;

return false;
}

/***********************************************************************
*                THERE_ARE_SPACES_IN_THE_DIRECTORY_PATH                *
***********************************************************************/
BOOLEAN there_are_spaces_in_the_directory_path()
{
if ( findchar(SpaceChar, exe_directory()) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       CHOOSE_INSTALL_OR_UPDATE_PROC                  *
***********************************************************************/
BOOL CALLBACK choose_install_or_update_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );

        if ( there_are_spaces_in_the_directory_path() )
            {
            resource_message_box( SPACES_IN_PATH_STRING, CANNOT_CONTINUE_STRING );
            MainWindow.close();
            }

        CurrentDialogWindow.set_focus( INSTALL_NEW_BUTTON );
        if ( source_is_on_floppies() )
            EnableWindow( GetDlgItem(hWnd, CREATE_FLOPPIES_BUTTON), FALSE );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case INSTALL_NEW_BUTTON:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("STARTUP_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) startup_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case CREATE_FLOPPIES_BUTTON:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("CREATE_FLOPPIES_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) create_floppies_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case UPDATE_EXISTING_BUTTON:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("UPDATE_STARTUP_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) update_startup_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        WhiteBrush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
        return 0;

    case WM_DESTROY:
        if ( WhiteBrush )
            {
            DeleteObject( WhiteBrush );
            WhiteBrush = 0;
            }
        PostQuitMessage( 0 );
        return 0;

    case WM_SETFOCUS:
        CurrentDialogWindow.set_focus();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( HINSTANCE this_instance, int cmd_show )
{

WNDCLASS wc;

get_os_type();

MainInstance = this_instance;

wc.lpszClassName = MyName;
wc.hInstance     = this_instance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindow(
    MyName,                         // window class name
    MyName,                         // window title
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 70,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("INSTALL_OR_UPDATE_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) choose_install_or_update_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL  status;

init( this_instance, cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( CurrentDialogWindow.handle() )
        status = IsDialogMessage( CurrentDialogWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

return(msg.wParam);
}
