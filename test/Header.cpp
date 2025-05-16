#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\listbox.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "header.h"

static TCHAR MyClassName[]       = TEXT( "Test" );

struct HEADER_COLUMN_PROPERTIES
    {
    int pixel_width;
    TCHAR * name;
    };

static HEADER_COLUMN_PROPERTIES HeaderColumn[] = {
    {  35, TEXT("View") },
    { 127, TEXT("Parameter Name") },
    {  58, TEXT("Minimum") },
    {  58, TEXT("Maximum") },
    {  60, TEXT("Use Limits") }
    };
const int NofHeaderColumns = sizeof(HeaderColumn)/sizeof(HEADER_COLUMN_PROPERTIES);

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS SpreadDialog;

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           SET_TAB_POSITIONS                          *
***********************************************************************/
void set_tab_positions( void )
{
int  i;
int  pixels_per_dialog_unit;
int  left;
HWND w;
RECT r;
int  tabs[NofHeaderColumns-1];

r.bottom = 1;
r.left   = 1;
r.right  = 4;
r.top    = 1;

if ( !SpreadDialog.handle() )
    return;

MapDialogRect( SpreadDialog.handle(), &r );
pixels_per_dialog_unit = r.right;

w = SpreadDialog.control(PARAMETER_LISTBOX);
GetWindowRect( w, &r );
left = r.left;

for ( i=0; i<NofHeaderColumns-1; i++ )
    {
    tabs[i] = 4 * HeaderColumn[i].pixel_width;
    tabs[i] /= pixels_per_dialog_unit;
    if ( i > 0 )
        tabs[i] += tabs[i-1];;
    }

SendMessage( w, LB_SETTABSTOPS, (WPARAM) NofHeaderColumns-1, (LPARAM) tabs );
InvalidateRect( w, 0, TRUE );
}

/***********************************************************************
*                           INSERT_HEADER_STRING                       *
***********************************************************************/
static int insert_header_string( int iInsertAfter, int slen, TCHAR * s )
{
HWND w;
HD_ITEM hdi;
int index;

hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
hdi.pszText = s;
hdi.cxy = slen;
hdi.cchTextMax = lstrlen(hdi.pszText);
hdi.fmt = HDF_LEFT | HDF_STRING;

w = SpreadDialog.control( PARAMETER_HEADER );
index = SendMessage( w, HDM_INSERTITEM, (WPARAM) iInsertAfter, (LPARAM) &hdi);

return index;
}

/***********************************************************************
*                           SPREAD_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK spread_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
LISTBOX_CLASS lb;


id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SpreadDialog = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_NOTIFY:
        NMHEADER * nm;

        nm = (NMHEADER *) lParam;
        if ( nm->hdr.code == HDN_ITEMCHANGED )
            {
            if ( nm->pitem )
                {
                if ( nm->pitem->mask & HDI_WIDTH )
                    {
                    if ( nm->iItem >= 0 && nm->iItem < NofHeaderColumns )
                        {
                        HeaderColumn[nm->iItem].pixel_width = nm->pitem->cxy;
                        set_tab_positions();
                        }
                    }
                }
            return TRUE;
            }
        break;

    case WM_DBINIT:
        MainWindow.shrinkwrap( hWnd );
        for ( id=0; id<NofHeaderColumns; id++ )
            insert_header_string( id, HeaderColumn[id].pixel_width, HeaderColumn[id].name );
        lb.init( hWnd, PARAMETER_LISTBOX );
        lb.add( "Yes\tFill Time\t0.0\t100.0\tYes" );
        lb.add( "Yes\tTotal Shot Time\t0.0\t100.0\tYes" );
        lb.add( "Yes\tSlow Shot Time\t0.0\t100.0\tNo" );
        lb.add( "Yes\tFast Shot Time\t0.0\t100.0\tNo" );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                return TRUE;
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
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
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
wc.hIcon         = 0;
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

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
    TEXT( "SPREAD_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) spread_dialog_proc );


MainWindow.show( cmd_show );
SpreadDialog.show( SW_SHOW );

UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( SpreadDialog.handle() )
        status = IsDialogMessage( SpreadDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
