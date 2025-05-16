#include <windows.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\computer.h"
#include "..\include\genlist.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\downtime.h"
#include "..\include\subs.h"

#include "proclass.h"
#include "resource.h"

const BITSETYPE BEGIN_RESTORE_TYPE = 1;
const BITSETYPE ABORT_RESTORE_TYPE = 2;

const int32 NO_ZIP_FILE_TYPE      = 0;
const int32 PART_ZIP_FILE_TYPE    = 1;
const int32 MACHINE_ZIP_FILE_TYPE = 2;

static TCHAR PartZipSuffix[]       = TEXT( ".PAZ" );
static TCHAR PartZipLowSuffix[]    = TEXT( ".paz" );
static TCHAR MachineZipSuffix[]    = TEXT( ".MAZ" );
static TCHAR MachineZipLowSuffix[] = TEXT( ".maz" );
static TCHAR WildProfileName[]     = TEXT( "*.PR?" );

const TCHAR NewDirName[]   = TEXT( "New" );
const TCHAR FinalDirName[] = TEXT( "Final" );

HWND      MainWindow;
HWND      MainDialogWindow;
HINSTANCE MainInstance;

TCHAR MyClassName[]   = "DsRestor";
TCHAR MyWindowTitle[] = "Restore";

TCHAR * NewDirectory   = 0;
TCHAR * RealDirectory  = 0;
TCHAR * FinalDirectory = 0;
TCHAR * ZipDirectory   = 0;

PART_CLASS DefaultPart;

BOOLEAN   AbortRequested    = FALSE;
BITSETYPE RestoreButtonType = BEGIN_RESTORE_TYPE;

static TCHAR DefaultRestoreDirectory[] = TEXT( "C:\\V5BACKUP" );
static TCHAR DsRestorIniFile[] = TEXT( "visitrak.ini" );
static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR RootDirKey[]      = TEXT( "RootDir" );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR UnderScoreChar    = TEXT( '_' );
static TCHAR Filter[]          = TEXT( "Visi-Trak Zip Files\0*.PAZ;*.MAZ\0" );
static TCHAR MachsetDBName[]   = MACHSET_DB;
static TCHAR PartsDBName[]     = PARTS_DB;

/***********************************************************************
*                        RESOURCE_MESSAGE_BOX                          *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                          FIND_DEFAULT_PART                           *
***********************************************************************/
static BOOLEAN find_default_part( void )
{
COMPUTER_CLASS c;
DB_TABLE       t;
BOOLEAN        status;
PART_NAME_ENTRY p;

status = FALSE;

lstrcpy( p.computer, c.whoami() );

/*
--------------------------------------------
Get the name of the first machine in my list
-------------------------------------------- */
if ( t.open(machset_dbname(p.computer), MACHSET_RECLEN, PFL) )
    {
    if ( t.get_next_record(NO_LOCK) )
        {
        if ( t.get_alpha( p.machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN) )
            status = TRUE;
        }
    t.close();
    }

/*
------------------------------
Get the name of the first part
------------------------------ */
if ( status )
    {
    status = FALSE;
    t.open( parts_dbname(p.computer, p.machine), PARTS_RECLEN, PFL );
    if ( t.get_next_record(NO_LOCK) )
        {
        if ( t.get_alpha( p.part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN) )
            status = TRUE;
        }
    t.close();
    }

/*
---------------------------------
Load the part as the default part
--------------------------------- */
if ( status )
    {
    status = DefaultPart.find( p.computer, p.machine, p.part );
    }

return status;
}

/***********************************************************************
*                            ZIP_FILE_TYPE                             *
***********************************************************************/
int32 zip_file_type( TCHAR * filename )
{
if ( findstring(PartZipSuffix, filename) )
    return PART_ZIP_FILE_TYPE;

if ( findstring(PartZipLowSuffix, filename) )
    return PART_ZIP_FILE_TYPE;

if ( findstring(MachineZipSuffix, filename) )
    return MACHINE_ZIP_FILE_TYPE;

if ( findstring(MachineZipLowSuffix, filename) )
    return MACHINE_ZIP_FILE_TYPE;

return NO_ZIP_FILE_TYPE;
}

