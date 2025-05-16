#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\bitclass.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\listbox.h"
#include "..\include\param.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "extern.h"
#include "fixparam.h"
#include "tcconfig.h"

const MAX_LB_LEN = PARAMETER_NAME_LEN + 1 + MAX_DOUBLE_LEN + 1 + MAX_DOUBLE_LEN + 1 + MAX_YES_LEN;

const TCHAR NChar       = TEXT( 'N' );
const TCHAR NullChar    = TEXT( '\0' );
const TCHAR TabChar     = TEXT( '\t' );
const TCHAR YChar       = TEXT( 'Y' );

const TCHAR NoString[]  = TEXT( "No" );
const TCHAR YesString[] = TEXT( "Yes" );

static TCHAR FixedParamIniFile[] = TEXT( "fixedparam.ini" );
static TCHAR TcConfigIniFile[]   = TEXT( "tcconfig.ini" );
static TCHAR ConfigSectionName[] = TEXT( "Config" );

static TCHAR AverageOverTypeKey[]   = TEXT("AverageOverType");
static TCHAR ChartTypeKey[]         = TEXT("ChartType");
static TCHAR FirstShotKey[]         = TEXT("FirstShot");
static TCHAR LastShotKey[]          = TEXT("LastShot");
static TCHAR LclSigmaValueKey[]     = TEXT("LclSigmaValue");
static TCHAR NOfPointsKey[]         = TEXT("NOfPoints");
static TCHAR NToAverageKey[]        = TEXT("NToAverage");
static TCHAR NeedAlarmLimitsKey[]   = TEXT("NeedAlarmLimits");
static TCHAR NeedLclLimitKey[]      = TEXT("NeedLclLimit");
static TCHAR NeedUclLimitKey[]      = TEXT("NeedUclLimit");
static TCHAR NeedWarningLimitsKey[] = TEXT("NeedWarningLimits");
static TCHAR TimePeriodTypeKey[]    = TEXT("TimePeriodType");
static TCHAR UclSigmaValueKey[]     = TEXT("UclSigmaValue");
static TCHAR VisibleListKey[]       = TEXT("VisibleList");
static TCHAR XAxisTypeKey[]         = TEXT("XAxisType");

static TCHAR IsFixedKey[] = TEXT( "IsFixed" );
static TCHAR MaxValueKey[] = TEXT( "MaxValue" );
static TCHAR MinValueKey[] = TEXT( "MinValue" );

static bool UpdatingEditControls = false;

static BIT_CLASS linechanged;

class TREND_SCREEN_CONFIG_CLASS : public TREND_CONFIG_CLASS
{
public:

void put_to_screen( HWND w );
void get_from_screen( HWND w );
};

static TREND_SCREEN_CONFIG_CLASS Tc;

void erase_trend_cursor_line( void );
TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                           BOOLEAN_FROM_INI                           *
***********************************************************************/
static BOOLEAN boolean_from_ini( TCHAR * key )
{
return boolean_from_ini( TcConfigIniFile, ConfigSectionName, key );
}

/***********************************************************************
*                            BOOLEAN_TO_INI                            *
***********************************************************************/
static void boolean_to_ini( TCHAR * key, BOOLEAN b )
{
boolean_to_ini( TcConfigIniFile, ConfigSectionName, key, b );
}

/***********************************************************************
*                            INT32_FROM_INI                            *
***********************************************************************/
static int32 int32_from_ini( TCHAR * key )
{
return int32_from_ini( TcConfigIniFile, ConfigSectionName, key );
}

/***********************************************************************
*                              INT32_TO_INI                            *
***********************************************************************/
static void int32_to_ini( TCHAR * key, int32 i )
{
int32_to_ini( TcConfigIniFile, ConfigSectionName, key, i );
}

/***********************************************************************
*                            FLOAT_FROM_INI                            *
***********************************************************************/
static float float_from_ini( TCHAR * key )
{
return float_from_ini( TcConfigIniFile, ConfigSectionName, key );
}

