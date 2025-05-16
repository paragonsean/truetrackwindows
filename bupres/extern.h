const BOOL OVERWRITE_EXISTING = FALSE;

#ifdef _MAIN_

HINSTANCE MainInstance;
HWND      MainWindow;
int       NextShotNumber = 1;
TCHAR     BackupDir[MAX_PATH+1] = TEXT( "A:\\" );
short     HistoryShotCount = 50;
BOOLEAN   HaveSureTrakControl = FALSE;
BOOLEAN   CreatedNewMachine   = FALSE;
BOOLEAN   BackupAllTextFiles  = FALSE;
MACHINE_NAME_LIST_CLASS MachList;

#else

extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern int       NextShotNumber;
extern TCHAR     BackupDir[];
extern short     HistoryShotCount;
extern BOOLEAN   HaveSureTrakControl;
extern BOOLEAN   CreatedNewMachine;
extern BOOLEAN   BackupAllTextFiles;

#ifdef _MACHINE_NAME_LIST_CLASS_
extern MACHINE_NAME_LIST_CLASS MachList;
#endif

#endif
