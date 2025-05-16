#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"

#include "resource.h"
#include "extern.h"

#define SINGLE_SHOT_LISTBOX   101
#define MULTIPLE_SHOT_LISTBOX 102
#define OK_BUTTON         1
#define ZABISH_SCREEN_TYPE    2

INT ShotListboxId = SINGLE_SHOT_LISTBOX;

static long LastX = 0;
static long LastY = 0;

PART_NAME_ENTRY MyPart;
TCHAR MyShotName[SHOT_LEN+1];

static HWND   MyWindow;

static short NSelected;
const static TCHAR NullChar  = TEXT( '\0' );
const static TCHAR SpaceChar = TEXT( ' ' );
const static TCHAR ZeroChar  = TEXT( '0' );

extern short MyScreenType;

void toggle_realtime( void );
void fill_machines( HWND w, UINT listbox_id );
RECT & my_desktop_client_rect();
bool overlaying( void );

extern void add_to_backup_list();   /* Profile.cpp */

/***********************************************************************
*                         CURRENT_MACHINE_NAME                         *
***********************************************************************/
static TCHAR * current_machine_name( void )
{
static TCHAR machine_name[MACHINE_NAME_LEN+1];
TCHAR       * cp;
LISTBOX_CLASS lb;

lb.init( MyWindow, CS_MACHINE_LISTBOX );

cp = lb.selected_text();
if ( lstrlen(cp) <= 0 )
    cp = MachineName;

lstrcpyn( machine_name, cp, MACHINE_NAME_LEN+1);

return machine_name;
}

/***********************************************************************
*                         CURRENT_PART_NAME                            *
***********************************************************************/
TCHAR * current_part_name( void )
{
static TCHAR part_name[PART_NAME_LEN+1];
TCHAR       * cp;
LISTBOX_CLASS lb;

lb.init( MyWindow, CS_PART_LISTBOX );
cp = lb.selected_text();
if ( lstrlen(cp) == 0 )
    cp = PartName;

lstrcpyn( part_name, cp, PART_NAME_LEN+1 );
replace_char_with_null( part_name, SpaceChar );

return part_name;
}

/***********************************************************************
*                            SET_LISTBOX_ID                            *
***********************************************************************/
void set_listbox_id( void )
{
INT  hidden_box_id;
HWND w;

if ( ShotListboxId == SINGLE_SHOT_LISTBOX )
    hidden_box_id = MULTIPLE_SHOT_LISTBOX;
else
    hidden_box_id = SINGLE_SHOT_LISTBOX;

w = GetDlgItem( MyWindow, hidden_box_id );
SendMessage( w, LB_RESETCONTENT, TRUE, 0L );
ShowWindow( w, SW_HIDE );

w = GetDlgItem( MyWindow, ShotListboxId );
ShowWindow( w, SW_SHOW );
}

/***********************************************************************
*                          SET_LISTBOX_STYLE                           *
***********************************************************************/
void set_listbox_style( void )
{
if ( overlaying() && ShotListboxId == SINGLE_SHOT_LISTBOX )
    ShotListboxId = MULTIPLE_SHOT_LISTBOX;

else if ( !overlaying()  && ShotListboxId == MULTIPLE_SHOT_LISTBOX )
    ShotListboxId = SINGLE_SHOT_LISTBOX;

else
    return;

set_listbox_id();
}

