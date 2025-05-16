#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\names.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR ThisComputerKey[] = TEXT( "ThisComputer" );
static TCHAR BackSlashChar     = TEXT( '\\' );
static TCHAR CommaChar         = TEXT( ',' );
static TCHAR CommaString[]     = TEXT( "," );
static TCHAR NullChar          = TEXT( '\0' );

static TCHAR   MyComputer[COMPUTER_NAME_LEN+1];

static short   NofComputers;
static COMPUTER_ENTRY * First = 0;

/***********************************************************************
*                       COMPUTER_CLASS::SHUTDOWN                       *
***********************************************************************/
void COMPUTER_CLASS::shutdown()
{
empty();
}

/***********************************************************************
*                     COMPUTER_CLASS::EMPTY                            *
***********************************************************************/
void COMPUTER_CLASS::empty()
{
COMPUTER_ENTRY * copy;
COMPUTER_ENTRY * ce;

copy         = First;
NofComputers = 0;
First        = 0;

while ( copy )
    {
    ce = copy->next;
    delete copy;
    copy = ce;
    }
}

/***********************************************************************
*                               LOAD                                   *
*  This reloads the computer list.                                     *
*  It does not change my name.                                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::load()
{
STRING_CLASS s;
TCHAR      name[COMPUTER_NAME_LEN+1];
TCHAR      directory[MAX_PATH+1];
TCHAR    * cp;
FILE_CLASS f;
DB_TABLE   t;

empty();

s = computer_csvname();
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
                cp++;
                copystring( directory, cp );
                add( name, directory );
                }
            }
        }
    f.close();
    }
else
    {
    if ( t.open( computer_dbname(), COMPUTER_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( name,      COMPUTER_NAME_OFFSET, COMPUTER_NAME_LEN );
            t.get_alpha( directory, COMPUTER_DIR_OFFSET,  OLD_COMPUTER_DIR_LEN );
            add( name, directory );
            }

        t.close();
        }
    }

if ( NofComputers )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       COMPUTER_CLASS::STARTUP                        *
*                                                                      *
*            Names_startup should be called before this.               *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::startup( TCHAR * exepath )
{
TCHAR *   cp;
int          n;
INI_CLASS    ini;
STRING_CLASS s;

lstrcpy( MyComputer, NO_COMPUTER );

s = exepath;
s.cat_path( VisiTrakIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(ThisComputerKey) )
    {
    s = ini.get_string();
    cp = s.text();
    if ( !unknown(cp) )
        {
        n = lstrlen( cp );
        if ( n > 0 && n <= COMPUTER_NAME_LEN )
            lstrcpy( MyComputer, cp );
        }
    }

return load();
}

/***********************************************************************
*                       COMPUTER_CLASS::STARTUP                        *
*                                                                      *
*            Names_startup should be called before this.               *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::startup()
{
TCHAR * cp;
int     n;

lstrcpy( MyComputer, NO_COMPUTER );

cp = get_ini_string( VisiTrakIniFile, ConfigSection, ThisComputerKey );
if ( !unknown(cp) )
    {
    n = lstrlen( cp );
    if ( n > 0 && n <= COMPUTER_NAME_LEN )
        lstrcpy( MyComputer, cp );
    }

return load();
}

/***********************************************************************
*                    COMPUTER_CLASS::WHOAMI                            *
***********************************************************************/
TCHAR * COMPUTER_CLASS::whoami()
{
return MyComputer;
}

/***********************************************************************
*                          COMPUTER_CLASS                              *
*                         IS_THIS_COMPUTER                             *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_this_computer()
{
if ( c )
    if ( lstrcmp(c->name, MyComputer) == 0 )
        return TRUE;

return FALSE;
}

/***********************************************************************
*                        COMPUTER_CLASS::COUNT                         *
***********************************************************************/
short COMPUTER_CLASS::count()
{
return NofComputers;
}

/***********************************************************************
*                         COMPUTER_CLASS::FIND                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::find( TCHAR * name )
{
rewind();
while ( next() )
    {
    if ( lstrcmp(name, c->name) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                 COMPUTER_CLASS::FIND_DIRECTORY                       *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::find_directory( const TCHAR * dir_to_find )
{
short len;

rewind();
while ( next() )
    {
    len = lstrlen( c->directory );
    if ( len > 0 )
        {
        if ( strings_are_equal(c->directory, dir_to_find, len) )
            return TRUE;
        }
    }

return FALSE;
}


/***********************************************************************
*                      COMPUTER_CLASS::DIRECTORY                       *
***********************************************************************/
TCHAR * COMPUTER_CLASS::directory( TCHAR * name )
{
if ( find(name) )
    return c->directory;

return 0;
}

/***********************************************************************
*                      COMPUTER_CLASS::DIRECTORY                       *
***********************************************************************/
TCHAR * COMPUTER_CLASS::directory()
{
if ( c )
    return c->directory;

return 0;
}

