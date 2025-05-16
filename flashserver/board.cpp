#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\fifo.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

unsigned short OUT_TO_OPTO        = 0x8017;  /* low byte only since we only have 8 outputs */
unsigned short RD_CUR_INPUT_WORD  = 0x8031;
unsigned short SET_BOARD_TYPE     = 0x801E; /* 0 == single, 1 == mux */

unsigned short MUX_TYPE          = 0x0001;
unsigned short ZERO              = 0x0000;

unsigned short DataPort      = 0x220;
unsigned short CmdStatusPort = 0x222;
unsigned short IntPort       = 0x224;
unsigned short ClrCmdPort    = 0x226;

HANDLE BoardEvent       = 0;
DWORD  WaitMs           = 100;

extern WINDOW_CLASS   MainWindow;
extern unsigned short Input_Bits;

FIFO_CLASS BoardCommands;

/***********************************************************************
*                               SETPORTS                               *
***********************************************************************/
static void setports( unsigned short base_port )
{

DataPort      = base_port;
CmdStatusPort = base_port + 2;
IntPort       = base_port + 4;
ClrCmdPort    = base_port + 6;
}

/***********************************************************************
*                             SET_MUX_TYPE                             *
***********************************************************************/
static void set_mux_type()
{
_outpw( DataPort,      MUX_TYPE );
_outpw( CmdStatusPort, SET_BOARD_TYPE );
_outpw( ClrCmdPort,    ZERO );
Sleep( 1 );
}

/***********************************************************************
*                          WRITE_OUTPUT_BITS                           *
***********************************************************************/
static void write_output_bits( unsigned short bits )
{
_outpw( DataPort,      bits );
_outpw( CmdStatusPort, OUT_TO_OPTO );
_outpw( ClrCmdPort,    ZERO );
Sleep( 1 );
}

/***********************************************************************
*                           READ_INPUT_BITS                            *
***********************************************************************/
static void read_input_bits()
{
unsigned short u;

_outpw( CmdStatusPort, RD_CUR_INPUT_WORD );
_outpw( ClrCmdPort,    ZERO );
Sleep( 1 );
u = (unsigned short) _inpw( DataPort );
if ( u != Input_Bits )
    MainWindow.post( WM_NEW_DATA, (WPARAM) u, 0 );
}

/***********************************************************************
*                              BOARD_THREAD                            *
***********************************************************************/
DWORD board_thread( int * notused )
{
bool    shutting_down;
bool    board_is_enabled;
TCHAR * cp;
unsigned short u;

shutting_down    = false;
board_is_enabled = false;

while ( TRUE )
    {
    while ( BoardCommands.count() )
        {
        cp = (TCHAR *) BoardCommands.pop();
        if ( lstrcmp(cp, TEXT("EnableBoard")) == 0 )
            {
            board_is_enabled = true;
            }
        else if ( lstrcmp(cp, TEXT("DisableBoard")) == 0 )
            {
            board_is_enabled = false;
            }
        else if ( lstrcmp(cp, TEXT("InitBoard")) == 0 )
            {
            if ( board_is_enabled )
                set_mux_type();
            }
        else if ( lstrcmp(cp, TEXT("Exit")) == 0 )
            {
            write_output_bits( 0 );
            shutting_down = true;
            }
        else if ( *cp == TEXT('A') )
            {
            u = (unsigned short) hextoul( cp+1 );
            setports( u );
            }
        else if ( *cp == TEXT('W') )
            {
            if ( board_is_enabled )
                {
                u = (unsigned short) hextoul( cp+1 );
                write_output_bits( u );
                }
            }
        delete cp;
        cp = 0;
        }

    if ( shutting_down )
        break;

    if ( board_is_enabled )
        read_input_bits();

    if ( WaitForSingleObject(BoardEvent, WaitMs) == WAIT_FAILED )
        break;
    }

return 0;
}

/***********************************************************************
*                         START_BOARD_THREAD                           *
***********************************************************************/
bool start_board_thread()
{
DWORD  id;
HANDLE h;

if ( !BoardEvent )
    BoardEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

if ( !BoardEvent )
    return false;

h = CreateThread(
    NULL,                                   /* no security attributes        */
    0,                                      /* use default stack size        */
    (LPTHREAD_START_ROUTINE) board_thread,  /* thread function       */
    0,                                      /* argument to thread function   */
    0,                                      /* use default creation flags    */
    &id );                                  /* returns the thread identifier */

if ( !h )
    return false;

return true;
}

/***********************************************************************
*                        SEND_COMMAND_TO_BOARD                         *
***********************************************************************/
void send_command_to_board( const TCHAR * cmd )
{
TCHAR * buf;

buf = maketext( cmd );
BoardCommands.push( buf );
SetEvent( BoardEvent );
}