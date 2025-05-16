#ifdef _MAIN_

HINSTANCE MainInstance;
HWND      MainWindow;
BOOLEAN   HaveMailslots;
TCHAR     ReloadRemoteComputers[] = TEXT( "RELOAD_REMOTE_COMPUTERS" );
TCHAR     ShowStatusRequest[]     = TEXT( "SHOW_STATUS" );
CRITICAL_SECTION ComputersCriticalSection; /* Used only in mail but init and delete in eventman */

#else

extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern BOOLEAN   HaveDataServer;
extern BOOLEAN   HaveMailslots;
extern BOOLEAN   DisplayingMessages;
extern TCHAR     VisiTrakIniFile[];
extern TCHAR     ConfigSection[];
extern TCHAR     DataServerKey[];
extern TCHAR     ReloadRemoteComputers[];
extern TCHAR     ShowStatusRequest[];
extern STRING_CLASS DataServerComputer;
extern CRITICAL_SECTION ComputersCriticalSection;
#endif