/***********************************************************************
*                              FILL_SHOTS                              *
***********************************************************************/
static void fill_shots( void )
{
SYSTEMTIME   st;
DB_TABLE     t;
TCHAR      * cp;
TCHAR        s[SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1];
long         shot_number;
HWND         w;
NAME_CLASS   fn;
LRESULT      x;
short        i;
short        nof_selected_shots;
long         selected_shot_number[MAX_OVERLAY_SHOTS];

MyPart.set( current_part_name() );

/*
----------------------------------------------
Don't do anything if there is no graphlst file
---------------------------------------------- */
fn = graphlst_dbname( MyPart.computer, MyPart.machine, MyPart.part );
if ( !fn.file_exists() )
    return;

hourglass_cursor();

nof_selected_shots = 0;
if ( overlaying() )
    {
    for ( i=0; i<NofOverlayShots; i++ )
        selected_shot_number[i] = asctoint32( OverlayShot[i].shot );
    nof_selected_shots = NofOverlayShots;
    }
else if ( HaveHeader )
    {
    selected_shot_number[0] = asctoint32( ShotName );
    nof_selected_shots = 1;
    }

set_listbox_style();

w = GetDlgItem( MyWindow, ShotListboxId );

SendMessage( w, LB_RESETCONTENT, TRUE, 0L );
SendMessage( w, WM_SETREDRAW, 0,  0l );
NSelected = 0;

t.open( fn.text(), GRAPHLST_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    cp = s;

    shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
    insalph( cp, shot_number, SHOT_LEN, ZeroChar, DECIMAL_RADIX );
    cp += SHOT_LEN;
    *cp = SpaceChar;
    cp++;

    t.get_date( st, GRAPHLST_DATE_OFFSET );
    copychars( cp, alphadate(st), ALPHADATE_LEN );
    cp += ALPHADATE_LEN;
    *cp = SpaceChar;
    cp++;

    t.get_time( st, GRAPHLST_TIME_OFFSET );
    copychars( cp, alphatime(st), ALPHATIME_LEN );
    cp += ALPHATIME_LEN;
    *cp = NullChar;
    x = SendMessage( w, LB_ADDSTRING, 0, (LPARAM)(LPSTR) s );
    if ( x != LB_ERR && NSelected < nof_selected_shots )
        {
        for ( i=0; i<nof_selected_shots; i++ )
            {
            if ( shot_number == selected_shot_number[i] )
                {
                NSelected++;
                if ( ShotListboxId == SINGLE_SHOT_LISTBOX )
                    SendMessage( w, LB_SETCURSEL, x, 0L );
                else
                    SendMessage( w, LB_SETSEL, TRUE, x );
                break;
                }
            }
        }
    }
t.close();

SendDlgItemMessage( MyWindow, ShotListboxId, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(MyWindow, ShotListboxId), NULL, TRUE );

restore_cursor();
}

/***********************************************************************
*                              FILL_PARTS                              *
*   You should set MyPart before calling this with with_shots == TRUE  *
***********************************************************************/
static void fill_parts( BOOLEAN with_shots )
{
LISTBOX_CLASS lb;

lb.init( MyWindow, CS_PART_LISTBOX );
lb.empty();

if ( !with_shots )
    {
    if ( MachList.find(current_machine_name()) )
        MyPart.set( MachList.computer_name(), MachList.name(), 0 );
    else
        return;
    }

fill_parts_and_shot_count_listbox( MyWindow, CS_PART_LISTBOX, MyPart.computer, MyPart.machine );

if ( with_shots )
    {
    lb.findprefix( MyPart.part );
    fill_shots();
    }
else
    {
    lb.setcursel( -1 );
    lb.init( MyWindow, ShotListboxId );
    lb.empty();
    }
}

/***********************************************************************
*                      COPY_SELECTIONS_TO_EXTERN                       *
***********************************************************************/
void copy_selections_to_extern()
{
HWND       w;
LRESULT    n;
LRESULT    x;
INT        i;
TCHAR      s[SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1];

w = GetDlgItem( MyWindow, ShotListboxId );

x = SendMessage( w, LB_GETCARETINDEX, 0, 0L );
if ( x != LB_ERR )
    {
    i = (INT) x;
    if ( ShotListboxId == MULTIPLE_SHOT_LISTBOX )
        {
        n = SendMessage( w, LB_GETSELCOUNT, 0, 0L );
        if ( n == LB_ERR )
            return;

        x = SendMessage( w, LB_GETSEL,      i, 0L );
        if ( x != LB_ERR )
            {
            if ( x == 0 )
                {
                /*
                -------------------------------------------------
                This entry has just been unselected, if it is the
                last entry, reselect it
                ------------------------------------------------- */
                if ( n == 0 )
                    {
                    SendMessage( w, LB_SETSEL, TRUE, i );
                    NSelected = 1;
                    return;
                    }
                }

            else if ( n > MAX_OVERLAY_SHOTS )
                {
                /*
                ------------------------------------------------------
                This entry has just been selected but there is no room
                ------------------------------------------------------ */
                SendMessage( w, LB_SETSEL, FALSE, i );
                NSelected--;
                resource_message_box( MyWindow, MainInstance, DEL_SOME_FIRST_STRING, MAX_OVERLAY_REACHED_STRING, MB_OK );
                return;
                }
            }
        }

    x = SendMessage( w, LB_GETTEXT, i, (LPARAM)(LPSTR) s );
    if ( x != LB_ERR )
        {
        lstrcpy( ComputerName, MyPart.computer );
        lstrcpy( MachineName,  MyPart.machine  );
        lstrcpy( PartName,     MyPart.part     );
        lstrcpyn( ShotName, s, SHOT_LEN+1 );
        fix_shotname( ShotName );
        }
    }

SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
if ( RealTime )
    toggle_realtime();
}