/***********************************************************************
*                           GET_ZIP_FILE_LIST                          *
***********************************************************************/
BOOLEAN get_zip_file_list( void )
{
const TCHAR default_start_dir[] = TEXT( "\\" );
const int32 buflen = 5000;
OPENFILENAME   fh;
TCHAR       *  cp;
TCHAR       *  fname;
LISTBOX_CLASS lb;
BOOLEAN       status;
TCHAR         startdir[MAX_PATH+1];

lstrcpy( startdir, default_start_dir );
cp = get_ini_string( DsRestorIniFile, "Restore", "StartDir" );
if ( !unknown(cp) )
    lstrcpy( startdir, cp );

status = FALSE;
fname = maketext( buflen );
if ( !fname )
    {
    resource_message_box( CANT_DO_THAT_STRING, UNABLE_TO_ALLOC_BUFFER_STRING );
    return FALSE;
    }

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow;
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = buflen;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = startdir;
fh.lpstrTitle        = "Choose Zip File to Load";
fh.Flags             = OFN_EXPLORER | OFN_ALLOWMULTISELECT ;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( GetOpenFileName( &fh ) )
    {
    lb.init( MainDialogWindow, RESTORE_FILES_LISTBOX );
    lb.empty();

    /*
    -----------------------------------------------------------
    If there is only one file, make it look like multiple files
    ----------------------------------------------------------- */
    if ( fh.nFileOffset < lstrlen(fname) )
        {
        if ( fh.nFileOffset > 0 )
            {
            cp = fname + fh.nFileOffset;
            cp--;
            *cp = NullChar;
            cp++;
            cp = nextstring( cp );
            *cp = NullChar;
            }
        }

    if ( fh.nFileOffset > lstrlen(fname) )
        {
        cp = fname + fh.nFileOffset;
        while ( *cp )
            {
            lb.add( cp );
            cp = nextstring( cp );
            }
        if ( ZipDirectory )
            delete[] ZipDirectory;
        ZipDirectory = maketext( fname );
        put_ini_string( DsRestorIniFile, "Restore", "StartDir", ZipDirectory );
        }
    status = TRUE;
    }

delete[] fname;
return status;
}

/***********************************************************************
*                           GET_RESTORE_DIRECTORY                      *
***********************************************************************/
static void get_restore_directory( void )
{
COMPUTER_CLASS c;
TCHAR        * cp;
int32          slen;

/*
--------------------------------------------------------
Default to the drive that has the current root directory
-------------------------------------------------------- */
cp = get_ini_string( VisiTrakIniFile, ConfigSection, RootDirKey );
if ( !unknown(cp) )
    {
    *DefaultRestoreDirectory = *cp;
    }

cp = get_ini_string( DsRestorIniFile, "Restore", "RestoreDir" );
if ( unknown(cp) )
    {
    cp = DefaultRestoreDirectory;
    put_ini_string( DsRestorIniFile, "Restore", "RestoreDir", cp );
    }

slen = lstrlen(cp);
if ( slen > 2 )
    {
    NewDirectory   = maketext( cp, lstrlen(NewDirName)   + 2 );
    FinalDirectory = maketext( cp, lstrlen(FinalDirName) + 2 );
    if ( NewDirectory && FinalDirectory )
        {
        append_filename_to_path( NewDirectory, NewDirName );
        append_filename_to_path( FinalDirectory, FinalDirName );
        return;
        }
    }

if ( NewDirectory )
    {
    delete[] NewDirectory;
    NewDirectory = 0;
    }

if ( FinalDirectory )
    {
    delete[] FinalDirectory;
    FinalDirectory = 0;
    }

resource_message_box( UNABLE_TO_CONTINUE_STRING, GET_RESTORE_FAILED_STRING );
PostMessage( MainWindow, WM_CLOSE, 0, 0L );
}

