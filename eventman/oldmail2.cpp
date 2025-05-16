#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\computer.h"
#include "..\include\events.h"
#include "..\include\fifo.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

#include "resource.h"
#include "extern.h"

static const DWORD MS_TO_WAIT = 505;
static const TCHAR MailSlotName[] = TEXT( "\\\\.\\mailslot\\v5\\eventman" );
static TCHAR MailslotSuffix[] = TEXT( "mailslot\\v5\\eventman" );
static TCHAR BackSlashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR NullChar = TEXT( '\0' );
static TCHAR SpaceString[] = TEXT( " " );
static TCHAR ZeroChar      = TEXT( '0' );

static FIFO_CLASS SendData;

class REMOTE_COMPUTER_ENTRY
    {
    public:
    STRING_CLASS name;
    STRING_CLASS directory;
    BOOLEAN      is_online;
    REMOTE_COMPUTER_ENTRY(){ is_online = TRUE; }
    ~REMOTE_COMPUTER_ENTRY(){}
    };

static REMOTE_COMPUTER_ENTRY * RemoteComputer = 0;
static int NofRemoteComputers = 0;

static HANDLE  CheckEvent;
static HANDLE  SendEvent;
static HANDLE  ReadHandle = INVALID_HANDLE_VALUE;

void show_message( const TCHAR * sorc );

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                       CONVERT_TO_MAILSLOT_NAME                       *
* The mailslot name is really just the network name of the computer    *
* such as \\ws0. You pass this a directory and it makes sure there     *
* are two backslashes at the start and then nullifies the next         *
* backslash that it finds. The result is what is stored in the         *
* RemoteMailslots list.                                                *
***********************************************************************/
BOOLEAN convert_to_mailslot_name( STRING_CLASS & s )
{
TCHAR * cp;

cp = s.text();

if ( *cp != BackSlashChar )
    return FALSE;
cp++;

if ( *cp != BackSlashChar )
    return FALSE;

cp++;

while ( *cp != BackSlashChar && *cp != NullChar )
    cp++;

*cp = NullChar;
return TRUE;
}

/***********************************************************************
*                          RELOAD_REMOTE_COMPUTERS                     *
***********************************************************************/
static void reload_remote_computers()
{
COMPUTER_CLASS c;
STRING_CLASS   s;
int            n;

NofRemoteComputers = 0;
if ( RemoteComputer != 0 )
    {
    delete[] RemoteComputer;
    RemoteComputer = 0;
    }

n = 0;
c.rewind();
while ( c.next() )
    {
    s = c.directory();
    if ( convert_to_mailslot_name(s) )
        n++;
    }

if ( n > 0 )
    {
    RemoteComputer = new REMOTE_COMPUTER_ENTRY[n];
    if ( RemoteComputer )
        {
        c.rewind();
        while ( c.next() )
            {
            s = c.directory();
            if ( convert_to_mailslot_name(s) )
                {
                RemoteComputer[NofRemoteComputers].name = s;
                RemoteComputer[NofRemoteComputers].directory = c.directory();
                remove_backslash_from_path( RemoteComputer[NofRemoteComputers].directory.text() );
                RemoteComputer[NofRemoteComputers].is_online = TRUE;
                if ( DisplayingMessages )
                    {
                    s = TEXT( "Remote Computer " );
                    s += NofRemoteComputers;
                    s += TEXT( " = " );
                    s += RemoteComputer[NofRemoteComputers].name;
                    show_message( s.text() );
                    }
                NofRemoteComputers++;
                if ( NofRemoteComputers == n )
                    break;
                }
            }
        }
    }
}

/***********************************************************************
*                          NEW_COMPUTER_SETUP                          *
* This event is sent by the network setup program and tells me to      *
* reload my computer list, implying that the list has changed.         *
***********************************************************************/
static void new_computer_setup(  TCHAR * sorc, int data_sorc )
{
COMPUTER_CLASS c;

c.load();
reload_remote_computers();
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status()
{
STRING_CLASS s;
int i;
for ( i=0; i<NofRemoteComputers; i++ )
    {
    s = RemoteComputer[i].name;
    s += SpaceString;
    if ( RemoteComputer[i].is_online )
        s += TEXT( "ON Line" );
    else
        s += TEXT( "OFF Line" );
    show_message( s.text() );
    }
}

/***********************************************************************
*                              IS_ONLINE                               *
***********************************************************************/
static BOOLEAN is_online( TCHAR * computer )
{
int i;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].name == computer )
        return RemoteComputer[i].is_online;
    }

return FALSE;
}

/***********************************************************************
*                         SET_COMPUTER_OFFLINE                         *
*  The "computer" is the network name preceeded by \\ as in \\WS0.     *
***********************************************************************/
static void set_computer_offline( TCHAR * computer )
{
STRING_CLASS s;
int i;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].name == computer )
        {
        RemoteComputer[i].is_online = FALSE;
        if ( DisplayingMessages )
            {
            s = computer;
            s += TEXT( " is OFF Line" );
            show_message( s.text() );
            }
        break;
        }
    }
}

