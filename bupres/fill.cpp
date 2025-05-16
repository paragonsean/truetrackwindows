#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "save.h"

static HWND CurrentWindow;
static int  MachineListBox;
static int  PartListBox;

/***********************************************************************
*                             FILL_STARTUP                             *
***********************************************************************/
void fill_startup( HWND w, int machine_listbox_id, int part_listbox_id )
{
CurrentWindow  = w;
MachineListBox = machine_listbox_id;
PartListBox    = part_listbox_id;
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
void fill_parts( MACHINE_NAME_LIST_CLASS & m, BITSETYPE backup_type )
{
TCHAR machine[MACHINE_NAME_LEN+1];
short n;

n = get_lb_select_count( CurrentWindow, MachineListBox );
if ( n != 1 )
    {
    SendDlgItemMessage( CurrentWindow, PartListBox, LB_RESETCONTENT, TRUE, 0L );
    }
else if ( get_lb_select(CurrentWindow, MachineListBox, machine) )
    {
    if ( m.find(machine) )
        {
        if ( backup_type & (DO_BACKUP | DO_RESTORE) )
            fill_parts_and_counts_listbox( CurrentWindow, PartListBox, m.computer_name(), machine );
        else
            fill_part_listbox( CurrentWindow, PartListBox, m.computer_name(), machine );
        lb_select_all( CurrentWindow, PartListBox );
        }
    }
}

/***********************************************************************
*                         FILL_MACHINE_LISTBOX                         *
***********************************************************************/
void fill_machine_listbox( HWND w, int id, MACHINE_NAME_LIST_CLASS & m )
{

UINT resetcontent;
UINT addstring;
UINT setcursel;

w = GetDlgItem( w, id );

if ( is_a_listbox(w) )
    {
    resetcontent = LB_RESETCONTENT;
    addstring    = LB_ADDSTRING;
    setcursel    = LB_SETCURSEL;
    }
else
    {
    resetcontent = CB_RESETCONTENT;
    addstring    = CB_ADDSTRING;
    setcursel    = CB_SETCURSEL;
    }


SendMessage( w, resetcontent, TRUE, 0L );
SendMessage( w, WM_SETREDRAW, 0,  0l );

m.rewind();
while ( m.next() )
    SendMessage( w, addstring, 0,(LPARAM) m.name() );

SendMessage( w, setcursel, 0, 0L);
SendMessage( w, WM_SETREDRAW, 1,  0l );
InvalidateRect( w, NULL, TRUE );
}

/***********************************************************************
*                            FILL_MACHINES                             *
***********************************************************************/
void fill_machines( MACHINE_NAME_LIST_CLASS & m )
{
fill_machine_listbox( CurrentWindow, MachineListBox, m );
lb_select_all( CurrentWindow, MachineListBox );
}