/***********************************************************************
*                         CHECK_FOR_EXISTANCE                          *
* I am assuming that the machset db has been unzipped into the         *
* NewDirectory.                                                        *
***********************************************************************/
BOOLEAN check_for_existance( PART_NAME_ENTRY & p )
{
BOOLEAN status;
TCHAR   path[MAX_PATH+1];
TCHAR * fname;
MACHINE_CLASS   dm;
MACHINE_CLASS   m;
PART_CLASS      pc;
PARAMETER_CLASS param;

/*
-------------------------------------------------
Copy the directory path and make a pointer to the
end so I can just append the file name each time.
------------------------------------------------- */
lstrcpy( path, NewDirectory );
append_backslash_to_path( path );
fname = path + lstrlen( path );

/*
-----------------------
Copy the machine setups
----------------------- */
lstrcpy( m.computer, p.computer );
lstrcpy( m.name,     p.machine  );

lstrcpy( fname, MachsetDBName );
if ( m.read_machset_db(path) )
    {
    if ( machine_exists(p.computer, p.machine) )
        {
        if ( dm.find(p.computer, p.machine) )
            {
            lstrcpy( m.current_part, dm.current_part );
            lstrcpy( m.opnumber,     dm.opnumber     );
            m.monitor_flags       = dm.monitor_flags;
            m.ft_board_number     = dm.ft_board_number;
            m.muxchan             = dm.muxchan;
            m.is_ftii             = dm.is_ftii;
            m.suretrak_controlled = dm.suretrak_controlled;
            m.profile             = dm.profile;
            m.params              = dm.params;
            }
        }
    else
        {
        /*
        -------------------------------------------------
        This is a new machine, it shouldn't be monitored.
        ------------------------------------------------- */
        m.monitor_flags = MA_NO_MONITORING;
        }
    m.save();
    }

status = machine_exists(p.computer, p.machine );
if ( !status )
    resource_message_box( CANT_DO_THAT_STRING, MACHINE_CREATE_ERROR_STRING );

/*
--------------------
Copy the part setups
-------------------- */
if ( status && !p.is_part(NO_PART) )
    {
    if ( !p.exists() )
        copypart( p.computer, p.machine, p.part, DefaultPart.computer, DefaultPart.machine, DefaultPart.name, COPY_PART_ALL );

    lstrcpy( pc.computer, p.computer );
    lstrcpy( pc.machine,  p.machine );
    lstrcpy( pc.name,     p.part );

    lstrcpy( fname, PartsDBName );
    status = pc.read_parts_db( path, p.part );

    if ( status )
        {
        lstrcpy( fname, PARMLIST_DB );

        lstrcpy( param.computer, p.computer );
        lstrcpy( param.machine,  p.machine );
        lstrcpy( param.part,     p.part );
        status = param.read_param_db( path );
        }

    if ( status )
        {
        lstrcpy( fname, FTCHAN_DB );
        status = pc.read_ftchan_db( path );
        }

    if ( status )
        {
        pc.save();
        lstrcpy( param.computer, pc.computer );
        lstrcpy( param.machine,  pc.machine );
        lstrcpy( param.part,     pc.name );
        param.save();
        }

    status = p.exists();
    if ( !status )
        resource_message_box( CANT_DO_THAT_STRING, PART_CREATE_ERROR_STRING );
    }

return status;
}

/***********************************************************************
*                            CLEAR_DIRECTORY                           *
***********************************************************************/
static void clear_directory( TCHAR * directory_path )
{
if ( directory_exists(directory_path) )
    empty_directory( directory_path );
else
    create_directory( directory_path );
}

/***********************************************************************
*                             UNZIP_FILES                              *
***********************************************************************/
static BOOLEAN unzip_files( TCHAR * fname )
{
TCHAR    buf[MAX_PATH+1];
BOOLEAN status;

SetCurrentDirectory( NewDirectory );

get_exe_directory( buf );
lstrcat( buf, TEXT("pkzip25 -ext -noz ") );
lstrcat( buf, fname );

status = execute_and_wait( buf, NewDirectory );
if ( !status )
    {
    resource_message_box( CANT_DO_THAT_STRING, UNZIP_ERROR_STRING );
    }

return status;
}

