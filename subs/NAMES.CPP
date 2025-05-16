#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static const TCHAR   VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static const TCHAR   ConfigSection[]   = TEXT( "Config" );
static const TCHAR   ResultsString[]   = TEXT( "\\results" );
static const int     ResultsStringLen  = 8;
static const TCHAR   RootDirKey[]      = TEXT( "RootDir" );
static const TCHAR   DefaultRootDir[]  = TEXT( "C:\\V5\\" );
static const TCHAR   BackSlashString[] = TEXT( "\\" );

static const TCHAR   BackSlashChar = TEXT( '\\' );
static const TCHAR   NullChar = TEXT( '\0' );
static const TCHAR   ZeroChar = TEXT( '0' );

static BOOLEAN HaveStartup = FALSE;
static TCHAR Buf[MAX_PATH+1]    = TEXT( "" );
static TCHAR RootDir[MAX_PATH+1] = TEXT( "" );
static COMPUTER_CLASS MyComputer;
static TCHAR EmptyString[] = TEXT("");

/***********************************************************************
*                        GET_ROOT_DIRECTORY                            *
***********************************************************************/
BOOLEAN get_root_directory( TCHAR * dest )
{
lstrcpy( dest, RootDir );
return TRUE;
}

/***********************************************************************
*                             ROOT_DIRECTORY                           *
***********************************************************************/
const TCHAR * root_directory( void )
{
return RootDir;
}

/***********************************************************************
*                        COMPUTER_ROOT_DIRECTORY                       *
*  Return the root directory of any computer I know about. Return      *
*  0 if I don't know about that computer.                              *
***********************************************************************/
const TCHAR * computer_root_directory( TCHAR * computer_name )
{
TCHAR * cp;

if ( !MyComputer.find(computer_name) )
    return 0;

lstrcpy( Buf, MyComputer.directory() );
cp = Buf + lstrlen(Buf);
if ( cp < Buf+2 )
    return 0;

/*
--------------------------------------------------------------------------------------
I am pointing at the null. I want to backup 2 chars, as the end char is the backslash.
-------------------------------------------------------------------------------------- */
cp -= 2;

/*
--------------------------------
Backup to the previous backslash
-------------------------------- */
while ( cp > Buf )
    {
    if ( *cp == BackSlashChar )
        {
        cp++;
        *cp = NullChar;
        return Buf;
        }
    cp--;
    }

return 0;
}

/***********************************************************************
*                   SET_BUF_TO_COMPUTER_DIRECTORY                      *
***********************************************************************/
BOOLEAN set_buf_to_computer_directory( TCHAR * computer_name  )
{

if ( MyComputer.find(computer_name) )
    lstrcpy( Buf, MyComputer.directory() );
else
    {
    get_root_directory( Buf );
    lstrcat( Buf, computer_name );
    lstrcat( Buf, TEXT("\\") );
    }

return TRUE;
}

/***********************************************************************
*                            NAMES_STARTUP                             *
***********************************************************************/
BOOLEAN names_startup( TCHAR * exe_dir )
{

TCHAR fname[MAX_PATH+1];
int bytes_read;

if ( HaveStartup )
    return TRUE;

HaveStartup = TRUE;

/*
----------------------------
Get the name of the ini file
---------------------------- */
lstrcpy( fname, exe_dir );
append_backslash_to_path( fname );
lstrcat( fname, VisiTrakIniFile );

bytes_read = GetPrivateProfileString( ConfigSection, RootDirKey, DefaultRootDir, RootDir, MAX_PATH, fname );
if ( !bytes_read )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            NAMES_STARTUP                             *
***********************************************************************/
BOOLEAN names_startup( void )
{
TCHAR path[MAX_PATH+1];

get_exe_directory( path );

return names_startup( path );
}

/***********************************************************************
*                        COMPUTER_DIRECTORY_NAME                       *
***********************************************************************/
TCHAR * computer_directory_name( TCHAR * computer_name )
{

if ( !set_buf_to_computer_directory(computer_name) )
    (*Buf) = NullChar;

return Buf;
}

/***********************************************************************
*                             ROOTDIR_NAME                             *
*         Return the name of a db in the root directory.               *
***********************************************************************/
TCHAR * rootdir_name( TCHAR * db_name )
{

get_root_directory( Buf );
lstrcat( Buf, db_name );

return Buf;
}

/***********************************************************************
*                              CODIR_NAME                              *
*         Return the name of a db in a computer directory.             *
***********************************************************************/
TCHAR * codir_name( TCHAR * computer_name, TCHAR * db_name )
{
set_buf_to_computer_directory( computer_name );
lstrcat( Buf, db_name );

return Buf;
}

/***********************************************************************
*                              MADIR_NAME                              *
*         Return the name of a db in a machine directory.              *
***********************************************************************/
TCHAR * madir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * db_name )
{
set_buf_to_computer_directory( computer_name );
lstrcat( Buf, machine_name );
if ( db_name )
    {
    lstrcat( Buf, BackSlashString );
    lstrcat( Buf, db_name );
    }

return Buf;
}

