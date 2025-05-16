#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\strarray.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "windowlist.h"
#include "vtwcontrol.h"
#include "resource.h"

extern HINSTANCE MainInstance;
extern FONT_LIST_CLASS FontList;
extern WINDOW_CLASS CurrentWindow;
extern WINDOW_LIST_CLASS MoveList;
extern WINDOW_LIST_CLASS MsControlList;

static TCHAR StaticControlName[]     = TEXT( "STATIC" );
static TCHAR EditControlName[]       = TEXT( "EDIT" );
static TCHAR ButtonControlName[]     = TEXT( "BUTTON" );
static TCHAR ProgressControlName[]   = TEXT( "msctls_progress32" );
POINT MousePosition; // Used to position the popup dialog box;
POINT CurrentMousePosition;   // Used to drag a window

static TCHAR VTWRectangleClassName[] = TEXT( "VTWRectangleClass");

LRESULT CALLBACK rectangle_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ms_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void register_vtwrectangle( void );
void lparam_to_point( POINT & p, LPARAM lParam );

/***********************************************************************
*                             CHOOSE_COLOR                             *
***********************************************************************/
static bool choose_color( HWND w, COLORREF & color )
{

CHOOSECOLOR cc;
static COLORREF acrCustClr[16];   // array of custom colors

ZeroMemory( &cc, sizeof(CHOOSECOLOR) );

cc.lStructSize    = sizeof(CHOOSECOLOR);
cc.hwndOwner      = w;
cc.hInstance      = (HWND) MainInstance;
cc.lpCustColors   = (LPDWORD) acrCustClr;
cc.rgbResult      = color;
cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
cc.lpTemplateName = TEXT( "ChooseColor" );

if ( ChooseColor(&cc) )
    {
    color = cc.rgbResult;
    return true;
    }

return false;
}

/***********************************************************************
*                             CHOOSE_FONT                              *
***********************************************************************/
static bool choose_font( HWND w, HFONT & fh,  COLORREF & color )
{
HFONT myfont;
COLORREF mycolor;
bool had_font;
FONT_CLASS lf;

had_font = false;

if ( fh != INVALID_FONT_HANDLE )
    {
    had_font = FontList.get_font( lf, fh );
    myfont = fh;
    }

if ( !had_font )
    {
    myfont = fh;
    if ( myfont == INVALID_FONT_HANDLE )
        myfont = (HFONT) GetStockObject(SYSTEM_FONT);
    GetObject( (HGDIOBJ) myfont, sizeof(LOGFONT), &lf );
    }

mycolor = color;
if ( lf.choose(w, mycolor) )
    {
    myfont = FontList.get_handle( lf );
    if ( had_font )
        FontList.free_handle( fh );
    fh = myfont;
    color = mycolor;
    return true;
    }

return false;
}

/***********************************************************************
*                             CHOOSE_FONT                              *
***********************************************************************/
static bool choose_font( HWND w, HFONT & fh )
{
HFONT myfont;
bool had_font;
FONT_CLASS lf;

had_font = false;

if ( fh != INVALID_FONT_HANDLE )
    {
    had_font = FontList.get_font( lf, fh );
    myfont = fh;
    }

if ( !had_font )
    {
    myfont = fh;
    if ( myfont == INVALID_FONT_HANDLE )
        myfont = (HFONT) GetStockObject(SYSTEM_FONT);
    GetObject( (HGDIOBJ) myfont, sizeof(LOGFONT), &lf );
    }

if ( lf.choose(w) )
    {
    myfont = FontList.get_handle( lf );
    if ( had_font )
        FontList.free_handle( fh );
    fh = myfont;
    return true;
    }

return false;
}

/***********************************************************************
*                        REGISTER_VTWRECTANGLE                         *
***********************************************************************/
void register_vtwrectangle( void )
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;
firstime = false;

wc.lpszClassName = VTWRectangleClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) rectangle_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = NULL;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
}