/***********************************************************************
*                             COPY_MASTER                              *
***********************************************************************/
static void copy_master( void )
{
const TCHAR MasterName[]   = CURRENT_MASTER_TRACE_NAME;
const TCHAR MasterSuffix[] = MASTER_TRACE_SUFFIX;

TCHAR * d;
TCHAR * s;
int32 slen;

slen = lstrlen( MasterName ) + lstrlen( MasterSuffix ) + 1;
s = maketext( NewDirectory, slen );
d = maketext( FinalDirectory, slen );

if ( s && d )
    {
    append_filename_to_path( s, MasterName );
    lstrcat( s, MasterSuffix );

    if ( file_exists(s) )
        {
        append_filename_to_path( d, MasterName );
        lstrcat( d, MasterSuffix );
        if ( file_exists(d) )
            DeleteFile( d );
        MoveFile( s, d );
        }
    }

if ( s )
    delete[] s;

if ( d )
    delete[] d;
}

/***********************************************************************
*                            MERGE_DOWNTIMES                           *
***********************************************************************/
BOOLEAN merge_downtimes( PART_NAME_ENTRY & p, TCHAR * fname )
{

TIME_CLASS mintime;
TCHAR s[MAX_PATH+1];

DOWNTIME_MERGE_CLASS sorc1;
DOWNTIME_MERGE_CLASS sorc2;
DOWNTIME_MERGE_CLASS dest;

clear_directory( NewDirectory );
clear_directory( FinalDirectory );

RealDirectory = maketext( mrdir_name(p.computer, p.machine, 0) );
if ( !RealDirectory )
    return FALSE;

if ( !unzip_files(fname) )
    return FALSE;

if ( !check_for_existance(p) )
    return FALSE;

sorc1.open( RealDirectory,   PFL );
sorc2.open( NewDirectory,    PFL );
dest.open(  FinalDirectory,  WL  );

sorc1.next();
sorc2.next();

while ( TRUE )
    {
    if ( sorc1.have_downtime() )
        mintime = sorc1.time();
    else if ( sorc2.have_downtime() )
        mintime = sorc2.time();
    else
        break;

    if ( sorc2 < mintime )
        mintime = sorc2.time();

    if ( sorc2 == mintime )
        {
        dest.add( sorc2 );
        if ( sorc1 == sorc2 )
            sorc1.next();
        sorc2.next();
        }
    else if ( sorc1 == mintime )
        {
        dest.add( sorc1 );
        sorc1.next();
        }
    }

sorc1.close();
sorc2.close();
dest.close();

lstrcpy( s, FinalDirectory );
append_filename_to_path( s, DOWNTIME_DB );

CopyFile( s, downtime_dbname(p.computer, p.machine), FALSE );

delete[] RealDirectory;
RealDirectory = 0;

return TRUE;
}

/***********************************************************************
*                           MAKE_ALARMSUM_PATH                         *
***********************************************************************/
static void make_alarmsum_path( TCHAR * dest, TCHAR * dir )
{

lstrcpy( dest, dir );
append_filename_to_path( dest, ALARMSUM_DB );
}

/***********************************************************************
*                             MERGE_ALARMS                             *
***********************************************************************/
BOOLEAN merge_alarms( void )
{
ALARM_SUMMARY_CLASS sum;
SYSTEMTIME st;

TCHAR sorc[MAX_PATH+1];
TCHAR dest[MAX_PATH+1];
TIME_CLASS stime;
TIME_CLASS dtime;
int32      myshift;
int32      last_shift;
DB_TABLE   t;

make_alarmsum_path( sorc, RealDirectory );

if ( !file_exists(sorc) )
    {
    /*
    ----------------------------------------------------------------
    There is no old file (or I don't want it), just copy the new one
    ---------------------------------------------------------------- */
    make_alarmsum_path( sorc, NewDirectory );
    make_alarmsum_path( dest, FinalDirectory );
    if ( file_exists(sorc) )
        CopyFile( sorc, dest, FALSE );
    return TRUE;
    }

make_alarmsum_path( dest, NewDirectory );

if ( !file_exists(dest) )
    {
    /*
    -------------------------------------------
    There is no new file, just copy the old one
    ------------------------------------------- */
    make_alarmsum_path( sorc, RealDirectory );
    make_alarmsum_path( dest, FinalDirectory );
    CopyFile( sorc, dest, FALSE );
    return TRUE;
    }

/*
------------------------------------------
Copy the older file to the final directory
------------------------------------------ */
get_file_write_time( st, sorc );
stime.set( st );

get_file_write_time( st, dest );
dtime.set( st );

if ( dtime < stime )
    make_alarmsum_path( sorc, NewDirectory );

make_alarmsum_path( dest, FinalDirectory );
CopyFile( sorc, dest, FALSE );

/*
--------------------------------------
Put the name of the other file in sorc
-------------------------------------- */
if ( dtime < stime )
    make_alarmsum_path( sorc, RealDirectory );
else
    make_alarmsum_path( sorc, NewDirectory );

init_systemtime_struct( st );
dtime.set( st );

last_shift = NO_SHIFT_NUMBER;

t.open( sorc, ALARMSUM_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_date( st, ALARMSUM_DATE_OFFSET );
    stime.set( st );
    myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );

    if ( stime != dtime || myshift != last_shift )
        {
        sum.get_counts( st, myshift, sorc );
        sum.save( st, myshift, TRUE, dest );
        dtime      = stime;
        last_shift = myshift;
        }
    if ( AbortRequested )
        break;
    }
