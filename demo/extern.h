const int START_MONITORING = 1;
const int STOP_MONITORING  = 2;
const int MANUAL_SHOT_REQ  = 3;

#ifdef _MAIN_

HANDLE       BoardMonitorEvent = 0;
HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS AutoShotDisplayWindow;
WINDOW_CLASS ManualShotWindow;
WINDOW_CLASS BrouseWindow;
STRING_CLASS SorcDir;
STRING_CLASS CycleTime;
long         SecondsBetweenShots = 0;

/*
--------------------------------------------------------------------------------
If the following is true then the chosen sorc directory has changed but the user
hasn't pressed the Start button (the monitor thread changes this to FALSE).
-------------------------------------------------------------------------------- */
BOOLEAN      HaveNewSorc  = TRUE;
HWND         SorcDirEbox = 0;

BOOLEAN      IsContinuous = FALSE;
BOOLEAN      ManualMode   = FALSE;
BOOLEAN      BoardMonitorIsRunning = FALSE;
BOOLEAN      ShuttingDown          = FALSE;
int          BoardMonitorCommand   = 0;
PART_NAME_ENTRY CurrentPart;
BOOLEAN         HaveNewPart  = FALSE;

BOOLEAN      SubtractTimeoutFromAutoDown = FALSE;
long         DownTimeoutSeconds = 0;
int          CurrentDownState   = NO_DOWN_STATE;

#else

extern HINSTANCE MainInstance;
extern HANDLE    BoardMonitorEvent;

#ifdef _STRING_CLASS_
extern STRING_CLASS SorcDir;
extern STRING_CLASS CycleTime;
#endif
extern long         SecondsBetweenShots;

extern BOOLEAN      HaveNewSorc;
extern HWND         SorcDirEbox;

extern BOOLEAN      IsContinuous;
extern BOOLEAN      ManualMode;
extern BOOLEAN      BoardMonitorIsRunning;
extern BOOLEAN      ShuttingDown;
extern int          BoardMonitorCommand;

#ifdef _STRUCTS_
extern PART_NAME_ENTRY CurrentPart;
#endif
extern BOOLEAN         HaveNewPart;

#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS MainWindow;
extern WINDOW_CLASS AutoShotDisplayWindow;
extern WINDOW_CLASS BrouseWindow;
extern WINDOW_CLASS ManualShotWindow;
#endif
extern BOOLEAN      SubtractTimeoutFromAutoDown;
extern long         DownTimeoutSeconds;
extern int          CurrentDownState;

#endif

