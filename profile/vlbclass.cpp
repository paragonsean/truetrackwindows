#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"

#include "resource.h"
#include "vlbclass.h"

static const int LB_LINE_LEN   = ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN+1) + PART_NAME_LEN + 1 + OPERATOR_NUMBER_LEN+1;

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
***********************************************************************/
VIRTUAL_LISTBOX_CLASS::VIRTUAL_LISTBOX_CLASS( void )
{
nof_lines      = 0;
top_line       = 0;
current_line   = 0;
lines_per_page = 0;
lbline         = 0;
sbw            = 0;
}


/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                             FILL_LISTBOX                             *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::fill_listbox( void )
{
int i;
int lastline;

lb.redraw_off();
lb.empty();

if ( !lbline )
    return;

lastline = top_line + lines_per_page - 1;
if ( lastline >= nof_lines )
    lastline = nof_lines - 1;

i = top_line;
while ( i <= lastline )
    {
    if ( t.goto_record_number(i) )
        {
        t.get_current_record( NO_LOCK );
        lb.add( lbline(t) );
        }
    i++;
    }

if ( current_line >= top_line && current_line < (top_line+lines_per_page) )
    lb.setcursel( current_line - top_line );
lb.redraw_on();
}
 
/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                            SET_SCROLLBAR                             *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::set_scrollbar( void )
{
SCROLLINFO si;

if ( !sbw )
    return;

si.cbSize = sizeof(si);
si.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
si.nMin   = 0;
si.nMax   = nof_lines - 1;
si.nPage  = lines_per_page;
si.nPos   = top_line;

SetScrollInfo( sbw, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                            SET_SCROLLBAR_POS                         *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::set_scrollbar_pos( void )
{
SCROLLINFO    si;

if ( !sbw )
    return;

si.cbSize = sizeof(si);
si.fMask  = SIF_POS;
si.nPos   = top_line;
SetScrollInfo( sbw, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                         SCROLL_DOWN_ONE_LINE                         *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::scroll_down_one_line( void )
{
int i;
int lastline;

i        = top_line + lines_per_page - 1;
lastline = nof_lines - 1;

if ( i < lastline )
    {
    top_line++;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                          SCROLL_UP_ONE_LINE                          *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::scroll_up_one_line( void )
{

if ( top_line > 0 )
    {
    top_line--;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                          SCROLL_UP_ONE_PAGE                          *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::scroll_up_one_page( void )
{
int i;

i = top_line - lines_per_page;
maxint( i, 0 );
if ( i < top_line )
    {
    top_line = i;
    set_scrollbar_pos();
    fill_listbox();
    }
}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                        SCROLL_DOWN_ONE_PAGE                          *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::scroll_down_one_page( void )
{
int i;

i = top_line + lines_per_page;
minint( i, (nof_lines - lines_per_page) );
if ( i > top_line )
    {
    top_line = i;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                              DO_VSCROLL                              *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::do_vscroll( WPARAM wParam )
{
int scroll_code;
int pos;

scroll_code = (int) LOWORD( wParam );
pos         = (int) HIWORD( wParam );

switch ( scroll_code )
    {
    case SB_LINEDOWN:
        scroll_down_one_line();
        break;

    case SB_LINEUP:
        scroll_up_one_line();
        break;

    case SB_PAGEDOWN:
        scroll_down_one_page();
        break;

    case SB_PAGEUP:
        scroll_up_one_page();
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        top_line = pos;
        if ( scroll_code == SB_THUMBPOSITION )
            set_scrollbar_pos();
        fill_listbox();
        break;

    }

}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                              DO_KEYPRESS                             *
*                                                                      *
*               -1 tells listbox to handle the keypress.               *
*               -2 tells listbox I handled the keypress.               *
*                                                                      *
***********************************************************************/
int VIRTUAL_LISTBOX_CLASS::do_keypress( int id, int current_index )
{
int i;

switch ( id )
    {
    case VK_DOWN:
        if ( current_index == (lines_per_page - 1) )
            {
            i = top_line + current_index;
            if ( i < (nof_lines - 1) )
                {
                current_line = i + 1;
                scroll_down_one_line();
                return -2;
                }
            }
        break;

    case VK_UP:
        if ( current_index == 0 )
            {
            if ( top_line > 0 )
                {
                current_line = top_line - 1;
                scroll_up_one_line();
                return -2;
                }
            }
        break;

    case VK_PRIOR:
        if ( current_index == 0 )
            {
            if ( top_line > 0 )
                {
                current_line = top_line - lines_per_page;
                maxint( current_line, 0 );
                scroll_up_one_page();
                return -2;
                }
            }

        break;

    case VK_NEXT:
        if ( current_index == (lines_per_page - 1) )
            {
            i = top_line + current_index;
            if ( i < (nof_lines - 1) )
                {
                current_line = i + lines_per_page;
                minint( current_line, (nof_lines - 1) );
                scroll_down_one_page();
                return -2;
                }
            }
        break;

    }

return -1;
}
 
/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                              GOTO_START                              *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::goto_start( void )
{
top_line = 0;
set_scrollbar();
fill_listbox();
}
 
/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                              GOTO_END                                *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::goto_end( void )
{
top_line = nof_lines - lines_per_page;
if ( top_line < 0 )
    top_line = 0;
set_scrollbar();
fill_listbox();
}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                                DO_SELECT                             *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::do_select( void )
{
if ( t.open_status() )
    current_line = top_line + lb.selected_index();

}

/***********************************************************************
*                         VIRTUAL_LISTBOX_CLASS                        *
*                                STARTUP                               *
***********************************************************************/
BOOLEAN VIRTUAL_LISTBOX_CLASS::startup( TCHAR * fname, short reclen, HWND parent, UINT listbox_id, UINT scrollbar_id, TCHAR * (*lbline_func)( DB_TABLE & t ) )
{

sbw = GetDlgItem( parent, scrollbar_id );

lb.init( parent, listbox_id );
lines_per_page = lb.visible_row_count();

lbline = lbline_func;

if ( t.open_status() )
    t.close();

if ( t.open( fname, reclen, PFL) )
    {
    nof_lines = t.nof_recs();
    top_line = nof_lines - lines_per_page;
    maxint( top_line, 0 );
    set_scrollbar();
    current_line = NO_INDEX;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
void VIRTUAL_LISTBOX_CLASS::shutdown( void )
{
if ( t.open_status() )
    t.close();

}
