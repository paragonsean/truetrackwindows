#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\computer.h"
#include "..\include\genlist.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\part.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\shotclas.h"

#include "resource.h"
#include "backupcl.h"
#include "extern.h"

static const int MAX_KEY_NAME_LEN = 40;
static const int32 ZIP_SUFFIX_LEN = 4;
static const DWORD ThirtyMinutesInMs = 1800000L;

static TCHAR BackupDirString[]        = TEXT( "BackupDir" );

static TCHAR CurrentPartName[]        = TEXT( "CurrentPartName" );
static TCHAR LastDownTime[]           = TEXT( "LastDownTime" );

static TCHAR LastSavedParamsTime[]    = TEXT( "LastSavedParamsTime" );
static TCHAR LastSavedParamsNumber[]  = TEXT( "LastSavedParamsNumber" );
static TCHAR NumberParamsAcquired[]   = TEXT( "NumberParamsAcquired"  );
static TCHAR AcquiringParams[]        = TEXT( "AcquiringParams"  );

static TCHAR LastSavedProfileTime[]   = TEXT( "LastSavedProfileTime" );
static TCHAR LastSavedProfileNumber[] = TEXT( "LastSavedProfileNumber" );
static TCHAR NumberProfilesAcquired[] = TEXT( "NumberProfilesAcquired"  );
static TCHAR AcquiringProfiles[]      = TEXT( "AcquiringProfiles"  );

static TCHAR LastExaminedShotNumber[] = TEXT( "LastExaminedShotNumber" );

static TCHAR ZipCount[]               = TEXT( "ZipCount" );
static TCHAR PartZipSuffix[]          = TEXT( ".PAZ" );
static TCHAR MachineZipSuffix[]       = TEXT( ".MAZ" );
static TCHAR DefaultPartFileName[]    = TEXT( "{m}_{p}_{nb}_{ne}" );
static TCHAR DefaultMachineFileName[] = TEXT( "{m}_{mm}_{dd}_{hh}_{mi}" );

/*
-----------------------------------------------------
These tokens are used only for the part zip file name
----------------------------------------------------- */
static TCHAR FirstShotToken[]         = TEXT( "{nb}" );
static TCHAR LastShotToken[]          = TEXT( "{ne}" );


static const TCHAR CrString[] = TEXT( "\r\n" );
static TCHAR StarDotStar[]            = TEXT( "*.*" );
static TCHAR EmptyString[]            = TEXT( "" );

static const TCHAR ProfileKeyPrefix[] = TEXT( "Profile" );
static const TCHAR ParamsKeyPrefix[]  = TEXT( "Params"  );

/*
----------------------------------------------------------
Add these names to the ProfileKeyPrefix or ParamsKeyPrefix
---------------------------------------------------------- */
static const TCHAR SkipCountName[]    = TEXT( "SkipCount"    );
static const TCHAR AcquireCountName[] = TEXT( "AcquireCount" );
static const TCHAR SkipSecondsName[]  = TEXT( "SkipSeconds"  );
static const TCHAR FlagsName[]        = TEXT( "Flags"        );

const TCHAR TabString[]             = TEXT( "\t" );
const TCHAR SpaceString[]           = TEXT( " " );
const TCHAR ColonSpaceString[]      = TEXT( ": " );

static const TCHAR BackslashChar    = TEXT( '\\' );
static const TCHAR ColonChar        = TEXT( ':' );
static const TCHAR CommaChar        = TEXT( ',' );
static const TCHAR DashChar         = TEXT( '-' );
static const TCHAR ForwardSlashChar = TEXT( '/' );
static const TCHAR LeftCurlyBracket = TEXT( '{' );
static const TCHAR NChar            = TEXT( 'N' );
static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR RightCurlyBracket= TEXT( '}' );
static const TCHAR SpaceChar        = TEXT( ' ' );
static const TCHAR TabChar          = TEXT( '\t' );
static const TCHAR UnderscoreChar   = TEXT( '_' );
static const TCHAR YChar            = TEXT( 'Y' );
static const TCHAR ZeroChar         = TEXT( '0' );

static const int StripPathNames = 1;

static STRING_CLASS Ls;

/*
------------------------------------------------------------------------
This is only used to show an error message if I can't save the part data
------------------------------------------------------------------------ */
static TCHAR * PartSaveBuf = 0;

int32 MaxZipCount = 0;
void zip_callback( const char * name, int object_ratio, int job_ratio );

