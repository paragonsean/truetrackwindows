#include <windows.h>
#include "..\include\genlist.h"

/***********************************************************************
*                       ~GENERIC_LIST_CLASS                            *
***********************************************************************/
GENERIC_LIST_CLASS::~GENERIC_LIST_CLASS( void )
{

while ( first )
    {
    prev = first->next;
    delete first;
    first = prev;
    }
}

/***********************************************************************
*                           CREATE_ENTRY                               *
***********************************************************************/
static GENERIC_LIST_ENTRY * create_entry( void * new_item )
{

GENERIC_LIST_ENTRY * ge;

/*
-----------------------------
Allocate a new list structure
----------------------------- */
ge = new GENERIC_LIST_ENTRY;
if ( ge )
    {
    /*
    -------------
    Initialize it
    ------------- */
    ge->item = new_item;
    ge->next = 0;
    }

return ge;
}

/***********************************************************************
*                              APPEND                                  *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::append( void * new_item )
{

GENERIC_LIST_ENTRY * ge;
GENERIC_LIST_ENTRY * gn;

gn = create_entry( new_item );
if ( !gn )
    return FALSE;

if ( !first )
    first = gn;
else
    {
    ge = first;
    while ( ge->next )
        ge = ge->next;
    ge->next = gn;
    }

return TRUE;
}

/***********************************************************************
*                              INSERT                                  *
*              Insert just before the current entry.                   *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::insert( void * new_item )
{

GENERIC_LIST_ENTRY * gn;

gn = create_entry( new_item );
if ( !gn )
    return FALSE;

/*
---------------------------------
If the list is empty, append this
--------------------------------- */
if ( !first )
    {
    first = gn;
    return TRUE;
    }

/*
--------------------------------------------------------------
If there is no current pointer, insert at the head of the list
-------------------------------------------------------------- */
if ( !gp )
    {
    gp = first;
    prev = 0;
    }

gn->next = gp;
if ( prev )
    prev->next = gn;
else
    first = gn;

prev = gn;

return TRUE;
}

/***********************************************************************
*                               NEXT                                   *
***********************************************************************/
void * GENERIC_LIST_CLASS::next( void )
{

if ( !gp && !prev )
    gp = first;

else if ( gp )
    {
    prev = gp;
    gp = gp->next;
    }

if ( gp )
    return gp->item;

return 0;
}

/***********************************************************************
*                               NEXT_LOOP                              *
***********************************************************************/
void * GENERIC_LIST_CLASS::next_loop( void )
{

if ( !gp && !prev )
    gp = first;

else if ( gp )
    {
    prev = gp;
    gp = gp->next;
    }
else
    {
    gp   = first;
    prev = 0;
    }

if ( gp )
    return gp->item;

return 0;
}

/***********************************************************************
*                               FIND                                   *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::find( void * item_to_find )
{

void * p;

/*
---------------------------------------
First, see if this is the current entry
--------------------------------------- */
if ( gp && gp->item == item_to_find )
    return TRUE;

rewind();
while ( TRUE )
    {
    p = next();
    if ( !p )
        break;
    if ( p == item_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             REWIND                                   *
***********************************************************************/
void GENERIC_LIST_CLASS::rewind( void )
{

prev = 0;
gp   = 0;

}

/***********************************************************************
*                              REMOVE                                  *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::remove( void )
{

GENERIC_LIST_ENTRY * copy;

if ( !gp )
    return FALSE;

copy = gp->next;

if ( prev )
    prev->next = gp->next;
else
    first = gp->next;

delete gp;

gp = copy;

return TRUE;
}

/***********************************************************************
*                              REMOVE                                  *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::remove( void * item_to_remove )
{

if ( find(item_to_remove) )
    return remove();

return FALSE;
}

/***********************************************************************
*                    GENERIC_LIST_CLASS::REMOVE_ALL                    *
***********************************************************************/
void GENERIC_LIST_CLASS::remove_all( void )
{

prev = 0;
gp   = 0;

while ( first )
    {
    gp = first->next;
    delete first;
    first = gp;
    }

}

/***********************************************************************
*                      GENERIC_LIST_CLASS::COUNT                       *
***********************************************************************/
short GENERIC_LIST_CLASS::count( void )
{
GENERIC_LIST_ENTRY * g;
short n;

n = 0;
g = first;
while ( g )
    {
    g = g->next;
    n++;
    }

return n;
}

/***********************************************************************
*                                SORT                                  *
***********************************************************************/
BOOLEAN GENERIC_LIST_CLASS::sort( int (* compare)(const void * sorc1, const void * sorc2) )
{
GENERIC_LIST_ENTRY * g;
void ** a;
int nof_items;
int high;
int low;
int middle;
int i;
int n;
int result;

nof_items = count();
if ( nof_items < 2 )
    return TRUE;

/*
-----------------------------------------
Make an array to hold the sorted pointers
----------------------------------------- */
a = new void*[nof_items];
if ( !a )
    return FALSE;

/*
-----------------------------
The zeroth element is a gimme
----------------------------- */
g = first;
a[0] = g->item;
g = g->next;
n = 1;

while ( g )
    {
    low  = 0;
    high = n-1;
    result = compare( g->item, a[low] );
    if ( result < 0 )
        {
        for ( i=n; i>low; i-- )
            a[i] = a[i-1];
        a[low] = g->item;
        }
    else
        {
        result = compare( g->item, a[high] );
        if ( result >= 0 )
            {
            high++;
            for ( i=n; i>high; i-- )
                a[i] = a[i-1];
            a[high] = g->item;
            }
        else
            {
            while ( TRUE )
                {
                if ( low >= (high-1) )
                    {
                    for ( i=n; i>high; i-- )
                        a[i] = a[i-1];
                    a[high] = g->item;
                    break;
                    }
                else
                    {
                    middle = low + high;
                    middle /= 2;
                    result = compare( g->item, a[middle] );
                    if ( result < 0 )
                        high = middle;
                    else
                        low  = middle;
                    }
                }
            }
        }
    n++;
    g = g->next;
    }

i = 0;
g = first;
while ( g )
    {
    g->item = a[i];
    i++;
    g = g->next;
    }

delete [] a;
rewind();
return TRUE;
}