/***********************************************************************
*                                DO_TAB                                *
***********************************************************************/
static bool do_tab( HWND w, TCHAR c, bool shift_key_is_down )
{
int low;
int high;
int i;
int j;
int dj;

MS_CONTROL_ENTRY * mce;

if ( MsControlList.find(w) )
    {
    i   = MsControlList.current_index();
    mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
    if ( mce )
        {
        if ( c != VK_TAB )
            {
            /*
            --------------------------------------
            Arrow keys only work for radio buttons
            -------------------------------------- */
            if ( !mce->is_radio_button() )
                return false;

            /*
            ---------------------------
            Get the range of this group
            --------------------------- */
            low  = i;
            high = i;
            while ( true )
                {
                if ( low < i )
                    mce = (MS_CONTROL_ENTRY *) GetWindowLong( MsControlList[low], GWL_USERDATA );
                if ( mce )
                    {
                    if ( !mce->is_radio_button() )
                        {
                        low++;
                        break;
                        }

                    if ( mce->style & WS_GROUP )
                        break;
                    }

                if ( low == 0 )
                    break;

                low--;
                }

            /*
            ------------------------------
            Check for the end of the group
            ------------------------------ */
            while ( true )
                {
                if ( high == MsControlList.last_index() )
                    break;

                high++;

                mce = (MS_CONTROL_ENTRY *) GetWindowLong( MsControlList[high], GWL_USERDATA );
                if ( mce )
                    {
                    if ( !mce->is_radio_button() || (mce->style & WS_GROUP) )
                        {
                        high--;
                        break;
                        }
                    }
                }


            if ( c==VK_UP || c==VK_LEFT )
                dj = -1;
            else
                dj = 1;

            j = i + dj;
            if ( j < low )
                j = high;
            else if ( j > high )
                j = low;

            set_checkbox( w, FALSE );
            set_checkbox( MsControlList[j], TRUE );
            SetFocus( MsControlList[j] );
            return true;
            }

        /*
        -------------------------------
        I get here if this is a tab key
        ------------------------------- */
        j = i;

        if ( shift_key_is_down )
            {
            dj = -1;
            /*
            ----------------------------------------------------------------------------------------
            Radio buttons are a special case because we need to backup to the first one of the group
            ---------------------------------------------------------------------------------------- */
            while ( j>0 && mce->is_radio_button() && !(mce->style & (DWORD) WS_TABSTOP) )
                {
                j--;
                w = MsControlList[j];
                mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
                }
            }
        else
            {
            dj = 1;
            }

        while ( true )
            {
            j += dj;

            if ( j == i )
                break;

            if ( j < 0 )
                j = MsControlList.last_index();

            if ( j > MsControlList.last_index() )
                j = 0;

            w = MsControlList[j];
            mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
            if ( mce->style & (DWORD) WS_TABSTOP )
                {
                if ( mce->is_radio_button() )
                    {
                    /*
                    --------------------------------------------------------
                    If this is a radio button, go to the one that is checked
                    -------------------------------------------------------- */
                    HWND cw = w;
                    while ( true )
                        {
                        if ( SendMessage(cw, BM_GETCHECK, 0, 0L) == BST_CHECKED )
                            {
                            w = cw;
                            break;
                            }

                        if ( j >= MsControlList.last_index() )
                            break;

                        j++;
                        cw = MsControlList[j];
                        mce = (MS_CONTROL_ENTRY *) GetWindowLong( cw, GWL_USERDATA );

                        /*
                        ---------------------------------------------------------------
                        If this is not a radio button or if it is a new group then quit
                        This will return to the first radio button in the group.
                        --------------------------------------------------------------- */
                        if ( !mce->is_radio_button() )
                            break;

                        if ( mce->style & (DWORD) WS_GROUP )
                            break;
                        }
                    /*
                    ----------------------------------
                    Make sure I have the correct class
                    ---------------------------------- */
                    mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
                    }

                SetFocus( w );
                if ( mce->is_editbox() )
                    SendMessage( w, EM_SETSEL, 0, -1 );
                return true;
                }
            }

        }
    return false;
    }

return false;
}

/***********************************************************************
*                                DO_RETURN                             *
***********************************************************************/
static bool do_return()
{
HWND w;

MS_CONTROL_ENTRY * mce;

MsControlList.rewind();
while ( (w = MsControlList.next()) )
    {
    mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
    if ( mce )
        {
        if ( mce->is_defpushbutton() )
            {
            PostMessage( w, BM_CLICK, 0, 0 );
            return true;
            }
        }
    }

return false;
}


