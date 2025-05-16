#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\iniclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "socket_class.h"
#include "socket_monitor.h"

const char CrChar         = '\r';
const char LfChar         = '\012';
const char NullChar       = '\0';
const char PlusChar       = '+';
const char QuestionChar   = '?';
const char UnderscoreChar = '_';

/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
static bool start_chars_are_equal( const char * sorc, const char * s )
{
while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    if ( *sorc != *s )
        return false;

    sorc++;
    s++;
    }
}

/***********************************************************************
*                            SOCKET_MONITOR                            *
***********************************************************************/
SOCKET_MONITOR::SOCKET_MONITOR()
{
/*
-----------------------
Start out in ascii mode
----------------------- */
current_type       = ASCII_TYPE;
bp                 = asciibuf;
skipping           = false;
skip_count         = 0;
shutting_down      = false;
binarybuf          = 0;
myevent            = CreateEvent( NULL, FALSE, FALSE, NULL );
monitor_is_running = false;
}

/***********************************************************************
*                            ~SOCKET_MONITOR                             *
***********************************************************************/
SOCKET_MONITOR::~SOCKET_MONITOR()
{
if ( myevent )
    {
    CloseHandle( myevent );
    myevent = 0;
    }

if ( binarybuf )
    {
    delete[] binarybuf;
    binarybuf = 0;
    }
}

/***********************************************************************
*                     EXTRACT_NOF_BYTES_FROM_LINE                      *
*                                                                      *
* The lines that start the binary data have the number of bytes        *
* immediately following the third underscore.                          *
* e.g. P_BEGIN_<shot number>_<number of bytes><LF>                     *
***********************************************************************/
int SOCKET_MONITOR::extract_nof_bytes_from_line()
{
int i;
char * cp;

cp = asciibuf;

for ( i=0; i<3; i++ )
    {
    if ( cp )
        {
        cp = findchar( UnderscoreChar, cp );
        if ( cp )
            cp++;
        }
    }

if ( cp )
    return asctoint32( cp );

return 0;
}

/***********************************************************************
*                             post_data                                *
*                                                                      *
* I create a buffer and a struct to hold the info and send them        *
* all to the recv_window, who is responsible for deleting the          *
* buffers when finished with them.                                     *
*                                                                      *
***********************************************************************/
void SOCKET_MONITOR::post_data()
{
NEW_FTII_DATA * nd;
char          * cp;
int             len;

nd = new NEW_FTII_DATA;
if ( nd )
    {
    nd->sorc = socket_id();
    nd->type = current_type;
    if ( current_type == SOCKET_ERROR_TYPE )
        {
        nd->buf = 0;
        nd->len = 0;
        }
    else if ( current_type == ASCII_TYPE )
        {
        /*
        ---------------------------------------------------------------------------------
        Bp currently points to a <LF>. I don't want this so I don't add one to the count.
        --------------------------------------------------------------------------------- */
        len = bp - asciibuf;
        if ( len == 0 && *bp == PlusChar )
            len = 1;
        if ( len > 0 )
            {
            cp = new char[len+1];
            if ( cp )
                {
                memcpy( cp, asciibuf, len );
                *(cp+len) = '\0';
                nd->buf = cp;
                nd->len = len;
                }
            }
        else
            {
            delete nd;
            nd = 0;
            }
        }
    else
        {
        /*
        ------------------------------------------------------------------------------
        This is binary data. Send it to the main window and reset myself to ascii mode
        ------------------------------------------------------------------------------ */
        if ( binarybuf && current_len > 0 )
            {
            nd->buf     = binarybuf;
            nd->len     = current_len;
            binarybuf         = 0;
            }
        else
            {
            delete nd;
            nd = 0;
            }

        current_type = ASCII_TYPE;
        bp          = asciibuf;
        }

    if ( nd )
        destwin.post( WM_NEW_DATA, 0, (LPARAM) nd );
    }
}

/***********************************************************************
*                           DO_ASCII_MESSAGE                           *
***********************************************************************/
void SOCKET_MONITOR::do_ascii_message()
{
const int START_OF_POS_SAMPLES       = 0;
const int START_OF_TIME_SAMPLES      = 1;
const int START_OF_PARAMETERS        = 2;
const int START_OF_OP_STATUS_DATA    = 3;
const int START_OF_OSCILLOSCOPE_DATA = 4;
const int array_count                = 5;

static const char * array[] = {
    { "P_BEGIN" },
    { "T_BEGIN" },
    { "C_BEGIN" },
    { "O_BEGIN" },
    { "S_BEGIN" }
    };

int  i;
int  nof_chars_in_line;

nof_chars_in_line = 1 + bp - asciibuf;

if ( nof_chars_in_line < 2 )
    {
    if ( *bp == PlusChar || *bp == QuestionChar )
        post_data();
    bp = asciibuf;
    return;
    }

/*
-----------------------------------------
Post data doesn't affect the ascii buffer
----------------------------------------- */
post_data();


for ( i=0; i<array_count; i++ )
    {
    if ( start_chars_are_equal(array[i], asciibuf) )
        break;
    }

chars_needed = 0;

switch ( i )
    {
    case START_OF_POS_SAMPLES:
    case START_OF_TIME_SAMPLES:
        chars_needed = extract_nof_bytes_from_line();
        current_type = SAMPLE_TYPE;
        break;

    case START_OF_PARAMETERS:
        chars_needed = extract_nof_bytes_from_line();
        current_type = PARAMETERS_TYPE;
        break;

    case START_OF_OP_STATUS_DATA:
        current_type = OP_STATUS_TYPE;
        chars_needed = sizeof(FTII_OP_STATUS_DATA);
        skip_count = 12 - nof_chars_in_line;
        skipping  = true;
        break;

    case START_OF_OSCILLOSCOPE_DATA:
        current_type = OSCILLOSCOPE_TYPE;
        chars_needed = sizeof(FTII_OSCILLOSCOPE_DATA);
        skip_count   = 12 - nof_chars_in_line;
        skipping     = true;
        break;
    }

if ( current_type != ASCII_TYPE && chars_needed > 0 )
    {
    if ( binarybuf )
        {
        delete[] binarybuf;
        binarybuf = 0;
        }
    current_len = chars_needed;
    binarybuf   = new char[chars_needed];
    bp          = binarybuf;
    }
else
    {
    current_type = ASCII_TYPE;
    bp           = asciibuf;
    }
}