/***********************************************************************
*                              FLOAT_TO_INI                            *
***********************************************************************/
static void float_to_ini( TCHAR * key, float x )
{
float_to_ini( TcConfigIniFile, ConfigSectionName, key, x );
}

/***********************************************************************
*                            SET_CHECKED_STATE                         *
***********************************************************************/
static void set_checked_state( HWND w, INT box, BOOLEAN button_is_checked )
{
UINT state;

if ( button_is_checked )
    state = BST_CHECKED;
else
    state = BST_UNCHECKED;

CheckDlgButton( w, box, state );
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::FIXED_PARAM_CLASS()
{
*min_value = NullChar;
*max_value = NullChar;
is_fixed = false;
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::FIXED_PARAM_CLASS( const FIXED_PARAM_CLASS & sorc )
{
*this = sorc;
}

/***********************************************************************
*                          ~FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::~FIXED_PARAM_CLASS()
{
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                  READ                                *
***********************************************************************/
void FIXED_PARAM_CLASS::read( TCHAR * parameter_name )
{
INI_CLASS inifile;
TCHAR   * cp;

*min_value = NullChar;
*max_value = NullChar;
is_fixed   = false;

if ( PartBasedDisplay )
    {
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
    if ( inifile.exists() )
        {
        if ( inifile.find(parameter_name, IsFixedKey) )
            {
            if ( inifile.get_boolean() )
                is_fixed = true;

            if ( inifile.find(parameter_name, MaxValueKey) )
                lstrcpyn( max_value, inifile.get_string(), MAX_DOUBLE_LEN+1);

            if ( inifile.find(parameter_name, MinValueKey) )
                lstrcpyn( min_value, inifile.get_string(), MAX_DOUBLE_LEN+1);

            return;
            }
        }
    }

is_fixed = bool_from_ini( FixedParamIniFile, parameter_name, IsFixedKey  );

cp = get_ini_string( FixedParamIniFile, parameter_name, MaxValueKey );
if ( !unknown(cp) )
    lstrcpyn( max_value, cp, MAX_DOUBLE_LEN+1);

cp = get_ini_string( FixedParamIniFile, parameter_name, MinValueKey );
if ( !unknown(cp) )
    lstrcpyn( min_value, cp, MAX_DOUBLE_LEN+1);
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                 WRITE                                *
***********************************************************************/
void FIXED_PARAM_CLASS::write( TCHAR * parameter_name )
{
INI_CLASS inifile;
BOOLEAN   b;

if ( PartBasedDisplay )
    {
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
    b = FALSE;
    if ( is_fixed )
        b = TRUE;
    inifile.put_boolean( parameter_name, IsFixedKey, b );
    inifile.put_string(  parameter_name, MaxValueKey, max_value );
    inifile.put_string(  parameter_name, MinValueKey, min_value );
    }
else
    {
    bool_to_ini(    FixedParamIniFile, parameter_name, IsFixedKey, is_fixed   );
    put_ini_string( FixedParamIniFile, parameter_name, MaxValueKey, max_value );
    put_ini_string( FixedParamIniFile, parameter_name, MinValueKey, min_value );
    }
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                   =                                  *
***********************************************************************/
FIXED_PARAM_CLASS & FIXED_PARAM_CLASS::operator=( const FIXED_PARAM_CLASS & sorc )
{
lstrcpy( min_value, sorc.min_value );
lstrcpy( max_value, sorc.max_value );
is_fixed  = sorc.is_fixed;

return *this;
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                LBLINE                                *
***********************************************************************/
TCHAR * FIXED_PARAM_CLASS::lbline( TCHAR * parameter_name )
{

static TCHAR s[MAX_LB_LEN+1];

TCHAR * cp;
UINT  id;

cp = copy_w_char( s,  parameter_name, TabChar );
cp = copy_w_char( cp, min_value,      TabChar );
cp = copy_w_char( cp, max_value,      TabChar );
if ( is_fixed )
    id = YES_STRING;
else
    id = NO_STRING;

lstrcpy( cp, resource_string(id) );

return s;
}

/***********************************************************************
*                              GET_FROM_LINE                           *
***********************************************************************/
void FIXED_PARAM_CLASS::get_from_line( TCHAR * sorc )
{
TCHAR s[MAX_LB_LEN+1];

lstrcpyn( s, sorc, MAX_LB_LEN+1 );

sorc = s;
if ( replace_char_with_null(sorc, TabChar) )
    {
    sorc = nextstring( sorc );
    if ( replace_char_with_null(sorc, TabChar) )
        {
        lstrcpy( min_value, sorc );
        sorc = nextstring( sorc );
        if ( replace_char_with_null(sorc, TabChar) )
            {
            lstrcpy( max_value, sorc );
            sorc = nextstring( sorc );
            if ( strings_are_equal(sorc, resource_string(YES_STRING)) )
                is_fixed = true;
            else
                is_fixed = false;
            }
        }
    }

}

/***********************************************************************
*                            WRITE_FROM_LINE                           *
*   This assumes that you are passing a line from the listbox.         *
***********************************************************************/
void FIXED_PARAM_CLASS::write_from_line( TCHAR * sorc )
{
TCHAR pname[PARAMETER_NAME_LEN + 1];
TCHAR * d;
TCHAR * s;
int len;

s = sorc;
d = pname;
len = 0;
while ( len <= PARAMETER_NAME_LEN )
    {
    *d = *s;
    if ( *s == NullChar || *s == TabChar )
        break;
    d++;
    s++;
    len++;
    }

*d = NullChar;
if ( len > 0 )
    {
    get_from_line( sorc );
    write( pname );
    }
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
***********************************************************************/
TREND_CONFIG_CLASS::TREND_CONFIG_CLASS()
{
int i;

chart_type          = TREND_CHART_TYPE;
nto_average         = 1;
average_over_type   = SHOT_AVERAGE_TYPE;
time_period_type    = LAST_N_TIME_PERIOD_TYPE;
nof_points          = 50;
first_shot          = 1;
last_shot           = 100;
x_axis_type         = SHOT_AVERAGE_TYPE;
need_alarm_limits   = FALSE;
need_warning_limits = FALSE;
need_ucl            = FALSE;
need_lcl            = FALSE;
ucl_sigma_value     = 1.0;
lcl_sigma_value     = 1.0;

for ( i=0; i<MAX_PARMS; i++ )
    visible[i] = TRUE;
}

/***********************************************************************
*                          ~TREND_CONFIG_CLASS                         *
***********************************************************************/
TREND_CONFIG_CLASS::~TREND_CONFIG_CLASS()
{
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                   =                                  *
***********************************************************************/
void TREND_CONFIG_CLASS::operator=( const TREND_CONFIG_CLASS & sorc )
{
int i;

chart_type          = sorc.chart_type;
nto_average         = sorc.nto_average;
average_over_type   = sorc.average_over_type;
time_period_type    = sorc.time_period_type;
nof_points          = sorc.nof_points;
first_shot          = sorc.first_shot;
last_shot           = sorc.last_shot;
x_axis_type         = sorc.x_axis_type;
need_alarm_limits   = sorc.need_alarm_limits;
need_warning_limits = sorc.need_warning_limits;
need_ucl            = sorc.need_ucl;
need_lcl            = sorc.need_lcl;
ucl_sigma_value     = sorc.ucl_sigma_value;
lcl_sigma_value     = sorc.lcl_sigma_value;

for ( i=0; i<MAX_PARMS; i++ )
    visible[i] = sorc.visible[i];
}

/***********************************************************************
*                         TREND_CONFIG_CLASS                           *
*                             N_VISIBLE                                *
***********************************************************************/
int32 TREND_CONFIG_CLASS::n_visible()
{
int32 i;
int32 count;

count = 0;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] )
        count++;
    }
return count;
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                  ==                                  *
***********************************************************************/
BOOLEAN TREND_CONFIG_CLASS::operator==(const TREND_CONFIG_CLASS & sorc )
{
int i;

if ( chart_type != sorc.chart_type )
    return FALSE;

if ( nto_average != sorc.nto_average )
    return FALSE;

if ( average_over_type != sorc.average_over_type )
    return FALSE;

if ( time_period_type  != sorc.time_period_type )
    return FALSE;

if ( nof_points != sorc.nof_points )
    return FALSE;

if ( first_shot != sorc.first_shot )
    return FALSE;

if ( last_shot != sorc.last_shot  )
    return FALSE;

if ( x_axis_type != sorc.x_axis_type )
    return FALSE;

if ( need_alarm_limits != sorc.need_alarm_limits )
    return FALSE;

if ( need_warning_limits != sorc.need_warning_limits )
    return FALSE;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] != sorc.visible[i] )
        return FALSE;
    }

return TRUE;

}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                  !=                                  *
***********************************************************************/
BOOLEAN TREND_CONFIG_CLASS::operator!=(const TREND_CONFIG_CLASS & sorc )
{
if ( operator==(sorc) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                 READ                                 *
***********************************************************************/
void TREND_CONFIG_CLASS::read( void )
{
INI_CLASS inifile;
int     i;
TCHAR * cp;

if ( PartBasedDisplay )
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );

if ( !inifile.exists() )
    inifile.set_file( ini_file_name(TcConfigIniFile) );

if ( inifile.exists() )
    {
    inifile.set_section( ConfigSectionName );
    /*
    -------------------------------------------------
    If the file has one entry it should have them all
    ------------------------------------------------- */
    if ( inifile.find(ChartTypeKey)  )
        {

        chart_type          = inifile.get_int();
        average_over_type   = inifile.get_int( AverageOverTypeKey  );
        time_period_type    = inifile.get_int( TimePeriodTypeKey   );
        x_axis_type         = inifile.get_int( XAxisTypeKey        );
        nof_points          = inifile.get_int( NOfPointsKey        );
        nto_average         = inifile.get_int( NToAverageKey       );
        first_shot          = inifile.get_int( FirstShotKey        );
        last_shot           = inifile.get_int( LastShotKey         );

        need_alarm_limits   = inifile.get_boolean( NeedAlarmLimitsKey   );
        need_warning_limits = inifile.get_boolean( NeedWarningLimitsKey );
        need_ucl            = inifile.get_boolean( NeedUclLimitKey      );
        need_lcl            = inifile.get_boolean( NeedLclLimitKey      );

        ucl_sigma_value     = inifile.get_double( UclSigmaValueKey );
        lcl_sigma_value     = inifile.get_double( LclSigmaValueKey );

        cp = inifile.get_string( VisibleListKey );
        i = 0;
        while ( i < MAX_PARMS )
            {
            if ( *cp == NullChar )
                break;

            if ( *cp == YChar )
                visible[i] = TRUE;
            else
                visible[i] = FALSE;

            cp++;
            i++;
            }

        /*
        ---------------
        Default to TRUE
        --------------- */
        while ( i < MAX_PARMS )
            {
            visible[i] = TRUE;
            i++;
            }
        return;
        }
    }
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                               WRITE                                  *
***********************************************************************/
void TREND_CONFIG_CLASS::write( void )
{
INI_CLASS inifile;
TCHAR buf[MAX_PARMS+1];
int   i;

if ( PartBasedDisplay )
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
else
    inifile.set_file( ini_file_name(TcConfigIniFile) );

inifile.set_section( ConfigSectionName );

inifile.put_int( ChartTypeKey,       chart_type        );
inifile.put_int( AverageOverTypeKey, average_over_type );
inifile.put_int( TimePeriodTypeKey,  time_period_type  );
inifile.put_int( XAxisTypeKey,       x_axis_type       );
inifile.put_int( NOfPointsKey,       nof_points        );
inifile.put_int( NToAverageKey,      nto_average       );
inifile.put_int( FirstShotKey,       first_shot        );
inifile.put_int( LastShotKey,        last_shot         );

inifile.put_boolean( NeedAlarmLimitsKey,   need_alarm_limits   );
inifile.put_boolean( NeedWarningLimitsKey, need_warning_limits );
inifile.put_boolean( NeedUclLimitKey,      need_ucl            );
inifile.put_boolean( NeedLclLimitKey,      need_lcl            );

inifile.put_double( UclSigmaValueKey, (double) ucl_sigma_value     );
inifile.put_double( LclSigmaValueKey, (double) lcl_sigma_value     );

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] )
        buf[i] = YChar;
    else
        buf[i] = NChar;
    }
buf[MAX_PARMS] = NullChar;
inifile.put_string( VisibleListKey, buf );
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                             PUT_TO_SCREEN                            *
***********************************************************************/
void TREND_SCREEN_CONFIG_CLASS::put_to_screen( HWND w )
{

set_text( w, N_TO_AVG_EBOX, int32toasc(nto_average) );

CheckRadioButton( w, AVG_SHOTS_RADIO,    AVG_MONTHS_RADIO,       AVG_SHOTS_RADIO     + average_over_type    );

CheckRadioButton( w, TREND_RADIO,         HISTOGRAM_RADIO,       TREND_RADIO         + chart_type       );
CheckRadioButton( w, LAST_N_RANGE_RADIO, SHOT_RANGE_RADIO,       LAST_N_RANGE_RADIO + time_period_type );
CheckRadioButton( w, SHOT_X_AXIS_RADIO,  DAY_MONTH_X_AXIS_RADIO, SHOT_X_AXIS_RADIO  + x_axis_type      );

set_text( w, LAST_N_RANGE_EBOX,   int32toasc(nof_points) );
set_text( w, SHOT_RANGE_FROM_EBOX,int32toasc(first_shot) );
set_text( w, SHOT_RANGE_TO_EBOX,  int32toasc(last_shot) );

/*
set_text( w, UCL_SIGMA_EBOX,      ascii_float(ucl_sigma_value) );
set_text( w, LCL_SIGMA_EBOX,      ascii_float(lcl_sigma_value) );
*/

set_checked_state( w, ALARM_LIMITS_CHECKBOX, need_alarm_limits );
set_checked_state( w, WARNING_LIMITS_CHECKBOX, need_warning_limits );

/*
set_checked_state( w, UCL_CHECKBOX, need_ucl );
set_checked_state( w, LCL_CHECKBOX, need_lcl );
*/
}

/***********************************************************************
*                           SAVE_TC_VISIBLE                            *
***********************************************************************/
static void save_tc_visible( HWND w )
{
INT * ip;
int   n;
int   i;
LISTBOX_CLASS lb;

lb.init( w, TC_VISIBLE_LB );
n = lb.get_select_list( &ip );
if ( n > 0 )
    {
    /*
    ----------------------
    Clear all the visibles
    ---------------------- */
    for ( i=0; i<MAX_PARMS; i++ )
        Tc.visible[i] = FALSE;

    for ( i=0; i<n; i++ )
        Tc.visible[ip[i]] = TRUE;

    delete[] ip;
    }

}

/***********************************************************************
*                          FILL_TC_VISIBLE_LB                          *
***********************************************************************/
static void fill_tc_visible_lb( HWND w )
{
int i;
int n;
LISTBOX_CLASS lb;

lb.init( w, TC_VISIBLE_LB );
lb.redraw_off();

n = MAX_PARMS;
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type == NO_PARAMETER_TYPE )
        {
        n = i;
        break;
        }
    lb.add( CurrentParam.parameter[i].name );
    }

for ( i=0; i<n; i++ )
    {
    if ( Tc.visible[i] )
        lb.set_select( i, TRUE );
    }

lb.redraw_on();
}

