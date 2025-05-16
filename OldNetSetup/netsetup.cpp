#include <windows.h>
#include <htmlhelp.h>
#include <winnetwk.h>   /* link to mpr.lib */

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\listbox.h"
#include "..\include\strarray.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "msres.h"

static BOOLEAN CancelEnumeration  = FALSE;
static BOOLEAN CancelAddNewComputers = FALSE;

/*
-------------------------------------
Global for RENAME_NEW_COMPUTER_DIALOG
------------------------------------- */
static STRING_CLASS NewComputerName;

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS ComputerWindow;
HWND         AddComputerDialogWindow = 0;
HWND         AddNewComputersDialogWindow = 0;
HWND         EnumerateComputersDialogWindow = 0;

TCHAR MyClassName[] = TEXT("NetSetup");

const static TCHAR AsterixChar   = TEXT( '*' );
const static TCHAR BackSlashChar = TEXT( '\\' );
const static TCHAR NullChar      = TEXT( '\0' );
const static TCHAR SpaceChar     = TEXT( ' ' );
const static TCHAR TabChar       = TEXT( '\t' );
const static TCHAR WChar         = TEXT( 'W' );
const static TCHAR CapVChar      = TEXT( 'V' );
const static TCHAR VChar         = TEXT( 'v' );
const static TCHAR Char5         = TEXT( '5' );

const int NO_COMPUTER_TYPE          = 0;
const int MONITORING_COMPUTER_TYPE  = 1;
const int WORKSTATION_COMPUTER_TYPE = 2;
const int DATA_SERVER_COMPUTER_TYPE = 4;

static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR DataServerKey[]      = TEXT( "DataServer" );
static TCHAR DataServerName[]     = TEXT( "FS" );
static TCHAR EmptyString[]        = TEXT( "" );
static TCHAR ThisComputerKey[]    = TEXT( "ThisComputer" );
static TCHAR NetDirPrefixString[] = TEXT( "\\\\" );
static TCHAR NoComputerName[]     = NO_COMPUTER;
static TCHAR VisiTrakIniFile[]    = TEXT("\\exes\\visitrak.ini");
static TCHAR ComputerDbName[]     = TEXT("\\data\\computer.txt");
static TCHAR DataDirString[]      = TEXT("\\data");
static TCHAR UnknownString[]      = UNKNOWN;
static TCHAR NetSetupDataFile[]   = TEXT( "NetSetup.dat" );
static TCHAR DataFileSeparator[]  = TEXT( ";" );
static STRING_CLASS YesName;
static STRING_CLASS NoName;
static BOOLEAN Updating = FALSE;

static LISTBOX_CLASS AddComputerListbox;

/*
---------------------------------
Global for NETWORK_COMPUTER_CLASS
--------------------------------- */
TCHAR * NceLine    = 0;
int     NceLineLen = 0;

class NETWORK_COMPUTER_ENTRY
    {
    public:
    TCHAR name[COMPUTER_NAME_LEN+1];
    TCHAR directory[COMPUTER_DIR_LEN+1];
    NETWORK_COMPUTER_ENTRY() { *name = NullChar; *directory = NullChar; }
    ~NETWORK_COMPUTER_ENTRY() {}
    void    operator=( const NETWORK_COMPUTER_ENTRY & sorc );
    BOOLEAN extract_name_from_directory( void );
    };

class NETWORK_COMPUTER_CLASS
    {
    public:
    NETWORK_COMPUTER_ENTRY local;
    NETWORK_COMPUTER_ENTRY net;
    int     type;
    bool    is_changed;
    bool    online;
    TEXT_LIST_CLASS connectlist;

    NETWORK_COMPUTER_CLASS() { type = NO_COMPUTER_TYPE; is_changed = false; online=false; }
    NETWORK_COMPUTER_CLASS( NETWORK_COMPUTER_CLASS & sorc );
    ~NETWORK_COMPUTER_CLASS() {}
    void    operator=( NETWORK_COMPUTER_CLASS & sorc );
    BOOLEAN extract_type_from_name( void );
    BOOLEAN is_connectable( NETWORK_COMPUTER_CLASS & dest );
    TCHAR * lbline( void );
    void    cleanup( void );
    bool    get_online_status();
    void    get_connect_list( void );
    void    save( void );
    void    save_data_server_connection( void );
    void    save_data_server_connection( TCHAR * name_to_save );
    void    remove_data_server_connection( void );
    };

class NETWORK_COMPUTER_LIST_CLASS
    {
    private:

    static NETWORK_COMPUTER_CLASS empty_entry;

    int                      x;
    NETWORK_COMPUTER_CLASS * nc;
    GENERIC_LIST_CLASS       clist;

    public:

    NETWORK_COMPUTER_LIST_CLASS() { nc = &empty_entry; x = -1; }
    ~NETWORK_COMPUTER_LIST_CLASS();
    NETWORK_COMPUTER_CLASS & entry( void ) { return *nc; }
    void    operator=( NETWORK_COMPUTER_LIST_CLASS & sorc );
    BOOLEAN find( int index_to_find );
    BOOLEAN find( TCHAR * computer_to_find );
    BOOLEAN find( NETWORK_COMPUTER_CLASS & computer_to_find );
    BOOLEAN add( NETWORK_COMPUTER_CLASS * nc_to_add );
    BOOLEAN remove();
    void    remove_all();
    void    rewind( void ) { clist.rewind(); nc = &empty_entry; x = -1; }
    BOOLEAN next( void );
    BOOLEAN read();
    BOOLEAN write();
    int     count( void ) { return (int) clist.count(); }
    int     current_index( void ) { return x; }
    };

NETWORK_COMPUTER_CLASS NETWORK_COMPUTER_LIST_CLASS::empty_entry;

