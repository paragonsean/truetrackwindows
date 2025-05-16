#include <windows.h>

#include "..\include\fifo.h"

/***********************************************************************
*                             ~FIFO_CLASS                              *
***********************************************************************/
FIFO_CLASS::FIFO_CLASS()
{
InitializeCriticalSection( &my_critical_section );
first=0;
last=0;
n=0;
}

/***********************************************************************
*                             ~FIFO_CLASS                              *
***********************************************************************/
FIFO_CLASS::~FIFO_CLASS()
{
DeleteCriticalSection( &my_critical_section );
}

/***********************************************************************
*                                PUSH                                  *
***********************************************************************/
BOOLEAN FIFO_CLASS::push( void * new_item )
{
FIFO_ENTRY * f;

if ( !new_item )
    return TRUE;

f = new FIFO_ENTRY;
if ( !f )
    return FALSE;

f->item = new_item;
f->next = 0;
f->prev = 0;

EnterCriticalSection( &my_critical_section );

if ( first )
    {
    f->next = first;
    first->prev = f;
    }

first = f;
n++;
if ( !last )
    last = first;

LeaveCriticalSection( &my_critical_section );

return TRUE;
}

/***********************************************************************
*                                 POP                                  *
***********************************************************************/
void * FIFO_CLASS::pop()
{

FIFO_ENTRY * f;
void       * p;

p = 0;
f = 0;

EnterCriticalSection( &my_critical_section );

if ( last )
    {
    f = last;
    last = last->prev;
    if ( last )
        last->next = 0;
    else
        first = 0;

    n--;
    }

LeaveCriticalSection( &my_critical_section );

if ( f )
    {
    p = f->item;
    delete f;
    }

return p;
}

/***********************************************************************
*                                 PEEK                                 *
***********************************************************************/
void * FIFO_CLASS::peek()
{
if ( last )
    return last->item;

return 0;
}
