#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\computer.h"
#include "..\include\updown.h"
#include "..\include\verrors.h"
#include "..\include\textlist.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#include "resource.h"

#define _MAIN_
#include "extern.h"
#include "..\include\events.h"

#define WM_HELP     0x0053
#define MAX_BOARD_TYPE_LEN 10

static const short NOF_BOARD_TYPES = 3;
static BITSETYPE BoardType[] = { SINGLE_MACHINE_BOARD_TYPE, MUX_BOARD_TYPE, FTII_BOARD_TYPE };
const int TCP_LEN = 22;  /*192.168.254.040  20000 */
const int BUF_LEN = COMPUTER_NAME_LEN + 1 + MAX_PATH;
const int BOARD_BUF_LEN = BOARD_NUMBER_LEN + 1 + TCP_LEN + 1 + HEX_FT_ADDRESS_LEN + 1 + MAX_BOARD_TYPE_LEN + 1 + MHZ_LEN;
const int MACHINE_BUF_LEN = MACHINE_NAME_LEN + 1 + BOARD_NUMBER_LEN + 1 + CHANNEL_NUMBER_LEN;

struct BOARD_ENTRY {
    short     address;
    short     number;
    BITSETYPE type;
    short     mhz;
    STRING_CLASS tcp_addr;
    STRING_CLASS tcp_port;
    };

struct MACHINE_ENTRY {
    TCHAR   name[MACHINE_NAME_LEN+1];
    short   board_number;
    short   ft_channel;
    };

HINSTANCE MainInstance;
HWND      MainWindow;
HWND      BoardsSetupWindow;

TCHAR           MyClassName[]     = TEXT( "V5SETUPS" );
STRING_CLASS    MyWindowTitle;
TEXT_LIST_CLASS AddedComputers;

static LISTBOX_CLASS AddComputerListbox;

static TCHAR   BackSlashChar     = TEXT( '\\' );
static TCHAR   NullChar          = TEXT( '\0' );
static TCHAR   TabChar           = TEXT( '\t' );
static TCHAR   SpaceChar         = TEXT( ' ' );
static TCHAR   SpaceString[]     = TEXT( " " );
static TCHAR   TabStr[]          = TEXT( "\t" );
static TCHAR   EmptyString[]     = TEXT( "" );

/*
--------------
Ini file names
-------------- */
static TCHAR   VisiTrakIniFile[] = TEXT("\\V5\\Exes\\Visitrak.ini");
static TCHAR   ConfigSection[]   = TEXT( "Config" );
static TCHAR   ThisComputerKey[] = TEXT( "ThisComputer" );

static TCHAR   ComputerLineBuf[BUF_LEN+1];
static TCHAR   BoardLineBuf[BOARD_BUF_LEN+1];
static TCHAR   MachineLineBuf[MACHINE_BUF_LEN+1];
static TCHAR   ThisComputer[COMPUTER_NAME_LEN+1];
static TCHAR   CommaString[] = TEXT( "," );

STRING_CLASS   ActualComputerName;              /* The network name of this computer */

static BOOLEAN BoardsHaveChanged;
static BOOLEAN ComputersHaveChanged;
static BOOLEAN MachinesHaveChanged;
static BOOLEAN ProgramIsUpdating;
static BOOLEAN HaveComputerBoxes = FALSE;

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                       GET_ACTUAL_COMPUTER_NAME                       *
***********************************************************************/
static void get_actual_computer_name()
{
TCHAR s[MAX_PATH+1];
DWORD n;

n  = MAX_PATH;

GetComputerName( s, &n );
ActualComputerName = TEXT("\\\\" );
ActualComputerName += s;
}

