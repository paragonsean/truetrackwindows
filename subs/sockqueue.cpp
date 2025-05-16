#include <windows.h>

#include "..\include\sockqueue.h"

/***********************************************************************
*                           NEW_SOCKET_ENTRY                           *
***********************************************************************/
SOCKET_ENTRY * new_socket_entry( DWORD buflen )
{
SOCKET_ENTRY * se;

if ( buflen )
    {
    se = new SOCKET_ENTRY;
    if ( se )
        {
        se->bp = new byte[buflen];
        if ( se->bp )
            {
            se->len = buflen;
            return se;
            }
        delete se;
        }
    }

return 0;
}

/***********************************************************************
*                            SOCKET_ENTRY                              *
***********************************************************************/
SOCKET_ENTRY::SOCKET_ENTRY( int buflen )
{
next = 0;
type = 0;
bp   = 0;
len  = 0;

if ( buflen > 0 )
    {
    bp = new byte[buflen];
    if ( bp )
        len = buflen;
    }

}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
***********************************************************************/
SOCKET_ENTRY_QUEUE::SOCKET_ENTRY_QUEUE()
{
first = 0;
InitializeCriticalSection( &my_critical_section );
}

/***********************************************************************
*                         ~SOCKET_ENTRY_QUEUE                          *
***********************************************************************/
SOCKET_ENTRY_QUEUE::~SOCKET_ENTRY_QUEUE()
{
clear();
DeleteCriticalSection( &my_critical_section );
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                CLEAR                                 *
***********************************************************************/
void  SOCKET_ENTRY_QUEUE::clear( void )
{
SOCKET_ENTRY * se;

while ( TRUE )
    {
    se = pop();
    if ( !se )
        break;
    delete se;
    }
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                 PUSH                                 *
***********************************************************************/
void  SOCKET_ENTRY_QUEUE::push( SOCKET_ENTRY * sorc )
{
if ( !sorc )
    return;

EnterCriticalSection( &my_critical_section );

sorc->next = first;

first = sorc;

LeaveCriticalSection( &my_critical_section );
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                  POP                                 *
***********************************************************************/
SOCKET_ENTRY * SOCKET_ENTRY_QUEUE::pop( void )
{
SOCKET_ENTRY * prev;
SOCKET_ENTRY * sp;

sp   = 0;

if ( first )
    {
    prev = 0;

    EnterCriticalSection( &my_critical_section );

    sp = first;
    while ( sp->next )
        {
        prev = sp;
        sp   = sp->next;
        }

    if ( prev )
        prev->next = 0;
    else
        first = 0;

    LeaveCriticalSection( &my_critical_section );
    }

return sp;
}