t.close();
return TRUE;
}

/***********************************************************************
*                             MERGE_SHOTS                              *
***********************************************************************/
BOOLEAN merge_shots( PART_NAME_ENTRY & p, TCHAR * fname )
{

PROFILE_MERGE_CLASS  gnew;
PROFILE_MERGE_CLASS  gori;
PROFILE_MERGE_CLASS  gfinal;
SHOTPARM_MERGE_CLASS pnew;
SHOTPARM_MERGE_CLASS pori;
SHOTPARM_MERGE_CLASS pfinal;
SHOT_CLASS           minshot;
SHOT_CLASS           nextshot;
TCHAR              * s;
TCHAR              * shotname;

clear_directory( NewDirectory );
clear_directory( FinalDirectory );

RealDirectory = maketext( result_directory_name(p.computer, p.machine, p.part) );
if ( !RealDirectory )
    return FALSE;

if ( !unzip_files(fname) )
    return FALSE;

if ( !check_for_existance(p) )
    return FALSE;

s = maketext( resource_string(MainInstance, MERGING_SHOT_STRING), SHOT_LEN );
shotname = 0;
if ( s )
    shotname = s + lstrlen(s);

gori.open_graphlst( RealDirectory, PFL );
pori.open_shotparm( RealDirectory, PFL );

gnew.open_graphlst( NewDirectory, PFL );
pnew.open_shotparm( NewDirectory, PFL );

gfinal.open_graphlst( FinalDirectory, WL );
pfinal.open_shotparm( FinalDirectory, WL );

gori.rewind();
gori.next();

gnew.rewind();
gnew.next();

pnew.rewind();
pnew.next();

pori.rewind();
pori.next();

nextshot.number = NO_SHOT_NUMBER;

while ( TRUE )
    {
    /*
    ------------------------------
    Get the min of the current lot
    ------------------------------ */
    if ( gori.have_shot() )
        minshot = gori.shot();
    else if ( gnew.have_shot() )
        minshot = gnew.shot();
    else if ( pori.have_shot() )
        minshot = pori.shot();
    else if ( pnew.have_shot() )
        minshot = pnew.shot();
    else
        break;

    /*
    -------------------------------------------------
    If gori had a shot, minshot will be equal to that
    so I don't have to test that instance.
    ------------------------------------------------- */
    if ( gnew < minshot.time )
        minshot = gnew.shot();

    if ( pori < minshot.time )
        minshot = pori.shot();

    if ( pnew < minshot.time )
        minshot = pnew.shot();

    nextshot.time = minshot.time;

    if ( minshot.number > nextshot.number || nextshot.number == NO_SHOT_NUMBER )
        nextshot.number = minshot.number;

    if ( shotname )
        {
        lstrcpy( shotname, int32toasc(nextshot.number) );
        set_text( MainDialogWindow, DS_STATUS_TBOX, s );
        }

    if ( gnew == nextshot.time )
        {
        gfinal.get( gnew, nextshot.number );
        if ( gori == gnew )
            gori.next();
        gnew.next();
        }
    else if ( gori == nextshot.time )
        {
        gfinal.get( gori, nextshot.number );
        gori.next();
        }

    if ( pnew == nextshot.time )
        {
        pfinal.get( pnew, nextshot.number );
        if ( pori == pnew )
            pori.next();
        pnew.next();
        }
    else if ( pori == nextshot.time )
        {
        pfinal.get( pori, nextshot.number );
        pori.next();
        }

    if ( AbortRequested )
        break;
    nextshot.number++;
    }

gori.close();
gnew.close();
gfinal.close();
pori.close();
pnew.close();
pfinal.close();

if ( s )
    delete[] s;


if ( !AbortRequested )
    {
    copy_master();
    merge_alarms();
    }

kill_directory( RealDirectory );
MoveFile( FinalDirectory, RealDirectory );

delete[] RealDirectory;
RealDirectory = 0;

clear_directory( NewDirectory );

set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(MainInstance, FILE_COMPLETE_STRING) );