/***********************************************************************
*                       SET_MS_SIZE_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_ms_size_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static WINDOW_CLASS parent;
static MS_CONTROL_ENTRY * mce;
static unsigned int    x;
static LONG            window_style;

int  id;
id  = LOWORD( wParam );
RECTANGLE_CLASS r;

switch (msg)
    {
    case WM_INITDIALOG:
        parent = (HWND) lParam;
        if ( parent.handle() != 0 )
            {
            GetWindowRect( hWnd, &r );
            r.moveto( MousePosition );
            MoveWindow( hWnd, r.left, r.top, r.width(), r.height(), TRUE );

            window_style = GetWindowLong( parent.handle(), GWL_STYLE );
            BOOLEAN need_check;
            if ( window_style & WS_THICKFRAME )
                need_check = TRUE;
            else
                need_check = FALSE;
            set_checkbox( hWnd, WINDOW_BORDER_RADIO, need_check );

            parent.get_move_rect( r );
            SetDlgItemInt( hWnd, RECT_X_EBOX, (UINT) r.left, TRUE );
            SetDlgItemInt( hWnd, RECT_Y_EBOX, (UINT) r.top, TRUE );
            SetDlgItemInt( hWnd, RECT_W_EBOX, (UINT) r.right-r.left, TRUE );
            SetDlgItemInt( hWnd, RECT_H_EBOX, (UINT) r.bottom-r.top, TRUE );
            mce = (MS_CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
            if ( mce )
                {
                SetDlgItemInt( hWnd, ID_EBOX, (UINT) mce->id, TRUE );

                if ( mce->name == ButtonControlName )
                    {
                    id = 0;
                    /*
                    ----------------------------------------
                    Note: BS_VCENTER == (BS_TOP | BS_BOTTOM)
                    ---------------------------------------- */
                    if ( (mce->style & BS_VCENTER) == BS_VCENTER )
                        id = VCTR_RADIO;
                    else if ( mce->style & BS_TOP )
                        id = TOP_RADIO;
                    else if ( mce->style & BS_BOTTOM )
                        id = BOTTOM_RADIO;

                    if ( id )
                        set_checkbox( hWnd, id, TRUE );

                    id = 0;

                    /*
                    ---------------------------------------
                    Note: BS_CENTER == (BS_LEFT | BS_RIGHT)
                    --------------------------------------- */
                    if ( (mce->style & BS_CENTER) == BS_CENTER  )
                        id = HCTR_RADIO;
                    else if ( mce->style & BS_LEFT )
                        id = LEFT_RADIO;
                    else if ( mce->style & BS_RIGHT )
                        id = RIGHT_RADIO;

                    if ( id )
                        set_checkbox( hWnd, id, TRUE );
                    }

                mce->s.set_text( hWnd, TEXT_EBOX );
                }
            }

        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {

            case IDOK:
                r.left   = GetDlgItemInt( hWnd, RECT_X_EBOX, 0, TRUE);
                r.top    = GetDlgItemInt( hWnd, RECT_Y_EBOX, 0, TRUE);
                r.right  = GetDlgItemInt( hWnd, RECT_W_EBOX, 0, TRUE) + r.left;
                r.bottom = GetDlgItemInt( hWnd, RECT_H_EBOX, 0, TRUE) + r.top;

                mce = (MS_CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
                if ( mce )
                    {
                    mce->r = r;

                    /*
                    -----------------------
                    Save the selected types
                    ----------------------- */
                    mce->id = GetDlgItemInt( hWnd, ID_EBOX, 0, TRUE );

                    if ( mce->name == ButtonControlName )
                        {
                        const DWORD mask = BS_LEFT | BS_RIGHT | BS_CENTER | BS_TOP | BS_BOTTOM | BS_VCENTER;
                        mce->style &= ~mask;

                        if ( is_checked(hWnd, TOP_RADIO) )
                            mce->style |= BS_TOP;
                        else if ( is_checked(hWnd, VCTR_RADIO) )
                            mce->style |= BS_VCENTER;
                        else if ( is_checked(hWnd, BOTTOM_RADIO) )
                            mce->style |= BS_BOTTOM;

                        if ( is_checked(hWnd, LEFT_RADIO) )
                                mce->style |= BS_LEFT;
                        else if ( is_checked(hWnd, HCTR_RADIO) )
                                mce->style |= BS_CENTER;
                        else if ( is_checked(hWnd, RIGHT_RADIO) )
                                mce->style |= BS_RIGHT;
                        }

                    window_style = mce->style;
                    if ( is_checked(hWnd, WINDOW_BORDER_RADIO) )
                        window_style |= (LONG) WS_THICKFRAME;
                    else
                        window_style &= (LONG) ~WS_THICKFRAME;
                    SetWindowLong( parent.handle(), GWL_STYLE, window_style );

                    if ( SendDlgItemMessage(hWnd, TEXT_EBOX, EM_GETMODIFY, 0, 0) )
                        {
                        mce->s.get_text(hWnd, TEXT_EBOX);
                        mce->s.set_text( parent.handle() );
                        }

                    if ( mce->font_handle != INVALID_FONT_HANDLE )
                        parent.send( WM_SETFONT, (WPARAM) mce->font_handle,  0 );
                    }

                /*
                -------------------------------------------------------------------------------------------------
                This may only be necessary when sending the groupboxes WM_NCHITTEST messages to the defwindowproc
                ------------------------------------------------------------------------------------------------- */
                if ( mce && (mce->is_groupbox || mce->is_static) )
                    SetWindowPos( parent.handle(), HWND_BOTTOM, r.left, r.top, r.width(), r.height(), SWP_FRAMECHANGED | SWP_NOACTIVATE );
                else
                    SetWindowPos( parent.handle(), 0, r.left, r.top, r.width(), r.height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE );

                parent.refresh();

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;

            case CHOOSE_FONT_BUTTON:
                mce = (MS_CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
                if ( mce )
                    choose_font( hWnd, mce->font_handle );
                return TRUE;
            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                          MS_CONTROL_PROC                             *
***********************************************************************/
LRESULT CALLBACK ms_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static MS_CONTROL_ENTRY * mce;
static bool moving = false;
static bool shift_key_is_down = false;
static TCHAR        c;
static WINDOW_CLASS hadfocus;
static WINDOW_CLASS w;
static RECTANGLE_CLASS r;

mce = (MS_CONTROL_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );
if ( mce )
    {
    switch ( msg )
        {
        case WM_DESTROY:
            delete mce;
            mce = 0;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG) mce );
            return 0;

        case WM_SETFOCUS:
            if ( mce->is_defpushbutton() )
                {
                if ( hWnd != (HWND) wParam )
                    hadfocus = (HWND) wParam;
                return 0;
                }
            break;

        case WM_NCHITTEST:
            if ( mce->is_groupbox || mce->is_static )
                return DefWindowProc( hWnd, msg, wParam, lParam );
            break;

        case WM_LBUTTONDOWN:
            if ( wParam & MK_SHIFT )
                {
                CurrentWindow = hWnd;
                lparam_to_point( CurrentMousePosition, lParam );
                if ( !moving )
                    SetCapture( hWnd );
                moving = true;
                if ( GetFocus() == hWnd )
                    {
                    w = GetParent( hWnd );
                    w.set_focus();
                    }
                return 0;
                }
            break;

        case WM_MOUSEMOVE:
            if ( moving )
                {
                POINT p;
                lparam_to_point( p, lParam );
                if ( p.x != CurrentMousePosition.x || p.y != CurrentMousePosition.y )
                    {
                    p.x -= CurrentMousePosition.x;
                    p.y -= CurrentMousePosition.y;
                    w = hWnd;
                    w.offset( p );
                    mce->r.offset( p );
                    MoveList.rewind();
                    while ( MoveList.next() )
                        {
                        w = MoveList.handle();
                        if ( w.handle() != hWnd )
                            w.offset(p);
                        }
                    }
                return 0;
                }
            break;

        case WM_MOVE:
            w = hWnd;
            w.get_move_rect( mce->r );
            return 0;

        case WM_LBUTTONUP:
            if ( moving )
                {
                ReleaseCapture();
                moving = false;
                return 0;
                }

            if ( hadfocus.handle() )
                {
                hadfocus.set_focus();
                hadfocus = 0;
                }
            break;

        case WM_KEYDOWN:
            c = (TCHAR) wParam;
            if ( c == VK_SHIFT )
                shift_key_is_down = true;
            else if ( c == VK_TAB || c == VK_RETURN )
                {
                if ( mce->is_editbox() )
                    return 0;
                }

            break;

        case WM_CHAR:
            /*
            ------------------------------------------------
            Edit controls just beep when you send them a tab
            ------------------------------------------------ */
            c = (TCHAR) wParam;
            if ( (c == VK_TAB || c == VK_RETURN) && mce->is_editbox() )
                return 0;
            break;

        case WM_KEYUP:
            c = (TCHAR) wParam;
            if ( c == VK_SHIFT )
                {
                shift_key_is_down = false;
                }
            else if ( c == VK_TAB || c == VK_UP || c == VK_DOWN || c == VK_LEFT || c == VK_RIGHT )
                {
                if ( do_tab(hWnd, c, shift_key_is_down) )
                    return 0;
                }
            else if ( c == VK_RETURN )
                {
                do_return();
                }
            break;

        case WM_RBUTTONUP:
            /*
            ----------------------------------
            Tell the dialog where the mouse is
            ---------------------------------- */
            lparam_to_point( MousePosition, lParam );
            GetWindowRect( hWnd, &r );
            MousePosition.x += r.left;
            MousePosition.y += r.top;
            DialogBoxParam(
                MainInstance,
                TEXT("SET_MS_SIZE_DIALOG"),
                hWnd,
                (DLGPROC) set_ms_size_dialog_proc,
                (LPARAM) hWnd
                );
            return 0;
        }

    return CallWindowProc( mce->winproc, hWnd, msg, wParam, lParam );
    }

return 0;
}

