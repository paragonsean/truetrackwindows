#include <windows.h>
#include <conio.h>
 
#include "..\include\visiparm.h"
#include "..\include\subs.h"

int NofThreads = 0;
int counts[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900 };

class SOCKET_SERVER_CLASS
    {
    private:

    HANDLE send_event;
    int    my_index;
    int    my_count;
    public:

     SOCKET_SERVER_CLASS();
    ~SOCKET_SERVER_CLASS();
    DWORD   send_thread( void );
    BOOLEAN startup( void );
    };

/***********************************************************************
*                          SOCKET_SERVER_CLASS                         *
***********************************************************************/
SOCKET_SERVER_CLASS::SOCKET_SERVER_CLASS( void )
{
send_event = 0;
my_index   = 0;
}

/***********************************************************************
*                         ~SOCKET_SERVER_CLASS                         *
***********************************************************************/
SOCKET_SERVER_CLASS::~SOCKET_SERVER_CLASS( void )
{
if ( send_event )
    {
    CloseHandle( send_event );
    send_event = 0;
    }
}
/***********************************************************************
*                               SEND_THREAD                            *
***********************************************************************/
DWORD SOCKET_SERVER_CLASS::send_thread( void )
{
const DWORD MS_TO_WAIT = 2000;

my_count = 10;
while ( my_count-- )
    {
    counts[my_index]++;
    cprintf( "my_index is %d, my_count is %d\n\r", my_index, counts[my_index] );

    WaitForSingleObject( send_event, MS_TO_WAIT );
    }

cprintf( "Bye bye %d\n\r", my_index );

return 0;
}

/***********************************************************************
*                          SERVER_SEND_THREAD                          *
***********************************************************************/
DWORD server_send_thread( void * sp )
{
SOCKET_SERVER_CLASS * ssc;

ssc = (SOCKET_SERVER_CLASS *) sp;
return ssc->send_thread();
}

/***********************************************************************
*                          SOCKET_SERVER_CLASS                         *
*                                STARTUP                               *
***********************************************************************/
BOOLEAN SOCKET_SERVER_CLASS::startup( void )
{
DWORD  id;
HANDLE h;

my_index = NofThreads;
NofThreads++;

if ( send_event )
    CloseHandle( send_event );

send_event = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !send_event )
    return FALSE;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) server_send_thread,   /* thread function       */
    (void *) this,                                 /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
// TCHAR   buf[103];

SOCKET_SERVER_CLASS s[10];

int i;
for ( i=0; i<10; i++ )
    s[i].startup();

Sleep( 30000 );

for ( i=0; i<NofThreads; i++ )
    cprintf( "i = %d, %d\n\r", i, counts[i] );

cprintf( "\n\r" );

//buf[0] = 10;
//cgets( buf );

return 0;
}

