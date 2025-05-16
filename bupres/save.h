#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#ifndef _TEXT_LIST_CLASS_
#include "..\include\textlist.h"
#endif

#define NO_BACKUP            0

#define SETUP_DATA              1
#define PROFILE_DATA            2
#define DOWNTIME_DATA           4
#define PARAMETER_DATA          8
#define ALARM_DATA             16
#define DO_BACKUP              32
#define DO_RESTORE             64
#define DO_DELETE             128
#define DO_RENUMBER           256
#define WITH_PURGE            512
#define SAVE_LAST_50         1024
#define PURGE_DOWNTIME       2048
#define SURETRAK_GLOBAL_DATA 4096
#define ANALOG_SENSOR_DATA   8192
#define FASTRAK_BOARDS_DATA 16384
#define PURGE_ALARM_SUMMARY 32768

struct SAVE_LIST_ENTRY
    {
    MACHINE_NAME_ENTRY name;
    TEXT_LIST_CLASS    part;
    };

#ifdef _MAIN_

BITSETYPE          BackupType;
HWND               BackupWindow;
TCHAR              BackupZipFileName[MAX_PATH+1];
PROFILE_NAME_ENTRY * BackupList    = 0;
int                BackupListCount = 0;

LISTBOX_CLASS      MachineBox;
LISTBOX_CLASS      PartBox;
LISTBOX_CLASS      ShotBox;

int32              ProfilesToCopy = 0;
int32              RenumberStartShot;
INT                NtoSave;
SAVE_LIST_ENTRY  * SaveList;
MACHINE_NAME_ENTRY DestMachine;

#else

extern BITSETYPE          BackupType;
extern HWND               BackupWindow;
extern TCHAR              BackupZipFileName[];
extern PROFILE_NAME_ENTRY * BackupList;
extern int                BackupListCount;

extern LISTBOX_CLASS      MachineBox;
extern LISTBOX_CLASS      PartBox;
extern LISTBOX_CLASS      ShotBox;

extern int32              ProfilesToCopy;
extern int32              RenumberStartShot;
extern INT                NtoSave;
extern SAVE_LIST_ENTRY  * SaveList;
extern MACHINE_NAME_ENTRY DestMachine;

#endif

void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id );
void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id, int shots_listbox_id, int32 renumber_start );
void save( TCHAR * zip_file_name, BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id );
void save_backup_list( TCHAR * zip_file_name, BITSETYPE backup_type );
void save_all_text_files( TCHAR * zip_file_name  );
void purge( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name );
void purge( PART_NAME_ENTRY & pne );

