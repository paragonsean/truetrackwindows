
#ifdef _MAIN_

HINSTANCE    MainInstance;
HWND         MainWindow;
HWND         MessageListbox = 0;
STRING_CLASS DataServerComputer;
BOOLEAN      HaveDataServer = FALSE;
BOOLEAN      HaveSockets    = FALSE;
BOOLEAN      HaveMailslots  = FALSE;
TCHAR        ReloadRemoteComputers[] = TEXT( "RELOAD_REMOTE_COMPUTERS" );
TCHAR        ShowStatusRequest[]     = TEXT( "SHOW_STATUS" );
#else

extern HINSTANCE    MainInstance;
extern HWND         MainWindow;
extern HWND         MessageListbox;
extern STRING_CLASS DataServerComputer;
extern BOOLEAN      HaveDataServer;
extern BOOLEAN      HaveSockets;
extern BOOLEAN      HaveMailslots;
extern TCHAR        ReloadRemoteComputers[];
extern TCHAR        ShowStatusRequest[];
extern BOOLEAN      ListEvents;

#endif
