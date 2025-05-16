/*
---------------------------
Types for get_zip_file_path
--------------------------- */
const  int   PART_PATH    = 0;
const  int   MACHINE_PATH = 1;

#ifdef _MAIN_

HINSTANCE MainInstance              = 0;
HWND      MainWindow                = 0;
HWND      MainDialogWindow          = 0;

TCHAR   * BackupDirectory           = 0;
TCHAR     BackupComputerName[]      = BACKUP_COMPUTER;
BOOLEAN   SkipDowntimeZip           = FALSE;
BOOLEAN   Using_Machine_Based_Zip_Paths = FALSE;
BOOLEAN   NeedToReloadZipPaths      = FALSE;
BOOLEAN   HaveMirror                = FALSE;
BOOLEAN   UsingLogFile              = FALSE;

STRING_CLASS DefaultMachineZipPath;
STRING_CLASS DefaultPartZipPath;
STRING_CLASS MirrorRoot;            /* Keep a second copy of the backup files here */
STRING_CLASS LogFilePath;

TCHAR DsBackupLogFile[]        = TEXT( "DsBackup.log" );
TCHAR BackupMachinePathString[]= TEXT( "BackupMachinePath" );
TCHAR BackupPartPathString[]   = TEXT( "BackupPartPath" );
TCHAR IniFileName[]            = TEXT( "dsbackup.ini" );

TCHAR ConfigSection[]          = TEXT( "Config" );
TCHAR DefaultSection[]         = TEXT( "Default" );
TCHAR DsBackupSection[]        = TEXT( "DsBackup" );
TCHAR ShotparmSection[]        = TEXT( "Shotparm" );

TCHAR MachineBasedZipPathsKey[]= TEXT( "MachineBasedZipPaths" );
TCHAR RecordFormatKey[]        = TEXT( "RecordFormat" );
TCHAR FileNameKey[]            = TEXT( "FileName" );
TCHAR HaveAlarmShotKey[]       = TEXT( "HaveAlarmShot" );
TCHAR HaveGoodShotKey[]        = TEXT( "HaveGoodShot" );
TCHAR HaveMirrorKey[]          = TEXT( "HaveMirror" );
TCHAR HaveWarningShotKey[]     = TEXT( "HaveWarningShot" );
TCHAR MirrorRootKey[]          = TEXT( "MirrorRoot" );
TCHAR UseLogFileKey[]          = TEXT( "UseLogFile" );

TCHAR * HaveAlarmShot   = 0;
TCHAR * HaveGoodShot    = 0;
TCHAR * HaveWarningShot = 0;
DS_BACKUP_CLASS BList;

#else

extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern HWND      MainDialogWindow;
extern TCHAR   * BackupDirectory;
extern TCHAR     BackupComputerName[];
extern BOOLEAN   SkipDowntimeZip;
extern BOOLEAN   Using_Machine_Based_Zip_Paths;
extern BOOLEAN   NeedToReloadZipPaths;
extern BOOLEAN   HaveMirror;
extern BOOLEAN   UsingLogFile;

extern STRING_CLASS DefaultMachineZipPath;
extern STRING_CLASS DefaultPartZipPath;
extern STRING_CLASS MirrorRoot;
extern STRING_CLASS LogFilePath;

extern TCHAR DsBackupLogFile[];
extern TCHAR BackupMachinePathString[];
extern TCHAR BackupPartPathString[];
extern TCHAR IniFileName[];

extern TCHAR DefaultSection[];
extern TCHAR ShotparmSection[];

extern TCHAR MachineBasedZipPathsKey[];
extern TCHAR RecordFormatKey[];
extern TCHAR FileNameKey[];

extern TCHAR * HaveAlarmShot;
extern TCHAR * HaveGoodShot;
extern TCHAR * HaveWarningShot;
#ifdef _DS_BACKUP_CLASS_
extern DS_BACKUP_CLASS BList;
#endif

#endif