/***********************************************************************
*                             EXEDIR_NAME                              *
***********************************************************************/
TCHAR * exedir_name( const TCHAR * file_name )
{
get_exe_directory( Buf );
lstrcat( Buf, file_name );

return Buf;
}

/***********************************************************************
*                              MRDIR_NAME                              *
*       Return the name of a db in a machine results directory.        *
***********************************************************************/
TCHAR * mrdir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * db_name )
{

set_buf_to_computer_directory( computer_name );
lstrcat( Buf, machine_name );
lstrcat( Buf, TEXT("\\results") );
if ( db_name )
    {
    lstrcat( Buf, BackSlashString );
    lstrcat( Buf, db_name );
    }

return Buf;
}

/***********************************************************************
*                              PADIR_NAME                              *
*         Return the name of a db in a part directory.                 *
***********************************************************************/
TCHAR * padir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * db_name )
{

set_buf_to_computer_directory( computer_name );
lstrcat( Buf, machine_name );
lstrcat( Buf, BackSlashString );
lstrcat( Buf, part_name );
if ( db_name )
    {
    lstrcat( Buf, BackSlashString );
    lstrcat( Buf, db_name );
    }

return Buf;
}

/***********************************************************************
*                              RSDIR_NAME                              *
*       Return the name of a db in a part results directory.           *
***********************************************************************/
TCHAR * rsdir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * db_name )
{

set_buf_to_computer_directory( computer_name );
lstrcat( Buf, machine_name );
lstrcat( Buf, BackSlashString );
lstrcat( Buf, part_name );
lstrcat( Buf, TEXT("\\results") );
if ( db_name )
    {
    lstrcat( Buf, BackSlashString );
    lstrcat( Buf, db_name );
    }

return Buf;
}

/***********************************************************************
*                      GET_PART_RESULTS_FILE_NAME                      *
*   Put the name of a file in the part results table into a string.    *
***********************************************************************/
BOOLEAN get_part_results_file_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * file_name )
{
COMPUTER_CLASS c;
int            n;

if ( !c.find(computer_name) )
    return FALSE;

/*
------------------------------------------------------------------------------------
Calculate the number of characters needed and make the dest long enough to hold them
      computer_dir + \ + machine_name + \ + part_name + ResultsString + \ + dbname
------------------------------------------------------------------------------------ */
n = lstrlen( c.directory() );
n += lstrlen( machine_name );
n += lstrlen( part_name );
n += lstrlen( file_name );
n += ResultsStringLen;
n += 3;
if ( dest.len() < n )
    {
    if ( !dest.upsize(n) )
        return FALSE;
    }

dest = c.directory();
dest.cat_path( machine_name );
dest.cat_path( part_name );
dest += ResultsString;
dest.cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                    GET_MACHINE_DIRECTORY_FILE_NAME                   *
*         Return the name of a db in a machine directory.              *
***********************************************************************/
BOOLEAN get_machine_directory_file_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name )
{
COMPUTER_CLASS c;
int            n;

if ( !c.find(computer_name) )
    return FALSE;

/*
------------------------------------------------------------------------------------
Calculate the number of characters needed and make the dest long enough to hold them
      computer_dir + \ + machine_name + \ + dbname
------------------------------------------------------------------------------------ */
n = lstrlen( c.directory() );
n += lstrlen( machine_name );
n += lstrlen( file_name );
n += 2;
if ( dest.len() < n )
    {
    if ( !dest.upsize(n) )
        return FALSE;
    }

dest = c.directory();
dest.cat_path( machine_name );
dest.cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                           ASCII_SHOT_NUMBER                          *
***********************************************************************/
TCHAR * ascii_shot_number( int32 shot_number )
{
static TCHAR shot_name[SHOT_LEN+1];

insalph( shot_name, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
*(shot_name+SHOT_LEN) = NullChar;

return shot_name;
}

/***********************************************************************
*                         GET_PROFILE_II_NAME                          *
***********************************************************************/
BOOLEAN get_profile_ii_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name )
{
TCHAR shot_file_name[SHOT_LEN+5];

lstrcpy( shot_file_name, shot_name );
if ( lstrlen(shot_file_name) < SHOT_LEN )
    rjust( shot_file_name, SHOT_LEN, ZeroChar );
lstrcat( shot_file_name, PROFILE_II_SUFFIX );

return get_part_results_file_name( dest, computer_name, machine_name, part_name, shot_file_name );
}

/***********************************************************************
*                         GET_PROFILE_II_NAME                          *
***********************************************************************/
BOOLEAN get_profile_ii_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number )
{
return get_profile_ii_name( dest, computer_name, machine_name, part_name, ascii_shot_number(shot_number) );
}

