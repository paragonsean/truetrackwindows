#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\downtime.h"
#include "..\include\shotclas.h"

#include "resource.h"
#include "proclass.h"

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
***********************************************************************/
PROFILE_MERGE_CLASS::PROFILE_MERGE_CLASS()
{
dir = 0;
}

/***********************************************************************
*                         ~PROFILE_MERGE_CLASS                         *
***********************************************************************/
PROFILE_MERGE_CLASS::~PROFILE_MERGE_CLASS()
{
t.close();
if ( dir )
    {
    delete[] dir;
    dir = 0;
    }
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                            OPEN_GRAPHLST                             *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::open_graphlst( TCHAR * directory_name, short table_lock_type )
{
TCHAR fname[MAX_PATH+1];

t.close();
have_current_shot = FALSE;

dir = maketext( lstrlen(directory_name) );
if ( !dir )
    return FALSE;
lstrcpy( dir, directory_name );
lstrcpy( fname, dir );
append_filename_to_path( fname, GRAPHLST_DB );

if ( !file_exists(fname) )
    t.create( fname );

return t.open( fname, GRAPHLST_RECLEN, table_lock_type );
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::next( void )
{
if ( t.get_next_record(NO_LOCK) )
    {
    have_current_shot = current_shot.get_from_graphlst( t );
    }
else
    {
    have_current_shot = FALSE;
    }

return have_current_shot;
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                  GET                                 *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::get( PROFILE_MERGE_CLASS & sorc, int32 new_shot_number )
{
TCHAR d[MAX_PATH+1];
TCHAR s[MAX_PATH+1];
TCHAR * cp;
bool is_ftii;

is_ftii = false;
lstrcpy( s, sorc.dir );
append_filename_to_path( s, ascii_shot_number(sorc.current_shot.number) );
cp = s + lstrlen(s);
lstrcpy( cp, PROFILE_NAME_SUFFIX );
if ( !file_exists(s) )
    {
    lstrcpy( cp, PROFILE_II_SUFFIX );
    is_ftii = true;
    }

lstrcpy( d, dir );
append_filename_to_path( d, ascii_shot_number(new_shot_number) );
if ( is_ftii )
    lstrcat( d, PROFILE_II_SUFFIX );
else
    lstrcat( d, PROFILE_NAME_SUFFIX );

if ( file_exists(d) )
    DeleteFile( d );
MoveFile( s, d );

if ( sorc.current_shot.number != new_shot_number )
    change_shot_number_in_profile( d, new_shot_number );

t.copy_rec( sorc.t );
t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, new_shot_number, SHOT_LEN );
t.rec_append();

have_current_shot = FALSE;

return TRUE;
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                               REWIND                                 *
***********************************************************************/
void PROFILE_MERGE_CLASS::rewind( void )
{
t.reset_search_mode();
have_current_shot = FALSE;
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                           GET_MIN_AND_MAX                            *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::get_min_and_max( void )
{
int32 n;
BOOLEAN status;

status = FALSE;
rewind();
if ( t.get_next_record(NO_LOCK) )
    {
    min.get_from_graphlst( t );
    max = min;

    n = t.nof_recs() - 1;
    if ( t.goto_record_number(n) )
        {
        max.get_from_graphlst( t );
        status = TRUE;
        }
    }

rewind();
return status;
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                  <                                   *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::operator<( const TIME_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

return ( current_shot.time < sorc );
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                  <                                   *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::operator<( const PROFILE_MERGE_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

if ( !sorc.have_current_shot )
    return TRUE;

return ( current_shot.time < sorc.current_shot.time );
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                  ==                                  *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::operator==( const PROFILE_MERGE_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

if ( !sorc.have_current_shot )
    return FALSE;

return ( current_shot.time == sorc.current_shot.time );
}

/***********************************************************************
*                         PROFILE_MERGE_CLASS                          *
*                                  ==                                  *
***********************************************************************/
BOOLEAN PROFILE_MERGE_CLASS::operator==( const TIME_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

return ( current_shot.time == sorc );
}


/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
***********************************************************************/
SHOTPARM_MERGE_CLASS::SHOTPARM_MERGE_CLASS()
{
dir = 0;
}

/***********************************************************************
*                         ~SHOTPARM_MERGE_CLASS                         *
***********************************************************************/
SHOTPARM_MERGE_CLASS::~SHOTPARM_MERGE_CLASS()
{
t.close();

if ( dir )
    {
    delete[] dir;
    dir = 0;
    }
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                            OPEN_SHOTPARM                             *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::open_shotparm( TCHAR * directory_name, short table_lock_type )
{
TCHAR fname[MAX_PATH+1];

t.close();
have_current_shot = FALSE;

dir = maketext( lstrlen(directory_name) );
if ( !dir )
    return FALSE;
lstrcpy( dir, directory_name );
lstrcpy( fname, dir );
append_filename_to_path( fname, SHOTPARM_DB );

if ( !file_exists(fname) )
    t.create( fname );

return t.open( fname, SHOTPARM_RECLEN, table_lock_type );
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::next( void )
{
if ( t.get_next_record(NO_LOCK) )
    {
    have_current_shot = current_shot.get_from_shotparm( t );
    }
else
    {
    have_current_shot = FALSE;
    }

return have_current_shot;
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                  GET                                 *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::get( SHOTPARM_MERGE_CLASS & sorc, int32 new_shot_number )
{

t.copy_rec( sorc.t );
t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, new_shot_number, SHOT_LEN );
t.rec_append();

have_current_shot = FALSE;

return TRUE;
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                               REWIND                                 *
***********************************************************************/
void SHOTPARM_MERGE_CLASS::rewind( void )
{
t.reset_search_mode();
have_current_shot = FALSE;
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                           GET_MIN_AND_MAX                            *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::get_min_and_max( void )
{
int32 n;
BOOLEAN status;

status = FALSE;
rewind();
if ( t.get_next_record(NO_LOCK) )
    {
    min.get_from_shotparm( t );
    max = min;

    n = t.nof_recs() - 1;
    if ( t.goto_record_number(n) )
        {
        max.get_from_shotparm( t );
        status = TRUE;
        }
    }

rewind();
return status;
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                  <                                   *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::operator<( const TIME_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

return ( current_shot.time < sorc );
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                  <                                   *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::operator<( const SHOTPARM_MERGE_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

if ( !sorc.have_current_shot )
    return TRUE;

return ( current_shot.time < sorc.current_shot.time );
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                  ==                                  *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::operator==( const SHOTPARM_MERGE_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

if ( !sorc.have_current_shot )
    return FALSE;

return ( current_shot.time == sorc.current_shot.time );
}

/***********************************************************************
*                         SHOTPARM_MERGE_CLASS                          *
*                                  ==                                  *
***********************************************************************/
BOOLEAN SHOTPARM_MERGE_CLASS::operator==( const TIME_CLASS & sorc )
{
if ( !have_current_shot )
    return FALSE;

return ( current_shot.time == sorc );
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
***********************************************************************/
DOWNTIME_MERGE_CLASS::DOWNTIME_MERGE_CLASS()
{
have_current_downtime = FALSE;
}

/***********************************************************************
*                        ~DOWNTIME_MERGE_CLASS                         *
***********************************************************************/
DOWNTIME_MERGE_CLASS::~DOWNTIME_MERGE_CLASS()
{
t.close();
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::next( void )
{
if ( t.get_next_record(NO_LOCK) )
    {
    have_current_downtime = d.get( t );
    }
else
    {
    have_current_downtime = FALSE;
    }

return have_current_downtime;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                 ADD                                  *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::add( const DOWNTIME_MERGE_CLASS & sorc )
{
d = sorc.d;
d.put( t );
t.rec_append();

have_current_downtime = FALSE;

return TRUE;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                 OPEN                                 *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::open( TCHAR * directory_name, short table_lock_type )
{
TCHAR fname[MAX_PATH+1];

t.close();
have_current_downtime = FALSE;

lstrcpy( fname, directory_name );
append_filename_to_path( fname, DOWNTIME_DB );

if ( !file_exists(fname) )
    t.create( fname );

return t.open( fname, DOWNTIME_RECLEN, table_lock_type );
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                               REWIND                                 *
***********************************************************************/
void DOWNTIME_MERGE_CLASS::rewind( void )
{
t.reset_search_mode();
have_current_downtime = FALSE;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                  <                                   *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::operator<( const DOWNTIME_MERGE_CLASS & sorc )
{
if ( !have_current_downtime )
    return FALSE;

if ( !sorc.have_current_downtime )
    return TRUE;

return d.time < sorc.d.time;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                   <                                  *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::operator<( const TIME_CLASS & sorc )
{
if ( !have_current_downtime )
    return FALSE;

return d.time < sorc;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                  ==                                  *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::operator==( const DOWNTIME_MERGE_CLASS & sorc )
{
if ( !have_current_downtime )
    return FALSE;

if ( !sorc.have_current_downtime )
    return FALSE;

return d.time == sorc.d.time;
}

/***********************************************************************
*                         DOWNTIME_MERGE_CLASS                         *
*                                  ==                                  *
***********************************************************************/
BOOLEAN DOWNTIME_MERGE_CLASS::operator==( const TIME_CLASS & sorc )
{
if ( !have_current_downtime )
    return FALSE;

return d.time == sorc;
}

