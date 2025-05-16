#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\ft.h"
#include "..\include\structs.h"

HINSTANCE MainInstance;
HWND      MainWindow;

TCHAR MyClassName[] = TEXT("Opto Test");
BOARD_DATA BoardType = 0;
BOARD_DATA Command = 0;

/*
--------------------------------
Pointer used by fastrak routines
-------------------------------- */
POS_SENSOR_SETUP_DATA * utype = 0;

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
static TCHAR       prompt[] = TEXT( "Space to toggle, Esc to exit" );
static HDC         dc;
static PAINTSTRUCT ps;
static int         id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        ft_send_command( GET_BOARD_TYPE );
        BoardType = ft_read_data();
        ft_set_type( FT_MUX_MACHINE_TYPE );
        ft_send_data( Command );
        ft_send_command( OUT_TO_OPTO );
        break;

     case WM_PAINT:
        dc = BeginPaint( hWnd, &ps );
        TextOut( dc, 0, 0, prompt, lstrlen(prompt) );
        EndPaint (hWnd, &ps) ;
        return 0 ;

    case WM_CHAR:
        if ( wParam == VK_ESCAPE )
            {
            PostMessage( hWnd, WM_CLOSE, 0, 0L );
            }
        else
            {
            if ( Command == 0 )
                {
                Command = 1;
                }
            else if ( Command < 0x80 )
                {
                Command <<=1;
                }
            else if ( Command == 0x80 )
                {
                Command=0xFF;
                }
            else
                {
                Command = 0;
                }

            // Command ^= 0xff;
            ft_send_data( Command );
            ft_send_command( OUT_TO_OPTO );
            }
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN init( int cmd_show )
{
WNDCLASS wc;

ft_startup();
ft_setports( 0X220 );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyClassName );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyClassName,
    WS_POPUP | WS_CAPTION,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    186, 46,                          // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG     msg;

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

ft_set_type( BoardType );
ft_shutdown();

return(msg.wParam);
}