NETWORK_COMPUTER_LIST_CLASS ComputerList;
NETWORK_COMPUTER_LIST_CLASS SavedList;

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( int index_to_find )
{

rewind();
while ( next() )
    {
    if ( x == index_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( TCHAR * computer_to_find )
{
if ( !computer_to_find )
    return FALSE;

rewind();
while ( next() )
    {
    if ( strings_are_equal(nc->net.name, computer_to_find) )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
*   This finds a match if either the network name or directory are     *
*   the same.                                                          *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( NETWORK_COMPUTER_CLASS & computer_to_find )
{
rewind();
while ( next() )
    {
    if ( strings_are_equal(nc->net.name, computer_to_find.net.name ) )
        return TRUE;

    if ( strings_are_equal(nc->net.directory, computer_to_find.net.directory) )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               NEXT                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::next( void )
{
NETWORK_COMPUTER_CLASS * e;

e = (NETWORK_COMPUTER_CLASS *) clist.next();
if ( e )
    {
    x++;
    nc = e;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                              READ                                    *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::read()
{
NETWORK_COMPUTER_CLASS * ncc;
FILE_CLASS f;
STRING_ARRAY_CLASS s;
int n;

s = exe_directory();
s += NetSetupDataFile;

n = 0;

if ( f.open_for_read(s.text()) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.len() == 0 )
            break;

        ncc = new NETWORK_COMPUTER_CLASS;
        if ( !ncc )
            return FALSE;

        s.split( DataFileSeparator );
        s.rewind();

        s.next();
        copystring( ncc->local.name, s.text() );

        s.next();
        copystring( ncc->local.directory, s.text() );

        s.next();
        copystring( ncc->net.name, s.text() );

        s.next();
        copystring( ncc->net.directory, s.text() );

        s.next();
        ncc->type = asctoint32( s.text() );

        while( s.next() )
            {
            if ( s.len() > 0 )
                ncc->connectlist.append( s.text(), s.len() );
            }

        add( ncc );
        ncc = 0;
        n++;
        s.remove_all();
        }

    f.close();
    }

if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                             WRITE                                    *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::write()
{
FILE_CLASS f;
STRING_ARRAY_CLASS s;

if ( count() == 0 )
    return FALSE;

s = exe_directory();
s += NetSetupDataFile;

if ( f.open_for_write(s.text()) )
    {
    rewind();
    while ( next() )
        {
        s = nc->local.name, s.text();
        s.add( nc->local.directory );
        s.add( nc->net.name );
        s.add( nc->net.directory );
        s.add( int32toasc((int32) nc->type) );
        nc->connectlist.rewind();
        while( nc->connectlist.next() )
            s.add( nc->connectlist.text() );

        s.join( DataFileSeparator );
        f.writeline( s.text() );
        }

    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               =                                      *
***********************************************************************/
void NETWORK_COMPUTER_LIST_CLASS::operator=( NETWORK_COMPUTER_LIST_CLASS & sorc )
{
NETWORK_COMPUTER_CLASS * cp;

remove_all();

sorc.rewind();
while ( sorc.next() )
    {
    cp = new NETWORK_COMPUTER_CLASS( sorc.entry() );
    add( cp );
    }

}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                           REMOVE_ALL                                 *
***********************************************************************/
void NETWORK_COMPUTER_LIST_CLASS::remove_all()
{
if ( clist.count() > 0 )
    {
    clist.rewind();
    while ( TRUE )
        {
        nc = (NETWORK_COMPUTER_CLASS *) clist.next();
        if ( !nc )
            break;
        delete nc;
        }

    clist.remove_all();
    }

nc = &empty_entry;
x = -1;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                              REMOVE                                  *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::remove()
{
if ( x >= 0 )
    {
    delete nc;
    clist.remove();
    rewind();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                                ADD                                   *
*  This adds a NETWORK_COMPUTER_CLASS to the list. You pass it a       *
*  pointer to a NETWORK_COMPUTER_CLASS that you have made with new.    *
*  The destructor will delete the NETWORK_COMPUTER_CLASS.              *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::add( NETWORK_COMPUTER_CLASS * nc_to_add )
{
return clist.append( (void *) nc_to_add );
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                   ~NETWORK_COMPUTER_LIST_CLASS                       *
***********************************************************************/
NETWORK_COMPUTER_LIST_CLASS::~NETWORK_COMPUTER_LIST_CLASS()
{
remove_all();
}

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                       NETWORK_COMPUTER_ENTRY                         *
*                                 =                                    *
***********************************************************************/
void NETWORK_COMPUTER_ENTRY::operator=( const NETWORK_COMPUTER_ENTRY & sorc )
{
name[0]      = NullChar;
directory[0] = NullChar;

lstrcpy( name,      sorc.name      );
lstrcpy( directory, sorc.directory );
}

/***********************************************************************
*                       NETWORK_COMPUTER_ENTRY                         *
*                    EXTRACT_NAME_FROM_DIRECTORY                       *
*    This is normally only used on the network computer entry          *
*    and not on the local entry.                                       *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_ENTRY::extract_name_from_directory( void )
{
TCHAR * dest;
TCHAR * sorc;
int     count;

sorc = directory;

while ( *sorc == BackSlashChar )
    sorc++;

dest = name;
count = 0;

while ( TRUE )
    {
    *dest = *sorc;

    if ( *dest == BackSlashChar || count >= COMPUTER_NAME_LEN )
        *dest = NullChar;

    if ( *dest == NullChar )
        break;

    count++;
    dest++;
    sorc++;
    }

return TRUE;
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                                  =                                   *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::operator=( NETWORK_COMPUTER_CLASS & sorc )
{
local       = sorc.local;
net         = sorc.net;
type        = sorc.type;
is_changed  = sorc.is_changed;
online      = sorc.online;
connectlist = sorc.connectlist;
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                         NETWORK_COMPUTER_CLASS                       *
***********************************************************************/
NETWORK_COMPUTER_CLASS::NETWORK_COMPUTER_CLASS( NETWORK_COMPUTER_CLASS & sorc )
{
*this = sorc;
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                         EXTRACT_TYPE_FROM_NAME                       *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_CLASS::extract_type_from_name( void )
{
TCHAR s[COMPUTER_NAME_LEN+1];

lstrcpy( s, net.name );
CharUpper( s );

if ( strings_are_equal(s, DataServerName) )
    type = DATA_SERVER_COMPUTER_TYPE;
else if ( s[0] == WChar && is_numeric(s[1]) )
    type = MONITORING_COMPUTER_TYPE;
else
    type = WORKSTATION_COMPUTER_TYPE;

return TRUE;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           GET_ONLINE_STATUS                          *
***********************************************************************/
bool NETWORK_COMPUTER_CLASS::get_online_status()
{
STRING_CLASS s;

s = net.directory;
s += VisiTrakIniFile;

if ( file_exists(s.text()) )
    online = true;
else
    online = false;

return online;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           GET_CONNECT_LIST                           *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::get_connect_list( void )
{
TCHAR dbname[MAX_PATH_NAME_LEN+1];
TCHAR computer[COMPUTER_NAME_LEN+1];
TCHAR buf[COMPUTER_DIR_LEN+1];
TCHAR ini_file_path[MAX_PATH_NAME_LEN+1];
DB_TABLE t;

lstrcpy( dbname, net.directory );
lstrcat( dbname, ComputerDbName );

connectlist.empty();

if ( t.open( dbname, COMPUTER_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( computer, COMPUTER_NAME_OFFSET, COMPUTER_NAME_LEN );
        t.get_alpha( buf,      COMPUTER_DIR_OFFSET,  COMPUTER_DIR_LEN  );
        if ( strings_are_equal(computer, local.name) )
            {
            lstrcpy( local.directory, buf );
            }
        else
            {
            if ( *buf == BackSlashChar )
                {
                if ( !connectlist.find(computer) )
                    connectlist.append( computer );
                }
            }
        }

    t.close();
    }

lstrcpy( ini_file_path, net.directory );
lstrcat( ini_file_path, VisiTrakIniFile );

GetPrivateProfileString( ConfigSection, DataServerKey, UnknownString, buf, COMPUTER_DIR_LEN+1, ini_file_path );
if ( !unknown(buf) && !strings_are_equal(buf, NoComputerName) )
    connectlist.append( buf );

}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           IS_CONNECTABLE                             *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_CLASS::is_connectable( NETWORK_COMPUTER_CLASS & dest )
{
BOOLEAN status;

status = FALSE;

switch( type )
    {
    case MONITORING_COMPUTER_TYPE:
        if (  dest.type & (WORKSTATION_COMPUTER_TYPE | DATA_SERVER_COMPUTER_TYPE) )
            status = TRUE;
        break;

    case WORKSTATION_COMPUTER_TYPE:
        if (  dest.type & MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;

    case DATA_SERVER_COMPUTER_TYPE:
        if (  dest.type & MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;
    }

return status;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_DATA_SERVER_CONNECTION                      *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_server_connection( TCHAR * name_to_save )
{
TCHAR ini_file_path[MAX_PATH_NAME_LEN+1];
TCHAR buf[COMPUTER_NAME_LEN+1];
lstrcpy( ini_file_path, net.directory );
lstrcat( ini_file_path, VisiTrakIniFile );

GetPrivateProfileString( ConfigSection, DataServerKey, NoComputerName, buf, COMPUTER_NAME_LEN+1, ini_file_path );
if ( !strings_are_equal(name_to_save, buf) )
    WritePrivateProfileString( ConfigSection, DataServerKey, name_to_save, ini_file_path );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_DATA_SERVER_CONNECTION                      *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_server_connection( void )
{
save_data_server_connection( ComputerList.entry().net.name );
}

/***********************************************************************
*                    REMOVE_DATA_SERVER_CONNECTION                     *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::remove_data_server_connection( void )
{
save_data_server_connection( NoComputerName );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                                SAVE                                  *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save( void )
{
TEXT_LIST_CLASS tc;
BOOLEAN         have_data_server_connection;
BOOLEAN         have_record;
TCHAR           dbname[COMPUTER_DIR_LEN+1];
TCHAR           computer_name[COMPUTER_NAME_LEN+1];
TCHAR           directory_path[COMPUTER_DIR_LEN+1];
TCHAR         * cp;
DB_TABLE        t;
int             i;
int             n;
NETWORK_COMPUTER_ENTRY * ne;

is_changed = false;
ne = 0;

tc = connectlist;

if ( type == MONITORING_COMPUTER_TYPE )
    {
    have_data_server_connection = FALSE;
    tc.sort();
    tc.rewind();
    while ( tc.next() )
        {
        if ( ComputerList.find(tc.text()) )
            {
            if ( ComputerList.entry().type == DATA_SERVER_COMPUTER_TYPE )
                {
                save_data_server_connection();
                have_data_server_connection = TRUE;
                break;
                }
            }
        }

    if ( !have_data_server_connection )
        remove_data_server_connection();
    }

tc.append( local.name );
tc.sort();
tc.rewind();

lstrcpy( dbname, net.directory );
lstrcat( dbname, ComputerDbName );

if ( t.open( dbname, COMPUTER_RECLEN, FL) )
    {
    /*
    --------------------------------------------
    Make a list of the computers in the old file
    -------------------------------------------- */
    n  = t.nof_recs();
    if ( n > 1 )
        {
        n--;
        ne = new NETWORK_COMPUTER_ENTRY[n];
        if ( ne )
            {
            i = 0;
            while ( t.get_next_record(NO_LOCK) && i < n )
                {
                t.get_alpha( ne[i].name,      COMPUTER_NAME_OFFSET, COMPUTER_NAME_LEN );
                t.get_alpha( ne[i].directory, COMPUTER_DIR_OFFSET,  COMPUTER_DIR_LEN  );
                if ( ne[i].directory[0] == BackSlashChar )
                    i++;
                }
            }
        }

    t.empty();
    while ( tc.next() )
        {
        have_record = FALSE;
        if ( strings_are_equal(local.name, tc.text()) )
            {
            lstrcpy( computer_name,  local.name );
            lstrcpy( directory_path, local.directory );
            have_record = TRUE;
            }
        else
            {
            if ( ComputerList.find(tc.text()) )
                {
                if ( ComputerList.entry().type != DATA_SERVER_COMPUTER_TYPE )
                    {
                    lstrcpy( computer_name,  ComputerList.entry().net.name );
                    lstrcpy( directory_path, ComputerList.entry().net.directory );
                    cp = findstring( DataDirString, ComputerList.entry().local.directory );
                    if ( cp )
                        {
                        lstrcat( directory_path, cp );
                        if ( lstrlen(directory_path) > COMPUTER_DIR_LEN )
                            MessageBox( 0, resource_string(MainInstance, DIR_TOO_LONG_STRING), directory_path, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );
                        else
                            have_record = TRUE;
                        }
                    }
                }
            else if ( ne )
                {
                /*
                ----------------------------------------------------------
                This computer is not on line, copy it back from the list I
                made from the file.
                ---------------------------------------------------------- */
                for ( i=0; i<n; i++ )
                    {
                    if ( strings_are_equal( tc.text(), ne[i].name) )
                        {
                        lstrcpy( computer_name,  ne[i].name );
                        lstrcpy( directory_path, ne[i].directory );
                        have_record = TRUE;
                        break;
                        }
                    }

                }
            }

        if ( have_record )
            {
            t.put_alpha( COMPUTER_NAME_OFFSET, computer_name,  COMPUTER_NAME_LEN );
            t.put_alpha( COMPUTER_DIR_OFFSET,  directory_path, COMPUTER_DIR_LEN );
            t.rec_append();
            }
        }
    t.close();
    }

if ( ne )
    {
    delete[] ne;
    ne = 0;
    }

if ( t.get_global_error() != VS_SUCCESS )
    resource_message_box( MainInstance, ERROR_WRITING_COMPUTER_STRING, CANT_COMPLY_STRING );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                               CLEANUP                                *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::cleanup( void )
{
if ( NceLine )
    {
    delete [] NceLine;
    NceLineLen = 0;
    }
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                                LBLINE                                *
***********************************************************************/
TCHAR * NETWORK_COMPUTER_CLASS::lbline( void )
{
int    nof_chars;
UINT   id;
static TCHAR s[100];
TCHAR * cp;

nof_chars = COMPUTER_NAME_LEN+1+COMPUTER_DIR_LEN+1+MAX_STRING_LEN+1 + YesName.len() + 1 + NoName.len() + 1;
nof_chars += (COMPUTER_NAME_LEN+1) * connectlist.count();

if ( nof_chars > NceLineLen )
    {
    if ( NceLine )
        delete [] NceLine;
    NceLineLen = nof_chars;
    NceLine = maketext( NceLineLen );
    if ( !NceLine )
        {
        NceLineLen = 0;
        return EmptyString;
        }
    }

switch ( type )
    {
    case DATA_SERVER_COMPUTER_TYPE:
        id = DATA_SERVER_COMPUTER_STRING;
        break;

    case MONITORING_COMPUTER_TYPE:
        id = MONITORING_COMPUTER_STRING;
        break;

    default:
        id = WORKSTATION_COMPUTER_STRING;
        break;
    }

cp = copy_w_char( s,  net.name,            TabChar );
cp = copy_w_char( cp, net.directory,       TabChar );
cp = copy_w_char( cp, resource_string(id), TabChar );
if ( connectlist.count() > 0 )
    {
    connectlist.rewind();
    while ( connectlist.next() )
        cp = copy_w_char( cp, connectlist.text(), SpaceChar );
    }

*cp = TabChar;
cp++;

if ( online )
    cp = copystring( cp, YesName.text() );
else
    cp = copystring( cp, NoName.text() );

if ( is_changed )
    {
    *cp = AsterixChar;
    cp++;
    *cp = NullChar;
    }
return s;
}

/***********************************************************************
*                         EXTRACT_COMPUTER_NAME                        *
***********************************************************************/
static TCHAR * extract_computer_name( TCHAR * sorc )
{
static TCHAR s[COMPUTER_NAME_LEN+1];
int count;
TCHAR * dest;
TCHAR * cp;

*s = NullChar;

if ( sorc )
    {
    cp    = sorc;
    dest  = s;
    count = 0;
    while ( TRUE )
        {
        *dest = *cp;

        if ( *dest == TabChar )
            *dest = NullChar;

        if ( count >= COMPUTER_NAME_LEN )
            *dest = NullChar;

        if ( *dest == NullChar )
            break;

        count++;
        dest++;
        cp++;
        }
    }

return s;
}

/***********************************************************************
*                  RENAME_NEW_COMPUTER_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK rename_new_computer_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static TCHAR old_name[COMPUTER_NAME_LEN+1];
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        copystring( old_name, NewComputerName.text() );
        NewComputerName.set_text( hWnd, OLD_COMPUTER_NAME_TBOX );
        set_text( hWnd, NEW_COMPUTER_NAME_EBOX, EmptyString );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, IDCANCEL );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                NewComputerName.get_text( hWnd, NEW_COMPUTER_NAME_EBOX);
                if ( NewComputerName.len() > COMPUTER_NAME_LEN )
                    {
                    resource_message_box( MainInstance, NEW_NAME_TOO_LONG_STRING, CANT_COMPLY_STRING );
                    return TRUE;
                    }
                else if ( NewComputerName.isempty() )
                    {
                    resource_message_box( MainInstance, NO_NAME_ENTERED_STRING, CANT_COMPLY_STRING );
                    return TRUE;
                    }
                else if ( NewComputerName == old_name )
                    {
                    resource_message_box( MainInstance, SAME_NAME_STRING, CANT_COMPLY_STRING );
                    return TRUE;
                    }
                else
                    {
                    EndDialog( hWnd, IDOK );
                    return TRUE;
                    }

            case IDCANCEL:
                EndDialog( hWnd, IDCANCEL );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                             CHECK_FOR_V5                             *
***********************************************************************/
static void check_for_v5( TCHAR * rootdir )
{
static TCHAR V5Ext[]   = TEXT( "\\V5" );
static TCHAR V5dsExt[] = TEXT( "\\V5ds" );
STRING_CLASS root;
STRING_CLASS s;
NETWORK_COMPUTER_CLASS * nce;
bool have_v5;
int  choice;

s = rootdir;
s += V5Ext;

have_v5 = true;
if ( !directory_exists(s.text()) )
    {
    s = rootdir;
    s += V5dsExt;
    if ( !directory_exists(s.text()) )
        {
        MessageBox( 0, resource_string(MainInstance, NO_V5_FOLDER_FOUND_STRING), rootdir, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );
        return;
        }
    }

s.set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX );

if ( CancelAddNewComputers )
    return;

root = s;
s += VisiTrakIniFile;

if ( file_exists(s.text()) )
    {
    nce = new NETWORK_COMPUTER_CLASS;
    if ( nce )
        {
        lstrcpy(  nce->net.directory, root.text() );
        nce->net.extract_name_from_directory();
        nce->extract_type_from_name();
        GetPrivateProfileString( ConfigSection, ThisComputerKey, NoComputerName, nce->local.name, COMPUTER_NAME_LEN+1, s.text() );
        nce->online = true;

        choice = IDCANCEL;

        if ( !CancelAddNewComputers )
            {
            nce->get_connect_list();
            choice = IDOK;

            if ( ComputerList.find(nce->net.name) )
                {
                if ( AddComputerDialogWindow )
                    {
                    NewComputerName = nce->net.name;
                    choice = DialogBox(
                        MainInstance,
                        TEXT("RENAME_NEW_COMPUTER_DIALOG"),
                        AddComputerDialogWindow,
                        (DLGPROC) rename_new_computer_dialog_proc );

                    if ( choice == IDOK )
                        copystring( nce->net.name, NewComputerName.text() );
                    }
                else
                    {
                    MessageBox(0, "AddComputerDialogWindow", "Window not Found", MB_SYSTEMMODAL | MB_OK );
                    }
                }
            }

        if ( choice == IDOK )
            ComputerList.add( nce );
        else
            delete nce;
        }
    }
}

/***********************************************************************
*                        FILL_COMPUTER_LISTBOX                         *
***********************************************************************/
static void fill_computer_listbox()
{
LISTBOX_CLASS lb;
INT           i;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
lb.empty();

if ( ComputerList.count() <= 0 )
    return;

ComputerList.rewind();
while ( ComputerList.next() )
    {
    i = lb.add( ComputerList.entry().lbline() );
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
NETWORK_COMPUTER_CLASS * cp;
LISTBOX_CLASS lb;
STRING_ARRAY_CLASS s;
INT * ip;
int   n;
int   i;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n < 1 )
    return;

for ( i=0; i<n; i++ )
    s.add( extract_computer_name(lb.item_text(ip[i])) );

s.rewind();
while ( s.next() )
    {
    if ( ComputerList.find(s.text()) )
        {
        /*
        ----------------------------------
        Copy the changes to the saved list
        ---------------------------------- */
        if ( SavedList.find(s.text()) )
            {
            SavedList.entry() = ComputerList.entry();
            }
        else
            {
            cp = new NETWORK_COMPUTER_CLASS( ComputerList.entry() );
            SavedList.add( cp );
            }

        /*
        ------------------------------------------------------------
        Save actually modifies which entry is current so I save last
        ------------------------------------------------------------ */
        ComputerList.entry().save();
        }
    }

/*
-----------------------------------------------------
Redisplay the computers to show the new changed flags
----------------------------------------------------- */
fill_computer_listbox();

/*
----------------------
Reselect the same list
---------------------- */
for ( i=0; i<n; i++ )
    lb.set_select( ip[i], TRUE );

/*
--------------------------
I own the array of indexes
-------------------------- */
delete[] ip;

}


/***********************************************************************
*                           REMOVE_COMPUTERS                           *
***********************************************************************/
static void remove_computers( HWND w )
{
LISTBOX_CLASS lb;
STRING_ARRAY_CLASS s;
INT * ip;
int   n;
int   i;

lb.init( w, COMPUTER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n < 1 )
    return;

for ( i=0; i<n; i++ )
    s.add( extract_computer_name(lb.item_text(ip[i])) );

/*
--------------------------
I own the array of indexes
-------------------------- */
delete[] ip;

i = resource_message_box( w, MainInstance, CONFIRM_REMOVE_STRING, WILL_REMOVE_STRING, MB_OKCANCEL | MB_ICONQUESTION | MB_SYSTEMMODAL );
if ( i == IDCANCEL )
    return;

s.rewind();
while ( s.next() )
    {
    if ( ComputerList.find(s.text()) )
        ComputerList.remove();

    if ( SavedList.find(s.text()) )
        SavedList.remove();
    }

fill_computer_listbox();
}

/***********************************************************************
*                        SET_COMPUTER_NAME_EBOX                        *
***********************************************************************/
static void set_computer_name_ebox()
{
static UINT disablelist[] = {
    COMPUTER_NAME_TBOX,
    CONNECTED_COMPUTERS_TBOX,
    AVAILABLE_COMPUTERS_TBOX,
    TYPE_GBOX
    };
const int NTO_DISABLE = sizeof(disablelist)/sizeof(UINT);

LISTBOX_CLASS lb;
TCHAR * cp;
int     id;
INT     n;
TCHAR s[COMPUTER_NAME_LEN+1];
TEXT_LIST_CLASS * mylist;
int               ok_types;
HWND              w;

Updating = TRUE;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );

/*
------------------------------------------------------------
The name cannot be edited if more than one line are selected
------------------------------------------------------------ */
n = lb.select_count();
if ( n > 1 )
    {
    w = ComputerWindow.control( COMPUTER_NAME_EBOX );
    set_text( w, EmptyString );
    EnableWindow( w, FALSE );

    for ( id=0; id<NTO_DISABLE; id++ )
        {
        w = ComputerWindow.control( disablelist[id] );
        EnableWindow( w, FALSE );
        }

    lb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
    lb.empty();

    lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
    lb.empty();

    for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_SERVER_RADIO; id++ )
        {
        w = ComputerWindow.control( id );
        set_checkbox( w, FALSE );
        EnableWindow( w, FALSE );
        }

    Updating   = FALSE;
    return;
    }

for ( id=0; id<NTO_DISABLE; id++ )
    {
    w = ComputerWindow.control( disablelist[id] );
    EnableWindow( w, TRUE );
    }

for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_SERVER_RADIO; id++ )
    {
    w = ComputerWindow.control( id );
    EnableWindow( w, TRUE );
    }

w  = ComputerWindow.control( COMPUTER_NAME_EBOX );
cp = lb.selected_text();
if ( !cp )
    {
    Updating = FALSE;
    return;
    }

lstrcpy( s, extract_computer_name(cp) );
set_text( w, s );
EnableWindow( w, TRUE );

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( strings_are_equal( ComputerList.entry().net.name, s) )
        {

        /*
        ------------------------------------
        Press the radio button for this type
        ------------------------------------ */
        switch( ComputerList.entry().type )
            {
            case MONITORING_COMPUTER_TYPE:
                id = MONITORING_COMPUTER_RADIO;
                break;

            case DATA_SERVER_COMPUTER_TYPE:
                id = DATA_SERVER_RADIO;
                break;

            default:
                id = OFFICE_WORKSTATION_RADIO;
                break;
            }

        CheckRadioButton( ComputerWindow.handle(), MONITORING_COMPUTER_RADIO, DATA_SERVER_RADIO, id );

        /*
        ---------------------------------------
        Fill the listbox of connected computers
        --------------------------------------- */
        lb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
        lb.empty();
        if ( ComputerList.entry().connectlist.count() > 0 )
            {
            ComputerList.entry().connectlist.rewind();
            while ( ComputerList.entry().connectlist.next() )
                lb.add( ComputerList.entry().connectlist.text() );
            }

        /*
        ---------------------------------------
        Fill the listbox of available computers
        --------------------------------------- */
        lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
        lb.empty();
        mylist = &ComputerList.entry().connectlist;

        /*
        -----------------------------------------------------------
        Only list the types of computers that I should connect with
        ----------------------------------------------------------- */
        ok_types = 0;
        switch( ComputerList.entry().type )
            {
            case MONITORING_COMPUTER_TYPE:
                ok_types = WORKSTATION_COMPUTER_TYPE | DATA_SERVER_COMPUTER_TYPE;
                break;

            case WORKSTATION_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;

            case DATA_SERVER_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;
            }

        ComputerList.rewind();
        while ( ComputerList.next() )
            {
            if ( ComputerList.entry().type & ok_types )
                {
                /*
                -------------------------------------------------
                This is available if it is not in my connect list
                ------------------------------------------------- */
                if ( !mylist->find(ComputerList.entry().net.name) )
                    lb.add( ComputerList.entry().net.name );
                }
            }

        break;
        }
    }

Updating = FALSE;
}

/***********************************************************************
*                       DO_CONNECTED_COMPUTER_SELECT                   *
***********************************************************************/
static void do_connected_computer_select( void )
{
LISTBOX_CLASS clb;
LISTBOX_CLASS lb;
INT           i;
INT           n;
STRING_CLASS  s;
STRING_CLASS  dead_computer_name;

clb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
dead_computer_name = clb.selected_text();
i = clb.current_index();
clb.remove( i );

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    ComputerList.entry().is_changed = true;
    ComputerList.entry().connectlist.empty();
    n = clb.count();
    for ( i=0; i<n; i++ )
        ComputerList.entry().connectlist.append( clb.item_text(i) );
    i = lb.current_index();
    lb.replace( ComputerList.entry().lbline() );

    lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
    lb.add( dead_computer_name.text() );
    }

}

/***********************************************************************
*                        DO_STANDARD_CONNECTIONS                       *
***********************************************************************/
static void do_standard_connections( void )
{
NETWORK_COMPUTER_CLASS * e;
INT                      i;
INT                      n;
INT                      sn;
LISTBOX_CLASS lb;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );

sn = lb.select_count();
if ( sn < 1 )
    return;

n = lb.count();
for ( i=0; i<n; i++ )
    {
    if ( lb.is_selected(i) )
        {
        if ( ComputerList.find(extract_computer_name(lb.item_text(i))) )
            {
            e = &ComputerList.entry();
            e->connectlist.empty();
            e->is_changed = true;

            ComputerList.rewind();
            while ( ComputerList.next() )
                {
                if ( e->is_connectable(ComputerList.entry()) )
                    e->connectlist.append( ComputerList.entry().net.name );
                }

            if ( lb.set_current_index(i) )
                {
                lb.replace( e->lbline() );
                }
            }

        if ( sn == 1 )
            set_computer_name_ebox();
        }

    }
}

/***********************************************************************
*                       DO_AVAILABLE_COMPUTER_SELECT                   *
***********************************************************************/
static void do_available_computer_select( void )
{
STRING_CLASS  s;
STRING_CLASS  new_computer_name;
LISTBOX_CLASS clb;
LISTBOX_CLASS lb;
INT           i;

lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
new_computer_name = lb.selected_text();
i = lb.current_index();
lb.remove( i );

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    if ( !ComputerList.entry().connectlist.find(new_computer_name.text()) )
        {
        ComputerList.entry().connectlist.append( new_computer_name.text() );
        ComputerList.entry().is_changed = true;
        i = lb.current_index();
        lb.replace( ComputerList.entry().lbline() );

        lb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
        lb.add( new_computer_name.text() );
        }
    }
}

/***********************************************************************
*                          SET_COMPUTER_TYPE                           *
***********************************************************************/
static void set_computer_type( int button )
{
STRING_CLASS  s;
LISTBOX_CLASS lb;
int           i;
int           newtype;

if ( Updating )
    return;

newtype = WORKSTATION_COMPUTER_TYPE;

switch ( button )
    {
    case MONITORING_COMPUTER_RADIO:
        newtype = MONITORING_COMPUTER_TYPE;
        break;

    case DATA_SERVER_RADIO:
        newtype = DATA_SERVER_COMPUTER_TYPE;
        break;
    }

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    if ( ComputerList.entry().type != newtype )
        {
        ComputerList.entry().type       = newtype;
        ComputerList.entry().is_changed = true;
        i = lb.current_index();
        lb.replace( ComputerList.entry().lbline() );
        set_computer_name_ebox();
        }
    }

}

/***********************************************************************
*                        SET_NEW_COMPUTER_NAME                         *
***********************************************************************/
static void set_new_computer_name( void )
{
TCHAR         new_computer_name[COMPUTER_NAME_LEN+1];
INT           i;
HWND          w;
LISTBOX_CLASS lb;
STRING_CLASS  s;

w = ComputerWindow.control( COMPUTER_NAME_EBOX );
if ( get_text(new_computer_name, w, COMPUTER_NAME_LEN) )
    {
    lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
    s = extract_computer_name( lb.selected_text() );
    if ( ComputerList.find(s.text()) )
        {
        copystring( ComputerList.entry().net.name, new_computer_name );
        ComputerList.entry().is_changed = true;
        i = lb.current_index();
        lb.replace( ComputerList.entry().lbline() );
        }
    }
}

/***********************************************************************
*                          FILL_ADD_COMPUTER_LISTBOX                   *
***********************************************************************/
static BOOLEAN fill_add_computer_listbox( NETRESOURCE * resource_to_enumerate )
{
HANDLE        hEnum;
DWORD         i;
DWORD         nof_bytes = 16384;      // 16K is a good size
DWORD         nof_entries;
DWORD         status;
NETRESOURCE * pr;
TCHAR         s[MAX_PATH+1];
bool          havedir;

TEXT_LIST_CLASS t;
TCHAR         * cp;

if ( CancelEnumeration )
    return FALSE;

/*
------------------------------------------------------------
Make a list of all the network computers in the current list
------------------------------------------------------------ */
ComputerList.rewind();
while ( ComputerList.next() )
    {
    lstrcpy( s, ComputerList.entry().net.directory );
    cp = s;

    /*
    ------------------------------------------
    Truncate the directory after the root name
    ------------------------------------------ */
    while ( *cp == BackSlashChar )
        cp++;
    while ( *cp != BackSlashChar && *cp != NullChar )
        cp++;
    *cp = NullChar;

    t.append( s );
    }

status = WNetOpenEnum(
    RESOURCE_GLOBALNET,
    RESOURCETYPE_DISK,
    RESOURCEUSAGE_CONNECTABLE | RESOURCEUSAGE_CONTAINER,
    resource_to_enumerate,
    &hEnum
    );

if ( status != NO_ERROR )
    return FALSE;

pr = (NETRESOURCE *) GlobalAlloc( GPTR, nof_bytes );

do  {
    if ( CancelEnumeration )
        break;

    ZeroMemory( pr, nof_bytes );

    /*
    ----------------------------------------
    I want to enumerate all possible entries
    ---------------------------------------- */
    nof_entries = 0xFFFFFFFF;
    status = WNetEnumResource( hEnum, &nof_entries, pr, &nof_bytes );
    if ( status == NO_ERROR )
        {
        for( i=0; i<nof_entries; i++ )
            {
            if ( CancelEnumeration )
                break;

            if ( pr[i].dwScope == RESOURCE_GLOBALNET )
                {
                /*
                ------------------------------------------------------------------
                If this NETRESOURCE is a container, call the function recursively.
                ------------------------------------------------------------------ */
                if ( (pr[i].dwUsage & RESOURCEUSAGE_CONTAINER) == RESOURCEUSAGE_CONTAINER )
                    {
                    havedir = false;
                    s[0] = NullChar;
                    lstrcpy( s, pr[i].lpRemoteName );
                    if ( s[0] != NullChar )
                        set_text( EnumerateComputersDialogWindow, CURRENT_RESOURCE_TBOX, s );
                    if ( s[0] == BackSlashChar )
                        {
                        if ( s[1] == BackSlashChar )
                            {
                            havedir = true;
                            if ( !t.find(s) )
                                AddComputerListbox.add( s );
                            }
                        }

                    if ( !havedir )
                        {
                        fill_add_computer_listbox( &pr[i] );
                        }
                    }
                }
            else
                {
                MessageBox( 0, "Resource not globalnet", "Note", MB_OK );
                }

            }
        }

    } while( status != ERROR_NO_MORE_ITEMS );

GlobalFree((HGLOBAL) pr);

status = WNetCloseEnum(hEnum);
if ( status != NO_ERROR)
    return FALSE;

return TRUE;
}

/***********************************************************************
*                      ADD_COMPUTERS_FROM_LISTBOX                      *
***********************************************************************/
bool add_computers_from_listbox()
{
LISTBOX_CLASS lb;
STRING_ARRAY_CLASS s;

INT * ip;
int   n;
int   i;

n = AddComputerListbox.get_select_list( &ip );
if ( n < 1 )
    return false;

for ( i=0; i<n; i++ )
    s.add( AddComputerListbox.item_text(ip[i]) );

/*
--------------------------
I own the array of indexes
-------------------------- */
delete[] ip;

s.rewind();
while ( s.next() )
    {
    if ( CancelAddNewComputers )
        break;
    set_text( AddNewComputersDialogWindow, CURRENT_RESOURCE_TBOX, s.text() );
    check_for_v5( s.text() );
    }

return true;
}

/***********************************************************************
*                 ADD_COMPUTERS_FROM_LISTBOX_THREAD                    *
***********************************************************************/
DWORD add_computers_from_listbox_thread( int * notused )
{
CancelAddNewComputers = FALSE;

if ( add_computers_from_listbox() )
    {
    fill_computer_listbox();
    set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX, EmptyString );
    PostMessage( AddNewComputersDialogWindow, WM_CLOSE, 0, 0L );
    PostMessage( AddComputerDialogWindow,     WM_CLOSE, 0, 0L );
    }

return 0;
}


/***********************************************************************
*                 START_ADD_COMPUTERS_FROM_LISTBOX_THREAD              *
***********************************************************************/
static void start_add_computers_from_listbox_thread()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                     /* no security attributes        */
    0,                                        /* use default stack size        */
    (LPTHREAD_START_ROUTINE) add_computers_from_listbox_thread, /* thread function       */
    0,                                        /* argument to thread function   */
    0,                                        /* use default creation flags    */
    &id );                                    /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, ADD_COMPUTERS_FROM_LISTBOX_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                    ADD_NEW_COMPUTERS_DIALOG_PROC                     *
*                                                                      *
* This is just a little dialog that lets the                           *
* add_computers_from_listbox_thread run once only and give a little    *
* feedback to the user.                                                *
*                                                                      *
***********************************************************************/
BOOL CALLBACK add_new_computers_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        AddNewComputersDialogWindow = hWnd;
        start_add_computers_from_listbox_thread();
        return TRUE;

    case WM_CLOSE:
        if ( AddNewComputersDialogWindow )
            {
            AddNewComputersDialogWindow = 0;
            EndDialog( hWnd, 0 );
            }
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                CancelAddNewComputers = TRUE;
                return TRUE;
            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                    ENUMERATION_THREAD                                *
***********************************************************************/
DWORD enumeration_thread( int * notused )
{
CancelEnumeration = FALSE;

fill_add_computer_listbox(0);

PostMessage( EnumerateComputersDialogWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                     START_ENUMERATION_THREAD                         *
***********************************************************************/
static void start_enumeration_thread()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                     /* no security attributes        */
    0,                                        /* use default stack size        */
    (LPTHREAD_START_ROUTINE) enumeration_thread, /* thread function       */
    0,                                        /* argument to thread function   */
    0,                                        /* use default creation flags    */
    &id );                                    /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, ENUMERATION_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}

/***********************************************************************
*                   ENUMERATE_COMPUTERS_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK enumerate_computers_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EnumerateComputersDialogWindow = hWnd;
        start_enumeration_thread();
        return TRUE;

    case WM_CLOSE:
        if ( EnumerateComputersDialogWindow )
            {
            EnumerateComputersDialogWindow = 0;
            EndDialog( hWnd, 0 );
            }
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                CancelEnumeration = TRUE;
                return TRUE;
            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                       END_ADD_COMPUTER_DIALOG                        *
***********************************************************************/
void end_add_computer_dialog( HWND w )
{
AddComputerDialogWindow = 0;
AddComputerListbox.init( 0 );
EndDialog( w, 0 );

if ( ComputerList.count() == 0 )
    {
    MessageBox( w, TEXT("No computers Selected"), TEXT("Finished"), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );
    MainWindow.close();
    }
}

/***********************************************************************
*                         ADD_COMPUTER_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK add_computer_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        AddComputerDialogWindow = hWnd;
        AddComputerListbox.init(hWnd, ADD_COMPUTER_LB );
        DialogBox(
            MainInstance,
            TEXT("ENUMERATE_COMPUTERS_DIALOG"),
            hWnd,
            (DLGPROC) enumerate_computers_dialog_proc
            );
        return TRUE;

    case WM_CLOSE:
        end_add_computer_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( AddComputerListbox.select_count() > 0 )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("ADD_NEW_COMPUTERS_DIALOG"),
                        hWnd,
                        (DLGPROC) add_new_computers_dialog_proc
                        );
                    }
                else
                    {
                    end_add_computer_dialog( hWnd );
                    }
                return TRUE;

            case IDCANCEL:
                end_add_computer_dialog( hWnd );
                return TRUE;
            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
append_filename_to_path(  s, TEXT("Netsetup.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                          COMPUTER_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK computer_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int title_ids[] = {
    DIRECTORY_TBOX,
    TYPE_TBOX,
    CONNECTIONS_TBOX,
    ONLINE_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {

    case WM_DBINIT:
        limit_text( w, COMPUTER_NAME_EBOX, COMPUTER_NAME_LEN );
        set_listbox_tabs_from_title_positions( w, COMPUTER_LISTBOX, title_ids, nof_title_ids );
        if ( SavedList.read() )
            {
            ComputerList = SavedList;
            ComputerList.rewind();
            while( ComputerList.next() )
                {
                set_text( w, CHECKING_DIR_TBOX, ComputerList.entry().net.name );
                if ( ComputerList.entry().get_online_status() )
                    ComputerList.entry().get_connect_list();
                }
            fill_computer_listbox();
            set_text( w, CHECKING_DIR_TBOX, EmptyString );
            }
        else
            {
            DialogBox(
                MainInstance,
                TEXT("ADD_COMPUTER_DIALOG"),
                w,
                (DLGPROC) add_computer_dialog_proc
                );
            }
        return TRUE;

    case WM_INITDIALOG:
        ComputerWindow = w;
        MainWindow.shrinkwrap( w );
        set_text( w, CHECKING_DIR_TBOX, EmptyString );
        ComputerWindow.post( WM_DBINIT );
        return TRUE;

    case WM_HELP:
        gethelp( w );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                save_changes();
                return TRUE;

            case HELP_BUTTON:
                gethelp( w );
                return TRUE;

            case IDCANCEL:
                MainWindow.close();
                return TRUE;

            case REMOVE_COMPUTER_BUTTON:
                remove_computers(w);
                return TRUE;

            case ADD_COMPUTER_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("ADD_COMPUTER_DIALOG"),
                    w,
                    (DLGPROC) add_computer_dialog_proc
                    );
                return TRUE;

            case STANDARD_CONNECTIONS_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    do_standard_connections();
                    return TRUE;
                    }
                break;

            case MONITORING_COMPUTER_RADIO:
            case OFFICE_WORKSTATION_RADIO:
            case DATA_SERVER_RADIO:
                set_computer_type( id );
                return TRUE;

            case COMPUTER_NAME_EBOX:
                if ( cmd == EN_CHANGE && !Updating )
                    {
                    set_new_computer_name();
                    return TRUE;
                    }
                break;

            case CONNECTED_COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE && !Updating )
                    {
                    do_connected_computer_select();
                    return TRUE;
                    }
                break;

            case AVAILABLE_COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE && !Updating )
                    {
                    do_available_computer_select();
                    return TRUE;
                    }
                break;

            case COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    set_computer_name_ebox();
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{

STRING_CLASS title;
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("NETSETUP_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title   = resource_string( MAIN_WINDOW_TITLE_STRING );
YesName = resource_string( YES_STRING );
NoName  = resource_string( NO_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("COMPUTER_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) computer_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
NETWORK_COMPUTER_CLASS nce;
nce.cleanup();

if ( SavedList.count() )
    SavedList.write();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( ComputerWindow.handle() )
        status = IsDialogMessage( ComputerWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
