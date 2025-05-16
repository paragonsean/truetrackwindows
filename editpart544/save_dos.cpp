#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\part.h"
#include "..\include\stparam.h"
#include "..\include\stringcl.h"
#include "..\include\stsetup.h"
#include "..\include\subs.h"

#include "resource.h"

static TCHAR EmptyString[]   = TEXT( "" );
static TCHAR XString[]       = TEXT( "X" );
static TCHAR PercentString[] = TEXT( "%" );
static TCHAR StarDotStar[]   = TEXT( "*.*" );

static TCHAR DestP42Directory[]     = TEXT( "a:\\p42" );
static TCHAR DestProfileDirectory[] = TEXT( "a:\\profile" );
static TCHAR SorcP42Directory[]     = TEXT( "c:\\p42" );
static TCHAR SorcProfileDirectory[] = TEXT( "c:\\p42\\profile" );

static TCHAR DestRoot[]     = TEXT( "a:\\" );
static TCHAR SorcRoot[]     = TEXT( "c:\\P42" );

static TCHAR * FileList[] = {
    TEXT( "autoexec.bat" ),
    TEXT( "config.sys" ),
    TEXT( "install.bat" )
    };

static int32 FileListCount = 3;

static FILE_CLASS F;

/***********************************************************************
*                              COPY_FILES                              *
***********************************************************************/
static BOOLEAN copy_files( TCHAR * dest_directory, TCHAR * sorc_directory, TCHAR ** sorcfile, int32 n, HWND w )
{
TCHAR * d;
TCHAR * s;
int32   i;
int32   j;
int32   slen;
BOOLEAN status;

status = TRUE;

slen = 0;
for ( i=0; i<n; i++ )
    {
    j = lstrlen( sorcfile[i] );
    if ( j > slen )
        slen = j;
    }
slen ++;

d = maketext( lstrlen(dest_directory) + slen );
s = maketext( lstrlen(sorc_directory) + slen );
if ( s && d )
    {
    for ( i=0; i<n; i++ )
        {
        lstrcpy( d, dest_directory );
        append_filename_to_path( d, sorcfile[i] );

        lstrcpy( s, sorc_directory );
        append_filename_to_path( s, sorcfile[i] );

        if ( w )
            set_text( w, MSDOS_BU_STATUS_TBOX, sorcfile[i] );

        /*
        ----------------------------------------------------
        If the file does not exist in the c:\p42 directory I
        assume it is not needed. This is normally true for
        config.sys only.
        ---------------------------------------------------- */
        if ( file_exists(s) )
            if ( CopyFile(s, d, FALSE) == 0 )
                status = FALSE;
        }

    }

if ( s )
    delete[] s;

if ( d )
    delete[] d;

return status;
}

/***********************************************************************
*                                WRITE                                 *
***********************************************************************/
BOOLEAN write( float x, TCHAR * suffix )
{
STRING_CLASS s;
s = ascii_float(x);
s += suffix;
return F.writeline( s.text() );
}

/***********************************************************************
*                                WRITE                                 *
***********************************************************************/
BOOLEAN write( float x )
{
return F.writeline( ascii_float(x) );
}

/***********************************************************************
*                                WRITE                                 *
***********************************************************************/
BOOLEAN write( int32 x )
{
return F.writeline( int32toasc(x) );
}

/***********************************************************************
*                                WRITE                                 *
***********************************************************************/
BOOLEAN write( uint32 x )
{
return F.writeline( ultoascii((unsigned long) x) );
}

/***********************************************************************
*                          SAVE_DOS_SURETRAK                           *
***********************************************************************/
BOOLEAN save_dos_suretrak( PART_CLASS & part, SURETRAK_SETUP_CLASS & setup, SURETRAK_PARAM_CLASS param, HWND w )
{
static TCHAR destfile[]    = TEXT( "A:\\P42\\CONTROL.PRG"  );
static TCHAR paramsfile[]  = TEXT( "A:\\P42\\params.sav"   );
static TCHAR lastprofile[] = TEXT( "A:\\P42\\last_pro.pro" );

int32 i;
int32 j;
TCHAR * cp;
BOOLEAN status;

status = TRUE;

if ( !directory_exists(DestP42Directory) )
    create_directory( DestP42Directory );

if ( !directory_exists(DestProfileDirectory) )
    create_directory( DestProfileDirectory );

if ( directory_exists(SorcP42Directory) )
    if ( !copy_all_files(DestP42Directory, SorcP42Directory, StarDotStar, w, MSDOS_BU_STATUS_TBOX) )
        status = FALSE;

if ( status )
    if ( directory_exists(SorcProfileDirectory) )
        if ( !copy_all_files(DestProfileDirectory, SorcProfileDirectory, StarDotStar, w, MSDOS_BU_STATUS_TBOX) )
            status = FALSE;

if ( status )
    if ( directory_exists(SorcRoot) )
        if ( !copy_files(DestRoot, SorcRoot, FileList, FileListCount, w) )
            status = FALSE;

if ( status )
    {
    set_text( w, MSDOS_BU_STATUS_TBOX, paramsfile );
    if ( F.open_for_write(paramsfile) )
        {
        write( setup.velocity_loop_gain() );

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            write( setup.gain_break_velocity(i) );

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            write( setup.gain_break_gain(i) );

        write( (float) 0.0 );
        write( (float) 0.0 );
        write( param.velocity_dividend() );
        write( (int32) 0 );
        write( param.zero_speed_check_pos() );
        write( param.counts_per_unit() );
        write( param.null_dac_offset() );
        write( param.lvdt_dac_offset() );
        write( param.jog_valve_percent(), PercentString );
        write( param.retract_valve_percent(), PercentString  );
        write( param.ft_valve_percent(), PercentString  );
        write( param.vel_multiplier() );
        for ( i=0; i<3; i++ )
            F.writeline( EmptyString );

        write( param.min_low_impact_percent(), PercentString );
        for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
            F.writeline( param.limit_switch_desc(i) );

        if ( F.error() != ERROR_SUCCESS )
            status = FALSE;

        F.close();
        }
    }

if ( status )
    {
    set_text( w, MSDOS_BU_STATUS_TBOX, lastprofile );
    if ( F.open_for_write(lastprofile) )
        {
        setup.rewind();
        while ( setup.next() )
            {
            F.writeline( setup.accel_string() );
            F.writeline( setup.vel_string() );
            F.writeline( setup.end_pos_string() );

            if ( setup.have_vacuum_wait() )
                cp = XString;
            else
                cp = EmptyString;
            F.writeline( cp );

            F.writeline( setup.low_impact_string() );

            if ( setup.have_low_impact_from_input() )
                cp = XString;
            else
                cp = EmptyString;
            F.writeline( cp );
            }

        /*
        -----------------------
        Fill in any empty steps
        ----------------------- */
        for ( i = setup.nof_steps(); i<MAX_ST_STEPS; i++ )
            for ( j=0; j<6; j++ )
                F.writeline( EmptyString );

        F.writeline( part.name );
        F.writeline( EmptyString );
        F.writeline( EmptyString );

        for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
            F.writeline( setup.limit_switch_pos_string(i) );

        if ( F.error() != ERROR_SUCCESS )
            status = FALSE;

        F.close();
        }
    }

if ( status )
    if ( CopyFile( control_program_name( part.computer ), destfile, FALSE ) == 0 )
        status = FALSE;

return status;
}