/***********************************************************************
*                         TC_VISIBLE_DIALOG_PROC                       *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK tc_visible_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        fill_tc_visible_lb( hWnd );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;

            case IDOK:
                save_tc_visible( hWnd );
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         GET_RADIO_BOX_INDEX                          *
***********************************************************************/
static int32 get_radio_box_index( HWND w, int32 first_box, int32 last_box )
{
int32 i;

for ( i=first_box; i<=last_box; i++ )
    if ( is_checked(w, i) )
        return i-first_box;

return 0;
}

/***********************************************************************
*                        CURRENT_PARAMETER_NAME                        *
***********************************************************************/
static TCHAR * current_parameter_name( HWND w )
{
static TCHAR s[PARAMETER_NAME_LEN+1];

INT i;
int     n;
TCHAR * dest;
TCHAR * sorc;
LISTBOX_CLASS lb;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
sorc = lb.item_text( i );
dest = s;
n    = 0;

while ( *sorc != TabChar && *sorc != NullChar && n <= PARAMETER_NAME_LEN  )
    {
    *dest = *sorc;
    dest++;
    sorc++;
    n++;
    }

*dest = NullChar;

return s;
}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry( HWND w )
{
LISTBOX_CLASS  lb;
FIXED_PARAM_CLASS fpc;
int i;

if ( UpdatingEditControls )
    return;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
linechanged.set(i);

get_text( fpc.min_value, w, PLOT_MIN_EBOX, MAX_DOUBLE_LEN );
get_text( fpc.max_value, w, PLOT_MAX_EBOX, MAX_DOUBLE_LEN );

if ( is_checked(w, USE_LIMITS_XBOX) )
    fpc.is_fixed = true;
else
    fpc.is_fixed = false;

lb.replace( fpc.lbline( current_parameter_name(w) ) );
}