return TRUE;
}

/***********************************************************************
*                             SET_BUTTON_TYPE                          *
***********************************************************************/
static void set_button_type( BITSETYPE new_button_type )
{
UINT id;

RestoreButtonType = new_button_type;

if ( new_button_type & BEGIN_RESTORE_TYPE )
    id = BEGIN_RESTORE_STRING;
else
    id = ABORT_RESTORE_STRING;

set_text( MainDialogWindow, BEGIN_RESTORE_BUTTON, resource_string(MainInstance, id) );
}

/***********************************************************************
*                           SET_BUTTON_STATE                           *
***********************************************************************/
static void set_button_state( int id, BOOL is_enabled )
{
EnableWindow( GetDlgItem(MainDialogWindow, id), is_enabled );
}

/***********************************************************************
*                        TURN_OFF_BEGIN_BUTTON                         *
***********************************************************************/
static void turn_off_begin_button( void )
{
set_button_state( BEGIN_RESTORE_BUTTON, FALSE );
}

/***********************************************************************
*                         TURN_ON_BEGIN_BUTTON                         *
***********************************************************************/
static void turn_on_begin_button( void )
{
set_button_state( BEGIN_RESTORE_BUTTON, TRUE );
}

/***********************************************************************
*                        TURN_OFF_CHOOSE_BUTTON                        *
***********************************************************************/
static void turn_off_choose_button( void )
{
set_button_state( CHOOSE_FILES_BUTTON, FALSE );
}

/***********************************************************************
*                        TURN_ON_CHOOSE_BUTTON                         *
***********************************************************************/
static void turn_on_choose_button( void )
{
set_button_state( CHOOSE_FILES_BUTTON, TRUE );
}

/***********************************************************************
*                           COPY_UNTIL_CHAR                            *
*                              WITH NULL                               *
***********************************************************************/
TCHAR * copy_until_char( TCHAR * dest, TCHAR * sorc, TCHAR c, int32 maxchars )
{
while ( maxchars > 0 )
    {
    if ( *sorc == c || *sorc == NullChar )
        break;

    *dest = *sorc;
    dest++;
    sorc++;
    maxchars--;
    }

*dest = NullChar;
sorc++;
return sorc;
}

/***********************************************************************
*                         PURGE_MACHINE_DATA                           *
***********************************************************************/
static void purge_machine_data( PART_NAME_ENTRY & p )
{
DB_TABLE t;
TCHAR * tn;

tn = downtime_dbname( p.computer, p.machine );

if ( file_exists(tn) )
    {
    if ( t.open( downtime_dbname(p.computer, p.machine), DOWNTIME_RECLEN, FL) )
        {
        t.empty();
        t.close();
        }
    }
}