/***********************************************************************
*                        COMPUTER_CLASS::REWIND                        *
***********************************************************************/
void COMPUTER_CLASS::rewind()
{
search_mode = GET_FIRST_COMPUTER;
}

/***********************************************************************
*                         COMPUTER_CLASS::NEXT                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::next()
{

if ( search_mode == GET_FIRST_COMPUTER )
    {
    search_mode = GET_NEXT_COMPUTER;
    c = First;
    }
else
    {
    c = c->next;
    }

if ( c )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         COMPUTER_CLASS::NAME                         *
***********************************************************************/
TCHAR * COMPUTER_CLASS::name()
{
if ( c )
    return c->name;

return 0;
}

/***********************************************************************
*                         COMPUTER_CLASS::ADD                          *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::add( TCHAR * new_name, TCHAR * new_directory )
{
TCHAR            nc;
COMPUTER_ENTRY * ce;
COMPUTER_ENTRY * nce;
COMPUTER_ENTRY * prev;
BOOLEAN          need_to_insert;

/*
--------------------------------------------------------
If I already have an entry for this computer, update the
directory name only.
-------------------------------------------------------- */
if ( find(new_name) )
    {
    lstrcpyn( c->directory, new_directory, MAX_PATH+1 );
    return TRUE;
    }

nce = new COMPUTER_ENTRY;
if ( nce )
    {
    nc = *new_directory;
    lstrcpyn( nce->name,      new_name,      COMPUTER_NAME_LEN+1 );
    lstrcpyn( nce->directory, new_directory, MAX_PATH+1  );

    ce   = First;
    prev = 0;

    while ( ce )
        {
        need_to_insert = FALSE;

        if ( *(ce->directory) == BackSlashChar )
            {
            /*
            ------------------------------------------------------------------------
            Insert the local directory (C:) in front of all network directories (\\)
            ------------------------------------------------------------------------ */
            if (nc != BackSlashChar )
                need_to_insert = TRUE;
            else if ( lstrcmp(new_name, ce->name) < 0 )
                need_to_insert = TRUE;
            }
        else if( (nc != BackSlashChar) && (lstrcmp(new_name, ce->name) < 0) )
            {
            /*
            ------------------------------------------------------------------
            If I am here it means that the new computer is local and so is the
            current list entry. I should never get here as there should only
            be one local computer.
            ------------------------------------------------------------------ */
            need_to_insert = TRUE;
            }

        if ( need_to_insert )
            {
            /*
            -----------
            Insert here
            ----------- */
            nce->next = ce;
            if ( prev )
                prev->next = nce;
            else
                First = nce;

            NofComputers++;
            return TRUE;
            }

        prev = ce;
        ce = ce->next;
        }

    /*
    ---------------------
    Append to end of list
    --------------------- */
    nce->next = 0;
    if ( prev )
        prev->next = nce;
    else
        First = nce;

    NofComputers++;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      COMPUTER_CLASS::IS_PRESENT                      *
*                                                                      *
*       Checks to see if I can access this computer directory          *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_present()
{
if ( c )
    {
    c->online = directory_exists( c->directory );
    return c->online;
    }

return FALSE;
}

/***********************************************************************
*                      COMPUTER_CLASS::IS_PRESENT                      *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_present( TCHAR * co )
{
if ( find(co) )
    return is_present();

return FALSE;
}

/***********************************************************************
*                         COMPUTER_CLASS::SAVE                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::save()
{
STRING_CLASS s;
FILE_CLASS   f;
BOOLEAN      status;

rewind();
status = FALSE;

s = computer_csvname();
f.open_for_write( s.text() );
while ( next() )
    {
    s = c->name;
    s += CommaString;
    s += c->directory;
    status = f.writeline( s.text() );
    }
f.close();

return status;
}

/***********************************************************************
*                            ONLINE_STATUS                             *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::online_status()
{
if ( c )
    return c->online;
return FALSE;
}

/***********************************************************************
*                            ONLINE_STATUS                             *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::online_status( TCHAR * co )
{
COMPUTER_ENTRY * ce;

if ( c )
    {
    if ( lstrcmp(co, c->name) == 0 )
        return c->online;
    }

ce = First;
while ( ce )
    {
    if ( lstrcmp(co, ce->name) == 0 )
        return ce->online;
    ce = ce->next;
    }

return FALSE;
}

/***********************************************************************
*                         SET_ONLINE_STATUS                            *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::set_online_status( BOOLEAN new_status )
{
if ( c )
    {
    c->online = new_status;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SET_ONLINE_STATUS                            *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::set_online_status( TCHAR * co, BOOLEAN new_status )
{
COMPUTER_ENTRY * ce;

if ( c )
    {
    if ( lstrcmp(co, c->name) == 0 )
        {
        c->online = new_status;
        return TRUE;
        }
    }

ce = First;
while ( ce )
    {
    if ( lstrcmp(co, ce->name) == 0 )
        {
        ce->online = new_status;
        return TRUE;
        }
    ce = ce->next;
    }

return FALSE;
}
