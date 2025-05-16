#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\chaxis.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\subs.h"

static TCHAR emptystring[] = TEXT( "" );

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
***********************************************************************/
CHAXIS_LABEL_CLASS::~CHAXIS_LABEL_CLASS()
{
cleanup();
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                 init                                 *
***********************************************************************/
BOOLEAN CHAXIS_LABEL_CLASS::init( int max_n )
{
int i;


cleanup();
if ( max_n > 0 )
    {
    label = new TCHAR *[max_n];
    if ( label )
        {
        n = max_n;
        for ( i=0; i<n; i++ )
            label[i] = 0;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                  set                                 *
***********************************************************************/
BOOLEAN CHAXIS_LABEL_CLASS::set( int i, TCHAR * sorc )
{
if ( i >= 0 && i < n )
    {
    if ( label[i] )
        delete[] label[i];

    label[i] = maketext( sorc );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                cleanup                               *
***********************************************************************/
void CHAXIS_LABEL_CLASS::cleanup( void )
{
int i;

if ( n && label )
    {
    for ( i=0; i<n; i++ )
        {
        if ( label[i] )
            delete[] label[i];
        }
    delete label;
    }

n     = 0;
label = 0;
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                  []                                  *
***********************************************************************/
TCHAR * CHAXIS_LABEL_CLASS::operator[]( int i )
{
if ( i >= 0 && i < n )
    {
    if ( label[i] )
        return label[i];
    }

return emptystring;
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                  get                                 *
***********************************************************************/
BOOLEAN CHAXIS_LABEL_CLASS::get( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
int        count;
FILE_CLASS f;

count = 0;

cleanup();

if ( f.open_for_read(chaxis_label_datname(cn, mn, pn)) )
    {
    while ( f.readline() )
        count++;

    if ( count > 0 )
        {
        f.rewind();
        if ( init(count) )
            {
            count = 0;
            while ( count < n )
                {
                label[count] = maketext( f.readline() );
                count++;
                }
            }
        }
    f.close();
    }

if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           CHAXIS_LABEL_CLASS                         *
*                                  put                                 *
***********************************************************************/
BOOLEAN CHAXIS_LABEL_CLASS::put( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
int        i;
FILE_CLASS f;

if ( n < 1 )
    return FALSE;

if ( f.open_for_write(chaxis_label_datname(cn, mn, pn)) )
    {
    for ( i=0; i<n; i++ )
        f.writeline( label[i] );
    f.close();
    return TRUE;
    }

return FALSE;
}
