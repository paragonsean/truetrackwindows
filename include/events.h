#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#define DDE_CODE_PAGE           CP_WINANSI
#define DDE_MONITOR_TOPIC       TEXT( "MON" )
#define DDE_CONFIG_TOPIC        TEXT( "CFG" )
#define DDE_ACTION_TOPIC        TEXT( "ACT" )
#define DDE_EXIT_TOPIC          TEXT( "EXIT")
#define MAX_DDE_DEFAULT_DATA_LEN 25
#define MAX_DDE_DATA_LEN        2000
#define MAX_DDE_ITEM_LEN        16
#define MAX_DDE_TOPIC_LEN       6

#define NEW_MACH_STATE_INDEX    0
#define SHOT_NUMBER_INDEX       1
#define MONITOR_SETUP_INDEX     2
#define PART_SETUP_INDEX        3
#define ALARM_INDEX             4
#define MACH_SETUP_INDEX        5
#define PARAM_DATA_INDEX        6
#define DOWN_DATA_INDEX         7
#define PW_LEVEL_INDEX          8
#define PLOT_MIN_INDEX          9
#define DS_NOTIFY_INDEX         10
#define COMPUTER_SETUP_INDEX    11

#define VISITRAK_EVENT_MANAGER  TEXT( "EventMan" )

struct EVENT_ITEM_ENTRY
    {
    short   index;
    void    (*callback)( TCHAR * topic, short item, HDDEDATA edata);
    };

struct EVENT_ENTRY
    {
    TCHAR   topic[MAX_DDE_TOPIC_LEN+1];
    HCONV   handle;
    GENERIC_LIST_CLASS items;
    };

struct ITEM_ENTRY
    {
    TCHAR   name[MAX_DDE_ITEM_LEN+1];
    TCHAR   default_value[MAX_DDE_DEFAULT_DATA_LEN+1];
    HSZ     handle;
    };

#ifdef _MAIN_

int NofItems = 12;

ITEM_ENTRY ItemList[] = {
    { TEXT("NEW_MACH_STATE"), NO_MACHINE TEXT("\t0000\t0000\015\012"), 0 },
    { TEXT("SHOT_NUMBER"),    TEXT("1\015\012"),                      0 },
    { TEXT("MONITOR_SETUP"),  NO_MACHINE TEXT("\015\012"),            0 },
    { TEXT("PART_SETUP"),     NO_PART,                          0 },
    { TEXT("PARAM_ALARM"),    TEXT("0\015\012"),                      0 },  /* Parameter Number [1,60] */
    { TEXT("MACH_SETUP"),     NO_MACHINE TEXT("\015\012"),            0 },
    { TEXT("PARAM_DATA"),     NO_MACHINE TEXT("\015\012"),            0 },
    { TEXT("DOWN_DATA"),      NO_MACHINE TEXT("\015\012"),            0 },
    { TEXT("PW_LEVEL"),       TEXT("0\015\012"),                      0 },
    { TEXT("PLOT_MIN"),       TEXT("0"),                              0 },
    { TEXT("DS_NOTIFY"),      TEXT("0"),                              0 },
    { TEXT("COMPUTER_SETUP"), TEXT("0\015\012"),                      0 }
    };
#else

extern int NofItems;
extern ITEM_ENTRY ItemList[];

#endif

void poke_data( TCHAR * topic, TCHAR * item, TCHAR * new_value );
void client_dde_shutdown( void );
void client_dde_startup( void );
void client_dde_startup( HWND main_window );
void unregister_for_event( TCHAR * topic, short item );
void register_for_event( TCHAR * topic, short item, void (*event_callback)( TCHAR * topic, short item, HDDEDATA edata) );