/***********************************************************************
*                                  log                                 *
*    This writes whatever you put in the global Ls STRING_CLASS.       *
***********************************************************************/
static void log()
{
FILE_CLASS   f;
TIME_CLASS   t;
STRING_CLASS s;

if ( !UsingLogFile )
    return;

t.get_local_time();
s = t.text();
s += SpaceString;
s += Ls;

if ( f.open_for_append( LogFilePath.text() ) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           RESOURCE_MESSAGE_BOX                       *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                           SAVE_TO_LOG_FILE                           *
***********************************************************************/
static void save_to_log_file( HWND w )
{
STRING_CLASS filename;
FILE_CLASS f;

filename.get_text( w, LOG_FILE_EBOX );

if ( f.open_for_write(filename.text()) )
    {
    f.writeline( PartSaveBuf );
    f.writeline( BackupDirectory );
    if ( IsWindowVisible(GetDlgItem(w,NO_PKZIP_TBOX)) )
        f.writeline( TEXT("pkzip not found") );
    f.close();
    }
}

/***********************************************************************
*                      ADD_PART_NAME_ENTRY_TO_LS                       *
***********************************************************************/
void add_part_name_entry_to_ls( PART_NAME_ENTRY & sorc )
{
Ls += " ";
Ls += sorc.computer;
Ls += ", ";
Ls += sorc.machine;
Ls += ", ";
Ls += sorc.part;
}

/***********************************************************************
*                           INIT_PART_SAVE_DIALOG                      *
***********************************************************************/
static void init_part_save_dialog( HWND w )
{
static TCHAR defaultname[] = TEXT( "LOGFILE.TXT" );
STRING_CLASS s;

set_text( w, PSERROR_CMD_TBOX, PartSaveBuf );
set_text( w, PSERROR_BACKUP_PATH_TBOX, BackupDirectory );

s = exe_directory();
s.cat_path( TEXT("pkzip25.exe") );
if ( !file_exists(s.text()) )
   ShowWindow( GetDlgItem(w,NO_PKZIP_TBOX), SW_SHOW );

s = exe_directory();
s.cat_path( defaultname );
s.set_text( w, LOG_FILE_EBOX );
}

/***********************************************************************
*                       PART_SAVE_ERROR_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK part_save_error_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        init_part_save_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_TO_LOG_PB:
                save_to_log_file( hWnd );
                return TRUE;

            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                           REPLACE_ALL_CHARS                          *
***********************************************************************/
static void replace_all_chars( TCHAR * dest, TCHAR oldchar, TCHAR newchar )
{
TCHAR * cp;

if ( !dest )
    return;

cp = dest;
while ( *cp )
    {
    if ( *cp == oldchar )
        *cp = newchar;
    cp++;
    }
}

/***********************************************************************
*                        MACHINE_DATA_ZIP_FILE_NAME                    *
*                           M01_12_31_20_45.MAZ                        *
***********************************************************************/
static TCHAR * machine_data_zip_file_name( STRING_CLASS & sorc, PART_NAME_ENTRY & p )
{
static TCHAR zipfile[MAX_PATH+1];

COMPUTER_CLASS c;
TCHAR        * cp;
STRING_CLASS   s;
DB_TABLE       t;
TIME_CLASS     tc;
int32          n;
BOOLEAN        have_time;

if ( findchar( LeftCurlyBracket, sorc.text()) )
    {
    have_time = FALSE;
    s = downtime_dbname( c.whoami(), p.machine );

    if ( file_exists(s.text()) )
        {
        if ( t.open(s.text(), DOWNTIME_RECLEN, PFL) )
            {
            n = t.nof_recs();
            if ( n > 0 )
                {
                n--;
                if ( t.goto_record_number(n) )
                    if ( t.get_current_record(NO_LOCK) )
                        if ( get_time_class_from_db(tc, t, DOWNTIME_DATE_OFFSET) )
                            have_time = TRUE;
                }
            t.close();
            }
        }

    if ( !have_time )
        tc.get_local_time();
    cp = copystring( zipfile, dynamic_file_name(tc, p, sorc.text()) );
    }
else
    {
    cp = copystring( zipfile, sorc.text() );
    }

copystring( cp, MachineZipSuffix );
return zipfile;
}

/***********************************************************************
*                         SHOT_DATA_ZIP_FILE_NAME                      *
*                         M01_VISITRAK_1_1000.PAZ                      *
***********************************************************************/
static TCHAR * shot_data_zip_file_name( STRING_CLASS & sorc, PART_NAME_ENTRY & p )
{
static TCHAR zipfile[MAX_PATH+1];

COMPUTER_CLASS c;
TCHAR      * cp;
DB_TABLE     t;
int32        n;
SHOT_CLASS   firstshot;
SHOT_CLASS   lastshot;
STRING_CLASS s;
TIME_CLASS   tc;
BOOLEAN      have_time;

have_time = FALSE;

s = graphlst_dbname( c.whoami(), p.machine, p.part );
if ( file_exists(s.text()) )
    {
    if ( t.open(s.text(), GRAPHLST_RECLEN, PFL) )
        {
        if ( t.get_next_record(NO_LOCK) )
            {
            firstshot.get_from_graphlst( t );
            lastshot = firstshot;
            n = t.nof_recs() - 1;
            if ( n > 0 )
                {
                if ( t.goto_record_number(n) )
                    if ( t.get_current_record(NO_LOCK) )
                        {
                        lastshot.get_from_graphlst( t );
                        if ( get_time_class_from_db(tc, t, GRAPHLST_DATE_OFFSET) )
                            have_time = TRUE;
                        }
                }
            }
        t.close();
        }
    }

s = sorc;

/*
------------------------------------------------------------------------------
I need to replace the shot tokens with the strings, as the dynamic_file_name
routine doesn't know how to do this. {nb} is the first shot, {ne} is the last.
------------------------------------------------------------------------------ */
s.replace( FirstShotToken, int32toasc(firstshot.number) );
s.replace( LastShotToken,  int32toasc(lastshot.number)  );

/*
-------------------------------------------------------------
If there are any curly brackets left then there is more to do
------------------------------------------------------------- */
if ( findchar( LeftCurlyBracket, s.text()) )
    {
    if ( !have_time )
        tc.get_local_time();
    s = dynamic_file_name(tc, p, s.text() );
    }

cp = copystring( zipfile, s.text() );
copystring( cp, PartZipSuffix );

return zipfile;
}

/***********************************************************************
*                         CHECK_FOR_LOCAL_PART                         *
***********************************************************************/
static BOOLEAN check_for_local_part( PART_NAME_ENTRY & sorc )
{
COMPUTER_CLASS c;
BOOLEAN        status;
MACHINE_CLASS  m;

status = machine_exists( c.whoami(), sorc.machine );

if ( !status )
    {
    if ( m.find(sorc.computer, sorc.machine) )
        {
        lstrcpy( m.computer, c.whoami() );
        status = m.save();
        }
    }

if ( status )
    {
    status = part_exists( c.whoami(), sorc.machine, sorc.part );
    if ( !status )
        status = copypart( c.whoami(), sorc.machine, sorc.part, sorc.computer, sorc.machine, sorc.part, COPY_PART_ALL );
    }

return status;
}

/***********************************************************************
*                             DS_SAVE_COUNTS                           *
*                              NEED_TO_SAVE                            *
***********************************************************************/
BOOLEAN DS_SAVE_COUNTS::need_to_save( SHOT_CLASS sorc )
{
TIME_CLASS t;

if ( flags & ACQUIRE_ALL_BIT )
    return TRUE;

if ( flags & ACQUIRE_NONE_BIT )
    return FALSE;

if ( acquiring )
    return TRUE;

if ( flags & SKIP_COUNT_BIT )
    {
    if ( sorc.number > (last_saved_shot.number + skip_count) )
        {
        number_acquired = 0;
        acquiring = TRUE;
        return TRUE;
        }
    }
else if ( flags & SKIP_TIME_BIT )
    {
    t = last_saved_shot.time;
    t += skip_seconds;
    if ( sorc.time > t )
        {
        number_acquired = 0;
        acquiring = TRUE;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             DS_SAVE_COUNTS                           *
*                                 TEXT                                 *
***********************************************************************/
TCHAR * DS_SAVE_COUNTS::text( void )
{

const int32 MAX_LABEL_LEN = 20;
const int32 SLEN = MAX_INTEGER_LEN + (2*MAX_LABEL_LEN) + 4;
static TCHAR buf[SLEN+1];
TCHAR * cp;
int32   n;

/*
-----------
Skip string
----------- */
n = 0;
if ( flags & SKIP_COUNT_BIT )
    n = skip_count;
else if ( flags & SKIP_TIME_BIT )
    n = skip_seconds;

cp = copystring( buf, int32toasc(n) );

n = 0;
if ( flags & SKIP_COUNT_BIT )
    n = SHOTS_LABEL_STRING;
else if ( flags & SKIP_TIME_BIT )
    n = SECONDS_ABREVIATION_STRING;
if ( n )
    {
    *cp = SpaceChar;
    cp++;
    cp = copystring( cp, resource_string(UINT(n)) );
    }

*cp = TabChar;
cp++;

if ( flags & ACQUIRE_ALL_BIT )
    cp = copystring( cp, resource_string(ALL_STRING) );
else
    {
    if ( flags & ACQUIRE_NONE_BIT )
        n = 0;
    else
        n = acquire_count;

    cp = copystring( cp, int32toasc(n) );
    }

*cp = SpaceChar;
cp++;
copystring( cp, resource_string(SHOTS_LABEL_STRING) );

return buf;
}

/***********************************************************************
*                             DS_SAVE_COUNTS                           *
*                                 CLEAR                                *
***********************************************************************/
void DS_SAVE_COUNTS::clear( void )
{
last_saved_shot.clear();
number_acquired = 0;
acquiring       = FALSE;
skip_count      = 0;
acquire_count   = 1;
skip_seconds    = 0;
flags           = ACQUIRE_ALL_BIT;
}

/***********************************************************************
*                             DS_SAVE_COUNTS                           *
*                                   =                                  *
***********************************************************************/
void DS_SAVE_COUNTS::operator=( const DS_SAVE_COUNTS & sorc )
{
last_saved_shot = sorc.last_saved_shot;
number_acquired = sorc.number_acquired;
acquiring       = sorc.acquiring;
skip_count      = sorc.skip_count;
acquire_count   = sorc.acquire_count;
skip_seconds    = sorc.skip_seconds;
flags           = sorc.flags;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
***********************************************************************/
DS_BACKUP_ENTRY::DS_BACKUP_ENTRY()
{
zip_count                 = 0;
last_examined_shot_number = NO_SHOT_NUMBER;

profile.clear();
params.clear();
}

/***********************************************************************
*                            ~DS_BACKUP_ENTRY                          *
***********************************************************************/
DS_BACKUP_ENTRY::~DS_BACKUP_ENTRY()
{
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             START_NEW_PART                           *
***********************************************************************/
void DS_BACKUP_ENTRY::start_new_part( TCHAR * new_part_name )
{
int32        n;
STRING_CLASS s;
DB_TABLE     t;

name.set( new_part_name );
s = graphlst_dbname( name.computer, name.machine, name.part );

Ls = "Start_new_part, ";
Ls += s;
Ls += " ";

zip_count = 0;
profile.clear();
params.clear();
last_examined_shot_number = NO_SHOT_NUMBER;

if ( !file_exists(s.text()) )
    {
    Ls += "Not Found";
    log();
    return;
    }

Ls += "OK";
log();

/*
-----------------------------------------------
I want to start saving at the next to last shot
----------------------------------------------- */
if ( t.open( s.text(), GRAPHLST_RECLEN, PFL) )
    {
    n = t.nof_recs();
    if ( n > 10 )
        {
        n -= 2;
        if ( t.goto_record_number(n) )
            {
            if ( t.get_current_record(NO_LOCK) )
                {
                params.last_saved_shot.get_from_graphlst( t );
                profile.last_saved_shot = params.last_saved_shot;
                last_examined_shot_number = params.last_saved_shot.number;
                }
            }
        }

    t.close();
    }
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                   =                                  *
***********************************************************************/
void DS_BACKUP_ENTRY::operator=( const DS_BACKUP_ENTRY & sorc )
{
name                      = sorc.name;
part_zip_file_path        = sorc.part_zip_file_path;
machine_zip_file_path     = sorc.machine_zip_file_path;
last_downtime             = sorc.last_downtime;
last_examined_shot_number = sorc.last_examined_shot_number;
zip_count                 = sorc.zip_count;
profile                   = sorc.profile;
params                    = sorc.params;
}

/***********************************************************************
*                              READ_STRING                             *
***********************************************************************/
static BOOLEAN read_string( TCHAR * dest, TCHAR * section, TCHAR * key )
{
TCHAR * s;
s = get_ini_string( IniFileName, section, key );

if ( unknown(s) )
    return FALSE;

lstrcpy( dest, s );
return TRUE;
}

/***********************************************************************
*                                READ_TIME                             *
***********************************************************************/
static BOOLEAN read_time( TIME_CLASS & dest, TCHAR * section, TCHAR * key )
{
TCHAR * s;

s = get_ini_string( IniFileName, section, key );
if ( unknown(s) )
    return FALSE;

dest = s;
return TRUE;
}

/***********************************************************************
*                              READ_INT32                              *
***********************************************************************/
static int32 read_int32( TCHAR * section, TCHAR * key )
{
int32   i;
TCHAR * s;

i = 0;
s = get_ini_string( IniFileName, section, key );
if ( !unknown(s) )
    i = asctoint32( s );

return i;
}

/***********************************************************************
*                              READ_BOOLEAN                            *
***********************************************************************/
static BOOLEAN read_boolean( TCHAR * section, TCHAR * key )
{
TCHAR * s;

s = get_ini_string( IniFileName, section, key );
if ( *s == YChar )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              READ_BITSET                             *
***********************************************************************/
static BITSETYPE read_bitset( TCHAR * section, TCHAR * key )
{
BITSETYPE b;
TCHAR   * s;

b = 0;
s = get_ini_string( IniFileName, section, key );
if ( !unknown(s) )
    b = (BITSETYPE) hextoul( s );

return b;
}

/***********************************************************************
*                            WRITE_BOOLEAN                             *
***********************************************************************/
static void write_boolean( TCHAR * section, TCHAR * key, BOOLEAN sorc )
{
static TCHAR s[2] = TEXT( "Y" );
if ( sorc )
    *s = YChar;
else
    *s = NChar;
put_ini_string( IniFileName, section, key, s );
}
/***********************************************************************
*                             WRITE_INT32                              *
***********************************************************************/
static void write_int32( TCHAR * section, TCHAR * key, int32 sorc )
{
put_ini_string( IniFileName, section, key, int32toasc(sorc) );
}

/***********************************************************************
*                             WRITE_BITSET                             *
***********************************************************************/
static void write_bitset( TCHAR * section, TCHAR * key, BITSETYPE sorc )
{
put_ini_string( IniFileName, section, key, ultoascii((unsigned long) sorc) );
}

/***********************************************************************
*                           WRITE_SAVE_COUNTS                          *
***********************************************************************/
static void write_save_counts( TCHAR * section, const TCHAR * key_prefix, DS_SAVE_COUNTS & sorc )
{
TCHAR   key[MAX_KEY_NAME_LEN+1];
TCHAR * cp;

lstrcpy( key, key_prefix );
cp = key + lstrlen(key);

lstrcpy( cp, SkipCountName );
write_int32( section, key, sorc.skip_count );

lstrcpy( cp, AcquireCountName );
write_int32( section, key, sorc.acquire_count );

lstrcpy( cp, SkipSecondsName );
write_int32( section, key, sorc.skip_seconds );

lstrcpy( cp, FlagsName );
write_bitset( section, key, sorc.flags );
}

/***********************************************************************
*                            READ_SAVE_COUNTS                          *
***********************************************************************/
static void read_save_counts( DS_SAVE_COUNTS & dest, TCHAR * section, const TCHAR * key_prefix )
{
TCHAR   key[MAX_KEY_NAME_LEN+1];
TCHAR * cp;

lstrcpy( key, key_prefix );
cp = key + lstrlen(key);

lstrcpy( cp, SkipCountName );
dest.skip_count = read_int32( section, key );

lstrcpy( cp, AcquireCountName );
dest.acquire_count = read_int32( section, key );
maxint32( dest.acquire_count, 1 );

lstrcpy( cp, SkipSecondsName );
dest.skip_seconds = read_int32( section, key );

lstrcpy( cp, FlagsName );
dest.flags = read_bitset( section, key );
}

/***********************************************************************
*                          GET_ZIP_FILE_PATH                           *
* The section is either the machine name or "Default".                 *
* the path type is PART_PATH (0) or MACHINE_PATH (1).                  *
***********************************************************************/
void get_zip_file_path( STRING_CLASS & dest, const TCHAR * section, int path_type )
{
const TCHAR * key;
const TCHAR * default_file_name;

if ( path_type == PART_PATH )
    {
    key = BackupPartPathString;
    default_file_name = DefaultPartFileName;
    }
else
    {
    key = BackupMachinePathString;
    default_file_name = DefaultMachineFileName;
    }

dest = get_long_ini_string( IniFileName, section, key );

if ( unknown(dest.text()) )
    {
    /*
    -------------------------------------------------------------
    See if this is an old file that hasn't been converted before,
    in which case there will be a backup directory string.
    ------------------------------------------------------------- */
    dest = get_long_ini_string( IniFileName, section, BackupDirString );
    if ( !unknown(dest.text()) )
        {
        /*
        ---------------------------------------------------------
        This is the first time this part has been loaded. Replace
        the BackupDirString entry with two new equivalent entries
        --------------------------------------------------------- */
        dest.cat_path(DefaultPartFileName);
        put_ini_string( IniFileName, section, BackupPartPathString, dest.text() );

        dest = get_long_ini_string( IniFileName, section, BackupDirString );
        dest.cat_path(DefaultMachineFileName);
        put_ini_string( IniFileName, section, BackupMachinePathString, dest.text() );

        dest = get_long_ini_string( IniFileName, section, BackupDirString );
        dest.cat_path( default_file_name );
        /*
        ---------------------------------------------------------
        Delete the entry for the BackupDirString for this machine
        --------------------------------------------------------- */
        put_ini_string( IniFileName, section, BackupDirString, 0 );
        }
    else
        {
        /*
        --------------------
        Get the default name
        -------------------- */
        dest = get_ini_string( IniFileName, DefaultSection, key );
        if ( unknown(dest.text()) )
            {
            /*
            -----------------------
            Get the old default dir
            ----------------------- */
            dest = get_ini_string( IniFileName, DefaultSection, BackupDirString );
            if ( unknown(dest.text()) )
                dest = root_directory();

            /*
            ------------------------------------------------------------------------------
            If I've gotten here then there are no default strings and I need to make some.
            ------------------------------------------------------------------------------ */
            STRING_CLASS s;
            s = dest;
            s.cat_path( DefaultPartFileName );
            put_ini_string( IniFileName, DefaultSection, BackupPartPathString, s.text() );

            s = dest;
            s.cat_path( DefaultMachineFileName );
            put_ini_string( IniFileName, DefaultSection, BackupMachinePathString, s.text() );

            /*
            --------------------------------------
            Delete the old backup directory string
            -------------------------------------- */
            put_ini_string( IniFileName, DefaultSection, BackupDirString, 0 );

            dest.cat_path( default_file_name );
            }
        }
    }

}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             READ_ZIP_PATHS                           *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read_zip_paths()
{
get_zip_file_path( part_zip_file_path,    name.machine, PART_PATH    );
get_zip_file_path( machine_zip_file_path, name.machine, MACHINE_PATH );
return TRUE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             READ_SAVE_CONFIG                         *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read_save_config( void )
{
read_zip_paths();
read_save_counts( profile, name.machine, ProfileKeyPrefix );
read_save_counts( params,  name.machine, ParamsKeyPrefix  );
return TRUE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                 READ                                 *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read( void )
{
if ( !read_string(name.part, name.machine, CurrentPartName) )
    {
    /*
    ---------------------------------------------
    Just fill the zip file names with the default
    --------------------------------------------- */
    get_zip_file_path( part_zip_file_path,    DefaultSection, PART_PATH    );
    get_zip_file_path( machine_zip_file_path, DefaultSection, MACHINE_PATH );
    return FALSE;
    }

read_time( last_downtime, name.machine, LastDownTime );
last_examined_shot_number = read_int32( name.machine, LastExaminedShotNumber );

profile.last_saved_shot.number = read_int32( name.machine, LastSavedProfileNumber );
read_time(  profile.last_saved_shot.time,   name.machine, LastSavedProfileTime   );
profile.number_acquired = read_int32( name.machine, NumberProfilesAcquired );
profile.acquiring       = read_boolean( name.machine, AcquiringProfiles );

params.last_saved_shot.number  = read_int32( name.machine, LastSavedParamsNumber );
read_time(  params.last_saved_shot.time,   name.machine, LastSavedParamsTime   );
params.number_acquired = read_int32( name.machine, NumberParamsAcquired );
params.acquiring       = read_boolean( name.machine, AcquiringParams );

zip_count = read_int32( name.machine, ZipCount );

read_save_config();

return TRUE;
}
/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             WRITE_ZIP_PATHS                          *
***********************************************************************/
void DS_BACKUP_ENTRY::write_zip_paths()
{
put_ini_string( IniFileName, name.machine, BackupPartPathString,    part_zip_file_path.text()    );
put_ini_string( IniFileName, name.machine, BackupMachinePathString, machine_zip_file_path.text() );
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                 WRITE                                *
***********************************************************************/
void DS_BACKUP_ENTRY::write( void )
{
put_ini_string( IniFileName, name.machine, CurrentPartName,     name.part            );
put_ini_string( IniFileName, name.machine, LastDownTime,        last_downtime.text() );

write_zip_paths();

put_ini_string( IniFileName, name.machine, LastExaminedShotNumber, int32toasc(last_examined_shot_number) );

put_ini_string( IniFileName, name.machine, LastSavedParamsTime,   params.last_saved_shot.time.text() );
put_ini_string( IniFileName, name.machine, LastSavedParamsNumber, int32toasc(params.last_saved_shot.number) );
write_int32( name.machine, NumberParamsAcquired, params.number_acquired );
write_boolean( name.machine, AcquiringParams, params.acquiring );

put_ini_string( IniFileName, name.machine, LastSavedProfileTime,   profile.last_saved_shot.time.text() );
put_ini_string( IniFileName, name.machine, LastSavedProfileNumber, int32toasc(profile.last_saved_shot.number) );
write_int32( name.machine, NumberProfilesAcquired, profile.number_acquired );
write_boolean( name.machine, AcquiringProfiles, profile.acquiring );

put_ini_string( IniFileName, name.machine, ZipCount,            int32toasc(zip_count) );

write_save_counts( name.machine, ProfileKeyPrefix, profile );
write_save_counts( name.machine, ParamsKeyPrefix,  params  );
}

/***********************************************************************
*                        EMPTY_BACKUP_DIRECTORY                        *
***********************************************************************/
void empty_backup_directory( void )
{
if ( !directory_exists(BackupDirectory) )
    {
    if ( !CreateDirectory( BackupDirectory, NULL ) )
        {
        resource_message_box( DIR_CREATE_FAIL, CANT_DO_THAT_STRING );
        return;
        }
    }
else
    {
    empty_directory( BackupDirectory );
    }
}

/***********************************************************************
*                     COPY_SETUP_FILES_FOR_ONE_PART                    *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::copy_setup_files_for_one_part( void )
{
TCHAR          d[MAX_PATH+1];
TCHAR          s[MAX_PATH+1];
DB_TABLE       sorc;
DB_TABLE       dest;
BOOLEAN        is_remote;
COMPUTER_CLASS c;

Ls = "Copy_setup_files_for_one_part, computer= ";
add_part_name_entry_to_ls(name);
log();

/*
------------------
Create machset.txt
------------------ */
lstrcpy( d, BackupDirectory );
append_filename_to_path( d, MACHSET_DB );
dest.create( d );

/*
---------------------------
Copy one record from remote
--------------------------- */
copystring( s, machset_dbname(name.computer) );
is_remote = TRUE;
if ( !file_exists(s) )
    {
    copystring( s, machset_dbname(c.whoami()) );
    is_remote = FALSE;
    }

sorc.open( s, MACHSET_RECLEN, PFL );
sorc.put_alpha( MACHSET_MACHINE_NAME_OFFSET, name.machine, MACHINE_NAME_LEN );
if ( sorc.get_next_key_match(1, NO_LOCK) )
    {
    /*
    ------------------------
    Copy to backup directory
    ------------------------ */
    dest.open( d, MACHSET_RECLEN, WL );
    dest.copy_rec( sorc );
    dest.rec_append();
    dest.close();
    if ( is_remote )
        {
        /*
        ---------------------------------------
        Update the machine setup in my database
        --------------------------------------- */
        copystring( d, machset_dbname(c.whoami()) );
        dest.open( d, MACHSET_RECLEN, WL );
        dest.copy_rec( sorc );
        if ( dest.get_next_key_match(1, NO_LOCK) )
            {
            dest.copy_rec( sorc );
            dest.rec_update();
            }
        dest.close();
        }
    }
sorc.close();

/*
----------------
Create parts.txt
---------------- */
copystring( d, BackupDirectory );
append_filename_to_path( d, PARTS_DB );
dest.create( d );

/*
---------------------------
Copy one record from remote
--------------------------- */
copystring( s, parts_dbname(name.computer, name.machine) );
is_remote = TRUE;
if ( !file_exists(s) )
    {
    copystring( s, parts_dbname(c.whoami(), name.machine) );
    is_remote = FALSE;
    }

sorc.open( s, PARTS_RECLEN, PFL );
sorc.put_alpha( PARTS_PART_NAME_OFFSET, name.part, PART_NAME_LEN );
if ( sorc.get_next_key_match(1, NO_LOCK) )
    {
    /*
    ------------------------
    Copy to backup directory
    ------------------------ */
    dest.open( d, PARTS_RECLEN, WL );
    dest.copy_rec( sorc );
    dest.rec_append();
    dest.close();
    if ( is_remote )
        {
        /*
        ------------------------------------
        Update the part setup in my database
        ------------------------------------ */
        copystring( d, parts_dbname(c.whoami(), name.machine) );
        dest.open( d, PARTS_RECLEN, WL );
        dest.copy_rec( sorc );
        if ( dest.get_next_key_match(1, NO_LOCK) )
            {
            dest.copy_rec( sorc );
            dest.rec_update();
            }
        dest.close();
        }
    }
sorc.close();

/*
----------------------------------
Copy all files from part directory
---------------------------------- */
if ( is_remote )
    {
    /*
    --------------------------------------------
    Try to copy the remote files to my directory
    -------------------------------------------- */
    copystring( s, part_directory_name(name.computer, name.machine, name.part) );
    copystring( d, part_directory_name(c.whoami(), name.machine, name.part) );
    copy_all_files( d, s, StarDotStar, 0, 0 );
    }

copystring( s, part_directory_name(c.whoami(), name.machine, name.part) );
copy_all_files( BackupDirectory, s, StarDotStar, 0, 0 );

/*
--------------------------------------
Copy master trace to results directory
-------------------------------------- */
copystring( s, master_name(name.computer, name.machine, name.part, CURRENT_MASTER_TRACE_NAME) );
copystring( d, master_name(c.whoami(), name.machine, name.part, CURRENT_MASTER_TRACE_NAME) );

if ( file_exists(s) )
    CopyFile( s, d, FALSE );

return TRUE;
}

/***********************************************************************
*                        CHANGE_TO_MIRROR_PATH                         *
* A Mirror path assumes that I an going to replace my root path        *
* like C: or \\backup-computer with E: or \\new-backup-computer, etc.  *
* I will replace ether the X: or \\xxxxxx with the new string. The     *
* MirrorRoot should be X: or \\xxxxxxxx without a trailing backslash.  *
***********************************************************************/
static BOOLEAN change_to_mirror_path( STRING_CLASS & dest, TCHAR * sorc )
{
TCHAR * cp;
int     slen;

if ( !HaveMirror )
    return FALSE;

slen = lstrlen( sorc );
if ( slen > 2 )
    {
    slen += MirrorRoot.len();
    dest.upsize( slen );

    dest = MirrorRoot;
    cp = sorc + 2;
    cp = findchar( BackslashChar, cp );
    if ( cp )
        {
        dest += cp;
        Ls = "change from ";
        Ls += sorc;
        Ls += " to ";
        Ls += dest;
        log();
        return TRUE;
        }
    }

Ls = "change_to_mirror_path: bad path = ";
Ls += sorc;
log();
return FALSE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                              ZIP_SHOT_DATA                           *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_shot_data( void )
{
const BUF_LEN = 3 * MAX_PATH;
TCHAR          buf[BUF_LEN+1];
COMPUTER_CLASS c;
DB_TABLE       sorc;
BOOLEAN        status;
TCHAR        * dirp;
STRING_CLASS   filename;
STRING_CLASS   s;

if ( name.is_part(NO_PART) )
    return;

if ( part_zip_file_path.isempty() )
    return;

s = graphlst_dbname( c.whoami(), name.machine, name.part );
lstrcpy( buf, s.text() );
if ( file_size(buf) < 1 )
    return;

empty_backup_directory();
copy_setup_files_for_one_part();

/*
-------------
Zip all files
------------- */
get_exe_directory( buf );
lstrcat( buf, TEXT("pkzip25 -add -noz -dir=none ") );

if ( Using_Machine_Based_Zip_Paths )
    filename = shot_data_zip_file_name( part_zip_file_path, name );
else
    filename = shot_data_zip_file_name( DefaultPartZipPath, name );

Ls = "PaZip ";
Ls += filename;
log();

/*
-----------------------
Create dir if necessary
----------------------- */
dirp = maketext( filename.text() );
if ( dir_from_filename(dirp) )
    {
    create_directory(dirp);
    if ( change_to_mirror_path(s, dirp) )
        create_directory( s.text() );
    }
delete[] dirp;

lstrcat( buf, filename.text() );
lstrcat( buf, SpaceString );

lstrcat( buf, BackupDirectory );
append_filename_to_path( buf, StarDotStar );

lstrcat( buf, SpaceString );

lstrcat( buf, rsdir_name(c.whoami(), name.machine, name.part, StarDotStar) );

SetCurrentDirectory( BackupDirectory );

status = execute_and_wait( buf, BackupDirectory, ThirtyMinutesInMs );
if ( status )
    {
    if ( change_to_mirror_path(s, filename.text()) )
        CopyFile( filename.text(), s.text(), FALSE );
    }
else
    {
    PartSaveBuf = buf;
    DialogBox(
        MainInstance,
        "PART_SAVE_ERROR_DIALOG",
        MainDialogWindow,
        (DLGPROC) part_save_error_dialog_proc );
    PartSaveBuf = 0;
    }

/*
----------------------
Delete/empty all files
---------------------- */
empty_directory( BackupDirectory );
s = result_directory_name( c.whoami(), name.machine, name.part );
empty_directory( s.text() );

s = alarmsum_dbname( c.whoami(), name.machine, name.part );
sorc.create( s.text() );
s = graphlst_dbname( c.whoami(), name.machine, name.part );
sorc.create( s.text() );
s = shotparm_dbname( c.whoami(), name.machine, name.part );
sorc.create( s.text() );

/*
--------------------------------------------
Zero the count of the number of files zipped
-------------------------------------------- */
zip_count = 0;
}

/***********************************************************************
*                            DS_BACKUP_ENTRY                           *
*                            ZIP_MACHINE_DATA                          *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_machine_data( void )
{
const BUF_LEN = 3 * MAX_PATH;
TCHAR          buf[BUF_LEN+1];
DB_TABLE       t;
COMPUTER_CLASS c;
BOOLEAN        status;
TCHAR        * dirp;
STRING_CLASS   filename;
STRING_CLASS   s;

if ( SkipDowntimeZip )
    return;

lstrcpy( buf, downtime_dbname(c.whoami(), name.machine) );
if ( !file_exists(buf) )
    return;

if ( file_size(buf) < 1 )
    return;

empty_backup_directory();
copy_setup_files_for_one_part();

get_exe_directory( buf );
lstrcat( buf, TEXT("pkzip25 -add -noz -dir=none ") );

if ( Using_Machine_Based_Zip_Paths )
    filename = machine_data_zip_file_name( machine_zip_file_path, name );
else
    filename = machine_data_zip_file_name( DefaultMachineZipPath, name );

Ls = "MaZip ";
Ls += filename;
log();

/*
-----------------------
Create dir if necessary
----------------------- */
dirp = maketext( filename.text() );
if ( dir_from_filename(dirp) )
    {
    create_directory( dirp );
    if ( change_to_mirror_path(s, dirp) )
        create_directory( s.text() );
    }
delete[] dirp;

lstrcat( buf, filename.text() );

lstrcat( buf, SpaceString );
lstrcat( buf, BackupDirectory );
append_filename_to_path( buf, StarDotStar );

lstrcat( buf, SpaceString );
lstrcat( buf, downtime_dbname(c.whoami(), name.machine) );

SetCurrentDirectory( BackupDirectory );

status = execute_and_wait( buf, BackupDirectory, ThirtyMinutesInMs );
if ( status )
    {
    if ( change_to_mirror_path(s, filename.text()) )
        CopyFile( filename.text(), s.text(), FALSE );
    }
else
    {
    resource_message_box( MainInstance, CANT_DO_THAT_STRING, MAZ_ZIP_ERROR_STRING );
    }

/*
------------------------
Empty the downtime table
------------------------ */
lstrcpy( buf, downtime_dbname(c.whoami(), name.machine) );
t.create( buf );
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                               ZIP_ALL_DATA                           *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_all_data( void )
{
LISTBOX_CLASS lb;

if ( name.is_part(NO_PART) )
    return;

zip_shot_data();
zip_machine_data();

lb.init( MainDialogWindow, ZIP_COUNT_LISTBOX );
lb.empty();
}

/***********************************************************************
*                        SAVE_NETWORK_SHOTPARM                         *
***********************************************************************/
void save_network_shotparm( PART_NAME_ENTRY & pn, DB_TABLE & db )
{
TCHAR   buf[2048];
TCHAR   realbuf[SHOTPARM_FLOAT_LEN+1];
TCHAR * pattern;
TCHAR * p;
TCHAR * dirp;
TCHAR * dp;
TCHAR * filename;
STRING_CLASS s;
TIME_CLASS   t;
FILE_CLASS   f;
FIELDOFFSET  offset;
PARAMETER_CLASS param;
int          i;

s = exe_directory();
s += IniFileName;

GetPrivateProfileString( ShotparmSection, RecordFormatKey, unknown_string(), buf, 512, s.text() );
if ( unknown(buf) )
    return;

pattern = maketext( buf );

/*
------------------
Make the file name
------------------ */
GetPrivateProfileString( ShotparmSection, FileNameKey, unknown_string(), buf, 512, s.text() );
if ( unknown(buf) )
    {
    delete[] pattern;
    pattern = 0;
    return;
    }

/*
----------------------------------------------
Extract the shot time from the shotparm record
---------------------------------------------- */
get_time_class_from_db( t, db, SHOTPARM_DATE_OFFSET );

if ( findchar( LeftCurlyBracket, buf)  )
    copystring( buf, dynamic_file_name(t, pn, buf) );

filename = maketext( buf );

/*
------------------------
Construct the new record
------------------------ */
p = pattern;
dp = buf;
while ( replace_char_with_null(p, LeftCurlyBracket) )
    {
    dp = copystring( dp, p );
    p = nextstring( p );
    if ( replace_char_with_null(p, RightCurlyBracket) )
        {
        if (strings_are_equal(p, TEXT("dd"), 2) )
            {
            dp = copystring( dp, t.dd() );
            }
        else if (strings_are_equal(p, TEXT("mmm"), 3) )
            {
            dp = copystring( dp, t.mmm() );
            }
        else if (strings_are_equal(p, TEXT("mm"), 2) )
            {
            dp = copystring( dp, t.mm() );
            }
        else if (strings_are_equal(p, TEXT("yyyy"), 4) )
            {
            dp = copystring( dp, t.yyyy() );
            }
        else if (strings_are_equal(p, TEXT("yy"), 2) )
            {
            dp = copystring( dp, t.yy() );
            }
        else if (strings_are_equal(p, TEXT("hh"), 2) || strings_are_equal(p, TEXT("mi"), 2) || strings_are_equal(p, TEXT("ss"), 2) )
            {
            TCHAR * hhmmss( void );
            if (strings_are_equal(p, TEXT("hh"), 2) )
                i = 0;
            else if ( strings_are_equal(p, TEXT("mi"), 2) )
                i = 3;
            else if ( strings_are_equal(p, TEXT("ss"), 2) )
                i = 6;
            copychars( dp, t.hhmmss()+i, 2 );
            dp += 2;
            *dp = NullChar;
            }
        else if (strings_are_equal(p, TEXT("c"), 1) )
            {
            dp = copystring( dp, pn.computer );
            }
        else if (strings_are_equal(p, TEXT("g"), 1) )
            {
            if ( param.find(pn.computer, pn.machine, pn.part) )
                {
                i = param.shot_goodness(db);
                if ( i == ALARM_SHOT )
                    dp = copystring( dp, HaveAlarmShot );
                else if ( i == WARNING_SHOT )
                    dp = copystring( dp, HaveWarningShot );
                else
                    dp = copystring( dp, HaveGoodShot );
                }
            }
        else if (strings_are_equal(p, TEXT("m"), 1) )
            {
            dp = copystring( dp, pn.machine );
            }
        else if (strings_are_equal(p, TEXT("p"), 1) )
            {
            dp = copystring( dp, pn.part );
            }
        else if (strings_are_equal(p, TEXT("n"), 1) )
            {
            i = db.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
            dp = copystring( dp, int32toasc((int32)i) );
            }
        else if ( is_numeric(*p) )
            {
            i = asctoint32( p );
            /*
            ----------------------------------------
            Parameter numbers are for humans, [1,60]
            so I need to subtract 1 for the index.
            ---------------------------------------- */
            if ( i > 0 )
                i--;
            i *= SHOTPARM_FLOAT_LEN+1;
            i += SHOTPARM_PARAM_1_OFFSET;
            offset = i;
            db.get_text_field( realbuf, offset );
            dp = copystring( dp, realbuf );
            }

        p = nextstring( p );
        }
    }

copystring( dp, p );

/*
-----------------------
Create dir if necessary
----------------------- */
dirp = maketext( filename );
if ( dir_from_filename(dirp) )
    create_directory(dirp);
delete[] dirp;

/*
------------------------
Create file if necessary
------------------------ */
if ( f.open_for_append(filename) )
    {
    f.writeline( buf );
    f.close();
    }

delete[] filename;
delete[] pattern;
}

/***********************************************************************
*                            COPY_PROFILES                             *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_profiles( SHOT_CLASS & current_shot )
{
SHOT_CLASS      record_shot;
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
DB_TABLE        s;
DB_TABLE        d;
STRING_CLASS    tn;
dest = name;
lstrcpy( dest.computer, c.whoami() );

tn = graphlst_dbname( name.computer, name.machine, name.part );
if ( !file_exists(tn.text()) )
    return;

if ( !s.open(tn.text(), GRAPHLST_RECLEN, PFL) )
    return;

tn = graphlst_dbname( dest.computer, dest.machine, dest.part );
if ( !file_exists(tn.text()) )
    d.create( tn.text() );

if ( d.open(tn.text(), GRAPHLST_RECLEN, WL) )
    {
    s.put_long( GRAPHLST_SHOT_NUMBER_OFFSET,  last_examined_shot_number, SHOT_LEN );
    if ( s.goto_first_greater_than_record(1) )
        {
        s.get_current_record( NO_LOCK );
        while ( TRUE )
            {
            record_shot.get_from_graphlst(s);
            if ( record_shot.number > current_shot.number )
                break;

            if ( profile.need_to_save(record_shot) )
                {
                copy_profile( dest, name, record_shot.number, 0, TRUE ); // TRUE means check for symphony
                zip_count++;

                d.copy_rec( s );
                d.rec_append();
                profile.last_saved_shot = record_shot;
                profile.number_acquired++;
                if ( profile.number_acquired >= profile.acquire_count )
                    profile.acquiring = FALSE;
                }

            if ( !s.get_next_record(FALSE) )
                break;
            }
        }

    d.close();
    }

s.close();
}

/***********************************************************************
*                           COPY_PARAMETERS                            *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_parameters( SHOT_CLASS & current_shot )
{
SHOT_CLASS      record_shot;
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
DB_TABLE        s;
DB_TABLE        d;
STRING_CLASS    tn;

dest = name;
lstrcpy( dest.computer, c.whoami() );

/*
---------------------
Open the source table
--------------------- */
tn = shotparm_dbname( name.computer, name.machine, name.part );
if ( !file_exists(tn.text()) )
    return;

if ( !s.open(tn.text(), SHOTPARM_RECLEN, PFL) )
    return;

tn = shotparm_dbname( dest.computer, dest.machine, dest.part );
if ( !file_exists(tn.text()) )
    d.create( tn.text() );

if ( d.open(tn.text(), SHOTPARM_RECLEN, WL) )
    {
    s.put_long( SHOTPARM_SHOT_NUMBER_OFFSET,  last_examined_shot_number, SHOT_LEN );
    if ( s.goto_first_greater_than_record(1) )
        {
        s.get_current_record( NO_LOCK );
        while ( TRUE )
            {
            record_shot.get_from_shotparm( s );
            if ( record_shot.number > current_shot.number )
                break;
            save_network_shotparm( name, s );
            if ( params.need_to_save(record_shot) )
                {
                d.copy_rec( s );
                d.rec_append();
                params.last_saved_shot = record_shot;
                params.number_acquired++;
                if ( params.number_acquired >= params.acquire_count )
                    params.acquiring = FALSE;
                }

            if ( !s.get_next_record(FALSE) )
                break;
            }
        }

    d.close();
    }

s.close();
}

/***********************************************************************
*                             COPY_ALARMS                              *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_alarms( SHOT_CLASS & shot )
{
COMPUTER_CLASS      c;
PART_NAME_ENTRY     dest;
ALARM_SUMMARY_CLASS a;
short               i;
int32               shift;
SYSTEMTIME          shift_date;

get_shift_number_and_date( &i, &shift_date, shot.time.system_time() );
shift = (int32) i;

a.set_part( name );
if ( a.get_counts(shift_date, shift) )
    {
    a.set_part( c.whoami(), name.machine, name.part );
    a.save( shift_date, shift, TRUE );
    }
}

/***********************************************************************
*                           SHOW_SEMAPHORE_ERROR                       *
***********************************************************************/
static void show_semaphore_error( PART_NAME_ENTRY & name )
{
STRING_CLASS s;

s = name.machine;
s += SpaceString;
s += name.part;
s += SpaceString;
s += resource_string( CANT_GET_SEMAPHORE_STRING );

set_text( MainDialogWindow, DS_STATUS_TBOX, s.text() );
}

/***********************************************************************
*                             SHOW_RECEIVED_SHOT                            *
***********************************************************************/
static void show_received_shot( PART_NAME_ENTRY & name, SHOT_CLASS & shot )
{
STRING_CLASS s;
s = name.machine;
s += SpaceString;
s += name.part;
s += SpaceString;
s += resource_string( SHOT_LABEL_STRING );
s += ColonSpaceString;
s += int32toasc( shot.number );

set_text( MainDialogWindow, DS_STATUS_TBOX, s.text() );
}

/***********************************************************************
*                            SHOW_ZIPPED_SHOT                          *
***********************************************************************/
static void show_zipped_shot( PART_NAME_ENTRY & name, SHOT_CLASS & shot, int32 zip_count )
{
STRING_CLASS s;
LISTBOX_CLASS lb;

s = name.machine;
s += TabString;
s += name.part;
s += TabString;
s += int32toasc( zip_count );
s += TabString;
s += shot.time.text();

lb.init( MainDialogWindow, ZIP_COUNT_LISTBOX );
if ( lb.findprefix(name.machine) )
    lb.replace( s.text() );
else
    lb.add( s.text() );
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                            CHECK_FOR_NEW_SHOT                        *
***********************************************************************/
void DS_BACKUP_ENTRY::check_for_new_shot( TCHAR * current_part )
{
SHOT_CLASS shot;

if ( name.is_part(NO_PART) )
    start_new_part( current_part );

if ( !name.is_part(current_part) )
    {
    zip_all_data();
    start_new_part( current_part );
    }

Ls = EmptyString;
add_part_name_entry_to_ls( name );

if ( check_for_local_part(name) )
    {
    if ( shot.get_current(name) )
        {
        show_received_shot( name, shot );

        Ls += SpaceString;
        Ls += int32toasc( shot.number );
        log();

        if ( shot.number < last_examined_shot_number )
            {
            zip_shot_data();
            profile.last_saved_shot.number = NO_SHOT_NUMBER;
            params.last_saved_shot.number  = NO_SHOT_NUMBER;
            last_examined_shot_number      = NO_SHOT_NUMBER;
            }
        }

    if  ( waitfor_shotsave_semaphor( name.computer, name.machine) )
        {
        copy_profiles( shot );
        copy_parameters( shot );
        copy_alarms( shot );
        free_shotsave_semaphore();
        if ( MaxZipCount > 0 && zip_count >= MaxZipCount )
            {
            zip_all_data();
            set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(ZIP_COMPLETE_STRING) );
            }
        show_zipped_shot( name, shot, zip_count );
        last_examined_shot_number = shot.number;
        }
    else
        {
        show_semaphore_error( name );
        }
    }
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                            CHECK_FOR_NEW_SHOT                        *
***********************************************************************/
void DS_BACKUP_ENTRY::check_for_new_shot( void )
{
TCHAR current_part[PART_NAME_LEN+1];

if ( name.is_part(NO_PART) )
    return;

lstrcpy( current_part, current_part_name(name.computer, name.machine) );
check_for_new_shot( current_part );
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                          CHECK_FOR_NEW_DOWNTIME                      *
***********************************************************************/
void DS_BACKUP_ENTRY::check_for_new_downtime( void )
{
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
DB_TABLE        s;
DB_TABLE        d;
SYSTEMTIME      st;
TIME_CLASS      t;
STRING_CLASS    tn;

dest = name;
lstrcpy( dest.computer, c.whoami() );

tn = downtime_dbname( name.computer, name.machine );

if ( !file_exists(tn.text()) )
    return;

t = last_downtime;

if ( s.open(tn.text(), DOWNTIME_RECLEN, PFL) )
    {
    st = last_downtime.system_time();
    s.put_date(  DOWNTIME_DATE_OFFSET, st );
    s.put_time(  DOWNTIME_TIME_OFFSET, st );

    if ( s.goto_first_greater_than_record(2) )
        {
        s.get_current_record( NO_LOCK );

        tn = downtime_dbname( c.whoami(), name.machine );
        if ( !file_exists(tn.text()) )
            d.create( tn.text() );

        /*
        -------------------------------------------------------------
        It's possible I cannot create the file because no dir exists.
        In that case do nothing.
        ------------------------------------------------------------- */
        if ( file_exists(tn.text()) )
            {
            if ( d.open(tn.text(), DOWNTIME_RECLEN, WL) )
                {
                while ( TRUE )
                    {
                    get_time_class_from_db( t, s, DOWNTIME_DATE_OFFSET );
                    d.copy_rec( s );
                    d.rec_append();

                    if ( !s.get_next_record(NO_LOCK) )
                        break;
                    }

                d.close();
                }
            }
        }

    s.close();
    }

if ( t > last_downtime )
    {
    last_downtime = t;
    put_ini_string( IniFileName, name.machine, LastDownTime, last_downtime.text() );
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
***********************************************************************/
DS_BACKUP_CLASS::DS_BACKUP_CLASS()
{
current_entry = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                 EMPTY                                *
***********************************************************************/
void  DS_BACKUP_CLASS::empty( void )
{
backuplist.rewind();
while ( TRUE )
    {
    current_entry = (DS_BACKUP_ENTRY *) backuplist.next();
    if ( !current_entry )
        break;
    delete current_entry;
    }

backuplist.remove_all();
}

/***********************************************************************
*                            ~DS_BACKUP_CLASS                          *
***********************************************************************/
DS_BACKUP_CLASS::~DS_BACKUP_CLASS()
{
empty();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                 REWIND                               *
***********************************************************************/
void DS_BACKUP_CLASS::rewind( void )
{
backuplist.rewind();
current_entry = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  NEXT                                *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::next( void )
{
current_entry = (DS_BACKUP_ENTRY *)backuplist.next();
if ( current_entry )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                   =                                  *
***********************************************************************/
void DS_BACKUP_CLASS::operator=( DS_BACKUP_CLASS & sorc )
{
DS_BACKUP_ENTRY * be;

empty();
sorc.rewind();
while ( sorc.next() )
    {
    be = new DS_BACKUP_ENTRY;
    if ( be )
        {
        *be = *(sorc.current_entry);
        if ( !add( be ) )
            delete( be );
        }
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  ADD                                 *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::add( DS_BACKUP_ENTRY * b )
{
short status;

rewind();

while ( next() )
    {
    status = compare( b->name.machine, current_entry->name.machine, MACHINE_NAME_LEN );
    /*
    --------------------------
    Only one entry per machine
    -------------------------- */
    if ( status == 0 )
        return FALSE;
    /*
    --------------------
    Sort by machine name
    -------------------- */
    if ( status < 0 )
        return backuplist.insert( b );
    }

return backuplist.append( b );
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  LOAD                                *
***********************************************************************/
void  DS_BACKUP_CLASS::load( void )
{
COMPUTER_CLASS    c;
DB_TABLE          t;
DS_BACKUP_ENTRY * be;
BOOLEAN           status;
TEXT_LIST_CLASS   machinelist;
STRING_CLASS      s;
FILE_CLASS        f;
TCHAR           * cp;

set_text( MainDialogWindow, DS_STATUS_TBOX, TEXT( "Loading machines") );

empty();
c.rewind();
while ( c.next() )
    {
    /*
    --------------------
    Ignore this computer
    -------------------- */
    if ( c.is_this_computer() )
        continue;

    /*
    --------------------------
    Ignore the backup computer
    -------------------------- */
    if ( strings_are_equal(c.name(), BackupComputerName) )
        continue;

    s = TEXT( "Loading machines at computer " );
    s += c.name();

    s.set_text( MainDialogWindow, DS_STATUS_TBOX );

    if ( c.is_present() )
        {

        s = machset_dbname( c.name() );
        if ( t.open(s.text(), MACHSET_RECLEN, PFL) )
            {
            while ( t.get_next_record(NO_LOCK) )
                {
                be = new DS_BACKUP_ENTRY;
                if ( be )
                    {
                    lstrcpy( be->name.computer, c.name() );
                    status = FALSE;
                    if ( t.get_alpha( be->name.machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN) )
                        {
                        machinelist.append( be->name.machine );
                        status = add( be );
                        }

                    if ( !status )
                        delete be;
                    }
                }
            t.close();
            }

        if ( machinelist.count() > 0 )
            {
            s = exe_directory();
            s.cat_path( c.name() );
            s += TEXT( "_machine_list.txt" );
            if ( f.open_for_write(s.text()) )
                {
                machinelist.rewind();
                while ( machinelist.next() )
                    f.writeline( machinelist.text() );
                f.close();
                }
            machinelist.empty();
            }
        }
    else
        {
        /*
        ------------------------------------------------------------------------------
        If the computer isn't online then see if I have saved the machine list locally
        ------------------------------------------------------------------------------ */
        s = exe_directory();
        s.cat_path( c.name() );
        s += TEXT( "_machine_list.txt" );
        if ( f.open_for_read(s.text()) )
            {
            while ( true )
                {
                cp = f.readline();
                if ( !cp )
                    break;
                if ( *cp != NullChar )
                    {
                    be = new DS_BACKUP_ENTRY;
                    if ( be )
                        {
                        lstrcpy( be->name.computer, c.name() );
                        lstrcpy( be->name.machine,  cp );
                        if ( !add(be) )
                            delete be;
                        }
                    }
                }
            f.close();
            }
        }
    }

set_text( MainDialogWindow, DS_STATUS_TBOX, EmptyString );

rewind();
while ( next() )
    current_entry->read();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  FIND                                *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::find( TCHAR * machine_to_find )
{
if ( current_entry )
    if ( current_entry->name.is_machine(machine_to_find) )
        return TRUE;

rewind();
while ( next() )
    if ( current_entry->name.is_machine(machine_to_find) )
        return TRUE;

return FALSE;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                              ZIP_ALL_PARTS                           *
***********************************************************************/
void DS_BACKUP_CLASS::zip_all_parts( void )
{
rewind();
while ( next() )
    {
    if ( current_entry->last_examined_shot_number != NO_SHOT_NUMBER )
        current_entry->zip_all_data();
    }

if ( MainDialogWindow )
    set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(ZIP_COMPLETE_STRING) );
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                         PROCESS_MAILSLOT_MESSAGE                     *
*                       MON,DS_NOTIFY,M01,VISITRAK,DT                  *
*                    MON,DS_NOTIFY,M01,VISITRAK,NS,1395                *
***********************************************************************/
void DS_BACKUP_CLASS::process_mailslot_message( TCHAR * msg )
{
TCHAR * cp;
TCHAR * p;

/*
-----------------------
Skip the topic and item
----------------------- */
cp = findchar( CommaChar, msg );
if ( cp )
    {
    cp++;
    cp = findchar( CommaChar, cp );
    }

if ( cp )
    {
    /*
    ---------------------------------------------------------------
    Point the start of the message at the start of the machine name
    --------------------------------------------------------------- */
    cp++;
    msg = cp;

    /*
    -----------------------------------------
    Look for the comma after the machine name
    ----------------------------------------- */
    cp = findchar( CommaChar, cp );
    }

if ( cp )
    {
    *cp = NullChar;

    /*
    ----------------------------
    Find this machine in my list
    ---------------------------- */
    if ( find(msg) )
        {
        cp++;
        p = cp;
        cp = findchar( CommaChar, cp );
        if ( cp )
            {
            *cp = NullChar;

            cp++;
            if ( *cp == NChar )
                current_entry->check_for_new_shot( p );
            else
                current_entry->check_for_new_downtime();
            }
        }
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                SHUTDOWN                              *
***********************************************************************/
void DS_BACKUP_CLASS::shutdown( void )
{
Ls = "DS_BACKUP_CLASS::shutdown()";
log();

rewind();
while ( next() )
    current_entry->write();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                           CLEAR_MAX_ZIP_COUNT                        *
***********************************************************************/
void DS_BACKUP_CLASS::clear_max_zip_count( void )
{
MaxZipCount = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                            SET_MAX_ZIP_COUNT                         *
***********************************************************************/
void DS_BACKUP_CLASS::set_max_zip_count( int32 new_count )
{
MaxZipCount = new_count;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                           RELOAD_SAVE_CONFIGS                        *
***********************************************************************/
void DS_BACKUP_CLASS::reload_save_configs( void )
{
rewind();
while ( next() )
    current_entry->read_save_config();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                           RELOAD_ZIP_PATHS                           *
***********************************************************************/
void DS_BACKUP_CLASS::reload_zip_paths()
{
rewind();
while ( next() )
    current_entry->read_zip_paths();
}
