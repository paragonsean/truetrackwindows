#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#include "extern.h"

static const TCHAR NormColorName[] = TEXT( "NormColor" );
static const TCHAR RefColorName[] = TEXT( "RefColor"  );
static const TCHAR PrColorName[] = TEXT( "PrColor"   );
static const TCHAR PrefColorName[] = TEXT( "PrefColor" );

static const TCHAR WarningColorName[] = TEXT("WarningColor"    );
static const TCHAR AlarmColorName[] = TEXT("AlarmColor"      );
static const TCHAR NoAlarmColorName[] = TEXT("NoAlarmColor"    );
static const TCHAR DialogTextColorName[] = TEXT("DialogTextColor" );

static TCHAR * AxisName[NOF_COLORS] = {
    TEXT("Time"),
    TEXT("YPos"),
    TEXT("XPos"),
    TEXT("Velocity"),
    TEXT("Analog1"),
    TEXT("Analog2"),
    TEXT("Analog3"),
    TEXT("Analog4"),
    TEXT("Backgr"),
    TEXT("Border"),
    TEXT("Grid"),
    TEXT("Text"),
    TEXT("Trend") };

#define MAX_BACKUP_NAME_LEN 40

/***********************************************************************
*                            GET_SAVED_COLOR                           *
***********************************************************************/
static BOOL get_saved_color( COLORREF * dest, const TCHAR * name )
{
TCHAR * cp;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, name );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    *dest = hextoul( cp );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           GET_SAVED_COLOR                            *
***********************************************************************/
static BOOL get_saved_color( COLORREF * dest, const TCHAR * basename, const TCHAR * suffix )
{
TCHAR myname[MAX_BACKUP_NAME_LEN+1];

lstrcpy( myname, basename );
lstrcat( myname, suffix );

return get_saved_color( dest, myname );
}

/***********************************************************************
*                              PUT_COLOR                               *
***********************************************************************/
static void put_color( const TCHAR * basename, const TCHAR * suffix, COLORREF color_to_save )
{
TCHAR myname[MAX_BACKUP_NAME_LEN+1];

lstrcpy( myname, basename );
lstrcat( myname, suffix );

put_ini_string( VisiTrakIniFile, ConfigSection, myname, ultohex(color_to_save) );
}

/***********************************************************************
*                          RESTORE_COLOR_SET                           *
***********************************************************************/
static void restore_color_set( COLORREF * dest, const TCHAR * basename )
{
int i;

for ( i=0; i<NOF_COLORS; i++ )
    get_saved_color( dest+i, basename, AxisName[i] );

}

/***********************************************************************
*                             SAVE_COLOR_SET                           *
***********************************************************************/
static void save_color_set( const TCHAR * basename, COLORREF * sorc )
{
int i;

for ( i=0; i<NOF_COLORS; i++ )
    put_color( basename, AxisName[i], sorc[i] );

}


/***********************************************************************
*                            RESTORE_COLORS                            *
***********************************************************************/
void restore_colors( void )
{

restore_color_set( NormalColor,      NormColorName );
restore_color_set( ReferenceColor,   RefColorName  );
restore_color_set( PrNormalColor,    PrColorName   );
restore_color_set( PrReferenceColor, PrefColorName );

get_saved_color( &WarningColor,    WarningColorName );
get_saved_color( &AlarmColor,      AlarmColorName );
get_saved_color( &NoAlarmColor,    NoAlarmColorName );
get_saved_color( &DialogTextColor, DialogTextColorName );
}

/***********************************************************************
*                             SAVE_COLORS                              *
***********************************************************************/
void save_colors( void )
{
save_color_set( NormColorName, NormalColor      );
save_color_set( RefColorName,  ReferenceColor   );
save_color_set( PrColorName,   PrNormalColor    );
save_color_set( PrefColorName, PrReferenceColor );

put_ini_string( VisiTrakIniFile, ConfigSection, WarningColorName,      ultohex(WarningColor) );
put_ini_string( VisiTrakIniFile, ConfigSection, AlarmColorName,        ultohex(AlarmColor) );
put_ini_string( VisiTrakIniFile, ConfigSection, NoAlarmColorName,      ultohex(NoAlarmColor) );
put_ini_string( VisiTrakIniFile, ConfigSection, DialogTextColorName,   ultohex(DialogTextColor) );
}