/***********************************************************************
*                         BOARD_MONITOR_THREAD                         *
***********************************************************************/
DWORD SOCKET_MONITOR::board_monitor_thread()
{
static DWORD wait_ms;

wait_ms            = INFINITE;
monitor_is_running = true;

while ( true )
    {
    /*
    -----------------------------------------------------------
    The socket class will wake me up whenever there is new data
    ----------------------------------------------------------- */
    if ( WaitForSingleObject(myevent, wait_ms) == WAIT_FAILED )
        break;

    if ( shutting_down )
        {
        wait_ms = 100;
        if ( !is_running() )
            break;
        }

    if ( !is_running() )
        {
        /*
        ------------------------
        I have been disconnected
        ------------------------ */
        current_type = SOCKET_ERROR_TYPE;

        if ( binarybuf )
            {
            delete[] binarybuf;
            binarybuf = 0;
            }
        current_len = 0;
        bp          = asciibuf;
        post_data();
        current_type = ASCII_TYPE;
        break;
        }

    while ( outptr != inptr )
        {
        if ( skipping )
            {
            skip_count--;
            if ( !skip_count )
                skipping = false;
            }
        else
            {
            *bp = *outptr;
            if ( current_type == ASCII_TYPE )
                {
                if ( *bp == LfChar || (bp==asciibuf && *bp == PlusChar ) || (bp==asciibuf && *bp == QuestionChar ) )
                    {
                    do_ascii_message();
                    }
                else if ( *bp != CrChar ) // && *bp != PlusChar )
                    bp++;
                }
            else
                {
                chars_needed--;
                if ( !chars_needed )
                    post_data();
                else
                    bp++;
                }
            }
        /*
        --------------------------------------------
        I maintain the output pointer for the socket
        -------------------------------------------- */
        if ( outptr == lastchar )
            outptr = buf;
        else
            outptr++;
        }
    }

monitor_is_running = false;
stopwin.close();
return 0;
}

/***********************************************************************
*                    START_BOARD_MONITOR_THREAD                        *
***********************************************************************/
DWORD start_board_monitor_thread( void * sp )
{
SOCKET_MONITOR * s;

s = (SOCKET_MONITOR *) sp;
return s->board_monitor_thread();
}

/***********************************************************************
*                            SOCKET_MONITOR                            *
*                           BEGIN_MONITORING                           *
***********************************************************************/
bool SOCKET_MONITOR::begin_monitoring( WINDOW_CLASS & notification_window )
{
HANDLE h;
DWORD  id;

destwin = notification_window;

if ( monitor_is_running )
    return true;

shutting_down = false;

h = CreateThread(
    NULL,                                         /* no security attributes        */
    0,                                            /* use default stack size        */
    (LPTHREAD_START_ROUTINE) start_board_monitor_thread,   /* thread function       */
    (void *) this,                                /* argument to thread function   */
    0,                                            /* use default creation flags    */
    &id );                                        /* returns the thread identifier */

if ( !h )
    return false;

return true;
}

/***********************************************************************
*                            SOCKET_MONITOR                            *
*                            STOP_MONITORING                           *
***********************************************************************/
void  SOCKET_MONITOR::stop_monitoring()
{
if ( is_running() )
    close( 0 );

if ( monitor_is_running && myevent )
    {
    shutting_down = true;
    SetEvent( myevent );
    }
}

/***********************************************************************
*                            SOCKET_MONITOR                            *
*                             OPEN_SOCKET                              *
***********************************************************************/
bool SOCKET_MONITOR::open_socket( char * ascii_tcp_ip_address, u_short port )
{
if ( open(myevent, 131071, ascii_tcp_ip_address, port) )
    return true;

return false;
}

/***********************************************************************
*                            SOCKET_MONITOR                            *
*                             CLOSE_SOCKET                             *
* I have the socket call my close callback, which I use to kill myself *
***********************************************************************/
void SOCKET_MONITOR::close_socket( void (*new_close_complete_callback)() )
{
close( new_close_complete_callback );
}

