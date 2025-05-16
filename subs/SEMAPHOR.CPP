#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

static const DWORD MS_TO_WAIT         = 200;
static const int32 WAIT_TIMEOUT_COUNT = 10;
static TCHAR BackslashChar = TEXT( '\\' );
static DB_TABLE T;

/***********************************************************************
*                         MAKE_SEMAPHOR_DBNAME                         *
***********************************************************************/
static BOOLEAN make_semaphor_dbname( TCHAR * dest, TCHAR * computer_name, TCHAR * machine_name )
{
COMPUTER_CLASS c;
TCHAR * cp;

if ( c.find(computer_name) )
    {
    cp = copystring( dest, c.directory() );
    cp = copystring( cp, machine_name );
    *cp++ = BackslashChar;
    copystring( cp, SEMAPHOR_DB );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_SHOTSAVE_SEMAPHORE                        *
***********************************************************************/
BOOLEAN get_shotsave_semaphore( TCHAR * computer, TCHAR * machine )
{
TCHAR tname[MAX_PATH+1];

if ( T.open_status() )
    return TRUE;

if ( make_semaphor_dbname(tname, computer, machine) )
    {
    if ( !file_exists(tname)  )
        T.create( tname );

    return T.open( tname, SEMAPHOR_RECLEN, FL, NO_RETRIES );
    }

return FALSE;
}

/***********************************************************************
*                       FREE_SHOTSAVE_SEMAPHORE                        *
***********************************************************************/
void free_shotsave_semaphore( void )
{
T.close();
}

/***********************************************************************
*                    WAITFOR_SHOTSAVE_SEMAPHOR                         *
***********************************************************************/
BOOLEAN waitfor_shotsave_semaphor( TCHAR * computer, TCHAR * machine )
{

int32  count;
HANDLE myevent;

/*
---------------------------------------------------
Get a full lock on the semaphor db for this machine
--------------------------------------------------- */
count   = 0;
myevent = 0;
while ( !get_shotsave_semaphore(computer, machine) )
    {
    if ( !myevent )
        {
        /*
        ------------------------------------------
        Create an event that I can use for a timer
        ------------------------------------------ */
        myevent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if ( !myevent )
            return FALSE;
        }

    WaitForSingleObject( myevent, MS_TO_WAIT );
    count++;
    if ( count > WAIT_TIMEOUT_COUNT )
        {
        CloseHandle( myevent );
        return FALSE;
        }
    }

if ( myevent )
    CloseHandle( myevent );

return TRUE;
}

