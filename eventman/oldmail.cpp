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

static HANDLE ReadMailslotEvent = 0;
static HANDLE SendMailslotEvent = 0;

static BOOLEAN ShuttingDown   = FALSE;

static FIFO_CLASS SendData;

class REMOTE_COMPUTER_ENTRY
    {
    public:
    STRING_CLASS name;
    BOOLEAN      is_online;
    DWORD        ms_at_last_check;
    REMOTE_COMPUTER_ENTRY(){ is_online = TRUE; ms_at_last_check = 0; }
    ~REMOTE_COMPUTER_ENTRY(){}
    };

static HANDLE ReadMailslotHandle = 0;

static REMOTE_COMPUTER_ENTRY * RemoteComputer = 0;
static int NofRemoteComputers = 0;

static const TCHAR LocalMailSlotName[] = TEXT( "\\\\.\\mailslot\\v5\\eventman" );
static const TCHAR MailslotSuffix[]    = TEXT( "\\mailslot\\v5\\eventman" );
static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR NullChar      = TEXT( '\0' );

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                          READ_NEXT_MESSAGE                           *
***********************************************************************/
static BOOLEAN read_next_message( HANDLE mailslot_handle, DWORD slen )
{
TCHAR * s;
DWORD   bytes_read;

if ( slen > 0 )
    {
    s = maketext( slen );
    if ( s )
        {
        if( ReadFile(mailslot_handle, s, slen, &bytes_read, NULL) )
            {
            *(s+bytes_read) = NullChar;
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
*                           READ_MAILSLOT_THREAD                       *
***********************************************************************/
DWORD read_mailslot_thread( int * notused )
{
BOOL   status;
DWORD  next_msg_size;
DWORD  nof_msg;
DWORD  timeout_ms = 60000UL;

ReadMailslotHandle = CreateMailslot(
        LocalMailSlotName,
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( ReadMailslotHandle == INVALID_HANDLE_VALUE)
    {
    resource_message_box(MAILSLOT_STRING, CREATE_FAILED_STRING );
    return 0;
    }

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    status = GetMailslotInfo(
        ReadMailslotHandle,
        (LPDWORD) NULL,               // no maximum message size
        &next_msg_size,               // size of next message
        &nof_msg,                     // number of messages
        &timeout_ms );                // read time-out // was (LPDWORD) NULL);              // no read time-out

    if ( status )
        {
        if ( next_msg_size != MAILSLOT_NO_MESSAGE )
            read_next_message( ReadMailslotHandle, next_msg_size );
        }
    else
        break;
    /*
    if ( WaitForSingleObject(ReadMailslotEvent, MS_TO_WAIT) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );
    */
    }

//CloseHandle( mailslot_handle );

CloseHandle( ReadMailslotEvent );
ReadMailslotEvent = 0;
if ( !SendMailslotEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                         SET_COMPUTER_OFFLINE                         *
***********************************************************************/
static void set_computer_offline( TCHAR * computer )
{
int i;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].name == computer )
        {
        RemoteComputer[i].is_online        = FALSE;
        RemoteComputer[i].ms_at_last_check = GetTickCount();
        break;
        }
    }
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

if ( *cp != BackslashChar )
    return FALSE;
cp++;

if ( *cp != BackslashChar )
    return FALSE;

cp++;

while ( *cp != BackslashChar && *cp != NullChar )
    cp++;

*cp = NullChar;

return TRUE;
}

/***********************************************************************
*                        CHECK_OFFLINE_COMPUTERS                       *
*                      I do this once every minute.                    *
***********************************************************************/
static void check_offline_computers()
{
const DWORD DELTA_T = 20000UL;
STRING_CLASS s;
TCHAR * cp;
int     i;
DWORD   rt;
DWORD   t;

t = GetTickCount();
for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( !RemoteComputer[i].is_online )
        {
        rt = RemoteComputer[i].ms_at_last_check;

        if ( t < rt || (t-rt) > DELTA_T )
            {
            /*
            -----------------------------------------
            Each remote computer name begins with \\.
            ----------------------------------------- */
            cp = RemoteComputer[i].name.text();
            cp += 2;
            s = TEXT( "Checking " );
            s += cp;
            SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) s.text() );
            if ( gethostbyname(cp) != 0 )
                {
                RemoteComputer[i].is_online = TRUE;
                s = TEXT( "On Line" );
                }
            else
                {
                RemoteComputer[i].ms_at_last_check = GetTickCount();
                s = TEXT( "OFF Line" );
                }
            SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) s.text() );
            }
        }
    }
}

/***********************************************************************
*                          CHECK_REMOTE_COMPUTERS                      *
* This is called manually and checks every remote computer to see      *
* if it is present or not.                                             *
***********************************************************************/
static void check_remote_computers()
{
STRING_CLASS   s;
TCHAR         * cp;
int             i;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    cp = RemoteComputer[i].name.text();
    cp += 2;
    s = TEXT( "Checking " );
    s += cp;
    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) s.text() );
//    if ( gethostbyname(cp) != 0 )

    s = RemoteComputer[i].name.text();
    s += "\\V5";
    if ( directory_exists(s.text()) )
        {
        RemoteComputer[i].is_online = TRUE;
        s = TEXT( "On Line" );
        }
    else
        {
        RemoteComputer[i].is_online = FALSE;
        RemoteComputer[i].ms_at_last_check = GetTickCount();
        s = TEXT( "OFF Line" );
        }
    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) s.text() );
    }
}