/***********************************************************************
*                         DO_NEW_LISTBOX_LINE                          *
***********************************************************************/
static void do_new_listbox_line( HWND w )
{
INT i;
TCHAR * cp;
LISTBOX_CLASS lb;
FIXED_PARAM_CLASS fpc;
BOOLEAN need_check;

UpdatingEditControls = true;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
cp = lb.item_text( i );
fpc.get_from_line( cp );
set_text( w, PLOT_MIN_EBOX, fpc.min_value );
set_text( w, PLOT_MAX_EBOX, fpc.max_value );
if ( fpc.is_fixed )
    need_check = TRUE;
else
    need_check = FALSE;

set_checkbox( w, USE_LIMITS_XBOX, need_check );

UpdatingEditControls = false;
}

/***********************************************************************
*                         FILL_PARAMETER_LIST                          *
***********************************************************************/
static void fill_parameter_list( HWND w )
{
int32 i;
TCHAR       * fname;
LISTBOX_CLASS lb;
INT32_ARRAY_CLASS ia;
FIXED_PARAM_CLASS fpc;

lb.init( w, TREND_PARAMETER_LISTBOX );
lb.redraw_off();

lb.set_tabs( PARAM_COL_TBOX, USE_COL_TBOX );

linechanged.clearall();

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
        {
        fpc.read( CurrentParam.parameter[i].name  );
        lb.add( fpc.lbline(CurrentParam.parameter[i].name) );
        }
    }

