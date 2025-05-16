struct MONITOR_ENTRY
     {
     BOOLEAN        has_changed;
     MACHINE_CLASS  machine;
     PART_CLASS     part;
     STRING_CLASS   display_seconds;
     };

#ifdef _MAIN_

HINSTANCE MainInstance;
HWND      MainWindow;

HWND      MonitorWindow = 0;

TCHAR     ComputerName[COMPUTER_NAME_LEN+1];

TCHAR     NoComputer[] = NO_COMPUTER;
TCHAR     NewSetupMachine[MACHINE_NAME_LEN+1];
BOOLEAN   ShuttingDown;

/*
---------------------------------------------
The following are changed ONLY in monitor.cpp
--------------------------------------------- */
MONITOR_ENTRY * MachineList = 0;
int             NofMachines;
int             CurrentMachineIndex = NO_INDEX;

#else

extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern HWND      MonitorWindow;
extern TCHAR     ComputerName[];
extern TCHAR     NoComputer[];
extern TCHAR     NewSetupMachine[];
extern BOOLEAN   ShuttingDown;

/*
---------------------------------------------
The following are changed ONLY in monitor.cpp
--------------------------------------------- */
extern MONITOR_ENTRY * MachineList;
extern int             NofMachines;
extern int             CurrentMachineIndex;

#endif