/***********************************************************************
*                               SEND_THREAD                            *
***********************************************************************/
DWORD send_thread( int * notused )
{
const  DWORD MS_FOR_CHECK_THREAD = 30000UL;
static DWORD last_tick_count;
HANDLE       send_handle;
STRING_CLASS msg;
TCHAR      * cp;
TCHAR      * s;
int          slen;
DWORD        current_tick_count;
BOOL         was_sent;
BOOLEAN      i_have_a_new_offline_computer;
BOOLEAN      ok_to_send;
TCHAR        mailslot_name[MAX_PATH+1];
DWORD        bytes_written;

/*
-------------------------------------------------------------------
Initialize the ms counter that I use to check for offline computers
------------------------------------------------------------------- */
last_tick_count = GetTickCount();

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(SendEvent, INFINITE) == WAIT_FAILED )
        resource_message_box( MainInstance, UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );

    if ( !HaveMailslots )
        break;

    /*
    -----------------------------------------------------------------------------------------------------
    Since I may reload the remote computer list it's important that I control when the check_thread runs.
    ----------------------------------------------------------------------------------------------------- */
    current_tick_count = GetTickCount();
    if ( current_tick_count > last_tick_count )
        {
        if ( (current_tick_count - last_tick_count) > MS_FOR_CHECK_THREAD )
            {
            SetEvent( CheckEvent );
            last_tick_count = current_tick_count;
            }
        }
    else
        {
        last_tick_count = current_tick_count;
        }

    /*
    ----------------------------------------------------------------------------------------
    If a computer goes offline I only want to try to send to it once. There may be more than
    one message in the que. Since I don't want to always be checking, I need a boolean.
    ---------------------------------------------------------------------------------------- */
    i_have_a_new_offline_computer = FALSE;

    while ( SendData.count() )
        {
        s = (TCHAR *) SendData.pop();
        if ( s )
            {
            if ( strings_are_equal(s, ReloadRemoteComputers, lstrlen(ReloadRemoteComputers)) )
                {
                /*
                ---------------------------------------------------------------------------------------------
                This is a local command only. Since this thread is the only one using the RemoteComputer list
                I need to reload it here also.
                --------------------------------------------------------------------------------------------- */
                reload_remote_computers();
                }
            else if ( strings_are_equal(s, ShowStatusRequest, lstrlen(ShowStatusRequest)) )
                {
                show_status();
                }
            else
                {
                /*
                ----------------
                Send the message
                ---------------- */
                cp = findchar( CommaChar, s );
                if ( cp )
                    {
                    *cp = NullChar;
                    ok_to_send = TRUE;
                    if ( i_have_a_new_offline_computer )
                        ok_to_send = is_online( s );
                    if ( ok_to_send )
                        {
                        lstrcpy( mailslot_name, s );
                        append_filename_to_path(  mailslot_name, MailslotSuffix );

                        cp++;
                        slen = lstrlen(cp) + 1;  /* Include the terminating null */
                        slen *= sizeof( TCHAR );

                        if ( DisplayingMessages )
                            {
                            msg = "Send[";
                            msg += s;
                            msg += cp;
                            msg += "]";
                            show_message( msg.text() );
                            }

                        was_sent = FALSE;
                        send_handle = CreateFile(
                            mailslot_name,
                            GENERIC_WRITE,
                            FILE_SHARE_READ,              /* required to write to a mailslot */
                            (LPSECURITY_ATTRIBUTES) NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            (HANDLE) NULL);

                        if ( send_handle != INVALID_HANDLE_VALUE)
                            {
                            was_sent = WriteFile( send_handle, cp, (DWORD) slen, &bytes_written, (LPOVERLAPPED) NULL );
                            CloseHandle( send_handle );
                            }

                        if ( !was_sent )
                            {
                            set_computer_offline( s );
                            i_have_a_new_offline_computer = TRUE;
                            }
                        }
                    }
                }
            delete[] s;
            }
        }
    }

while ( SendData.count() )
    {
    s = (TCHAR *) SendData.pop();
    if ( s )
        delete[] s;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( !CheckEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                             CHECK_THREAD                             *
***********************************************************************/
DWORD check_thread( int * notused )
{
int i;

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(CheckEvent, INFINITE) == WAIT_FAILED )
        {
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );
        break;
        }

    if ( !HaveMailslots )
        break;

    for ( i=0; i<NofRemoteComputers; i++ )
        {
        if ( !RemoteComputer[i].is_online )
            {
            if ( directory_exists(RemoteComputer[i].directory.text()) )
                RemoteComputer[i].is_online = TRUE;
            }
        }
    }

if ( CheckEvent )
    {
    CloseHandle( CheckEvent );
    CheckEvent = 0;
    }