/***********************************************************************
*                          RELOAD_REMOTE_COMPUTERS                     *
***********************************************************************/
static void reload_remote_computers()
{
COMPUTER_CLASS c;
STRING_CLASS   s;
TEXT_LIST_CLASS t;
TCHAR         * cp;
int             i;
int             n;

NofRemoteComputers = 0;
if ( RemoteComputer != 0 )
    {
    delete[] RemoteComputer;
    RemoteComputer = 0;
    }

c.rewind();
while ( c.next() )
    {
    s = c.directory();
    if ( convert_to_mailslot_name(s) )
        t.append( s.text() );
    }
n = t.count();
if ( n > 0 )
    {
    RemoteComputer = new REMOTE_COMPUTER_ENTRY[n];
    if ( RemoteComputer )
        {
        t.rewind();
        for ( i=0; i<n; i++ )
            {
            t.next();
            RemoteComputer[i].name = t.text();
            cp = t.text();
            cp += 2;
            if ( gethostbyname(cp) != 0 )
                {
                RemoteComputer[i].is_online = TRUE;
                }
            else
                {
                RemoteComputer[i].is_online = FALSE;
                RemoteComputer[i].ms_at_last_check = GetTickCount();
                }
            }
        NofRemoteComputers = n;
        }
    }
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status()
{
int i;
STRING_CLASS s;
TCHAR      * cp;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    cp = RemoteComputer[i].name.text();
    cp += 2;
    s = cp;
    s += TEXT( "   " );
    if ( RemoteComputer[i].is_online )
        s += TEXT( "On Line" );
    else
        s += TEXT( "OFF Line" );
    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) s.text() );
    }
}

/***********************************************************************
*                           SEND_MAILSLOT_THREAD                       *
***********************************************************************/
DWORD send_mailslot_thread( int * notused )
{
//const  DWORD MS_PER_MINUTE = 60000UL;
const  DWORD MS_PER_MINUTE = 5000UL;
static DWORD last_tick_count;
STRING_CLASS mailslot_name;
HANDLE  mailslot_handle;
DWORD   bytes_written;
TCHAR * cp;
TCHAR * s;
int32   slen;
DWORD   current_tick_count;
/*
-------------------------------------------------------------------
Initialize the ms counter that I use to check for offline computers
------------------------------------------------------------------- */
last_tick_count = GetTickCount();

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(SendMailslotEvent, INFINITE) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );

    if ( !HaveMailslots )
        break;

    /*
    ------------------------------------
    Check offline computers every minute
    ------------------------------------ */
    current_tick_count = GetTickCount();
    if ( current_tick_count > last_tick_count )
        {
        if ( (current_tick_count - last_tick_count) > MS_PER_MINUTE )
            check_offline_computers();
        }
    last_tick_count = current_tick_count;

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
            else if ( strings_are_equal(s, CheckRemotesRequest, lstrlen(CheckRemotesRequest)) )
                {
                check_remote_computers();
                }
            else
                {
                if ( ListEvents )
                    {
                    mailslot_name = "Send[";
                    mailslot_name += s;
                    mailslot_name += "]";
                    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) mailslot_name.text() );
                    }
                cp = findchar( CommaChar, s );
                if ( cp )
                    {
                    *cp = NullChar;
                    cp++;
                    slen = lstrlen(cp) + 1;  /* Include the terminating null */
                    slen *= sizeof( TCHAR );

                    mailslot_name = s;
                    mailslot_name += MailslotSuffix;

                    mailslot_handle = CreateFile(
                        mailslot_name.text(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ,  /* required to write to a mailslot */
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

                    if ( mailslot_handle == INVALID_HANDLE_VALUE)
                        {
                        set_computer_offline( s );
                        }
                    else
                        {
                        if ( WriteFile(mailslot_handle, cp, (DWORD) slen, &bytes_written, (LPOVERLAPPED) NULL) == 0 )
                            {
                            if ( ListEvents )
                                {
                                mailslot_name += TEXT( " UNABLE TO WRITE" );
                                SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) mailslot_name.text() );
                                }
                            }
                        CloseHandle( mailslot_handle );
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

CloseHandle( SendMailslotEvent );
SendMailslotEvent = 0;
if ( !ReadMailslotEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                             TO_MAILSLOT                              *
* This is called from eventman and is not in the send_mailslot_thread  *
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
    SetEvent( SendMailslotEvent );
    }
}

/***********************************************************************
*                         SEND_MAILSLOT_EVENT                          *
* This is called from eventman and is not in the send_mailslot_thread  *
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
        slen = 2; /* Room for commas in {topic,item,data} */
        slen += lstrlen(topic) + lstrlen(item) + lstrlen(data);

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
    SetEvent( SendMailslotEvent );
}

/***********************************************************************
*                        STOP_MAILSLOT_THREADS                         *
***********************************************************************/
void stop_mailslot_threads()
{
HaveMailslots = FALSE;
if ( ReadMailslotHandle != 0 )
    CloseHandle( ReadMailslotHandle );
SetEvent( ReadMailslotEvent );
SetEvent( SendMailslotEvent );
}

/***********************************************************************
*                          START_MAILSLOT_THREADS                      *
***********************************************************************/
BOOLEAN start_mailslot_threads()
{
HANDLE h;
DWORD  id;

reload_remote_computers();

ReadMailslotEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
SendMailslotEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

HaveMailslots = TRUE;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) send_mailslot_thread, /* thread function               */
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
    (LPTHREAD_START_ROUTINE) read_mailslot_thread, /* thread function               */
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