/***********************************************************************
*                           PROFILE_II_NAME                            *
***********************************************************************/
TCHAR * profile_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name )
{

TCHAR shot_file_name[SHOT_LEN+5];

lstrcpy( shot_file_name, shot_name );
if ( lstrlen(shot_file_name) < SHOT_LEN )
    rjust( shot_file_name, SHOT_LEN, ZeroChar );
lstrcat( shot_file_name, PROFILE_II_SUFFIX );

return rsdir_name( computer_name, machine_name, part_name, shot_file_name );
}

/***********************************************************************
*                            PROFILE_II_NAME                           *
***********************************************************************/
TCHAR * profile_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number )
{
return profile_ii_name( computer_name, machine_name, part_name, ascii_shot_number(shot_number) );
}

/***********************************************************************
*                            * PROFILE_NAME                            *
***********************************************************************/
TCHAR * profile_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name )
{

TCHAR shot_file_name[SHOT_LEN+5];

lstrcpy( shot_file_name, shot_name );
lstrcat( shot_file_name, PROFILE_NAME_SUFFIX );

return rsdir_name( computer_name, machine_name, part_name, shot_file_name );
}

/***********************************************************************
*                            * PROFILE_NAME                            *
***********************************************************************/
TCHAR * profile_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number )
{
return profile_name( computer_name, machine_name, part_name, ascii_shot_number(shot_number) );
}

/***********************************************************************
*                             MASTER_NAME                              *
***********************************************************************/
TCHAR * master_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * master_name )
{

TCHAR fname[MASTER_TRACE_NAME_LEN+5];

lstrcpy( fname, master_name );
lstrcat( fname, MASTER_TRACE_SUFFIX );

return rsdir_name( computer_name, machine_name, part_name, fname );
}

/***********************************************************************
*                           MASTER_II_NAME                             *
***********************************************************************/
TCHAR * master_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * master_name )
{

TCHAR fname[MASTER_TRACE_NAME_LEN+5];

lstrcpy( fname, master_name );
lstrcat( fname, MASTER_TRACE_II_SUFFIX );

return rsdir_name( computer_name, machine_name, part_name, fname );
}

/***********************************************************************
*                     FT2_CONTROL_PROGRAM_NAME                         *
* This returns the name of the file only. You need to append this      *
* to the exes directory to make the entire path.                       *
* The flash version is either 4 or 5.
***********************************************************************/
TCHAR * ft2_control_program_name( int flash_version, MACHINE_CLASS & m )
{
static TCHAR control4c[] = TEXT("control4c.txt" );
static TCHAR control4m[] = TEXT("control4m.txt" );
static TCHAR control5c[] = TEXT("control5c.txt" );
static TCHAR control5m[] = TEXT("control5m.txt" );
static TCHAR control5e[] = TEXT("control5e.txt" );
static TCHAR control5p[] = TEXT("control5p.txt" );
static TCHAR controlall[] = TEXT("control_all.txt" );

TCHAR * cp;

if ( flash_version < 5 )
    {
    if ( m.suretrak_controlled )
        cp = control4c;
    else
        cp = control4m;
    }
else if ( flash_version >= 6 )
    {
    cp = controlall;
    }
else
    {
    if ( m.suretrak_controlled )
        {
        if ( m.monitor_flags & MA_HAS_DIGITAL_SERVO )
            {
            if ( m.monitor_flags & MA_HAS_PRESSURE_CONTROL )
                cp = control5p;
            else
                cp = control5c;
            }
        else
            {
            cp = control5e;
            }
        }
    else
        {
        cp = control5m;
        }
    }
return cp;
}