if ( !SendEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                             TO_MAILSLOT                              *
* This is called from eventman and is not in the send_thread.          *
***********************************************************************/
void to_mailslot( TCHAR * dest, TCHAR * sorc )
{
TCHAR * cp;
TCHAR * s;
int32   slen;

if ( !HaveMailslots )
    return;

slen = lstrlen( dest ) + 1 + lstrlen( sorc );
s = maketext( slen );
if ( s )
    {
    cp = copy_w_char( s, dest, CommaChar );
    copystring( cp, sorc );
    SendData.push( s );
    SetEvent( SendEvent );
    }
}

/***********************************************************************
*                         SEND_MAILSLOT_EVENT                          *
* This is called from eventman and is not in the send_mailslot_thread  *
* I pad the beginning of each dest name with two spaces so I will      *
* always have enough room to put the string length in the first four   *
* characters of the message.                                           *
***********************************************************************/
void send_mailslot_event( TCHAR * topic, TCHAR * item, TCHAR * data )
{
int     count;
int     i;
int     slen;
TCHAR * s;
TCHAR * cp;

if ( !HaveMailslots )
    return;

if ( strings_are_equal(item,ItemList[DS_NOTIFY_INDEX].name) )
    {
    if ( HaveDataServer )
        {
        slen = 2 + lstrlen(topic) + lstrlen(item) + lstrlen(data);

        s = maketext( slen );
        if ( s )
            {
            cp = copy_w_char( s,  topic, CommaChar );
            cp = copy_w_char( cp, item,  CommaChar );
            copystring( cp, data );
            to_mailslot( DataServerComputer.text(), s );
            delete[] s;
            }
        }
    return;
    }

count = 0;
for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].is_online )
        {
        slen = RemoteComputer[i].name.len();
        slen += 3; /* Room for commas in {computer,topic,item,data} */
        slen += lstrlen(topic) + lstrlen(item) + lstrlen(data);

        s = maketext( slen );
        if ( s )
            {
            cp = copy_w_char( s,  RemoteComputer[i].name.text(), CommaChar );
            cp = copy_w_char( cp, topic, CommaChar );
            cp = copy_w_char( cp, item,  CommaChar );
            copystring( cp, data );
            SendData.push( s );
            count++;
            }
        }
    }

if ( count > 0 )
    SetEvent( SendEvent );
}

/***********************************************************************
*                          READ_NEXT_MESSAGE                           *
***********************************************************************/
static BOOLEAN read_next_message( DWORD slen )
{
TCHAR * s;
DWORD   bytes_read;

if ( slen > 0 )
    {
    s = maketext( slen );
    if ( s )
        {
        slen *= sizeof( TCHAR );
        if( ReadFile(ReadHandle, s, slen, &bytes_read, NULL) )
            {
            slen = bytes_read;
            slen /= sizeof( TCHAR );
            *(s+slen) = NullChar;
            PostMessage( MainWindow, WM_NEWMAIL, 0, (LPARAM) s );
            }
        else
            {
            delete[] s;
            }
        }
    }
return TRUE;
}

/***********************************************************************
*                             READ_THREAD                              *
* I made the handle used here global so I can stop the thread by       *
* closing the handle.                                                  *
***********************************************************************/
DWORD read_thread( int * notused )
{
BOOL   status;
DWORD  next_msg_size;
DWORD  nof_msg;

ReadHandle = CreateMailslot(
        MailSlotName,
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( ReadHandle == INVALID_HANDLE_VALUE)
    {
    resource_message_box( MainInstance,  MAILSLOT_STRING, CREATE_FAILED_STRING );
    return 0;
    }

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    status = GetMailslotInfo(
        ReadHandle,
        (LPDWORD) NULL,               // no maximum message size
        &next_msg_size,               // size of next message
        &nof_msg,                     // number of messages
        (LPDWORD) NULL);              // no read time-out

    if ( !HaveMailslots )
        break;

    if ( status )
        {
        if ( next_msg_size != MAILSLOT_NO_MESSAGE )
            read_next_message( next_msg_size );
        }
    else
        break;
    }

PostMessage( MainWindow, WM_CLOSE, 0, 0L );
return 0;
}

/***********************************************************************
*                          SHUTDOWN_MAILSLOTS                          *
***********************************************************************/
void shutdown_mailslots()
{
HaveMailslots = FALSE;
/*
------------------------------------------------------------------------
The read thread doesn't have an event. I just close the mailslot handle.
------------------------------------------------------------------------ */
CloseHandle( ReadHandle );
ReadHandle = INVALID_HANDLE_VALUE;

SetEvent( CheckEvent );
}

/***********************************************************************
*                           START_MAILSLOTS                            *
***********************************************************************/
BOOLEAN start_mailslots()
{
DWORD  id;
HANDLE h;

HaveMailslots = TRUE;
reload_remote_computers();

SendEvent  = CreateEvent( NULL, FALSE, FALSE, NULL );
CheckEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) send_thread,          /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) check_thread,         /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) read_thread,          /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

return TRUE;
}