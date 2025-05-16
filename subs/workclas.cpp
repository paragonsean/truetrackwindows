#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\workclas.h"

static const TCHAR SpaceChar       = TEXT( ' ' );
static       TCHAR UnknownWorker[] = UNKNOWN;

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                                 ADD                                  *
***********************************************************************/
BOOLEAN WORKER_LIST_CLASS::add( TCHAR * new_number, TCHAR * new_firstname, TCHAR * new_lastname )
{

WORKER_ENTRY * previous;
WORKER_ENTRY * mylist;
WORKER_ENTRY * w;

w = new WORKER_ENTRY;
if ( !w )
    return FALSE;

lstrcpyn( w->number,    new_number,    OPERATOR_NUMBER_LEN+1     );
lstrcpyn( w->firstname, new_firstname, OPERATOR_FIRST_NAME_LEN+1 );
lstrcpyn( w->lastname,  new_lastname,  OPERATOR_LAST_NAME_LEN+1  );

/*
------------------------------------------------------
Insert into the list alphabetically by operator number
------------------------------------------------------ */
mylist    = list;
previous  = 0;
w->next   = 0;
while ( mylist )
    {
    if ( compare(w->number, mylist->number, OPERATOR_NUMBER_LEN) < 0 )
        {
        w->next = mylist;
        break;
        }
    previous = mylist;
    mylist = mylist->next;
    }

if ( previous )
    previous->next = w;
else
    list = w;

n++;
return TRUE;
}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                                 LOAD                                 *
***********************************************************************/
BOOLEAN WORKER_LIST_CLASS::load( void )
{
DB_TABLE t;
WORKER_ENTRY w;

t.open( operator_dbname(), OPERATOR_RECLEN, PFL );

while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( w.number,    OPERATOR_NUMBER_OFFSET,     OPERATOR_NUMBER_LEN     );
    t.get_alpha( w.firstname, OPERATOR_FIRST_NAME_OFFSET, OPERATOR_FIRST_NAME_LEN );
    t.get_alpha( w.lastname,  OPERATOR_LAST_NAME_OFFSET,  OPERATOR_LAST_NAME_LEN  );
    add( w.number, w.firstname, w.lastname );
    }

t.close();
return TRUE;
}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                               CLEANUP                                *
***********************************************************************/
void WORKER_LIST_CLASS::cleanup( void )
{
WORKER_ENTRY * w;

n = 0;

while ( list )
    {
    w    = list;
    list = list->next;
    delete w;
    }

}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                              FULL_NAME                               *
***********************************************************************/
TCHAR * WORKER_LIST_CLASS::full_name( TCHAR * number_to_find )
{
static TCHAR s[OPERATOR_FIRST_NAME_LEN + 1 + OPERATOR_LAST_NAME_LEN + 1];

WORKER_ENTRY * w;
TCHAR        * cp;

w = list;
while ( w )
    {
    if ( strings_are_equal(number_to_find, w->number, OPERATOR_NUMBER_LEN) )
        {
        cp = copystring( s, w->firstname );
        *cp++ = SpaceChar;
        copystring( cp, w->lastname );
        break;
        }

    w = w->next;
    }

if ( !w )
    lstrcpy( s, UnknownWorker );

return s;
}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                               FIND                                   *
***********************************************************************/
BOOLEAN WORKER_LIST_CLASS::find( TCHAR * number_to_find )
{
WORKER_ENTRY * w;

w = list;
while ( w )
    {
    if ( strings_are_equal(number_to_find, w->number, OPERATOR_NUMBER_LEN) )
        {
        current_entry = w;
        return TRUE;
        }

    w = w->next;
    }

return FALSE;
}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                              FIRST_NAME                              *
***********************************************************************/
TCHAR * WORKER_LIST_CLASS::first_name()
{
if ( current_entry )
    return current_entry->firstname;

return UnknownWorker;
}

/***********************************************************************
*                          WORKER_LIST_CLASS                           *
*                               LAST_NAME                              *
***********************************************************************/
TCHAR * WORKER_LIST_CLASS::last_name()
{
if ( current_entry )
    return current_entry->lastname;

return UnknownWorker;
}