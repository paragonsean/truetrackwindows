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
static TCHAR DataArchiverRootDir[] = TEXT( "V5DS" );
static TCHAR MailSlotName[] = TEXT( "\\\\.\\mailslot\\v5\\eventman" );
static TCHAR MailslotSuffix[] = TEXT( "mailslot\\v5\\eventman" );
static TCHAR NetworkDirPrefix[] = TEXT( "\\\\" );
static TCHAR NoComputerName[]  = NO_COMPUTER;

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
    void         operator=( const REMOTE_COMPUTER_ENTRY & sorc );
    };

static REMOTE_COMPUTER_ENTRY * RemoteComputer = 0;
static int NofRemoteComputers = 0;

static REMOTE_COMPUTER_ENTRY DataArchiver;
static BOOLEAN HaveDataArchiver;

static HANDLE  CheckEvent = 0;
static HANDLE  ReadEvent  = 0;
static HANDLE  SendEvent  = 0;
static HANDLE  ReadHandle = INVALID_HANDLE_VALUE;

static BOOLEAN HaveOfflineComputer = FALSE;

void show_message( const TCHAR * sorc );
TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                          REMOTE_COMPUTER_ENTRY                       *
*                                   =                                  *
***********************************************************************/
void REMOTE_COMPUTER_ENTRY::operator=( const REMOTE_COMPUTER_ENTRY & sorc )
{
name = sorc.name;
directory = sorc.directory;
is_online = sorc.is_online;
}

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
TCHAR        * cp;

