#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\catclass.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

static DOWNCAT_NAME_ENTRY  EmptyDowncatEntry;
static DOWNSCAT_NAME_ENTRY EmptyDownscatEntry;

/***********************************************************************
*                            DOWNCAT_NAME_ENTRY                        *
***********************************************************************/
DOWNCAT_NAME_ENTRY::DOWNCAT_NAME_ENTRY()
{
lstrcpy( number, NO_DOWNCAT_NUMBER );
lstrcpy( name,   NO_DOWNCAT_NAME );
};

/***********************************************************************
*                            DOWNCAT_NAME_CLASS                        *
*                                CLEANUP                               *
***********************************************************************/
void DOWNCAT_NAME_CLASS::cleanup( void )
{
if ( p )
    {
    delete[] p;
    p = 0;
    }
n = 0;
}

/***********************************************************************
*                           DOWNCAT_NAME_CLASS                         *
*                                  []                                  *
***********************************************************************/
DOWNCAT_NAME_ENTRY & DOWNCAT_NAME_CLASS::operator[]( int32 i )
{
if ( p && i>=0 && i< n )
    return p[i];

return EmptyDowncatEntry;
}

/***********************************************************************
*                            DOWNCAT_NAME_CLASS                        *
*                                  READ                                *
***********************************************************************/
BOOLEAN DOWNCAT_NAME_CLASS::read( void )
{
DB_TABLE t;
int32    nrecs;
BOOLEAN  status;

cleanup();

status = FALSE;
if ( t.open(downcat_dbname(), DOWNCAT_RECLEN, PFL) )
    {
    nrecs = t.nof_recs();
    if ( nrecs > 0 )
        {
        p = new DOWNCAT_NAME_ENTRY[nrecs];
        if ( p )
            {
            status = TRUE;
            n      = 0;
            while( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( p[n].number, DOWNCAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( p[n].name,   DOWNCAT_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
                n++;
                if ( n >= nrecs )
                    break;
                }
            }
        else
            {
            n=0;
            }
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                            DOWNCAT_NAME_CLASS                        *
*                                  NAME                                *
***********************************************************************/
TCHAR * DOWNCAT_NAME_CLASS::name( TCHAR * cat )
{
static TCHAR no_name[] = NO_DOWNCAT_NAME;
int i;

if ( p )
    {
    for ( i=0; i<n; i++ )
        {
        if ( lstrcmp(p[i].number, cat) == 0 )
            return p[i].name;
        }
    }

return no_name;
}

/***********************************************************************
*                           DOWNSCAT_NAME_ENTRY                        *
***********************************************************************/
DOWNSCAT_NAME_ENTRY::DOWNSCAT_NAME_ENTRY()
{
lstrcpy( catnumber, NO_DOWNCAT_NUMBER );
lstrcpy( subnumber, NO_DOWNCAT_NUMBER );
lstrcpy( name,   NO_DOWNCAT_NAME );
color = 0;
};

/***********************************************************************
*                           DOWNSCAT_NAME_CLASS                        *
*                                CLEANUP                               *
***********************************************************************/
void DOWNSCAT_NAME_CLASS::cleanup( void )
{
if ( p )
    {
    delete[] p;
    p = 0;
    }
n = 0;
}

/***********************************************************************
*                            DOWNSCAT_NAME_CLASS                        *
*                                  READ                                *
***********************************************************************/
BOOLEAN DOWNSCAT_NAME_CLASS::read( void )
{
DB_TABLE t;
int32    nrecs;
BOOLEAN  status;
short    record_length;
TCHAR  * s;
TCHAR    buf[DOWNSCATC_COLOR_LEN+1];

cleanup();

status = FALSE;

/*
-----------------------------------------------------------
Use the new file if it exists. The new file has color info.
----------------------------------------------------------- */
s = downscatc_dbname();
if ( file_exists(s) )
    {
    record_length = DOWNSCATC_RECLEN;
    }
else
    {
    record_length = DOWNSCAT_RECLEN;
    s = downscat_dbname();
    }

if ( t.open(s, record_length, PFL) )
    {
    nrecs = t.nof_recs();
    if ( nrecs > 0 )
        {
        p = new DOWNSCAT_NAME_ENTRY[nrecs];
        if ( p )
            {
            status = TRUE;
            n      = 0;
            while( t.get_next_record(NO_LOCK) )
                {
                /*
                ----------------------------------------------------------------------------
                The lengths of the first three fields are the same in DOWNSCAT AND DOWNSCATC
                ---------------------------------------------------------------------------- */
                t.get_alpha( p[n].catnumber, DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( p[n].subnumber, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( p[n].name,      DOWNSCAT_SUB_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
                if ( record_length == DOWNSCATC_RECLEN )
                    {
                    t.get_alpha( buf, DOWNSCATC_COLOR_OFFSET, DOWNSCATC_COLOR_LEN   );
                    p[n].color = (COLORREF) hextoul( buf );
                    }
                n++;
                if ( n >= nrecs )
                    break;
                }
            }
        else
            {
            n=0;
            }
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                            DOWNSCAT_NAME_CLASS                       *
*                                  INDEX                               *
***********************************************************************/
int32 DOWNSCAT_NAME_CLASS::index( TCHAR * cat, TCHAR * sub )
{
int32 i;

if ( p )
    {
    for ( i=0; i<n; i++ )
        {
        if ( lstrcmp(p[i].catnumber, cat) == 0 && lstrcmp(p[i].subnumber, sub) == 0 )
            return i;
        }
    }

return NO_INDEX;
}

/***********************************************************************
*                            DOWNSCAT_NAME_CLASS                       *
*                                  NAME                                *
***********************************************************************/
TCHAR * DOWNSCAT_NAME_CLASS::name( TCHAR * cat, TCHAR * sub )
{
static TCHAR no_name[] = NO_DOWNCAT_NAME;
int32 i;

i = index( cat, sub );
if ( i != NO_INDEX )
    return p[i].name;

return no_name;
}

/***********************************************************************
*                            DOWNSCAT_NAME_CLASS                       *
*                                  COLOR                               *
***********************************************************************/
COLORREF DOWNSCAT_NAME_CLASS::color( TCHAR * cat, TCHAR * sub )
{
int32 i;

i = index( cat, sub );
if ( i != NO_INDEX )
    return p[i].color;

return 0;
}

/***********************************************************************
*                          DOWNSCAT_NAME_CLASS                         *
*                                  []                                  *
***********************************************************************/
DOWNSCAT_NAME_ENTRY & DOWNSCAT_NAME_CLASS::operator[]( int32 i )
{
if ( p && i>=0 && i< n )
    return p[i];

return EmptyDownscatEntry;
}