/***********************************************************************
*                       END_SET_RECT_SIZE_DIALOG                       *
***********************************************************************/
static void end_set_rect_size_dialog( HWND w )
{
EndDialog( w, 0 );
}

/***********************************************************************
*                      SET_RECT_SIZE_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK set_rect_size_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static WINDOW_CLASS parent;
static CONTROL_ENTRY * cep;
static unsigned int    x;
static LONG            window_style;
static int             count = 0;

int  id;
id  = LOWORD( wParam );
RECTANGLE_CLASS r;
COLORREF mycolor;

switch (msg)
    {
    case WM_INITDIALOG:
        parent = (HWND) lParam;
        if ( parent.handle() != 0 )
            {
            GetWindowRect( hWnd, &r );
            r.moveto( MousePosition );
            MoveWindow( hWnd, r.left, r.top, r.width(), r.height(), TRUE );

            window_style = GetWindowLong( parent.handle(), GWL_STYLE );
            BOOLEAN need_check;
            if ( window_style & WS_THICKFRAME )
                need_check = TRUE;
            else
                need_check = FALSE;
            set_checkbox( hWnd, WINDOW_BORDER_RADIO, need_check );

            parent.get_move_rect( r );
            SetDlgItemInt( hWnd, RECT_X_EBOX, (UINT) r.left, TRUE );
            SetDlgItemInt( hWnd, RECT_Y_EBOX, (UINT) r.top, TRUE );
            SetDlgItemInt( hWnd, RECT_W_EBOX, (UINT) r.right-r.left, TRUE );
            SetDlgItemInt( hWnd, RECT_H_EBOX, (UINT) r.bottom-r.top, TRUE );
            cep = (CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
            if ( cep )
                {
                SetDlgItemInt( hWnd, ID_EBOX, (UINT) cep->id, TRUE );

                if ( cep->type & FILLED_RECTANGLE_TYPE )
                    set_checkbox( hWnd, FILLED_RADIO, TRUE );

                if ( cep->type & BORDER_RECTANGLE_TYPE )
                    set_checkbox( hWnd, BORDER_RADIO, TRUE );

                if ( cep->type & TEXT_TYPE )
                    set_checkbox( hWnd, TEXT_RADIO, TRUE );

                if ( cep->type & BAR_TYPE )
                    set_checkbox( hWnd, BAR_TYPE_XBOX, TRUE );

                if ( cep->type & CENTER_VERTICALLY )
                    set_checkbox( hWnd, VERTICAL_CTR_XBOX, TRUE );

                if ( cep->type & RIGHT_JUSTIFY )
                    set_checkbox( hWnd, RIGHT_JUSTIFY_XBOX, TRUE );

                if ( cep->type & CENTER_HORIZONTALLY )
                    set_checkbox( hWnd, HORIZONTAL_CTR_XBOX, TRUE );

                if ( cep->type & BACKGROUND_TYPE )
                    set_checkbox( hWnd, ISBACKGROUND_XBOX, TRUE );

                cep->s.set_text( hWnd, TEXT_EBOX );
                }
            }

        count++;
        return TRUE;

    case WM_CLOSE:
        end_set_rect_size_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {

            case IDOK:

                window_style = GetWindowLong( parent.handle(), GWL_STYLE );
                if ( is_checked(hWnd, WINDOW_BORDER_RADIO) )
                    window_style |= (LONG) WS_THICKFRAME;
                else
                    window_style &= (LONG) ~WS_THICKFRAME;
                SetWindowLong( parent.handle(), GWL_STYLE, window_style );

                r.left   = GetDlgItemInt( hWnd, RECT_X_EBOX, 0, TRUE);
                r.top    = GetDlgItemInt( hWnd, RECT_Y_EBOX, 0, TRUE);
                r.right  = GetDlgItemInt( hWnd, RECT_W_EBOX, 0, TRUE) + r.left;
                r.bottom = GetDlgItemInt( hWnd, RECT_H_EBOX, 0, TRUE) + r.top;

                x = 0;

                if ( is_checked(hWnd, FILLED_RADIO) )
                    x |= FILLED_RECTANGLE_TYPE;

                if ( is_checked(hWnd, BORDER_RADIO) )
                    x |= BORDER_RECTANGLE_TYPE;

                if ( is_checked(hWnd, TEXT_RADIO) )
                    x |= TEXT_TYPE;

                cep = (CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
                if ( cep )
                    {
                    /*
                    -----------------------
                    Save the selected types
                    ----------------------- */
                    cep->id = GetDlgItemInt( hWnd, ID_EBOX, 0, TRUE );

                    cep->type &= ~CONTROL_TYPE_MASK;
                    cep->type |= x;

                    if ( is_checked(hWnd, VERTICAL_CTR_XBOX) )
                        cep->type |= CENTER_VERTICALLY;
                    else
                        cep->type &= ~CENTER_VERTICALLY;

                    if ( is_checked(hWnd, RIGHT_JUSTIFY_XBOX) )
                        cep->type |= RIGHT_JUSTIFY;
                    else
                        cep->type &= ~RIGHT_JUSTIFY;

                    if ( is_checked(hWnd, HORIZONTAL_CTR_XBOX) )
                        cep->type |= CENTER_HORIZONTALLY;
                    else
                        cep->type &= ~CENTER_HORIZONTALLY;

                    if ( SendDlgItemMessage(hWnd, TEXT_EBOX, EM_GETMODIFY, 0, 0) )
                        cep->s.get_text(hWnd, TEXT_EBOX);

                    if ( is_checked(hWnd, BAR_TYPE_XBOX) )
                        cep->type |= BAR_TYPE;

                    if ( is_checked(hWnd, ISBACKGROUND_XBOX) )
                        cep->type |= BACKGROUND_TYPE;
                    else
                        cep->type &= ~BACKGROUND_TYPE;
                    }

                if ( cep && cep->type & BACKGROUND_TYPE )
                    SetWindowPos( parent.handle(), HWND_BOTTOM, r.left, r.top, r.width(), r.height(), SWP_FRAMECHANGED | SWP_NOACTIVATE );
                else
                    SetWindowPos( parent.handle(), 0, r.left, r.top, r.width(), r.height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE );

                parent.refresh();

            case IDCANCEL:
                end_set_rect_size_dialog( hWnd );
                return TRUE;

            case CHOOSE_COLOR_BUTTON:
                cep = (CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
                if ( cep )
                    {
                    mycolor = cep->color;
                    if ( choose_color(hWnd, mycolor) )
                        cep->color = mycolor;
                    }
                return TRUE;

            case CHOOSE_FONT_BUTTON:

                cep = (CONTROL_ENTRY *) GetWindowLong( parent.handle(), GWL_USERDATA );
                if ( cep )
                    {
                    if ( is_checked(hWnd, BAR_TYPE_XBOX) )
                        {
                        mycolor = cep->font_color;
                        if ( choose_color(hWnd, mycolor) )
                            cep->font_color = mycolor;
                        }
                    else
                        {
                        choose_font( hWnd, cep->font_handle, cep->font_color );
                        }
                    }
                return TRUE;
            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                          RECTANGLE_PROC                              *
***********************************************************************/
LRESULT CALLBACK rectangle_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static bool moving = false;
static int id;
static int cmd;
static CONTROL_ENTRY * cep;
static HDC dc;
static PAINTSTRUCT ps;
static WINDOW_CLASS w;
static RECTANGLE_CLASS r;

if ( msg == WM_CREATE )
    {
    /*
    -----------------------------------------------------
    I have no user data when I am created so just return.
    ----------------------------------------------------- */
    SetWindowLong( hWnd, GWL_USERDATA, (LONG) 0 );
    return 0;
    }

cep = (CONTROL_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );
id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_DESTROY:
        if ( cep )
            {
            delete cep;
            cep = 0;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG) cep );
            }
        return 0;

    case WM_LBUTTONDOWN:
        CurrentWindow = hWnd;
        lparam_to_point( CurrentMousePosition, lParam );
        SetCapture( hWnd );
        moving = true;
        return 0;

    case WM_MOUSEMOVE:
        if ( moving )
            {
            POINT p;
            lparam_to_point( p, lParam );
            if ( p.x != CurrentMousePosition.x || p.y != CurrentMousePosition.y )
                {
                p.x -= CurrentMousePosition.x;
                p.y -= CurrentMousePosition.y;
                w = hWnd;
                w.offset(p);
                MoveList.rewind();
                while ( MoveList.next() )
                    {
                    w = MoveList.handle();
                    if ( w.handle() != hWnd )
                        w.offset(p);
                    }

                }
            }
        return 0;

    case WM_NEW_DATA:
        if ( cep )
            {
            switch ( lParam )
                {
                case SET_MAX_VALUE:
                    cep->max_value = (int) wParam;
                    break;

                case SET_MIN_VALUE:
                    cep->min_value = (int) wParam;
                    break;

                case SET_VALUE:
                    cep->current_value = (int) wParam;
                    if ( !(cep->flags & FIXED_LIMITS_FLAG) )
                        {
                        if ( cep->current_value < cep->min_value )
                             cep->min_value = cep->current_value;

                        if ( cep->current_value > cep->max_value )
                             cep->max_value = cep->current_value;
                        }
                    break;
                }
            }
        return 0;

    case WM_LBUTTONUP:
        if ( moving )
            {
            ReleaseCapture();
            moving = false;
            }
        return 0;

    case WM_RBUTTONUP:
        /*
        ----------------------------------
        Tell the dialog where the mouse is
        ---------------------------------- */
        lparam_to_point( MousePosition, lParam );
        GetWindowRect( hWnd, &r );
        MousePosition.x += r.left;
        MousePosition.y += r.top;
        DialogBoxParam(
            MainInstance,
            TEXT("SET_RECT_SIZE_DIALOG"),
            hWnd,
            (DLGPROC) set_rect_size_dialog_proc,
            (LPARAM) hWnd
            );
        return 0;

    case WM_SETTEXT:
        if ( cep )
            cep->s = (TCHAR *) lParam;
        return 0;

    case WM_MOVE:
    case WM_SIZE:
        if ( cep )
            {
            w = hWnd;
            w.get_move_rect( cep->r );
            }
        return 0;

    case WM_PAINT:
        dc = BeginPaint( hWnd, &ps );
        if ( cep )
            {
            GetClientRect(hWnd, &r );
            DWORD window_style = (DWORD) GetWindowLong( hWnd, GWL_STYLE );

            if ( cep->type & BORDER_RECTANGLE_TYPE )
                {
                r.right--;
                r.bottom--;
                r.draw( dc, cep->color );
                }

            if ( cep->type & FILLED_RECTANGLE_TYPE )
                {
                if ( (window_style & WS_DISABLED) )
                    {
                    HWND parent = GetParent( hWnd );
                    HBRUSH b = (HBRUSH) GetClassLong( parent, GCL_HBRBACKGROUND );
                    if ( b )
                        r.fill( dc, b );
                    }
                else
                    {
                    r.fill( dc, cep->color );
                    }
                }

            if ( cep->type & BAR_TYPE )
                {
                if ( cep->type & BORDER_RECTANGLE_TYPE && (r.bottom-r.top > 2) )
                    {
                    r.top++;
                    r.left++;
                    }
                LONG divisor;
                divisor = (LONG)( cep->max_value - cep->min_value );
                if ( divisor == 0 )
                    divisor = 1;
                r.right *= (LONG) cep->current_value - cep->min_value;
                r.right /= divisor;
                r.fill( dc, cep->font_color );
                }

            if ( cep->type & TEXT_TYPE )
                {
                SetBkMode( dc, TRANSPARENT );
                COLORREF mycolor = cep->font_color;
                if ( (window_style & WS_DISABLED) )
                    mycolor = GetSysColor(COLOR_GRAYTEXT);

                SetTextColor( dc, mycolor );
                SetTextAlign( dc, TA_TOP | TA_LEFT );
                HFONT oldfont = INVALID_FONT_HANDLE;
                if ( cep->font_handle != INVALID_FONT_HANDLE )
                    oldfont = (HFONT) SelectObject( dc, cep->font_handle );
                SIZE sz;
                GetTextExtentPoint32( dc, cep->s.text(), cep->s.len(), &sz );

                int x = r.left;
                int y = r.top;
                int d;
                if ( cep->type & CENTER_VERTICALLY )
                    {
                    d = r.bottom - r.top - sz.cy;
                    if ( d > 0 )
                        {
                        d /= 2;
                        y += d;
                        }
                    }

                if ( cep->type & CENTER_HORIZONTALLY )
                    {
                    d = r.right - r.left - sz.cx;
                    if ( d > 0 )
                        {
                        d /= 2;
                        x += d;
                        }
                    }
                else if ( cep->type & RIGHT_JUSTIFY )
                    {
                    x = r.right - sz.cx;
                    if ( x > 1 )
                        x-=2;
                    }
                else
                    {
                    if ( (x+sz.cx+1)<r.right )
                        x+=2;
                    }

                TextOut( dc, x, y, cep->s.text(), cep->s.len() );
                if ( oldfont != INVALID_FONT_HANDLE )
                    SelectObject( dc, oldfont );
                }
            }
        EndPaint( hWnd, &ps );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                   =                                  *