/***********************************************************************
*                        COMPUTER_NAME_AND_DIR                         *
***********************************************************************/
static BOOLEAN computer_name_and_dir( TCHAR ** name, TCHAR ** directory, TCHAR * sorc )
{
TCHAR * cp;

*ComputerLineBuf = NullChar;
*name            = ComputerLineBuf;
*directory       = ComputerLineBuf;

if ( sorc )
    {
    lstrcpy( ComputerLineBuf, sorc );
    cp = findchar( TabChar, ComputerLineBuf, BUF_LEN );
    if ( cp )
        {
        *cp = NullChar;
        cp++;
        *directory = cp;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                       MAKE_COMPUTER_NAME_LIST                        *
*   Fill a text list with the names in the computer listbox            *
***********************************************************************/
static void make_computer_name_list( TEXT_LIST_CLASS & dest )
{
TCHAR * cp;
LISTBOX_CLASS lb;
int n;
int i;

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

n = lb.count();
for ( i=0; i<n; i++ )
    {
    copystring( ComputerLineBuf, lb.item_text(i) );
    cp = findchar( TabChar, ComputerLineBuf, BUF_LEN );
    if ( cp )
        {
        *cp = NullChar;
        dest.append( ComputerLineBuf );
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
TCHAR       * cp;
TCHAR       * directory;
TCHAR         s[MAX_PATH+1];
BOOLEAN       havedir;
BOOLEAN       ok_to_add;
int x;
int n;
TEXT_LIST_CLASS t;
LISTBOX_CLASS lb;

status = WNetOpenEnum(
    RESOURCE_GLOBALNET,
    RESOURCETYPE_DISK,
    RESOURCEUSAGE_CONNECTABLE | RESOURCEUSAGE_CONTAINER,
    resource_to_enumerate,
    &hEnum
    );

if ( status != NO_ERROR )
    return FALSE;

/*
------------------------------------------------------
Make a list of all the directories in the current list
------------------------------------------------------ */
lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

n = lb.count();
for ( x=0; x<n; x++ )
    {
    if ( computer_name_and_dir(&cp, &directory, lb.item_text(x)) )
        {
        if ( !is_empty(directory) )
            {
            cp = directory;

            /*
            ------------------------------------------
            Truncate the directory after the root name
            ------------------------------------------ */
            while ( *cp == BackSlashChar )
                cp++;
            while ( *cp != BackSlashChar && *cp != NullChar )
                cp++;
            *cp = NullChar;

            /*
            ---------------
            Add to the list
            --------------- */
            t.append( directory );
            }
        }
    }

pr = (NETRESOURCE *) GlobalAlloc( GPTR, nof_bytes );

do  {
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
            if ( pr[i].dwScope == RESOURCE_GLOBALNET )
                {
                /*
                ------------------------------------------------------------------
                If this NETRESOURCE is a container, call the function recursively.
                ------------------------------------------------------------------ */
                if ( (pr[i].dwUsage & RESOURCEUSAGE_CONTAINER) == RESOURCEUSAGE_CONTAINER )
                    {
                    havedir = FALSE;
                    s[0] = NullChar;
                    lstrcpy( s, pr[i].lpRemoteName );
                    if ( s[0] == BackSlashChar )
                        {
                        if ( s[1] == BackSlashChar )
                            {
                            havedir   = TRUE;
                            ok_to_add = TRUE;
                            if ( ActualComputerName == s )
                                ok_to_add = FALSE;
                            else
                                {
                                t.rewind();
                                while ( t.next() )
                                    {
                                    if ( strings_are_equal(s, t.text())  )
                                        {
                                        ok_to_add = FALSE;
                                        break;
                                        }
                                    }
                                }
                            if ( ok_to_add )
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
                MessageBox( 0, TEXT("Resource not globalnet"), TEXT("Note"), MB_OK );
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
*                        SELECTED_COMPUTER_NAME                        *
***********************************************************************/
static TCHAR * selected_computer_name( void )
{
static TCHAR name[COMPUTER_NAME_LEN+1];
TCHAR         * cp;
LISTBOX_CLASS   lb;

if ( HaveComputerBoxes )
    {
    lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

    lstrcpyn( name, lb.selected_text(), COMPUTER_NAME_LEN+1 );
    *(name+COMPUTER_NAME_LEN) = NullChar;
    cp = findchar( TabChar, name, COMPUTER_NAME_LEN );
    if ( cp )
        *cp = NullChar;
    }
else
    lstrcpy( name, ThisComputer );

return name;
}

/***********************************************************************
*                        MACHINE_LISTBOX_STRING                        *
***********************************************************************/
static TCHAR * machine_listbox_string( MACHINE_ENTRY & m )
{
TCHAR * cp;

cp = MachineLineBuf;

cp = copystring( cp, m.name );
cp = copystring( cp, TabStr   );

insalph( cp, m.board_number, BOARD_NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
cp += BOARD_NUMBER_LEN;
cp = copystring( cp, TabStr   );

insalph( cp, m.ft_channel, CHANNEL_NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
cp += CHANNEL_NUMBER_LEN;

*cp = NullChar;

return MachineLineBuf;
}

/***********************************************************************
*                       MACHINE_FROM_LISTBOXLINE                       *
***********************************************************************/
static MACHINE_ENTRY * machine_from_listboxline( TCHAR * sorc )
{
static MACHINE_ENTRY m;
TCHAR              * cp;

cp = MachineLineBuf;
lstrcpy( cp, sorc );

lstrcpy( m.name, NO_MACHINE );
m.board_number = MIN_FT_NUMBER;
m.ft_channel   = MIN_FT_CHANNEL;

if ( replace_tab_with_null(cp) )
    {
    lstrcpy( m.name, cp );

    cp = nextstring( cp );
    if ( replace_tab_with_null(cp) )
        {
        m.board_number = (short) asctoint32( cp );

        cp = nextstring( cp );
        m.ft_channel = (short) asctoint32( cp );
        }
    }

return &m;
}

/***********************************************************************
*                     REFRESH_MACHINE_LISTBOX_ENTRY                    *
***********************************************************************/
static void refresh_machine_listbox_entry( void )
{
LISTBOX_CLASS   lb;
INT             n;
INT             x;
MACHINE_ENTRY * m;

/*
-------------------------
React to human input only
------------------------- */
if ( ProgramIsUpdating )
    return;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

m = machine_from_listboxline( lb.selected_text() );
n = 1 + MACHINE_BOARD_RADIO_8 - MACHINE_BOARD_RADIO_1;

for ( x=0; x<n; x++ )
    {
    if ( is_checked(BoardsSetupWindow, MACHINE_BOARD_RADIO_1 + x) )
        {
        m->board_number = x+1;
        break;
        }
    }

n = 1 + MACHINE_MUX_RADIO_8 - MACHINE_MUX_RADIO_1;
for ( x=0; x<n; x++ )
    {
    if ( is_checked(BoardsSetupWindow, MACHINE_MUX_RADIO_1 + x) )
        {
        m->ft_channel = x+1;
        break;
        }
    }

if ( lb.replace(machine_listbox_string(*m)) )
    MachinesHaveChanged = TRUE;

}

/***********************************************************************
*                          DO_MACHINE_SELECT                           *
***********************************************************************/
static void do_machine_select()
{
LISTBOX_CLASS  lb;
MACHINE_ENTRY  * m;
INT              x;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

m = machine_from_listboxline( lb.selected_text() );

ProgramIsUpdating = TRUE;

x = m->board_number - 1 + MACHINE_BOARD_RADIO_1;
CheckRadioButton( BoardsSetupWindow, MACHINE_BOARD_RADIO_1, MACHINE_BOARD_RADIO_8, x );

x = m->ft_channel - 1 + MACHINE_MUX_RADIO_1;
CheckRadioButton( BoardsSetupWindow, MACHINE_MUX_RADIO_1, MACHINE_MUX_RADIO_8, x );

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             FILL_MACHINES                            *
***********************************************************************/
static void fill_machines( void )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
DB_TABLE       t;
MACHINE_ENTRY  m;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );
lb.empty();
lb.redraw_off();

c.rewind();
while( c.next() )
    {
    if ( lstrcmp(c.name(), ThisComputer) != 0 )
        continue;

    if ( c.is_present() )
        {
        t.open( machset_dbname(ThisComputer), MACHSET_RECLEN, PFL );
        while ( t.get_next_record(FALSE) )
            {
            t.get_alpha( m.name, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
            m.board_number = t.get_short(   MACHSET_FT_BOARD_NO_OFFSET );
            m.ft_channel   = t.get_short(   MACHSET_MUX_CHAN_OFFSET );
            lb.add( machine_listbox_string(m) );
            }
        t.close();
        break;
        }
    }

lb.redraw_on();
lb.setcursel( 0 );
do_machine_select();
MachinesHaveChanged = FALSE;
}

/***********************************************************************
*                            SAVE_MACHINES                             *
***********************************************************************/
static void save_machines( void )
{
LISTBOX_CLASS   lb;
INT             i;
INT             n;
DB_TABLE        t;
MACHINE_ENTRY * m;

if ( !MachinesHaveChanged )
    return;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

n = lb.count();

t.open( machset_dbname(ThisComputer), MACHSET_RECLEN, PWL );

for ( i=0; i<n; i++ )
    {
    m = machine_from_listboxline( lb.item_text(i) );
    t.reset_search_mode();
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, m->name, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, WITH_LOCK) )
        {
        t.put_short( MACHSET_FT_BOARD_NO_OFFSET, m->board_number, BOARD_NUMBER_LEN );
        t.put_short( MACHSET_MUX_CHAN_OFFSET,    m->ft_channel,   CHANNEL_NUMBER_LEN );
        t.rec_update();
        t.unlock_record();
        }
    }

t.close();

MachinesHaveChanged = FALSE;
}

/***********************************************************************
*                       COMPUTER_LISTBOX_STRING                        *
***********************************************************************/
TCHAR * computer_listbox_string( TCHAR * computer_name, TCHAR * computer_directory )
{
lstrcpy( ComputerLineBuf, computer_name );
lstrcat( ComputerLineBuf, TabStr   );
lstrcat( ComputerLineBuf, computer_directory );

return ComputerLineBuf;
}

/***********************************************************************
*                       COMPUTER_LISTBOX_STRING                        *
***********************************************************************/
TCHAR * computer_listbox_string( HWND w )
{
TCHAR * cp;

cp = ComputerLineBuf;
SendDlgItemMessage( w, COMPUTER_NAME_EDITBOX, WM_GETTEXT, COMPUTER_NAME_LEN+1, (LPARAM)(LPSTR) cp );

lstrcat( ComputerLineBuf, TabStr );

cp = ComputerLineBuf + lstrlen(ComputerLineBuf);
SendDlgItemMessage( w, COMPUTER_DIRECTORY_EDITBOX, WM_GETTEXT, MAX_PATH+1, (LPARAM)(LPSTR) cp );

return ComputerLineBuf;
}

/***********************************************************************
*                           FILL_COMPUTERS                             *
***********************************************************************/
void fill_computers( void )
{
LISTBOX_CLASS  lb;
COMPUTER_CLASS c;

set_text( BoardsSetupWindow, THIS_COMPUTER_TBOX, ThisComputer );

if ( !HaveComputerBoxes )
    return;

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );
lb.empty();
lb.redraw_off();

c.rewind();
while( c.next() )
    lb.add( computer_listbox_string( c.name(), c.directory()) );

lb.setcursel( 0 );
lb.redraw_on();

ComputersHaveChanged   = FALSE;
BoardsHaveChanged      = FALSE;
}

/***********************************************************************
*                      COMPUTER_FROM_LISTBOXLINE                       *
***********************************************************************/
static TCHAR * computer_from_listboxline( TCHAR * sorc )
{
static TCHAR computer[COMPUTER_NAME_LEN+1];
TCHAR      * cp;

if ( HaveComputerBoxes )
    {
    lstrcpyn( computer, sorc, COMPUTER_NAME_LEN+1 );
    computer[COMPUTER_NAME_LEN] = NullChar;

    cp = findchar( TabChar, computer, COMPUTER_NAME_LEN );
    if ( cp )
        *cp = NullChar;
    }
else
    lstrcpy( computer, ThisComputer );

return computer;
}

/***********************************************************************
*                        BOARD_FROM_LISTBOXLINE                        *
***********************************************************************/
static BOARD_ENTRY * board_from_listboxline( TCHAR * sorc )
{
static BOARD_ENTRY b;

TCHAR        * cp;
TCHAR        * ap;
STRING_CLASS   type_name;

int32          i;

cp = BoardLineBuf;
lstrcpy( cp, sorc );

b.number  = MIN_FT_NUMBER;
b.address = MIN_FT_ADDRESS;
b.type    = BoardType[0];
b.mhz     = DEF_FT_MHZ;

/*
------------
Board number
------------ */
if ( replace_tab_with_null(cp) )
    {
    b.number = (short) asctoint32( cp );
    cp = nextstring( cp );

    if ( replace_tab_with_null(cp) )
        {
        ap = cp;
        cp = nextstring( cp );
        replace_tab_with_null( cp );
        }

    /*
    --------------------------------
    Board type
    cp now points to the type string
    -------------------------------- */
    for ( i=0; i<NOF_BOARD_TYPES; i++ )
        {
        type_name = resource_string( SINGLE_BOARD_STRING+i );
        if ( type_name == cp )
            {
            b.type = BoardType[i];
            break;
            }
        }

    /*
    -------------
    Board Address
    ------------- */
    if ( b.type & FTII_BOARD_TYPE )
        {
        b.tcp_addr = ap;
        }
    else
        {
        b.tcp_addr = EmptyString;
        b.address = (short) extlong( ap, lstrlen(ap), HEX_RADIX );
        }

    cp = nextstring( cp );
    if ( b.type & FTII_BOARD_TYPE )
        b.tcp_port = cp;
    else
        b.mhz = (short) asctoint32( cp );
    }

return &b;
}

/***********************************************************************
*                           SHOW_BOARD_BOXES                           *
***********************************************************************/
static void show_board_boxes( BOARD_ENTRY & b )
{
int old_board_ids[] = {MHZ_4_BUTTON, MHZ_12_BUTTON, CLOCK_SPEED_TXT, MHZ_TXT,
BOARD_ADDRESS_UPDOWN, BOARD_ADDR_TXT, BOARD_ADDRESS_EDITBOX };
const int nof_old_ids = sizeof(old_board_ids)/sizeof(int);

int ftii_board_ids[] = {TCP_ADDR_EBOX, TCP_PORT_EBOX, TCP_ADDR_TXT, TCP_PORT_TXT };
const int nof_ftii_ids = sizeof(ftii_board_ids)/sizeof(int);

int i;
WINDOW_CLASS   w;

if ( b.type & FTII_BOARD_TYPE )
    {
    for ( i=0; i<nof_old_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, old_board_ids[i] );
        w.hide();
        }

    for ( i=0; i<nof_ftii_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, ftii_board_ids[i] );
        w.show();
        }
    }
else
    {
    for ( i=0; i<nof_old_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, old_board_ids[i] );
        w.show();
        }

    for ( i=0; i<nof_ftii_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, ftii_board_ids[i] );
        w.hide();
        }
    }
}

/***********************************************************************
*                           DO_BOARD_SELECT                            *
***********************************************************************/
static void do_board_select( void )
{
BOARD_ENTRY  * b;
LISTBOX_CLASS  lb;
int32          current_index;
int32          i;
int32          n;
int32          x;
BOOL           is_enabled[MAX_BOARDS];


lb.init( BoardsSetupWindow, BOARD_LISTBOX );

b = board_from_listboxline( lb.selected_text() );

ProgramIsUpdating = TRUE;

x = b->number - 1 + BOARD_RADIO_1;
CheckRadioButton( BoardsSetupWindow, BOARD_RADIO_1, BOARD_RADIO_8, x );

set_text( BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, ultohex((unsigned long) b->address) );

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( b->type & BoardType[i] )
        {
        x = SINGLE_BOARD_TYPE_BUTTON + i;
        CheckRadioButton( BoardsSetupWindow, SINGLE_BOARD_TYPE_BUTTON, FTII_BOARD_TYPE_BUTTON, x );
        break;
        }
    }

show_board_boxes(*b);

if ( b->type & FTII_BOARD_TYPE )
    {
    b->tcp_addr.set_text( BoardsSetupWindow, TCP_ADDR_EBOX );
    b->tcp_port.set_text( BoardsSetupWindow, TCP_PORT_EBOX );
    }
else
    {
    if ( b->mhz == 4 )
        x = MHZ_4_BUTTON;
    else
        x = MHZ_12_BUTTON;
    CheckRadioButton( BoardsSetupWindow, MHZ_4_BUTTON, MHZ_12_BUTTON, x );
    }

n = lb.count();
if ( n > 1 )
    current_index = lb.selected_index();
else
    current_index = 0;

for ( i=0; i<MAX_BOARDS; i++ )
    is_enabled[i] = TRUE;

/*
-------------------------------------------------
Disable any radio button that already has a board
------------------------------------------------- */
for ( i=0; i<n; i++ )
    {
    if ( i != current_index )
        {
        b = board_from_listboxline( lb.item_text(i) );
        is_enabled[b->number-1] = FALSE;
        }
    }

for ( i=0; i<MAX_BOARDS; i++ )
    {
    x = BOARD_RADIO_1 + i;
    EnableWindow( GetDlgItem(BoardsSetupWindow, x), is_enabled[i] );
    }

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                          BOARD_LISTBOX_STRING                        *
***********************************************************************/
TCHAR * board_listbox_string( BOARD_ENTRY & b )
{
TCHAR   * cp;
int       i;

cp = BoardLineBuf;

insalph( cp, b.number,  BOARD_NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
cp += BOARD_NUMBER_LEN;
cp = copystring( cp, TabStr   );

if ( b.type & FTII_BOARD_TYPE )
    {
    cp = copystring( cp, b.tcp_addr.text() );
    }
else
    {
    insalph( cp, b.address, HEX_FT_ADDRESS_LEN, SpaceChar, HEX_RADIX );
    cp += HEX_FT_ADDRESS_LEN;
    }

cp = copystring( cp, TabStr   );

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( b.type & BoardType[i] )
        {
        cp = copystring( cp, resource_string(SINGLE_BOARD_STRING+i) );
        break;
        }
    }

cp = copystring( cp, TabStr   );

if ( b.type & FTII_BOARD_TYPE )
    {
    cp = copystring( cp, b.tcp_port.text() );
    }
else
    {
    insalph( cp, b.mhz, MHZ_LEN, SpaceChar, DECIMAL_RADIX );
    cp += MHZ_LEN;
    }

*cp = NullChar;

return BoardLineBuf;
}

/***********************************************************************
*                          BOARD_LISTBOX_STRING                        *
*                                                                      *
*              This uses the current record in the table               *
***********************************************************************/
TCHAR * board_listbox_string( DB_TABLE & t )
{
BOARD_ENTRY b;

b.number  = t.get_short( BOARDS_NUMBER_OFFSET );
b.address = t.get_short( BOARDS_ADDRESS_OFFSET );
b.type    = (BITSETYPE) t.get_long( BOARDS_TYPE_OFFSET );
b.mhz     = t.get_short( BOARDS_MHZ_OFFSET );

return board_listbox_string( b );
}

/***********************************************************************
*                     FTII_BOARD_LISTBOX_STRING                        *
*                                                                      *
*              This uses the current record in the table               *
***********************************************************************/
TCHAR * ftii_board_listbox_string( DB_TABLE & t )
{
BOARD_ENTRY b;

b.tcp_addr.upsize( TCP_ADDRESS_LEN );
b.tcp_port.upsize( TCP_PORT_LEN );

b.type    = (BITSETYPE) FTII_BOARD_TYPE;
b.number  = t.get_short( FTII_BOARDS_NUMBER_OFFSET );
t.get_alpha( b.tcp_addr.text(), FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );
t.get_alpha( b.tcp_port.text(), FTII_BOARDS_PORT_OFFSET,    TCP_PORT_LEN );

return board_listbox_string( b );
}

/***********************************************************************
*                             FILL_BOARDS                              *
***********************************************************************/
static void fill_boards( void )
{
LISTBOX_CLASS  lb;
COMPUTER_CLASS c;
DB_TABLE       t;
STRING_CLASS   s;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );
lb.empty();
lb.redraw_off();

c.rewind();
while( c.next() )
    {
    if ( lstrcmp(c.name(), ThisComputer) != 0 )
        continue;

    if ( c.is_present() )
        {
        s = boards_dbname( c.name() );
        if ( file_exists(s.text()) )
            {
            t.open( s.text(), BOARDS_RECLEN, PFL );
            while ( t.get_next_record(FALSE) )
                lb.add( board_listbox_string(t) );
            t.close();
            }

        s = ftii_boards_dbname( c.name() );
        if ( file_exists(s.text()) )
            {
            t.open( s.text(), FTII_BOARDS_RECLEN, PFL );
            while ( t.get_next_record(FALSE) )
                lb.add( ftii_board_listbox_string(t) );
            t.close();
            }
        else
            {
            t.create( s.text() );
            }
        break;
        }
    }

lb.redraw_on();
lb.setcursel( 0 );
do_board_select();
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
const  int NOF_COMPUTER_TABS = 1;
const  int NOF_BOARD_TABS    = 3;
const  int NOF_MACHINE_TABS  = 2;
const  int MAX_TABS          = 3;
static int tabs[MAX_TABS];
static int board_title_ids[NOF_BOARD_TABS] = { BOARD_ADDR_TITLE, BOARD_TYPE_TITLE, CLOCK_MHZ_TITLE };

int x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

/*
----------------
Computer listbox
---------------- */
tabs[0] = (COMPUTER_NAME_LEN + 6) * x;

if ( HaveComputerBoxes )
    SendDlgItemMessage( BoardsSetupWindow, COMPUTER_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_COMPUTER_TABS, (LPARAM) tabs );

/*
-------------
Board listbox
------------- */
set_listbox_tabs_from_title_positions( BoardsSetupWindow, BOARD_LISTBOX, board_title_ids, NOF_BOARD_TABS );

/*
---------------
Machine listbox
--------------- */
tabs[0] = (MACHINE_NAME_LEN + 6) * x;
tabs[1] = tabs[0] + (BOARD_NUMBER_LEN + 6) * x;

SendDlgItemMessage( BoardsSetupWindow, MACHINE_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_MACHINE_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                     REFRESH_BOARD_LISTBOX_ENTRY                      *
***********************************************************************/
static void refresh_board_listbox_entry( void )
{
BOARD_ENTRY b;

LISTBOX_CLASS  lb;
int            i;
TCHAR          s[HEX_FT_ADDRESS_LEN+1];

if ( ProgramIsUpdating )
    return;

get_text( s, BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, HEX_FT_ADDRESS_LEN+1 );
b.address = (short) hextoul( s );

b.number = NO_BOARD_NUMBER;
for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( is_checked(BoardsSetupWindow, BOARD_RADIO_1 + i) )
        {
        b.number = i+1;
        break;
        }
    }

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( is_checked(BoardsSetupWindow, SINGLE_BOARD_TYPE_BUTTON + i) )
        {
        b.type = BoardType[i];
        break;
        }
    }

if ( b.type & FTII_BOARD_TYPE )
    {
    b.tcp_addr.get_text( BoardsSetupWindow, TCP_ADDR_EBOX );
    b.tcp_port.get_text( BoardsSetupWindow, TCP_PORT_EBOX );
    }
else
    {
    if ( is_checked(BoardsSetupWindow, MHZ_4_BUTTON) )
        b.mhz = 4;
    else
        b.mhz = 12;
    }

show_board_boxes( b );

lb.init( BoardsSetupWindow, BOARD_LISTBOX );
if ( lb.replace(board_listbox_string(b)) )
    BoardsHaveChanged = TRUE;
}

/***********************************************************************
*                               SAVE_BOARDS                            *
***********************************************************************/
static void save_boards( void )
{
LISTBOX_CLASS  lb;
INT            i;
INT            n;
DB_TABLE       t;
DB_TABLE       ft;
BOARD_ENTRY  * b;

if ( !BoardsHaveChanged )
    return;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();

t.open( boards_dbname(ThisComputer), BOARDS_RECLEN, FL );
t.empty();

ft.open( ftii_boards_dbname(ThisComputer), FTII_BOARDS_RECLEN, FL );
ft.empty();

for ( i=0; i<n; i++ )
    {
    b = board_from_listboxline( lb.item_text(i) );
    if ( b->type & FTII_BOARD_TYPE  )
        {
        ft.put_short( FTII_BOARDS_NUMBER_OFFSET,  b->number,          BOARD_NUMBER_LEN );
        ft.put_alpha( FTII_BOARDS_ADDRESS_OFFSET, b->tcp_addr.text(), TCP_ADDRESS_LEN );
        ft.put_alpha( FTII_BOARDS_PORT_OFFSET,    b->tcp_port.text(), TCP_PORT_LEN );
        ft.rec_append();
        }
    else
        {
        t.put_short( BOARDS_NUMBER_OFFSET,  b->number,  BOARD_NUMBER_LEN );
        t.put_short( BOARDS_ADDRESS_OFFSET, b->address, DECIMAL_FT_ADDRESS_LEN );
        t.put_long(  BOARDS_TYPE_OFFSET,    b->type,    BITSETYPE_LEN );
        t.put_short( BOARDS_MHZ_OFFSET,     b->mhz,     BOARDS_DB_MHZ_LEN );
        t.put_short( BOARDS_CYCLES_PER_TIME_COUNT_OFFSET,  DEF_CYCLES_PER_TIME_COUNT, BOARDS_DB_CYCLES_LEN );
        t.put_short( BOARDS_COUNTS_PER_POS_MARK_OFFSET,    DEF_COUNTS_PER_POS_MARK,   BOARDS_DB_COUNTS_LEN );
        t.rec_append();
        }
    }
t.close();
ft.close();

BoardsHaveChanged = FALSE;
}

/***********************************************************************
*                         IS_NETWORK_DIRECTORY                         *
***********************************************************************/
BOOLEAN is_network_directory( TCHAR * s )
{

if ( *s == BackSlashChar )
    {
    s++;
    if ( *s == BackSlashChar )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        UPDATE_UPDOWN_EDITBOX                         *
***********************************************************************/
static void update_updown_editbox( void )
{
TCHAR s[HEX_FT_ADDRESS_LEN+1];
short i;

UP_DOWN_CLASS u( BoardsSetupWindow, BOARD_ADDRESS_UPDOWN );

i = u.getvalue();

insalph( s, i, HEX_FT_ADDRESS_LEN, SpaceChar, HEX_RADIX );
s[HEX_FT_ADDRESS_LEN] = NullChar;

set_text( BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, s );
}

/***********************************************************************
*                         INIT_UPDOWN_CONTROL                          *
***********************************************************************/
static void init_updown_control( void )
{

UP_DOWN_CLASS u( BoardsSetupWindow, BOARD_ADDRESS_UPDOWN );
u.sethex();
u.setrange( MIN_FT_ADDRESS, MAX_FT_ADDRESS );
u.setincrement( FT_ADDRESS_INCREMENT );

u.setbuddy( BOARD_ADDRESS_EDITBOX );
}


/***********************************************************************
*                     POSITION_COMPUTER_DIALOG                         *
* Position a dialog just to the right of the computer listbox and      *
* just below the "New" button                                          *
***********************************************************************/
static void position_computer_dialog( HWND w )
{
LONG x;
LONG y;
RECT r;

GetWindowRect( GetDlgItem(BoardsSetupWindow, COMPUTER_LISTBOX), &r );
x = r.right;

GetWindowRect( GetDlgItem(BoardsSetupWindow, NEW_COMPUTER_BUTTON), &r );
y = r.bottom;

GetWindowRect( w, &r );

MoveWindow( w, x, y, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                            SAVE_NEW_NAME                             *
***********************************************************************/
static void save_new_name( HWND w )
{
LISTBOX_CLASS lb;
TCHAR       * cp;
TCHAR       * name;
STRING_CLASS   s;
STRING_CLASS   directory;

s.get_text( w, COMPUTER_NAME_EBOX );
if ( !s.isempty() )
    {
    lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );
    if ( computer_name_and_dir(&name, &cp, lb.selected_text()) )
        {
        directory = cp;
        lb.replace( computer_listbox_string(s.text(), directory.text()) );
        ComputersHaveChanged = TRUE;
        }
    }
}

/***********************************************************************
*                         EDIT_COMPUTER_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK edit_computer_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int id;
static TCHAR * name;
static WINDOW_CLASS wc;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        position_computer_dialog( w );
        name = selected_computer_name();
        set_text( w, COMPUTER_NAME_EBOX, name );
        if ( compare(ThisComputer, name, COMPUTER_NAME_LEN) == 0 )
            {
            resource_message_box( MainInstance, NO_EDIT_THIS_COMPUTER_STRING, UNABLE_TO_COMPLY_STRING );
            wc = w;
            wc.post( WM_DBCLOSE );
            }
        return TRUE;

    case WM_DBCLOSE:
        EndDialog( w, 0 );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_new_name( w );

            case IDCANCEL:
                EndDialog( w, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                    ADD_SELECTED_NETWORK_COMPUTERS                    *
***********************************************************************/
static void add_selected_network_computers()
{
int             i;
int             n;
INT           * ip;
INT             x;
TCHAR         * cp;
INI_CLASS       ini;
LISTBOX_CLASS   lb;
STRING_CLASS    computer_name;
STRING_CLASS    base;
STRING_CLASS    s;
TEXT_LIST_CLASS computer_list;
BOOLEAN         have_duplicate_names;

have_duplicate_names = FALSE;
x = -1;

/*
------------------------------------------------------------------------
Make a list of the existing computers so I can check for duplicate names
------------------------------------------------------------------------ */
make_computer_name_list( computer_list );

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

n = AddComputerListbox.get_select_list( &ip );
if ( n > 0 )
    {
    for ( i=n-1; i>=0; i-- )
        {
        base = AddComputerListbox.item_text(ip[i]);

        s    = resource_string( ADDING_STRING );
        s   += base;
        temp_message( s.text(), 0 );

        computer_name = base.text() + 2;
        if ( computer_name.len() > COMPUTER_NAME_LEN )
            {
            cp = computer_name.text();
            *(cp+COMPUTER_NAME_LEN) = NullChar;
            }

        s = base;
        s += VisiTrakIniFile;
        ini.set_file( s.text() );
        if ( !ini.exists() )
            {
            kill_temp_message();
            MessageBox( 0, base.text(), resource_string(V5_DIR_NOT_FOUND_STRING), MB_OK );
            continue;
            }

        if ( !have_duplicate_names )
            have_duplicate_names = computer_list.find_exact( computer_name.text() );

        cp = ini.get_string( ConfigSection, ThisComputerKey );
        if ( cp )
            {
            s = base;
            s += TEXT( "\\V5\\DATA\\" );
            s += cp;
            s += TEXT( "\\" );

            x = lb.add( computer_listbox_string( computer_name.text(), s.text()) );
            ComputersHaveChanged = TRUE;
            }
        AddComputerListbox.remove( ip[i] );
        kill_temp_message();

        /*
        -------------------------------------------
        Keep a list of messages to send to eventman
        ------------------------------------------- */
        computer_name += CommaString;
        computer_name += s;

        AddedComputers.append(computer_name.text() );

        /*
        ------------------------------------------
        Select the last computer that I have added
        ------------------------------------------ */
        if ( x >= 0 )
            lb.setcursel( x );
        }

    delete[] ip;

    if ( have_duplicate_names )
        resource_message_box( MainInstance, RENAME_BEFORE_SAVING_STRING, SAME_NAME_STRING );
    }
}

/***********************************************************************
*                          ADD_COMPUTER_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK add_computer_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
LISTBOX_CLASS  lb;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        position_computer_dialog( w );
        AddComputerListbox.init(w, ADD_COMPUTER_LB );
        fill_add_computer_listbox( 0 );
        if ( AddComputerListbox.count() < 1 )
            {
            resource_message_box( MainInstance, NO_OTHER_COMPUTERS_STRING, NO_CREATE_COMPUTER_STRING );
            EndDialog( w, 0 );
            SetFocus( BoardsSetupWindow );
            }
        return TRUE;

    case WM_DESTROY:
        AddComputerListbox.init( 0 );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                add_selected_network_computers();
                return TRUE;

            case IDCANCEL:
                EndDialog( w, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                            ADD_NEW_BOARD                             *
***********************************************************************/
static void add_new_board( void )
{
INT            i;
INT            n;
LISTBOX_CLASS  lb;
BOARD_ENTRY    b;
BOARD_ENTRY    pb;

b.number  = MIN_FT_NUMBER;
b.address = MIN_FT_ADDRESS;
b.type    = BoardType[0];
b.mhz     = DEF_FT_MHZ;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();
if ( n >= MAX_BOARDS )
    {
    resource_message_box( MainInstance, ONLY_EIGHT_BOARDS_STRING, NO_CREATE_BOARD_STRING );
    return;
    }

if ( n > 0 )
    {
    i = lb.current_index();
    b = *board_from_listboxline( lb.selected_text() );

    /*
    ------------------------------------------------------
    See if there is a missing board before the current one
    ------------------------------------------------------ */
    if ( i > 0 )
        pb = *board_from_listboxline( lb.item_text(i-1) );
    else
        pb.number = NO_BOARD_NUMBER;

    if ( pb.number < (b.number - 1) )
        {
        b.number--;
        if ( b.address > MIN_FT_ADDRESS )
            b.address -= FT_ADDRESS_INCREMENT;
        }
    else
        {
        b = *board_from_listboxline( lb.item_text(n-1) );
        if ( b.number < MAX_MUX_CHANNELS )
            b.number++;

        if ( b.address < MAX_FT_ADDRESS )
            b.address += FT_ADDRESS_INCREMENT;
        }

    }

i = lb.add( board_listbox_string(b) );
if ( i != NO_INDEX )
    {
    lb.setcursel( i );
    do_board_select();

    SetFocus( GetDlgItem(BoardsSetupWindow, BOARD_RADIO_1) );

    BoardsHaveChanged = TRUE;
    }

}

/***********************************************************************
*                       DELETE_CURRENT_BOARD                           *
***********************************************************************/
void delete_current_board( void )
{
INT            x;
LISTBOX_CLASS  lb;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

if ( lb.count() > 1 )
    {
    x = lb.current_index();
    lb.remove( x );
    x = lb.current_index();
    lb.setcursel( x );
    do_board_select();
    BoardsHaveChanged = TRUE;
    }

}

/***********************************************************************
*                     DELETE_COMPUTER_ENTRY                            *
***********************************************************************/
void delete_computer_entry( void )
{
INT            x;
LISTBOX_CLASS  lb;

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

if ( lb.count() > 1 )
    {
    x = lb.current_index();
    lb.remove( x );
    x = lb.current_index();
    lb.setcursel( x );
    ComputersHaveChanged = TRUE;
    }
}

/***********************************************************************
*                      FILL_LISTBOX_WITH_MACHINES                      *
***********************************************************************/
static void fill_listbox_with_machines( HWND w, INT listbox_id, TCHAR * computer_name )
{
LISTBOX_CLASS  lb;
DB_TABLE       t;
TCHAR          machine_name[MACHINE_NAME_LEN+1];

lb.init( w, listbox_id );
lb.empty();
lb.redraw_off();

t.open( machset_dbname(computer_name), MACHSET_RECLEN, PFL );
while ( t.get_next_record(FALSE) )
    {
    t.get_alpha( machine_name, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
    lb.add( machine_name );
    }
t.close();

lb.redraw_on();
}

/***********************************************************************
*                       DELETE_CURRENT_COMPUTER                        *
***********************************************************************/
static void delete_current_computer( void )
{
LISTBOX_CLASS  lb;
TCHAR        * name;
TCHAR        * directory;

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );

if ( compare(ThisComputer, lb.selected_text(), COMPUTER_NAME_LEN) == 0 )
    {
    resource_message_box( MainInstance, NO_DELETE_THIS_COMPUTER_STRING, UNABLE_TO_COMPLY_STRING );
    return;
    }

if ( computer_name_and_dir( &name, &directory, lb.selected_text()) )
    delete_computer_entry();
}

/***********************************************************************
*                          BOARDS_SETUP_PROC                           *
***********************************************************************/
BOOL CALLBACK boards_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BoardsSetupWindow = hWnd;
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        init_updown_control();
        set_tab_stops();
        SendMessage( GetDlgItem(hWnd, COMPUTER_NAME_EDITBOX), EM_LIMITTEXT, COMPUTER_NAME_LEN, 0L );
        fill_computers();
        fill_boards();
        fill_machines();
        SetFocus( GetDlgItem(hWnd, COMPUTER_LISTBOX) );
        return TRUE;

    case WM_VSCROLL:
        w = (HWND) lParam;
        if ( w == GetDlgItem(BoardsSetupWindow, BOARD_ADDRESS_UPDOWN) )
            {
            update_updown_editbox();
            return TRUE;
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_boards();
                save_machines();
                return TRUE;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return 0;

            case MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_machine_select();
                    return TRUE;
                    }
                break;

            case BOARD_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_board_select();
                    return TRUE;
                    }
                break;

            case DELETE_BOARD_BUTTON:
                delete_current_board();
                return TRUE;

            case DELETE_COMPUTER_BUTTON:
                delete_current_computer();
                return TRUE;

            case NEW_BOARD_BUTTON:
                add_new_board();
                return TRUE;

            case NEW_COMPUTER_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("ADD_COMPUTER_DIALOG"),
                    BoardsSetupWindow,
                    (DLGPROC) add_computer_dialog_proc
                    );
                return TRUE;

            case EDIT_COMPUTER_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_COMPUTER_DIALOG"),
                    BoardsSetupWindow,
                    (DLGPROC) edit_computer_dialog_proc
                    );
                return TRUE;

            case BOARD_RADIO_1:
            case BOARD_RADIO_2:
            case BOARD_RADIO_3:
            case BOARD_RADIO_4:
            case BOARD_RADIO_5:
            case BOARD_RADIO_6:
            case BOARD_RADIO_7:
            case BOARD_RADIO_8:
            case SINGLE_BOARD_TYPE_BUTTON:
            case MUX_BOARD_TYPE_BUTTON:
            case FTII_BOARD_TYPE_BUTTON:
            case MHZ_4_BUTTON:
            case MHZ_12_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    refresh_board_listbox_entry();
                    return TRUE;
                    }
                break;

            case BOARD_ADDRESS_EDITBOX:
            case TCP_ADDR_EBOX:
            case TCP_PORT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    refresh_board_listbox_entry();
                    return TRUE;
                    }
                break;

            case MACHINE_BOARD_RADIO_1:
            case MACHINE_BOARD_RADIO_2:
            case MACHINE_BOARD_RADIO_3:
            case MACHINE_BOARD_RADIO_4:
            case MACHINE_BOARD_RADIO_5:
            case MACHINE_BOARD_RADIO_6:
            case MACHINE_BOARD_RADIO_7:
            case MACHINE_BOARD_RADIO_8:

            case MACHINE_MUX_RADIO_1:
            case MACHINE_MUX_RADIO_2:
            case MACHINE_MUX_RADIO_3:
            case MACHINE_MUX_RADIO_4:
            case MACHINE_MUX_RADIO_5:
            case MACHINE_MUX_RADIO_6:
            case MACHINE_MUX_RADIO_7:
            case MACHINE_MUX_RADIO_8:
                if ( cmd == BN_CLICKED )
                    {
                    refresh_machine_listbox_entry();
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );
TCHAR * cp;
static TCHAR full_dialog_name[]        = TEXT( "BOARDS_SETUP_DIALOG" );
static TCHAR no_computer_dialog_name[] = TEXT( "NO_COMPUTER_BOARDS_SETUP_DIALOG" );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );
    }

if ( !BoardsSetupWindow )
    {

    if ( CurrentPasswordLevel >= COMPUTER_LIST_PASSWORD_LEVEL )
        {
        HaveComputerBoxes = TRUE;
        cp = full_dialog_name;
        }
    else
        {
        HaveComputerBoxes = FALSE;
        cp = no_computer_dialog_name;
        }

    BoardsSetupWindow  = CreateDialog(
        MainInstance,
        cp,
        MainWindow,
        (DLGPROC) boards_setup_proc );

    }

}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch ( msg )
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX, password_level_callback );
        get_current_password_level();
        return 0;

    case WM_SETFOCUS:
        if ( BoardsSetupWindow )
            SetFocus( GetDlgItem(BoardsSetupWindow, COMPUTER_LISTBOX) );
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, FASTRAK_BOARD_SETUP_HELP );
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

BoardsSetupWindow    = 0;

names_startup();
c.startup();

lstrcpy( ThisComputer, c.whoami() );
get_actual_computer_name();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(V5SETUPS_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    225, 225,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
COMPUTER_CLASS c;

unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX );

client_dde_shutdown();
c.shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

InitCommonControls();

MainInstance = this_instance;
MyWindowTitle = resource_string( MAIN_WINDOW_TITLE );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
   {
   status = FALSE;

   if (BoardsSetupWindow )
      status = IsDialogMessage( BoardsSetupWindow, &msg );

   if ( !status )
      {
      TranslateMessage( &msg );
      DispatchMessage(  &msg );
      }
   }

shutdown();
return msg.wParam;
}
