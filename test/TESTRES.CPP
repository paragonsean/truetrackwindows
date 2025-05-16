#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#include "testres.h"

HINSTANCE MainInstance;
HWND      ExportDialogWindow;
HWND      TabControl;

#define MAX_TAB_LEN          25

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}


HICON hIcon;

HBITMAP    hBmp;
HIMAGELIST hIml;

hIml = ImageList_Create ( BITMAP_WIDTH, BITMAP_HEIGHT, FALSE, 2, 10 );

if (hIml == NULL)
    MessageBox (NULL, "ImageList_Create failed!", NULL, MB_OK);

hBmp       = LoadBitmap (hInst, MAKEINTRESOURCE (FORSALE_BMP));
idxForSale = ImageList_Add (hIml, hBmp, NULL);

hBmp    = LoadBitmap (hInst, MAKEINTRESOURCE (CITY_BMP));
idxCity = ImageList_Add (hIml, hBmp, NULL);

hBmp      = LoadBitmap (hInst, MAKEINTRESOURCE (SELCITY_BMP));
idxSelect = ImageList_Add (hIml, hBmp, NULL);


hIcon = LoadIcon ( hInst, MAKEINTRESOURCE(HOUSE_ICON) );
if ((ImageList_AddIcon (hSmall, hIcon) == -1 )
    MessageBox (NULL, "ImageList_AddIcon failed!", NULL, MB_OK);



































/***********************************************************************
*                          CREATE_TAB_CONTROL                          *
***********************************************************************/
static void create_tab_control( void )
{

TC_ITEM tie;
int i;
TCHAR buf[MAX_TAB_LEN+1];

TabControl = GetDlgItem( ExportDialogWindow, EXPORT_TAB_CONTROL );
if ( TabControl == NULL)
    return;

tie.mask = TCIF_TEXT | TCIF_IMAGE;
tie.iImage = -1;
tie.pszText = buf;

for ( i=0; i<TAB_COUNT; i++ )
    {
    copystring( buf, resource_string(FIRST_TAB_STRING + i) );
    if ( TabCtrl_InsertItem(TabControl, i, &tie) == -1 )
        {
        DestroyWindow(  TabControl);
        TabControl = 0;
        break;
        }
    }

}

/***********************************************************************
*                          EXPORT_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK export_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ExportDialogWindow = hWnd;
        create_tab_control();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                ExportDialogWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        ExportDialogWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
void startup( void )
{
InitCommonControls();
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MainInstance = this_instance;

startup();

DialogBox( MainInstance, TEXT("EXPORT_TO_CLIPBOARD_DIALOG"), NULL,(DLGPROC) export_dialog_proc );

shutdown();
return 0;
}