***********************************************************************/
void CONTROL_ENTRY::operator=( const CONTROL_ENTRY & sorc )
{
FONT_CLASS         lf;

type = sorc.type;
color = sorc.color;
font_color = sorc.font_color;
id = sorc.id;
s = sorc.s;
r = sorc.r;

/*
--------------------------------------------------------------------------
I need to re-get the font handle so the font_class can keep count of users
-------------------------------------------------------------------------- */
font_handle = sorc.font_handle;
if ( font_handle != INVALID_FONT_HANDLE )
    {
    if ( FontList.get_font( lf, font_handle ) )
        font_handle = FontList.get_handle( lf );
    else
        font_handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                  READ                                *
***********************************************************************/
bool CONTROL_ENTRY::read( FILE_CLASS & f )
{
static TCHAR delimiter[] = TEXT( ":" );
static TCHAR spaces[]    = TEXT( " " );

FONT_CLASS         lf;
STRING_ARRAY_CLASS sa;

r.empty();
sa.add( f.readline() );

sa.split( delimiter );
sa.rewind();
while ( sa.next() )
    sa.strip( spaces );
sa.rewind();

sa.next();
type       = (unsigned int) hextoul( sa.text() );

sa.next();
color      = (COLORREF)  hextoul( sa.text() );

sa.next();
font_color = (COLORREF)  hextoul( sa.text() );

sa.next();
font_handle = INVALID_FONT_HANDLE;
if ( sa.len() > 6 )  /* >NoFont */
    {
    lf.get( sa.text() );
    font_handle = FontList.get_handle( lf );
    }

sa.next();
id = (int) asctoint32( sa.text() );

sa.next();
flags = (unsigned int) hextoul( sa.text() );

sa.next();
r.get( sa.text() );

sa.next();
s = sa.text();

/*
------------------------------------------------------------
If this is a bar control then initialize with the text value
------------------------------------------------------------ */
if ( type & BAR_TYPE )
    {
    current_value = s.int_value();
    }

return true;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                 WRITE                                *
***********************************************************************/
bool CONTROL_ENTRY::write( FILE_CLASS & f )
{
static TCHAR no_font_string[] = TEXT( "NoFont" );
static TCHAR delimiter[] = TEXT( " : " );
STRING_ARRAY_CLASS sa;

FONT_CLASS lf;
bool no_font;

sa.add( ultohex((unsigned long) type) );
sa.add( ultohex((unsigned long) color) );
sa.add( ultohex((unsigned long) font_color) );

no_font = true;
if ( font_handle != INVALID_FONT_HANDLE )
    {
    if ( FontList.get_font(lf, font_handle) )
        {
        sa.add( lf.put() );
        no_font = false;
        }
    }
if ( no_font )
    sa.add( no_font_string );

sa.add( int32toasc((int32) id) );
sa.add( ultohex(flags) );

sa.add( r.put() );
sa.add( s.text() );

sa.join( delimiter );

f.writeline( sa.text() );

return true;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                CREATE                                *
*                                                                      *
*  Call this routine to create the control window and attach this      *
*  control to it.                                                      *
*                                                                      *
***********************************************************************/
HWND CONTROL_ENTRY::create( HWND parent, HINSTANCE main_instance )
{
HWND w;

register_vtwrectangle();

w = CreateWindow(
    VTWRectangleClassName,
    0,
    WS_CHILD | WS_CLIPSIBLINGS,
    r.left, r.top,                      // x, y
    r.right-r.left, r.bottom-r.top,   // w, h
    parent,
    (HMENU) id,
    main_instance,
    NULL
    );

/*
----------------------------------------------------------------
The window is not made visible until after the user data is set.
The window is now responsible for killing me.
---------------------------------------------------------------- */
SetWindowLong( w, GWL_USERDATA, (LONG) this );
if ( type & BACKGROUND_TYPE )
    SetWindowPos( w, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE );

return w;
}

/***********************************************************************
*                            MS_CONTROL_ENTRY                          *
*                             IS_RADIO_BUTTON                          *
***********************************************************************/
bool MS_CONTROL_ENTRY::is_radio_button()
{
DWORD x;

x = style & BUTTON_MASK;

if ( name == ButtonControlName && (x == BS_AUTORADIOBUTTON || x == BS_RADIOBUTTON) )
    return true;

return false;
}

/***********************************************************************
*                            MS_CONTROL_ENTRY                          *
*                             IS_DEFPUSHBUTTON                         *
***********************************************************************/
bool MS_CONTROL_ENTRY::is_defpushbutton()
{
DWORD x;
const DWORD mymask = 0xFF;

if ( name == ButtonControlName )
    {
    x = style & mymask;
    if ( x == BS_DEFPUSHBUTTON )
        return true;
    }

return false;
}

/***********************************************************************
*                            MS_CONTROL_ENTRY                          *
*                              IS_EDITBOX                              *
***********************************************************************/
bool MS_CONTROL_ENTRY::is_editbox()
{

if ( name == EditControlName )
    return true;

return false;
}

/***********************************************************************
*                            MS_CONTROL_ENTRY                          *
*                                   =                                  *
***********************************************************************/
void MS_CONTROL_ENTRY::operator=( const MS_CONTROL_ENTRY & sorc )
{
FONT_CLASS         lf;

style = sorc.style;
id = sorc.id;
name = sorc.name;
s    = sorc.s;
r    = sorc.r;

/*
--------------------------------------------------------------------------
I need to re-get the font handle so the font_class can keep count of users
-------------------------------------------------------------------------- */
font_handle = sorc.font_handle;
if ( font_handle != INVALID_FONT_HANDLE )
    {
    if ( FontList.get_font( lf, font_handle ) )
        font_handle = FontList.get_handle( lf );
    else
        font_handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                             MS_CONTROL_ENTRY                         *
*                                CREATE                                *
*                                                                      *
*  Call this routine to create the control window and attach this      *
*  control to it.                                                      *
*                                                                      *
***********************************************************************/
HWND MS_CONTROL_ENTRY::create( HWND parent, HINSTANCE main_instance )
{
HWND w;
DWORD x;
const TCHAR * cp;

if ( s.isempty() )
    cp = 0;
else
    cp = s.text();

/*
---------------------------------------------------
Don't make this visible until after I attach myself
--------------------------------------------------- */
if ( is_editbox() )
    {
    w = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        name.text(),
        cp,
        style & ~WS_VISIBLE,
        r.left, r.top,                      // x, y
        r.right-r.left, r.bottom-r.top,   // w, h
        parent,
        (HMENU) id,
        main_instance,
        0
        );
    }
else
    {
    w = CreateWindow(
        name.text(),
        cp,
        style & ~WS_VISIBLE,
        r.left, r.top,                      // x, y
        r.right-r.left, r.bottom-r.top,   // w, h
        parent,
        (HMENU) id,
        main_instance,
        0
        );
    }

/*
----------------------------------------------------------------
The window is not made visible until after the user data is set.
The window is now responsible for killing me.
---------------------------------------------------------------- */
SetWindowLong( w, GWL_USERDATA, (LONG) this );

winproc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) ms_control_proc );

if ( font_handle != INVALID_FONT_HANDLE )
    SendMessage( w, WM_SETFONT, (WPARAM) font_handle, TRUE );

if ( name == ProgressControlName )
    {
    PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );
    PostMessage( w, PBM_SETPOS, (WPARAM) 2000, 0L );
    }

x = style & BUTTON_MASK;

if ( name == ButtonControlName && x == BS_GROUPBOX )
    is_groupbox = true;

if ( name == StaticControlName )
    is_static = true;

return w;
}

/***********************************************************************
*                             MS_CONTROL_ENTRY                         *
*                                  READ                                *
***********************************************************************/
bool MS_CONTROL_ENTRY::read( FILE_CLASS & f )
{
static TCHAR delimiter[] = TEXT( ":" );
static TCHAR spaces[]    = TEXT( " " );

FONT_CLASS lf;
STRING_ARRAY_CLASS sa;

r.empty();

sa.add( f.readline() );
sa.split( delimiter );
sa.rewind();
while ( sa.next() )
    sa.strip( spaces );
sa.rewind();

sa.next();
name  = sa.text();

sa.next();
style = (DWORD) hextoul( sa.text() );

sa.next();
font_handle = INVALID_FONT_HANDLE;
if ( sa.len() > 6 )  /* >NoFont */
    {
    lf.get( sa.text() );
    font_handle = FontList.get_handle( lf );
    }

sa.next();
id = (int) asctoint32( sa.text() );

sa.next();
r.get( sa.text() );

if ( sa.next() )
    s = sa.text();

return true;
}

/***********************************************************************
*                             MS_CONTROL_ENTRY                         *
*                                 WRITE                                *
***********************************************************************/
bool MS_CONTROL_ENTRY::write( FILE_CLASS & f )
{
static TCHAR no_font_string[] = TEXT( "NoFont" );
static TCHAR delimiter[] = TEXT( " : " );
STRING_ARRAY_CLASS sa;

FONT_CLASS lf;
bool no_font;

sa.add( name.text() );
sa.add( ultohex((unsigned long) style) );
no_font = true;
if ( font_handle != INVALID_FONT_HANDLE )
    {
    if ( FontList.get_font(lf, font_handle) )
        {
        sa.add( lf.put() );
        no_font = false;
        }
    }
if ( no_font )
    sa.add( no_font_string );

sa.add( int32toasc((int32) id) );
sa.add( r.put() );
sa.add( s.text() );

sa.join( delimiter );

f.writeline( sa.text() );

return true;
}

