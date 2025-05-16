#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\events.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

#define NO_INDEX            -1
#define DDE_TIMEOUT_MS      5000

static TCHAR ServerName[] = VISITRAK_EVENT_MANAGER;
static HSZ   ServerNameHandle;

static HWND  MyMainWindow = 0;

static DWORD DdeId;
static  int  CodePageId    = CP_WINANSI;
static  UINT DdeClipFormat = CF_TEXT;

static HCONV ServerHandle;

static GENERIC_LIST_CLASS EventList;

static const TCHAR CrLf[]     = TEXT( "\015\012" );
static const TCHAR DollarChar = TEXT( '$' );
static const TCHAR NullChar   = TEXT( '\0');

/***********************************************************************
*                              ITEM_INDEX                              *
***********************************************************************/
static int item_index( HSZ item_handle )
{
int i;

for ( i=0; i<NofItems; i++ )
    {
    if ( DdeCmpStringHandles(item_handle, ItemList[i].handle ) == 0 )
        return i;
    }
return NO_INDEX;
}

/***********************************************************************
*                              ITEM_INDEX                              *
***********************************************************************/
static short item_index( TCHAR * item_to_find )
{
int i;

for ( i=0; i<NofItems; i++ )
    {
    if ( compare(item_to_find, ItemList[i].name, MAX_DDE_ITEM_LEN) == 0 )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                          FIND_CONVERSATION                           *
***********************************************************************/
EVENT_ENTRY * find_conversation( HCONV hc )
{

EVENT_ENTRY * e;

e = 0;

if ( hc != ServerHandle )
    {
    EventList.rewind();
    while ( TRUE )
        {
        e = (EVENT_ENTRY *) EventList.next();
        if ( !e )
            break;
        if ( e->handle == hc )
            break;
        }
    }

return e;
}

/***********************************************************************
*                              POKE_DATA                               *
***********************************************************************/
void poke_data( TCHAR * topic, TCHAR * item, TCHAR * new_value )
{
const int slen = MAX_DDE_TOPIC_LEN+MAX_DDE_DATA_LEN+2;
TCHAR       buf[slen+1];
short       i;
DWORD       n;

if ( !ServerHandle )
    return;

i = item_index(item);

if ( i != NO_INDEX )
    {
    lstrcpy( buf, topic );
    lstrcat( buf, TEXT(",") );
    lstrcat( buf, new_value );
    lstrcat( buf, CrLf );
    n = (DWORD) lstrlen(buf) + 1;
    n *= sizeof( TCHAR );
    DdeClientTransaction( (LPBYTE) buf, n, ServerHandle, ItemList[i].handle, DdeClipFormat, XTYP_POKE, TIMEOUT_ASYNC, NULL );
    }
}

/***********************************************************************
*                          CONNECT_TO_SERVER                           *
***********************************************************************/
HCONV connect_to_server( TCHAR * topic )
{
HCONV h;
HSZ   sh;

sh = DdeCreateStringHandle( DdeId, topic, CodePageId );
if ( sh )
    {
    h = DdeConnect( DdeId, ServerNameHandle, sh, 0 );
    if ( !h )
        MessageBox( NULL, TEXT("DdeConnect Failed"), topic, MB_OK );

    DdeFreeStringHandle( DdeId, sh );
    }

return h;
}

/***********************************************************************
*                             ONE_HOT_LINK                             *
***********************************************************************/
void one_hot_link( HCONV h, short item, UINT transaction_type )
{
static STRING_CLASS s;
static FILE_CLASS f;
static EVENT_ENTRY * e;

if ( h )
    {
    if ( !DdeClientTransaction(NULL, 0, h, ItemList[item].handle, DdeClipFormat, transaction_type, DDE_TIMEOUT_MS, NULL) )
        {
        MessageBox( NULL, TEXT("Dmlerr"), ultoascii(DdeGetLastError(DdeId)), MB_OK );
        f.open_for_write( TEXT("dmlerr.txt") );

        e = find_conversation( h );
        if ( e )
            {
            s = TEXT( "Conversation = " );
            s += e->topic;
            }
        else
            {
            s = TEXT( "No Conversation found" );
            }
        f.writeline( s.text() );

        s = TEXT( "Item = " );
        s += ItemList[item].name;
        s += TEXT( ", default_value = " );
        s += ItemList[item].default_value;
        f.writeline( s.text() );

        s = TEXT( "Transaction type = " );
        s += transaction_type;
        f.writeline( s.text() );

        s = TEXT( "Error = " );
        s += DdeGetLastError( DdeId );
        f.writeline( s.text() );
        f.close();
        }
    }
}

/***********************************************************************
*                              HOT_LINKS                               *
***********************************************************************/
void hot_links( EVENT_ENTRY * e, UINT transaction_type )
{

EVENT_ITEM_ENTRY * i;

if ( !e->handle )
    return;

e->items.rewind();
while ( TRUE )
    {
    i = (EVENT_ITEM_ENTRY *) e->items.next();
    if ( !i )
        break;
    /*
    MessageBox( 0, e->topic, ItemList[i->index].name, MB_OK );
    */
    one_hot_link( e->handle, i->index, transaction_type );
    }

}

/***********************************************************************
*                          CONNECT_TO_SERVER                           *
***********************************************************************/
void connect_to_server( void )
{

EVENT_ENTRY * e;

if ( !ServerHandle )
    ServerHandle = connect_to_server( DDE_MONITOR_TOPIC );

if ( ServerHandle )
    {
    EventList.rewind();
    while ( TRUE )
        {
        e = (EVENT_ENTRY *) EventList.next();
        if ( !e )
            break;
        if ( !e->handle )
            {
            e->handle = connect_to_server( e->topic );
            hot_links( e, XTYP_ADVSTART );
            }
        }
    }
}


#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                           DDE_CALLBACK                               *
***********************************************************************/
HDDEDATA CALLBACK dde_callback( UINT type, UINT clip_format, HCONV conversation_handle,
        HSZ hs1, HSZ hs2, HDDEDATA hdata, DWORD d1, DWORD d2 )
{
EVENT_ENTRY * e;
EVENT_ITEM_ENTRY * i;
short ix;

switch ( type )
    {
    case XTYP_DISCONNECT:
        if ( conversation_handle == ServerHandle )
            {
            ServerHandle = 0;
            if ( MyMainWindow )
                SendMessage( MyMainWindow, WM_EV_SHUTDOWN, 0, 0L );
            }
        else
            {
            e = find_conversation( conversation_handle );
            if ( e )
                {
                e->handle = 0;
                }
            }
        break;

    case XTYP_REGISTER:
        if ( !ServerHandle )
            connect_to_server();
        break;

    case XTYP_XACT_COMPLETE:
    case XTYP_ADVDATA:
        e = find_conversation( conversation_handle );
        if ( e )
            {
            ix = item_index( hs2 );
            e->items.rewind();
            while ( TRUE )
                {
                i = (EVENT_ITEM_ENTRY *) e->items.next();
                if ( !i )
                    break;
                if ( i->index == ix && i->callback )
                    i->callback( e->topic, ix, hdata );
                }
            }
        return (HDDEDATA) DDE_FACK;

    default:
        break;
    }

return (HDDEDATA) NULL;
}

/***********************************************************************
*                      FIND_EXISTING_EVENT_TOPIC                       *
***********************************************************************/
static EVENT_ENTRY * find_existing_event_topic( TCHAR * topic )
{

EVENT_ENTRY * e;

EventList.rewind();
while ( TRUE )
    {
    e = (EVENT_ENTRY *) EventList.next();
    if ( !e )
        break;
    if ( compare( topic, e->topic, MAX_DDE_TOPIC_LEN) == 0 )
        break;
    }

return e;
}

/***********************************************************************
*                           FIND_EVENT_TOPIC                           *
***********************************************************************/
static EVENT_ENTRY * find_event_topic( TCHAR * topic )
{

EVENT_ENTRY * e;

e = find_existing_event_topic( topic );

if ( !e )
    {
    e = new EVENT_ENTRY;

    if ( e )
        {
        lstrcpy( e->topic, topic );
        e->handle = 0;
        if ( !EventList.append(e) )
            {
            delete e;
            e = 0;
            }
        }
    }

return e;
}

/***********************************************************************
*                        FIND_EXISTING_EVENT_ITEM                       *
***********************************************************************/
static EVENT_ITEM_ENTRY * find_existing_event_item( EVENT_ENTRY * e, short item )
{

EVENT_ITEM_ENTRY * i;

i = 0;
if ( e )
    {
    e->items.rewind();
    while ( TRUE )
        {
        i = (EVENT_ITEM_ENTRY *) e->items.next();
        if ( !i )
            break;
        if ( i->index == item )
            break;
        }
    }

return i;
}

/***********************************************************************
*                            FIND_EVENT_ITEM                           *
***********************************************************************/
static EVENT_ITEM_ENTRY * find_event_item( EVENT_ENTRY * e, short item, void (*event_callback)( TCHAR * topic, short item, HDDEDATA edata) )
{
EVENT_ITEM_ENTRY * i;

i = 0;
if ( e )
    {
    i = find_existing_event_item( e, item );
    if ( !i )
        {
        i = new EVENT_ITEM_ENTRY;
        if ( i )
            {
            i->index    = item;
            i->callback = event_callback;
            if ( !e->items.append(i) )
                {
                delete i;
                i = 0;
                }
            }
        }
    }

return i;
}

/***********************************************************************
*                          REGISTER_FOR_EVENT                          *
***********************************************************************/
void register_for_event( TCHAR * topic, short item, void (*event_callback)( TCHAR * topic, short item, HDDEDATA edata) )
{
EVENT_ENTRY      * e;

e = find_event_topic( topic );
if ( find_event_item(e, item, event_callback) )
    {
    /*
    ---------------------------------------------------
    If I am not connected to the server, try to connect
    --------------------------------------------------- */
    if ( !e->handle )
        connect_to_server();
    else
        one_hot_link( e->handle, item, XTYP_ADVSTART );
    }

}

/***********************************************************************
*                         UNREGISTER_FOR_EVENT                         *
***********************************************************************/
void unregister_for_event( TCHAR * topic, short item )
{
EVENT_ENTRY      * e;
EVENT_ITEM_ENTRY * i;

e = find_existing_event_topic( topic );
if ( e )
    {
    i = find_existing_event_item( e, item );
    if ( i )
        {
        one_hot_link( e->handle, item, XTYP_ADVSTOP );

        e->items.remove();
        delete i;

        /*
        ------------------------------------------------------------
        If there are no more items, disconnect and remove this topic
        ------------------------------------------------------------ */
        e->items.rewind();
        i = (EVENT_ITEM_ENTRY *) e->items.next();
        if ( !i )
            {
            if ( e->handle )
                {
                DdeDisconnect( e->handle );
                e->handle = 0;
                }
            EventList.remove(e);
            delete e;
            }
        }
    }
}

/***********************************************************************
*                          CLIENT_DDE_STARTUP                          *
***********************************************************************/
void client_dde_startup( void )
{

int   i;
UINT  status;
DWORD ddeflags;

ddeflags = APPCLASS_STANDARD | APPCMD_FILTERINITS | APPCMD_CLIENTONLY;
DdeId    = 0;

status = DdeInitialize( &DdeId, (PFNCALLBACK) dde_callback, ddeflags, 0 );
if ( status != DMLERR_NO_ERROR )
    MessageBox( NULL, TEXT("Failed"), TEXT("DdeInitialize"), MB_OK );

ServerNameHandle = DdeCreateStringHandle( DdeId, ServerName, CodePageId );
if ( !ServerNameHandle )
    MessageBox( NULL, TEXT("Failed"), TEXT("DdeCreateStringHandle"), MB_OK );

/*
-----------------------------------
Create handles for each of my items
----------------------------------- */
for ( i=0; i<NofItems; i++ )
    ItemList[i].handle = DdeCreateStringHandle( DdeId, ItemList[i].name, 0 );

connect_to_server();
}

/***********************************************************************
*                          CLIENT_DDE_STARTUP                          *
***********************************************************************/
void client_dde_startup( HWND main_window )
{
MyMainWindow = main_window;

#ifdef UNICODE
    DdeClipFormat = CF_UNICODETEXT;
    CodePageId = CP_WINUNICODE;
#endif

client_dde_startup();
}

/***********************************************************************
*                         CLIENT_DDE_SHUTDOWN                          *
***********************************************************************/
void client_dde_shutdown( void )
{
int i;
EVENT_ENTRY * e;

/*
---------------------------------------
Remove any events left on the eventlist
--------------------------------------- */
EventList.rewind();
while ( TRUE )
    {
    e = (EVENT_ENTRY *) EventList.next();
    if ( !e )
        break;
    if ( e->handle )
        {
        hot_links( e, XTYP_ADVSTOP );
        DdeDisconnect( e->handle );
        }
    delete e;
    }

EventList.remove_all();

/*
--------------------------
Disconnect from the server
-------------------------- */
if ( ServerHandle )
    {
    DdeDisconnect( ServerHandle );
    ServerHandle = 0;
    }

DdeFreeStringHandle( DdeId, ServerNameHandle );
ServerNameHandle = 0;

/*
---------------------------------
Free handles for each of my items
--------------------------------- */
for ( i=0; i<NofItems; i++ )
    {
    DdeFreeStringHandle( DdeId, ItemList[i].handle );
    ItemList[i].handle = 0;
    }

DdeUninitialize( DdeId );
DdeId = 0;
}

/***********************************************************************
*                    GET_CURRENT_PASSWORD_LEVEL                        *
***********************************************************************/
void get_current_password_level( void )
{
EVENT_ENTRY * e;

e = find_existing_event_topic( DDE_CONFIG_TOPIC );
if ( !e )
    return;

if ( !e->handle )
    connect_to_server();

if ( e->handle )
    DdeClientTransaction( 0, 0, e->handle, ItemList[PW_LEVEL_INDEX].handle, DdeClipFormat, XTYP_REQUEST, TIMEOUT_ASYNC, NULL );
}
