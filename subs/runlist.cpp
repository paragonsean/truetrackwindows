#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\runlist.h"

static const TCHAR NullChar     = TEXT( '\0' );
static const TCHAR NChar        = TEXT( 'N' );
static const TCHAR LowerNChar   = TEXT( 'n' );
static       TCHAR ZeroString[] = TEXT( "0" );

static RUNLIST_ENTRY Empty;

/***********************************************************************
*                           RUNLIST_CLASS                              *
*                              cleanup                                 *
***********************************************************************/
void RUNLIST_CLASS::cleanup()
{
if ( list )
    {
    delete[] list;
    list = 0;
    }

n    = 0;
mask = 0;
}

/***********************************************************************
*                           RUNLIST_CLASS                              *
***********************************************************************/
RUNLIST_CLASS::RUNLIST_CLASS( const RUNLIST_CLASS & sorc )
{
list = 0;
n    = 0;

if ( sorc.n > 0 )
    {
    list = new RUNLIST_ENTRY[sorc.n];
    if ( list )
        {
        while ( n < sorc.n )
            {
            list[n].bits = sorc.list[n].bits;
            copystring( list[n].partname, sorc.list[n].partname );
            n++;
            }
        }
    }

mask = sorc.mask;
}

/***********************************************************************
*                          ~RUNLIST_CLASS                              *
***********************************************************************/
RUNLIST_CLASS::~RUNLIST_CLASS()
{
cleanup();
}

/***********************************************************************
*                           RUNLIST_CLASS                              *
*                                 []                                   *
***********************************************************************/
RUNLIST_ENTRY & RUNLIST_CLASS::operator[]( int i )
{
if ( i >= 0 && i < n )
    return list[i];

return Empty;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                          current_part_name                           *
***********************************************************************/
TCHAR * RUNLIST_CLASS::current_part_name()
{
if ( n > 0 )
    return list[0].partname;
else
    return Empty.partname;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                              find_part                               *
***********************************************************************/
TCHAR * RUNLIST_CLASS::find_part( unsigned int suretrak_bits )
{
int i;
unsigned int x;

if ( n < 1 )
    return Empty.partname;

x = (unsigned int) suretrak_bits;
x &= mask;

for ( i=0; i<n; i++ )
    {
    if ( list[i].bits == x )
        return list[i].partname;
    }

/*
---------------------------------------------------------------------
The zeroth entry is the current part. Return this if nothing matches.
--------------------------------------------------------------------- */
return list[0].partname;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                                GET                                   *
***********************************************************************/
BOOLEAN RUNLIST_CLASS::get( TCHAR * tablename )
{
DB_TABLE   t;
TCHAR      buf[MAX_HEX_LEN+1];
int        nof_recs;

cleanup();

if ( file_exists(tablename) )
    {
    if ( t.open( tablename, RUNLIST_RECLEN, PFL) )
        {
        nof_recs = (int) t.nof_recs();
        if ( nof_recs > 0 )
            {
            list = new RUNLIST_ENTRY[nof_recs];
            if ( list )
                {
                while ( t.get_next_record(FALSE) && n < nof_recs )
                    {
                    t.get_alpha( buf, RUNLIST_BITS_OFFSET, MAX_HEX_LEN );
                    list[n].bits = hextoul( buf );
                    mask |= list[n].bits;
                    t.get_alpha( list[n].partname, RUNLIST_PART_NAME_OFFSET, PART_NAME_LEN   );
                    n++;
                    }
                }
            }
        t.close();
        }
    }

if ( list && n < 1 )
    cleanup();

if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                                GET                                   *
***********************************************************************/
BOOLEAN RUNLIST_CLASS::get( TCHAR * co, TCHAR * ma )
{
return get(runlist_dbname(co, ma) );
}

