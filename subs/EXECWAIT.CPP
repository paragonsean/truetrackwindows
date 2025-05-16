#include <windows.h>

static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                            EXECUTE_AND_WAIT                          *
***********************************************************************/
BOOLEAN execute_and_wait( TCHAR * command_line, TCHAR * current_directory, DWORD ms_to_wait )
{
BOOLEAN return_status;
BOOL    status;

STARTUPINFO         si;
PROCESS_INFORMATION pi;

si.cb              = sizeof( si );
si.lpReserved      = 0;
si.lpDesktop       = EmptyString;
si.lpTitle         = 0;
si.dwX             = 0;
si.dwY             = 0;
si.dwXSize         = 0;
si.dwYSize         = 0;
si.dwXCountChars   = 0;
si.dwYCountChars   = 0;
si.dwFillAttribute = 0;
si.dwFlags         = 0;
si.wShowWindow     = 0;
si.cbReserved2     = 0;
si.lpReserved2     = 0;
si.hStdInput       = 0;
si.hStdOutput      = 0;
si.hStdError       = 0;

return_status = FALSE;

status = CreateProcess(
    0,                   /* lpApplicationName */
    command_line,
    0,                   /* lpProcessAttributes */
    0,                   /* lpThreadAttributes*/
    FALSE,               /*  BOOL bInheritHandles */
    CREATE_NEW_CONSOLE,  /*  DWORD dwCreationFlags */
    0,                   /*  LPVOID lpEnvironment */
    current_directory,   /* LPCTSTR lpCurrentDirectory */
    &si,
    &pi
    );

if ( status != 0 )
    {
    /*
    -------------------------------------------
    Wait for the monitor program to be unzipped
    (The handle is signaled upon exit).
    ------------------------------------------- */
    if ( ms_to_wait > 0 )
        WaitForSingleObject( pi.hProcess, ms_to_wait );
    return_status = TRUE;
    }

return return_status;
}

/***********************************************************************
*                            EXECUTE_AND_WAIT                          *
***********************************************************************/
BOOLEAN execute_and_wait( TCHAR * command_line, TCHAR * current_directory )
{
const DWORD DEFAULT_MS_TO_WAIT = 120000L;

return execute_and_wait( command_line, current_directory, DEFAULT_MS_TO_WAIT );
}