/***********************************************************************
*                           PURGE_PART_DATA                            *
***********************************************************************/
static void purge_part_data( PART_NAME_ENTRY & p )
{
DB_TABLE t;
TCHAR  * tablename;

if ( !machine_exists(p.computer, p.machine) )
    return;

if ( !part_exists(p) )
    return;

/*
------------------
Purge the profiles
------------------ */
if ( t.open( graphlst_dbname(p.computer, p.machine, p.part), GRAPHLST_RECLEN, FL) )
    {
    t.empty();
    t.close();
    }

empty_directory( result_directory_name(p.computer, p.machine, p.part), WildProfileName );

/*
--------------------
Purge the parameters
-------------------- */
if ( t.open( shotparm_dbname(p.computer, p.machine, p.part), SHOTPARM_RECLEN, FL) )
    {
    t.empty();
    t.close();
    }

/*
----------------
Purge the alarms
---------------- */
tablename = alarmsum_dbname( p.computer, p.machine, p.part );
if ( file_exists(tablename) )
    {
    if ( t.open( tablename, ALARMSUM_RECLEN, FL) )
        {
        t.empty();
        t.close();
        }
    }
else
    {
    t.create( tablename );
    }


set_last_shot_number( p, 0 );
}

/***********************************************************************
*                          PART_FROM_ZIP_FILE                          *
* I extract the machset and part files to the NewDiirectory,           *
* read the machine and part, and then delete them.                     *
***********************************************************************/
static PART_NAME_ENTRY & part_from_zip_file( TCHAR * zipfile )
{
static PART_NAME_ENTRY p;
static TCHAR   pkzip_command[] = TEXT( "pkzip25 -ext -noz " );
static TCHAR   files_to_unzip[] = TEXT( " ") MACHSET_DB TEXT(" ") PARTS_DB;

COMPUTER_CLASS c;
DB_TABLE       t;
STRING_CLASS   s;
BOOLEAN        status;

SetCurrentDirectory( NewDirectory );
p.set( c.whoami(), NO_MACHINE, NO_PART );

/*
-------------------------------------------------------------
Make sure the files I'm trying to restore don't exist already
------------------------------------------------------------- */
DeleteFile( MachsetDBName );
DeleteFile( PartsDBName );

s = exe_directory();
s += pkzip_command;
s += zipfile;
s += files_to_unzip;

status = execute_and_wait( s.text(), NewDirectory );
if ( status )
    {
    if ( t.open(MachsetDBName, MACHSET_RECLEN, PFL) )
        {
        if ( t.get_next_record(NO_LOCK) )
            t.get_alpha( p.machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
        t.close();
        }
    DeleteFile( MachsetDBName );

    if ( t.open(PartsDBName, PARTS_RECLEN, PFL) )
        {
        if ( t.get_next_record(NO_LOCK) )
            t.get_alpha( p.part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
        t.close();
        }
    DeleteFile( PartsDBName );
    }
else
    {
    resource_message_box( CANT_DO_THAT_STRING, UNZIP_ERROR_STRING );
    }

return p;
}

/***********************************************************************
*                           RESTORE_THREAD                             *
***********************************************************************/
DWORD restore_thread( int * notused )
{
int32           i;
int32           n;
LISTBOX_CLASS   lb;
TCHAR           fname[MAX_PATH+1];
TCHAR         * cp;
PART_NAME_ENTRY * p;

lb.init( MainDialogWindow, RESTORE_FILES_LISTBOX );
n = lb.count();
if ( n > 0 )
    {
    /*
    ------------------------------------
    Create an array of part name entries
    ------------------------------------ */
    p = new PART_NAME_ENTRY[n];
    if ( p )
        {

        lstrcpy( fname, ZipDirectory );
        append_backslash_to_path( fname );
        cp = fname + lstrlen( fname );

        /*
        -------------------------------------------------------------------------------------------------------
        I have to get the parts from the zip files. I used to get these from the names but that no longer works
        ------------------------------------------------------------------------------------------------------- */
        for ( i=0; i<n; i++ )
            {
            lb.setcursel( i );
            lstrcpy( cp, lb.selected_text() );
            p[i] = part_from_zip_file( fname );
            }

        if ( is_checked(MainDialogWindow, PURGE_BEFORE_RESTORE_RADIOBUTTON) )
            {
            for ( i=0; i<n; i++ )
                {
                lb.setcursel( i );
                lstrcpy( cp, lb.selected_text() );

                if ( zip_file_type(cp) == PART_ZIP_FILE_TYPE )
                    {
                    purge_part_data( p[i] );
                    }
                else if (zip_file_type(cp) == MACHINE_ZIP_FILE_TYPE )
                    {
                    purge_machine_data( p[i] );
                    }

                if ( AbortRequested )
                    break;
                }
            }

        if ( !AbortRequested )
            {
            for ( i=0; i<n; i++ )
                {
                lb.setcursel( i );
                lstrcpy( cp, lb.selected_text() );
                if ( zip_file_type(cp) == PART_ZIP_FILE_TYPE )
                    {
                    merge_shots( p[i], fname );
                    }
                else if (zip_file_type(cp) == MACHINE_ZIP_FILE_TYPE )
                    {
                    merge_downtimes( p[i], fname );
                    }

                if ( AbortRequested )
                    break;
                }
            }

        delete[] p;
        }
    else
        {
        resource_message_box( CANT_DO_THAT_STRING, UNABLE_TO_ALLOC_BUFFER_STRING );
        }

    }

AbortRequested = FALSE;

set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(MainInstance, COPY_COMPLETE_STRING) );
set_button_type( BEGIN_RESTORE_TYPE );
turn_on_choose_button();

return 0;
}

/***********************************************************************
*                         START_RESTORE_THREAD                         *
***********************************************************************/
static void start_restore_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                      /* no security attributes        */
    0,                                         /* use default stack size        */
    (LPTHREAD_START_ROUTINE) restore_thread,   /* thread function       */
    0,                                         /* argument to thread function   */
    0,                                         /* use default creation flags    */
    &id );                                     /* returns the thread identifier */

