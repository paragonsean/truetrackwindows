#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR ZeroChar      = TEXT( '0' );
static TCHAR Display[] = TEXT( "display" );
static TCHAR Exes[]    = TEXT( "exes" );
static TCHAR Results[] = TEXT( "\\results" );

BOOLEAN get_symphony_machine_dir( TCHAR * computer, TCHAR * machine );
/***********************************************************************
*                             GET_V5_DIRECTORY                         *
*                           \\data-archiver\v5ds\                      *
***********************************************************************/
BOOLEAN NAME_CLASS::get_v5_directory( TCHAR * computer_name )
{
COMPUTER_CLASS c;
TCHAR * cp;
int     count;

if ( c.find(computer_name) )
    {
    STRING_CLASS::operator=( c.directory() );
    cp = text();
    if ( *cp == NullChar )
        return FALSE;

    /*
    ---------------------------------
    Skip backslashes at the beginning
    --------------------------------- */
    while ( *cp == BackslashChar )
        cp++;

    count = 0;
    while ( *cp != NullChar )
        {
        if ( *cp == BackslashChar )
            {
            count++;
            if ( count == 2 )
                break;
            }
        cp++;
        }

    if ( count == 2 )
        {
        cp++;
        *cp = NullChar;

        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                        GET_EXE_DIR_FILE_NAME                         *
*                 \\data-archiver\v5\exes\visitrak.ini                 *
***********************************************************************/
BOOLEAN NAME_CLASS::get_exe_dir_file_name( TCHAR * computer_name, TCHAR * file_name )
{
if ( get_v5_directory(computer_name) )
    {
    STRING_CLASS::operator+=( Exes );

    if ( file_name )
        cat_path( file_name );

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_EXE_DIR_FILE_NAME                         *
*                        c:\v5\exes\visitrak.ini                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_exe_dir_file_name( TCHAR * file_name )
{
COMPUTER_CLASS c;

return get_exe_dir_file_name( c.whoami(), file_name );
}

/***********************************************************************
*                      GET_DISPLAY_DIR_FILE_NAME                       *
*                 \\data-archiver\v5\display\display.ini               *
***********************************************************************/
BOOLEAN NAME_CLASS::get_display_dir_file_name( TCHAR * computer, TCHAR * file_name )
{

if ( get_v5_directory(computer) )
    {
    STRING_CLASS::operator+=( Display );

    if ( file_name )
        cat_path( file_name );

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      GET_DISPLAY_DIR_FILE_NAME                       *
*                 \\data-archiver\v5\display\display.ini               *
***********************************************************************/
BOOLEAN NAME_CLASS::get_display_dir_file_name( TCHAR * computer, TCHAR * machine,  TCHAR * file_name )
{

if ( get_v5_directory(computer) )
    {
    STRING_CLASS::operator+=( Display );

    if ( machine )
        {
        cat_path( machine );
        if ( file_name )
            cat_path( file_name );
        }

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    GET_SYMPHONY_MACHINE_DIRECTORY                    *
*                         c:\v5\symphony\e20\                          *
*                    if machine is 0 then c:\v5\symphony\              *
***********************************************************************/
BOOLEAN NAME_CLASS::get_symphony_machine_directory( TCHAR * computer, TCHAR * machine )
{
static TCHAR symphony_dir_name[] = TEXT( "symphony" );

if ( get_v5_directory(computer) )
    {
    STRING_CLASS::cat_w_char( symphony_dir_name, BackslashChar );
    if ( machine )
        STRING_CLASS::cat_w_char( machine, BackslashChar );
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                        GET_ROOT_DIR_FILE_NAME                        *
*                        \\w01\v5\data\file_name                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_root_dir_file_name( TCHAR * computer_name, TCHAR * file_name )
{
COMPUTER_CLASS c;
TCHAR * start;
TCHAR * cp;

if ( c.find(computer_name) )
    {
    STRING_CLASS::operator=( c.directory() );
    start = text();
    cp = start;
    if ( *cp == NullChar )
        return FALSE;

    cp += len();

    /*
    ---------------------------------------------
    Skip the last character, which is a backslash
    --------------------------------------------- */
    cp--;

    /*
    --------------------------------
    Backup to the previous backslash
    -------------------------------- */
    while ( cp > start )
        {
        cp--;
        if ( *cp == BackslashChar )
            {
            cp++;
            *cp = NullChar;
            break;
            }
        }

    if ( file_name )
        STRING_CLASS::operator+=( file_name );

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_ROOT_DIR_FILE_NAME                        *
*                        \\w01\v5\data\file_name                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_root_dir_file_name( TCHAR * file_name )
{
TCHAR cn[COMPUTER_NAME_LEN+1];
COMPUTER_CLASS c;

copystring( cn, c.whoami() );
return get_root_dir_file_name( cn, file_name );
}

/***********************************************************************
*                      GET_COMPUTER_DIR_FILE_NAME                      *
***********************************************************************/
BOOLEAN NAME_CLASS::get_computer_dir_file_name( TCHAR * computer_name, TCHAR * file_name )
{
COMPUTER_CLASS c;

if ( c.find(computer_name) )
    {
    STRING_CLASS::operator=( c.directory() );
    if ( isempty() )
        return FALSE;

    if ( file_name )
        STRING_CLASS::operator+=( file_name );

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      GET_MACHINE_DIR_FILE_NAME                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name )
{
if ( get_computer_dir_file_name(computer_name, NULL) )
    {
    if ( machine_name )
        {
        STRING_CLASS::operator+=( machine_name );
        if ( file_name )
            cat_path( file_name );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_PART_DIR_FILE_NAME                        *
***********************************************************************/
BOOLEAN NAME_CLASS::get_part_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * file_name )
{
if ( get_machine_dir_file_name(computer_name, machine_name, NULL) )
    {
    if ( part_name )
        {
        cat_path( part_name );
        if ( file_name )
            cat_path( file_name );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    GET_PART_RESULTS_DIR_FILE_NAME                    *
***********************************************************************/
BOOLEAN NAME_CLASS::get_part_results_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * file_name )
{

if ( get_part_dir_file_name(computer_name, machine_name, part_name, NULL) )
    {
    STRING_CLASS::operator+=( Results );
    if ( file_name )
        cat_path( file_name );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                  GET_MACHINE_RESULTS_DIR_FILE_NAME                   *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_results_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name )
{
if ( get_machine_dir_file_name(computer_name, machine_name, NULL) )
    {
    STRING_CLASS::operator+=( Results );
    if ( file_name )
        cat_path( file_name );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      GET_MACHINE_LIST_FILE_NAME                      *
* This file is saved in the local exe directory and contains a list    *
* of the machines at the sorc_computer the last time it was online.    *
* c:\v5\exes\m02_machine_list.txt                                      *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_list_file_name( TCHAR * co )
{
COMPUTER_CLASS c;

if ( get_exe_dir_file_name(c.whoami(), co) )
    {
    STRING_CLASS::operator+=( TEXT("_machine_list.txt") );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           GET_PROFILE_NAME                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, const TCHAR * shot_name, const TCHAR * suffix )
{
TCHAR shot_file[SHOT_LEN+5];

lstrcpy( shot_file, shot_name );
if ( lstrlen(shot_file) < SHOT_LEN )
    ::rjust( shot_file, SHOT_LEN, ZeroChar );
lstrcat( shot_file, suffix );

return get_part_results_dir_file_name( co, ma, pa, shot_file );
}

/***********************************************************************
*                           GET_PROFILE_NAME                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, int32 shot_number, const TCHAR * suffix )
{
TCHAR shot_file[SHOT_LEN+5];
TCHAR * cp;

insalph( shot_file, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
cp = shot_file + SHOT_LEN;
copystring( cp, suffix );

return get_part_results_dir_file_name( co, ma, pa, shot_file );
}