HaveDataArchiver = FALSE;
DataArchiver.is_online = FALSE;
cp = get_ini_string( VisiTrakIniFile, ConfigSection, DataServerKey );
if ( !unknown(cp) )
    {
    if ( !strings_are_equal(cp, NoComputerName, COMPUTER_NAME_LEN) )
        {
        DataArchiver.name  = NetworkDirPrefix;
        DataArchiver.name += cp;
        DataArchiver.directory = DataArchiver.name;
        DataArchiver.directory += BackSlashChar;
        DataArchiver.directory += DataArchiverRootDir;
        DataArchiver.is_online = TRUE;
        HaveDataArchiver = TRUE;
        if ( DisplayingMessages )
            {
            s = resource_string( DATA_ARCHIVER_STRING );
            s += TEXT( " = " );
            s += DataArchiver.name;
            show_message( s.text() );
            }
        }
    }

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
                    s = resource_string( REMOTE_COMPUTER_STRING );
                    s += SpaceString;
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
static void new_computer_setup()
{
COMPUTER_CLASS c;

EnterCriticalSection( &ComputersCriticalSection );

c.load();
reload_remote_computers();

LeaveCriticalSection( &ComputersCriticalSection );
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status()
{
STRING_CLASS s;
int i;

if ( HaveDataArchiver )
    {
    s = DataArchiver.name;
    s += SpaceString;
    if ( DataArchiver.is_online )
        s += resource_string( IS_ONLINE_STRING );
    else
        s += resource_string( IS_OFFLINE_STRING );
    show_message( s.text() );
    }

for ( i=0; i<NofRemoteComputers; i++ )
    {
    s = RemoteComputer[i].name;
    s += SpaceString;
    if ( RemoteComputer[i].is_online )
        s += resource_string( IS_ONLINE_STRING );
    else
        s += resource_string( IS_OFFLINE_STRING );
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

if ( HaveDataArchiver )
    {
    if ( DataArchiver.name == computer )
        return DataArchiver.is_online;
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
int          i;
BOOLEAN      have_computer;

have_computer = FALSE;

if ( HaveDataArchiver )
    {
    if ( DataArchiver.name == computer )
        {
        DataArchiver.is_online = FALSE;
        have_computer = TRUE;
        }
    }

if ( !have_computer )
    {
    for ( i=0; i<NofRemoteComputers; i++ )
        {
        if ( RemoteComputer[i].name == computer )
            {
            RemoteComputer[i].is_online = FALSE;
            have_computer = TRUE;
            break;
            }
        }
    }

if ( DisplayingMessages && have_computer )
    {
    s = computer;
    s += resource_string( IS_OFFLINE_STRING );
    show_message( s.text() );
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
BOOLEAN      ok_to_send;
BOOLEAN      have_new_offline;
TCHAR        mailslot_name[MAX_PATH+1];
DWORD        bytes_written;

/*
-------------------------------------------------------------------
Initialize the ms counter that I use to check for offline computers
------------------------------------------------------------------- */
last_tick_count = GetTickCount();
have_new_offline = FALSE;

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(SendEvent, INFINITE) == WAIT_FAILED )
        resource_message_box( MainInstance, UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );

    if ( !HaveMailslots )
        break;

    //show_message( "Wakeup" );

    /*
    -----------------------------------------------------------------------------------------------------
    Since I may reload the remote computer list it's important that I control when the check_thread runs.
    ----------------------------------------------------------------------------------------------------- */
    if ( HaveOfflineComputer || have_new_offline )
        {
        current_tick_count = GetTickCount();
        if ( current_tick_count > last_tick_count )
            {
            if ( (current_tick_count - last_tick_count) > MS_FOR_CHECK_THREAD )
                {
                //show_message( "Check Offline" );

                SetEvent( CheckEvent );
                last_tick_count = current_tick_count;
                have_new_offline = FALSE;
                }
            }
        else
            {
            /*
            --------------------------------------------------------
            if the tick count has rolled over just restart the count
            -------------------------------------------------------- */
            last_tick_count = current_tick_count;
            }
        }

    while ( SendData.count() )
        {
        s = (TCHAR *) SendData.pop();
        if ( s )
            {
            if ( strings_are_equal(s, ReloadRemoteComputers, lstrlen(ReloadRemoteComputers)) )
                {
                //show_message( "New Computer Setup" );

                /*
                ---------------------------------------------------------------------------------------------
                This is a local command only. Since this thread is the only one using the RemoteComputer list
                I need to reload it here also.
                --------------------------------------------------------------------------------------------- */
                new_computer_setup();
                }
            else if ( strings_are_equal(s, ShowStatusRequest, lstrlen(ShowStatusRequest)) )
                {
                //show_message( "Show Status" );
                show_status();
                }
            else
                {
                /*
                ----------------
                Send the message
                ---------------- */
                //msg = "send[";
                //msg += s;
                //msg += "]";
                //show_message( msg.text() );
                cp = findchar( CommaChar, s );
                if ( cp )
                    {
                    *cp = NullChar;
                    ok_to_send = TRUE;
                    if ( HaveOfflineComputer || have_new_offline )
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
                            msg = TEXT( "Send[" );
                            msg += s;
                            msg += cp;
                            msg += TEXT( "]" );
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
                            have_new_offline = TRUE;
                            }
                        }
                    }
                }
            delete[] s;
            s = 0;
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

show_message( TEXT("The SEND Thread is Closed") );

if ( !CheckEvent && !ReadEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                             CHECK_THREAD                             *
***********************************************************************/
DWORD check_thread( int * notused )
{
int i;
BOOLEAN b;
BOOLEAN offline;
REMOTE_COMPUTER_ENTRY rc;

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

    offline = FALSE;

    /*
    ----------------------------------------------------------------------------------
    I have to be careful accessing the remote computer list, as it may change any time
    ---------------------------------------------------------------------------------- */
    i = 0;
    while ( true )
        {
        EnterCriticalSection( &ComputersCriticalSection );
        if ( i >= NofRemoteComputers )
            break;
        rc = RemoteComputer[i];
        LeaveCriticalSection( &ComputersCriticalSection );

        if ( !rc.is_online )
            {
            if ( directory_exists(rc.directory.text()) )
                {
                EnterCriticalSection( &ComputersCriticalSection );
                if ( i < NofRemoteComputers )
                    RemoteComputer[i].is_online = TRUE;
                LeaveCriticalSection( &ComputersCriticalSection );
                }
            else
                {
                offline = TRUE;
                }
            }
        i++;
        }

    /*
    ---------------------------------------------------------------
    When I leave the loop above I have entered the critical section
    --------------------------------------------------------------- */
    rc = DataArchiver;
    b  = HaveDataArchiver;
    LeaveCriticalSection( &ComputersCriticalSection );

    if ( b )
        {
        if ( !rc.is_online )
            {
            if ( directory_exists(rc.directory.text()) )
                {
                EnterCriticalSection( &ComputersCriticalSection );
                DataArchiver.is_online = TRUE;
                LeaveCriticalSection( &ComputersCriticalSection );
                }
            else
                {
                offline = TRUE;
                }
            }
        }

    HaveOfflineComputer = offline;
    }

if ( CheckEvent )
    {
    CloseHandle( CheckEvent );
    CheckEvent = 0;
    }
show_message( TEXT("The CHECK Thread is Closed") );

if ( !SendEvent && !ReadEvent )
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
//STRING_CLASS st;

if ( !HaveMailslots )
    return;

//st = "to_mailslot  [";
//st += sorc;
//st += "][";

slen = lstrlen( dest ) + 1 + lstrlen( sorc );
s = maketext( slen );
if ( s )
    {
    cp = copy_w_char( s, dest, CommaChar );
    copystring( cp, sorc );
    //st += s;
    //st += "]";
    SendData.push( s );
    SetEvent( SendEvent );
    //show_message( st.text() );
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

EnterCriticalSection( &ComputersCriticalSection );

count = 0;

if ( strings_are_equal(item,ItemList[DS_NOTIFY_INDEX].name) )
    {
    if ( HaveDataArchiver )
        {
        slen = DataArchiver.name.len();
        slen += 3 + lstrlen(topic) + lstrlen(item) + lstrlen(data);

        s = maketext( slen );
        if ( s )
            {
            cp = copy_w_char( s,  DataArchiver.name.text(), CommaChar );
            cp = copy_w_char( cp,  topic, CommaChar );
            cp = copy_w_char( cp, item,  CommaChar );
            copystring( cp, data );
            SendData.push( s );
            count++;
            }
        }
    }
else
    {
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
    }

LeaveCriticalSection( &ComputersCriticalSection );

if ( count > 0 )
    {
    SetEvent( SendEvent );
    //show_message( "SetEvent from send_mailslot" );
    }
}

/***********************************************************************
*                                 IS_NT                                *
*                 Return true if this is at least Win 2000             *
***********************************************************************/
bool is_nt()
{
OSVERSIONINFO vi;

vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

GetVersionEx( &vi );

if ( vi.dwMajorVersion > 4 )
    return true;

return false;
}

/***********************************************************************
*                             READ_THREAD                              *
* I made the handle used here global so I can stop the thread by       *
* closing the handle.                                                  *
***********************************************************************/
DWORD read_thread( int * notused )
{
TCHAR      * s;
int          slen;
DWORD        bytes_read;
DWORD        next_msg_size;
DWORD        nof_msg;
BOOL         status;
STRING_CLASS msg;

ReadHandle = CreateMailslot(
        MailSlotName,
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // read timeout (use only 0 or MAILSLOT_WAIT_FOREVER )
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

    if ( ReadHandle == INVALID_HANDLE_VALUE )
        break;

    status = GetMailslotInfo(
        ReadHandle,
        (LPDWORD) NULL,               // current maximum message size
        &next_msg_size,               // size of next message
        &nof_msg,                     // number of messages qued up
        (LPDWORD) NULL );             // current read time-out seting

    if ( !status )
        break;

    if ( next_msg_size == MAILSLOT_NO_MESSAGE )
        {
        WaitForSingleObject( ReadEvent, 32 );
        }
    else if ( next_msg_size > 0 )
        {
        slen = (int) next_msg_size;
        slen++; // In case odd
        slen /= sizeof( TCHAR );

        s = maketext( slen );

        status     = FALSE;
        bytes_read = 0;
        if ( s )
            status = ReadFile( ReadHandle, s, next_msg_size, &bytes_read, NULL );

        if ( !HaveMailslots )
            {
            if ( s )
                {
                delete[] s;
                s = 0;
                }
            break;
            }

        *(s+slen) = NullChar;

        if ( DisplayingMessages )
            {
            msg = TEXT( "Read [" );
            msg += (int) bytes_read;
            msg += TEXT( "][" );
            msg += s;
            msg += TEXT( "]" );
            show_message( msg.text() );
            }
        PostMessage( MainWindow, WM_NEWMAIL, 0, (LPARAM) s );
        }
    }

if ( ReadEvent )
    {
    CloseHandle( ReadEvent );
    ReadEvent = 0;
    }

if ( ReadHandle != INVALID_HANDLE_VALUE )
    {
    CloseHandle( ReadHandle );
    ReadHandle = INVALID_HANDLE_VALUE;
    }

show_message( TEXT("The READ Thread is Closed") );
if ( !SendEvent && !CheckEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );
return 0;
}

/***********************************************************************
*                          SHUTDOWN_MAILSLOTS                          *
***********************************************************************/
void shutdown_mailslots()
{
HaveMailslots = FALSE;

SetEvent( CheckEvent );
SetEvent( ReadEvent  );
SetEvent( SendEvent  );
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

CheckEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
ReadEvent  = CreateEvent( NULL, FALSE, FALSE, NULL );
SendEvent  = CreateEvent( NULL, FALSE, FALSE, NULL );

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