fname = 0;
if ( PartBasedDisplay )
    {
    fname = tc_parameter_list_filename(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
    if ( !file_exists(fname) )
        fname = 0;
    }

if ( !fname )
    fname = tc_parameter_list_filename();

ia.read( fname );
while ( ia.next() )
    if ( ia.value() < lb.count() )
        lb.set_select( ia.value(), TRUE );

lb.redraw_on();
do_new_listbox_line( w );
}

/***********************************************************************
*                           GET_FROM_SCREEN                            *
***********************************************************************/
void TREND_SCREEN_CONFIG_CLASS::get_from_screen( HWND w )
{
TCHAR buf[MAX_FLOAT_LEN+1];

average_over_type = get_radio_box_index( w, AVG_SHOTS_RADIO,    AVG_MONTHS_RADIO       );
chart_type        = get_radio_box_index( w, TREND_RADIO,        HISTOGRAM_RADIO        );
time_period_type  = get_radio_box_index( w, LAST_N_RANGE_RADIO, SHOT_RANGE_RADIO       );
x_axis_type       = get_radio_box_index( w, SHOT_X_AXIS_RADIO,  DAY_MONTH_X_AXIS_RADIO );

get_text( buf, w, N_TO_AVG_EBOX, MAX_FLOAT_LEN );
nto_average = asctoint32( buf );

get_text( buf, w, LAST_N_RANGE_EBOX, MAX_FLOAT_LEN );
nof_points = asctoint32( buf );

get_text( buf, w, SHOT_RANGE_FROM_EBOX, MAX_FLOAT_LEN );
first_shot = asctoint32( buf );

get_text( buf, w, SHOT_RANGE_TO_EBOX, MAX_FLOAT_LEN );
last_shot = asctoint32( buf );

/*
get_text( buf, w, UCL_SIGMA_EBOX, MAX_FLOAT_LEN );
ucl_sigma_value = extfloat( buf );
get_text( buf, w, LCL_SIGMA_EBOX, MAX_FLOAT_LEN );
lcl_sigma_value = extfloat( buf );
*/

need_alarm_limits   = is_checked( w, ALARM_LIMITS_CHECKBOX );
need_warning_limits = is_checked( w, WARNING_LIMITS_CHECKBOX );

/*
need_ucl            = is_checked( w, UCL_CHECKBOX );
need_lcl            = is_checked( w, LCL_CHECKBOX );
*/
}