/***********************************************************************
*                          CHOOSE_THESE_SHOTS                          *
***********************************************************************/
static void choose_these_shots()
{
LRESULT n;

/*
------------------------------------------------------------
If this is a multiple selection listbox, see if this is just
an arrow key.
------------------------------------------------------------ */
if ( ShotListboxId == MULTIPLE_SHOT_LISTBOX )
    {
    n = SendDlgItemMessage( MyWindow, ShotListboxId, LB_GETSELCOUNT, 0, 0L );

    if ( n == LB_ERR )
        return;

    if ( n == NSelected )
        return;
    else
        NSelected = short( n );
    }

copy_selections_to_extern();
}

/***********************************************************************
*                            SAVE_POSITION                             *
***********************************************************************/
static void save_position()
{
RECT r;

if ( MyScreenType != ZABISH_SCREEN_TYPE )
    {
    GetWindowRect( MyWindow, &r );
    LastX = r.left;
    LastY = r.top;
    }
}

/***********************************************************************
*                           POSITION_DIALOG                            *
***********************************************************************/
static void position_dialog()
{
RECT r;

GetWindowRect( MyWindow, &r );

r.right  += LastX - r.left;
r.bottom += LastY - r.top;
r.left   = LastX;
r.top    = LastY;

MoveWindow( MyWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                            CHOSSHOT_PROC                             *
***********************************************************************/
BOOL CALLBACK chosshot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int tabs[2] = { 45, 78 };
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
    case WM_NEWPART:
        MyPart.set( ComputerName, MachineName, PartName );
        fill_machines( hWnd, CS_MACHINE_LISTBOX );
        fill_parts( TRUE );

    case WM_OVERLAY_CHANGE:
        fill_shots();
        return TRUE;

    case WM_SETFOCUS:
        SetFocus( GetDlgItem(MyWindow, ShotListboxId) );
        return TRUE;

    case WM_SHOWWINDOW:
        /*
        ---------------------------------------------------------------------
        wParam is true if I am being awakened, lParam is 0 if from showwindow
        --------------------------------------------------------------------- */
        if ( (BOOL) wParam &&  int(lParam) == 0 )
            {
            MyPart.set( ComputerName, MachineName, PartName );
            fill_machines( hWnd, CS_MACHINE_LISTBOX );
            fill_parts( TRUE );
            fill_shots();
            }
        return TRUE;

    case WM_INITDIALOG:
        MyWindow = hWnd;
        position_dialog();
        if ( PRIMARYLANGID(GetUserDefaultLangID()) == LANG_CHINESE )
            tabs[0] = 60;
        SendDlgItemMessage( hWnd, CS_PART_LISTBOX, LB_SETTABSTOPS, (WPARAM) 2, (LPARAM) tabs );
        set_listbox_id();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_CLOSE:
        save_position();
        ChosshotWindow = 0;
        MyWindow       = 0;
        DestroyWindow( hWnd );
        return TRUE;

    case WM_VKEYTOITEM:
        if ( LOWORD(wParam) == VK_F9 )
            add_to_backup_list();
        return -1;       /* Default action by listbox */

    case WM_COMMAND:
        switch ( id )
            {
            case CS_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts( FALSE );
                return TRUE;

            case CS_PART_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_shots();
                return TRUE;

            case SINGLE_SHOT_LISTBOX:
            case MULTIPLE_SHOT_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    choose_these_shots();
                return TRUE;

            case IDCANCEL:
            case OK_BUTTON:
                SendMessage( MyWindow,   WM_CLOSE,   0, 0L );
                return TRUE;
            }
    }

return FALSE;
}
