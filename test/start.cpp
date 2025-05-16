#include <Windows.h>
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )

{
if ( (int) ShellExecute(NULL, NULL, cmd_line, NULL, NULL, SW_SHOWNORMAL) <= 32)
    {
    TCHAR sz[1024];
    wsprintf(sz, TEXT("Error openning %s on CD-ROM disc."), cmd_line);
    MessageBox(NULL, sz, TEXT("ShellExecute"), MB_OK | MB_ICONWARNING);
    }

return(0);
}