/***********************************************************************
*                            INIT_CONTROLS                             *
***********************************************************************/
static void init_controls( HWND w )
{
/*
------------------------------------------------------------------
Initialize the bits that indicate whether the operator changed any
fixed parameter limits
------------------------------------------------------------------ */
linechanged.init( MAX_PARMS );

Tc.read();
Tc.put_to_screen( w );
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( HWND w )
{
INT   * ip;
INT   * p;
TCHAR * fname;
int32  i;
int32  n;
LISTBOX_CLASS lb;
INT32_ARRAY_CLASS ia;
FIXED_PARAM_CLASS fpc;
WPARAM force_change;

Tc.get_from_screen( w );
Tc.write();

lb.init( w, TREND_PARAMETER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n > 0 )
    {
    if ( ia.init( n ) )
        {
        p = ip;
        while ( ia.next() )
            {
            ia.set( int32(*p) );
            p++;
            }


        if ( PartBasedDisplay )
            fname = tc_parameter_list_filename(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
        else
            fname = tc_parameter_list_filename();

        ia.write( fname );
        }
    delete[] ip;
    }

force_change = 0;
n = lb.count();
for ( i=0; i<n; i++ )
    {
    if ( linechanged[i] )
        {
        force_change = 1;
        fpc.write_from_line( lb.item_text(i) );
        }
    }

linechanged.clearall();

SendMessage( MainWindow, WM_NEW_TC_CONFIG, force_change, 0L );
}

/***********************************************************************
*                        TREND_CONFIG_DIALOG_PROC                      *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK trend_config_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;
int     cmd;
LISTBOX_CLASS lb;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        init_controls( hWnd );
        fill_parameter_list( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case PLOT_MIN_EBOX:
            case PLOT_MAX_EBOX:
                if ( cmd != EN_CHANGE )
                    break;
                refresh_listbox_entry( hWnd );
                return TRUE;

            case USE_LIMITS_XBOX:
                if ( cmd != BN_CLICKED )
                    break;
                refresh_listbox_entry(hWnd);
                return TRUE;

            case CHOOSE_VISIBLE_PB:
                DialogBox(
                    MainInstance,
                    TEXT("TC_VISIBLE_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) tc_visible_dialog_proc );
                return TRUE;

            case NO_TREND_PARAMS_BUTTON:
                lb.init( hWnd, TREND_PARAMETER_LISTBOX );
                lb.unselect_all();
                return TRUE;

            case TREND_PARAMETER_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_new_listbox_line( hWnd );
                    return TRUE;
                    }
                break;

            case IDOK:
            case TREND_CHART_SAVE_CHANGES_BUTTON:
                save_changes( hWnd );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