if ( !h )
   resource_message_box( CREATE_FAILED_STRING, RESTORE_THREAD_STRING );

}

/***********************************************************************
*                              ABOUT_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int           id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        break;

    }

return FALSE;
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MainDialogWindow = hWnd;
        turn_off_begin_button();
        turn_on_choose_button();
        CheckRadioButton( hWnd, PURGE_BEFORE_RESTORE_RADIOBUTTON, MERGE_WITH_EXISTING_RADIOBUTTON, PURGE_BEFORE_RESTORE_RADIOBUTTON );

        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case BEGIN_RESTORE_BUTTON:
                if ( RestoreButtonType == ABORT_RESTORE_TYPE )
                    {
                    AbortRequested = TRUE;
                    }
                else
                    {
                    AbortRequested = FALSE;
                    set_button_type( ABORT_RESTORE_TYPE );
                    turn_off_choose_button();
                    start_restore_thread();
                    }
                return TRUE;

            case CHOOSE_FILES_BUTTON:
                turn_off_choose_button();
                if ( get_zip_file_list() )
                    turn_on_begin_button();
                turn_on_choose_button();
                return TRUE;

            default:
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {

    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        shrinkwrap( MainWindow, MainDialogWindow );
        get_restore_directory();
        if ( !find_default_part() )
            {
            resource_message_box( CANT_DO_THAT_STRING, DEFAULT_PART_ERROR_STRING );
            PostMessage( hWnd, WM_CLOSE, 0, 0L );
            }
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case ABOUT_MENU:
                DialogBox(
                    MainInstance,
                    "ABOUT_DIALOG",
                    hWnd,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_MENU:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOL init( HINSTANCE this_instance )
{

WNDCLASS wc;
COMPUTER_CLASS c;

MainInstance = this_instance;

names_startup();
c.startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("DATA_SERVER_RESTORE_ICON") );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

MainDialogWindow  = CreateDialog(
    MainInstance,
    "MainDialog",
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_SHOW );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
if ( ZipDirectory )
    {
    delete[] ZipDirectory;
    ZipDirectory = 0;
    }

if ( NewDirectory )
    {
    delete[] NewDirectory;
    NewDirectory = 0;
    }

if ( RealDirectory )
    {
    delete[] RealDirectory;
    RealDirectory = 0;
    }

if ( FinalDirectory )
    {
    delete[] FinalDirectory;
    FinalDirectory = 0;
    }

}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{

MSG  msg;
BOOL status;

if ( is_previous_instance(MyClassName, MyWindowTitle) )
    return 0;

if ( !init(this_instance) )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
return msg.wParam;
}


