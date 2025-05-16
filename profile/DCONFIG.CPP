#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"
#include "fixaxis.h"

#define PRESSURE_STRING 20
#define CHANNEL_STRING  28

TCHAR * resource_string( UINT resource_id );

static HWND MyWindowHandle;
static BITSETYPE PreChannelTypes;
static BITSETYPE PostChannelTypes;
static BITSETYPE PrePressureTypes;
static BITSETYPE PostPressureTypes;

static TCHAR PRE_PRES_CONFIG[]  = TEXT( "PresPreConfig"  );
static TCHAR POST_PRES_CONFIG[] = TEXT( "PresPostConfig" );
static TCHAR PRE_CHAN_CONFIG[]  = TEXT( "ChanPreConfig"  );
static TCHAR POST_CHAN_CONFIG[] = TEXT( "ChanPostConfig" );
static TCHAR IsPresDisplayKey[] = TEXT( "IsPressureDisplay" );
static TCHAR NChar              = TEXT( 'N' );
static TCHAR NString[]          = TEXT( "N" );
static TCHAR NullChar           = TEXT( '\0' );
static TCHAR YChar              = TEXT( 'Y' );
static TCHAR YString[]          = TEXT( "Y" );
static TCHAR EmptyString[]      = TEXT( "" );

static BITSETYPE PressureTypesToCheck[] = {
    HEAD_PRESSURE_CURVE,
    ROD_PRESSURE_CURVE,
    DIFF_PRESSURE_CURVE
    };

const int NofPressureTypesToCheck = sizeof(PressureTypesToCheck)/sizeof(BITSETYPE);

static BITSETYPE ChannelTypesToCheck[] = {
    CHAN_1_ANALOG_CURVE,
    CHAN_2_ANALOG_CURVE,
    CHAN_3_ANALOG_CURVE,
    CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE,
    CHAN_6_ANALOG_CURVE,
    CHAN_7_ANALOG_CURVE,
    CHAN_8_ANALOG_CURVE
    };

const int NofChannelTypesToCheck = sizeof(ChannelTypesToCheck)/sizeof(BITSETYPE);

class FIXED_SCREEN_CLASS : public FIXED_AXIS_CLASS
    {
    public:
    void put_to_screen(   void );
    void get_from_screen( void );
    void set_one_fixed_axis( int32 dest_axis, int32 sorc_index );
    void set_fixed_axes(  void );
    };

static FIXED_SCREEN_CLASS FixedPressures;
static FIXED_SCREEN_CLASS FixedChannels;

static INI_CLASS Ini;

void set_smooth( int32 n, float factor );

/***********************************************************************
*                       SET_IS_PRESSURE_DISPLAY                        *
***********************************************************************/
static void set_is_pressure_display( void )
{
BITSETYPE alltypes;

alltypes = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;

if ( alltypes & PRESSURE_DISPLAY_MASK )
    IsPressureDisplay = TRUE;
else
    IsPressureDisplay = FALSE;
}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                            PUT_TO_SCREEN                             *
***********************************************************************/
void FIXED_SCREEN_CLASS::put_to_screen( void )
{
int32 i;
UINT id;
UINT end_box;
UINT xbox;
UINT min_ebox;
UINT max_ebox;
BOOLEAN is_clickable;

if ( n < 1 )
    return;

if ( is_checked(MyWindowHandle, PRESSURES_RADIO_BUTTON) )
    {
    id = PRESSURE_STRING;
    is_clickable = FALSE;
    }
else
    {
    id = CHANNEL_STRING;
    is_clickable = TRUE;
    }

xbox     = FIX_FTCHAN_1_XBOX;
min_ebox = MIN_FTCHAN_1_EBOX;
max_ebox = MAX_FTCHAN_1_EBOX;
if ( CurrentMachine.is_ftii )
    end_box = FIX_VELOCITY_XBOX;
else
    end_box = FIX_FTCHAN_5_XBOX;

for ( i=0; i<n; i++ )
    {
    if ( xbox < FIX_FTCHAN_4_XBOX )
        {
        set_text( GetDlgItem(MyWindowHandle, xbox), resource_string(id) );
        id++;
        }
    else if ( xbox < end_box )  /*else if ( xbox < FIX_VELOCITY_XBOX )*/
        {
        EnableWindow( GetDlgItem(MyWindowHandle, xbox),     is_clickable );
        EnableWindow( GetDlgItem(MyWindowHandle, min_ebox), is_clickable );
        EnableWindow( GetDlgItem(MyWindowHandle, max_ebox), is_clickable );
        }

    set_checkbox( MyWindowHandle, xbox, f[i].enabled );

    set_text( MyWindowHandle, min_ebox, ascii_float( f[i].min) );
    set_text( MyWindowHandle, max_ebox, ascii_float( f[i].max) );

    xbox++;
    min_ebox++;
    max_ebox++;
    }
}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                           GET_FROM_SCREEN                            *
***********************************************************************/
void FIXED_SCREEN_CLASS::get_from_screen( void )
{
TCHAR   buf[MAX_FLOAT_LEN+1];

UINT    xbox;
UINT    min_ebox;
UINT    max_ebox;

int32   i;
const int32 nof_boxes = FIX_VELOCITY_XBOX - FIX_FTCHAN_1_XBOX + 1;

empty();

if ( !init(nof_boxes) )
    return;

xbox     = FIX_FTCHAN_1_XBOX;
min_ebox = MIN_FTCHAN_1_EBOX;
max_ebox = MAX_FTCHAN_1_EBOX;

for ( i=0; i<nof_boxes; i++ )
    {
    f[i].enabled = is_checked( MyWindowHandle, xbox );

    get_text( buf, MyWindowHandle, min_ebox, MAX_FLOAT_LEN );
    f[i].min = extfloat( buf );

    get_text( buf, MyWindowHandle, max_ebox, MAX_FLOAT_LEN );
    f[i].max = extfloat( buf );

    xbox++;
    min_ebox++;
    max_ebox++;
    }

}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                          SET_ONE_FIXED_AXIS                          *
***********************************************************************/
void FIXED_SCREEN_CLASS::set_one_fixed_axis( int32 dest_axis, int32 sorc_index )
{

if ( sorc_index<0 || sorc_index >= n )
    {
    Axis[dest_axis].is_fixed = FALSE;
    Axis[dest_axis].min = 0.0;
    Axis[dest_axis].max = 0.0;
    return;
    }

Axis[dest_axis].is_fixed = f[sorc_index].enabled;
if ( Axis[dest_axis].is_fixed )
    {
    Axis[dest_axis].min = f[sorc_index].min;
    Axis[dest_axis].max = f[sorc_index].max;
    }

}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                            SET_FIXED_AXES                            *
***********************************************************************/
void FIXED_SCREEN_CLASS::set_fixed_axes( void )
{
int32 axis_index;
int32 fixed_index;
int32 n;

UINT  pre_button;
UINT  post_button;

for ( axis_index=0; axis_index<NOF_AXES; axis_index++ )
    Axis[axis_index].is_fixed = FALSE;

/*
---------------------------------------------------------
Only call this after setting the global IsPressureDisplay
--------------------------------------------------------- */
pre_button  = PRE_CHANNEL_1_XBOX;
post_button = POST_CHANNEL_1_XBOX;
axis_index  = FIRST_ANALOG_AXIS;
fixed_index = 0;

if ( CurrentMachine.is_ftii )
    n = NOF_FTII_ANALOGS;
else
    n = MAX_CONTINUOUS_CHANNELS;

while ( axis_index< NOF_AXES && fixed_index < n )
    {
    /*
    ------------------------------------------------------------------
    The axes are not 1 to 1 with the curves.
    If a curve is not displayed, that axis is used for the next curve.
    ------------------------------------------------------------------ */
    if ( is_checked(MyWindowHandle, pre_button) || is_checked(MyWindowHandle, post_button) )
        {
        set_one_fixed_axis( axis_index, fixed_index );
        axis_index++;
        }
    fixed_index++;
    pre_button++;
    post_button++;
    }

set_one_fixed_axis( VELOCITY_AXIS,     FIXAXIS_VELOCITY_INDEX  );
}

/***********************************************************************
*                           SAVE_ONE_CONFIG                            *
***********************************************************************/
static void save_one_config( TCHAR * key, BITSETYPE b )
{
TCHAR s[5];

insalph( s,  int32(b), 4, TEXT('0'), HEX_RADIX );
s[4] = TEXT( '\0' );

Ini.put_string( key, s );
}

/***********************************************************************
*                    GET_PRESSURE_TYPES_FROM_SCREEN                    *
***********************************************************************/
void get_pressure_types_from_screen( BITSETYPE & dest, int button )
{
int32 i;

dest = 0;

for ( i=0; i<NofPressureTypesToCheck; i++ )
    {
    if ( is_checked(MyWindowHandle, button) )
        dest |= PressureTypesToCheck[i];
    button++;
    }
}

/***********************************************************************
*                    GET_CHANNEL_TYPES_FROM_SCREEN                     *
***********************************************************************/
void get_channel_types_from_screen( BITSETYPE & dest, int button )
{
int32 i;

dest = 0;

for ( i=0; i<NofChannelTypesToCheck; i++ )
    {
    if ( is_checked(MyWindowHandle, button) )
        dest |= ChannelTypesToCheck[i];
    button++;
    }

}

/***********************************************************************
*                    SAVE_SMOOTH_CONFIG_FROM_SCREEN                    *
***********************************************************************/
static void save_smooth_config_from_screen()
{
STRING_CLASS s;
int32 n;
int   id;
int   i;

s.upsize( 10 );

/*
------------------------------------------
Smooth factors are in the display.ini file
------------------------------------------ */
s.get_text( MyWindowHandle, SMOOTH_N_EBOX );
n = s.int_value();
Ini.put_string( SmoothNKey, s.text() );

s.get_text( MyWindowHandle, SMOOTH_MULTIPLIER_EBOX );
Ini.put_string( SmoothFactorKey, s.text() );

set_smooth( n, s.float_value() );
if ( n > 0 )
    Smoothing = TRUE;
else
    Smoothing = FALSE;

s = EmptyString;

id = SMOOTH_P1_XBOX;
for ( i=0; i<(1+SMOOTH_P8_XBOX-SMOOTH_P1_XBOX); i++ )
    {
    SmoothingChannel[i]= is_checked( MyWindowHandle, id );
    if ( SmoothingChannel[i] )
        s += YString;
    else
        s += NString;
    id++;
    }

SmoothingVelocity= is_checked( MyWindowHandle, SMOOTH_VELOCITY_XBOX );
if ( SmoothingVelocity )
    s += YString;
else
    s += NString;

Ini.put_string( SmoothCurvesKey, s.text() );
}

/***********************************************************************
*                             LOAD_SMOOTH                              *
***********************************************************************/
static void load_smooth( BOOLEAN need_to_update_screen )
{
HWND    w;
TCHAR * cp;
float factor;
int32 n;
int   i;
int   id;
STRING_CLASS smooth_n;
STRING_CLASS smoothfactor;

if ( need_to_update_screen )
    {
    smooth_n.upsize( 10 );
    smoothfactor.upsize( 20 );
    smooth_n = EmptyString;
    smoothfactor = EmptyString;
    }

if ( Ini.find(SmoothNKey) )
    {
    if ( need_to_update_screen )
        {
        smooth_n = Ini.get_string();
        n = smooth_n.int_value();
        }
    else
        {
        n = Ini.get_int();
        }
    if ( n > 0 )
        {
        factor = 0.0;
        if ( Ini.find(SmoothFactorKey) )
            {
            if ( need_to_update_screen )
                {
                smoothfactor = Ini.get_string();
                factor = smoothfactor.float_value();
                }
            else
                {
                factor = (float) Ini.get_double();
                }
            }
        set_smooth( n, factor );
        Smoothing = TRUE;
        }
    else
        {
        Smoothing = FALSE;
        }
    }

if ( need_to_update_screen )
    {
    smoothfactor.set_text( MyWindowHandle, SMOOTH_MULTIPLIER_EBOX );
    smooth_n.set_text( MyWindowHandle, SMOOTH_N_EBOX );
    }

for ( i=0; i<(1+SMOOTH_P8_XBOX-SMOOTH_P1_XBOX); i++ )
    SmoothingChannel[i] = FALSE;
SmoothingVelocity = FALSE;

if ( Ini.find(SmoothCurvesKey) )
    {
    cp = Ini.get_string();
    for ( i=0; i<MAX_FT_CHANNELS; i++ )
        {
        if ( cp[i] == YChar )
            SmoothingChannel[i] = TRUE;
        else if ( cp[i] == NullChar )
            break;
        }

    if ( cp[i] == YChar )
        SmoothingVelocity = TRUE;
    }

if ( need_to_update_screen )
    {
    id = SMOOTH_P1_XBOX;
    for ( i=0; i<MAX_FT_CHANNELS; i++ )
        {
        w = GetDlgItem( MyWindowHandle, id );
        set_checkbox( w, SmoothingChannel[i] );
        id++;
        }

    w = GetDlgItem( MyWindowHandle, SMOOTH_VELOCITY_XBOX );
    set_checkbox( w, SmoothingVelocity );
    }
}

/***********************************************************************
*                           SAVE_THIS_CONFIG                           *
***********************************************************************/
static void save_this_config( int id )
{
COMPUTER_CLASS c;
NAME_CLASS s;
INI_CLASS  ini;
int        i;

/*
------------------------------------------
Id is the id of the button that was pushed
------------------------------------------ */
if ( id == SAVE_DEFAULT_DISPLAY_CONFIG_PB )
    {
    s.get_exe_dir_file_name( c.whoami(), 0 );
    }
else
    {
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, 0 );
    if ( !s.create_directory() )
        {
        MessageBox( 0, s.text(), resource_string(CANT_CREATE_DEST_DIR_STRING), MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
        return;
        }
    }

s.cat_path( DisplayIniFile );
Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

if ( is_checked(MyWindowHandle, PRESSURES_RADIO_BUTTON) )
    {
    get_pressure_types_from_screen( PreImpactPlotCurveTypes,  PRE_CHANNEL_1_XBOX );
    get_pressure_types_from_screen( PostImpactPlotCurveTypes, POST_CHANNEL_1_XBOX );
    save_one_config( PRE_PRES_CONFIG,  PreImpactPlotCurveTypes );
    save_one_config( POST_PRES_CONFIG, PostImpactPlotCurveTypes );
    FixedPressures.get_from_screen();
    IsPressureDisplay = TRUE;
    FixedPressures.set_fixed_axes();
    }
else
    {
    get_channel_types_from_screen( PreImpactPlotCurveTypes,  PRE_CHANNEL_1_XBOX );
    get_channel_types_from_screen( PostImpactPlotCurveTypes, POST_CHANNEL_1_XBOX );
    save_one_config( PRE_CHAN_CONFIG,  PreImpactPlotCurveTypes );
    save_one_config( POST_CHAN_CONFIG, PostImpactPlotCurveTypes );
    FixedChannels.get_from_screen();
    IsPressureDisplay = FALSE;
    FixedChannels.set_fixed_axes();
    }

Ini.put_boolean( IsPresDisplayKey, IsPressureDisplay );

if ( id == SAVE_DEFAULT_DISPLAY_CONFIG_PB )
    {
    FixedPressures.write( rootdir_name(FIXPRES_DB) );
    FixedChannels.write(  rootdir_name(FIXCHAN_DB) );
    }
else
    {
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXPRES_DB );
    FixedPressures.write( s.text() );
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXCHAN_DB );
    FixedChannels.write( s.text() );
    }

save_smooth_config_from_screen();
s.get_text( MyWindowHandle, CURVE_PEN_WIDTH_EBOX );
i = s.int_value();
if ( i > 0 )
    {
    CurvePenWidth = i;
    Ini.put_string( CurvePenWidthKey, s.text() );
    }
}

/***********************************************************************
*                           LOAD_ONE_CONFIG                            *
*          The file and section should already be set in ini.          *
***********************************************************************/
static void load_one_config( BITSETYPE & b, TCHAR * key )
{
TCHAR * cp;

b = 0;
if ( Ini.find(key) )
    {
    cp = Ini.get_string();
    b = (BITSETYPE) extlong( cp, 4, HEX_RADIX );
    }
}

/***********************************************************************
*                        LOAD_THE_OTHER_CONFIG                         *
***********************************************************************/
void load_the_other_config()
{
if ( is_checked(MyWindowHandle, PRESSURES_RADIO_BUTTON) )
    {
    load_one_config(  PreChannelTypes,  PRE_CHAN_CONFIG  );
    load_one_config(  PostChannelTypes, POST_CHAN_CONFIG );
    }
else
    {
    load_one_config( PrePressureTypes,  PRE_PRES_CONFIG  );
    load_one_config( PostPressureTypes, POST_PRES_CONFIG );
    }
}

/***********************************************************************
*                       READ_THE_FIXED_PRESSURES                       *
***********************************************************************/
static void read_the_fixed_pressures()
{
COMPUTER_CLASS c;
NAME_CLASS     s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXPRES_DB );
if ( !s.file_exists() )
    s = rootdir_name( FIXPRES_DB );
FixedPressures.read( s.text() );
}

/***********************************************************************
*                       READ_THE_FIXED_CHANNELS                       *
***********************************************************************/
static void read_the_fixed_channels()
{
COMPUTER_CLASS c;
NAME_CLASS     s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXCHAN_DB );
if ( !s.file_exists() )
    s = rootdir_name( FIXCHAN_DB );
FixedChannels.read( s.text() );
}

/***********************************************************************
*                       INIT_GLOBAL_CURVE_TYPES                        *
*                                                                      *
*    This is called by profile at startup and any time the config      *
*    changes, either by toggling the display type or by loading a      *
*    different machine.                                                *
***********************************************************************/
void init_global_curve_types()
{
int       axis_index;
int       fixed_index;
BITSETYPE type_mask;
COMPUTER_CLASS c;
NAME_CLASS     s;
BOOLEAN        using_machine_config;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
using_machine_config = s.file_exists();
if ( !using_machine_config )
    s.get_exe_dir_file_name( c.whoami(), DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

/*
---------------------------
Default to pressure display
--------------------------- */
IsPressureDisplay = Ini.get_boolean( IsPresDisplayKey );

axis_index  = FIRST_ANALOG_AXIS;
fixed_index = 0;

if ( IsPressureDisplay )
    {
    load_one_config( PreImpactPlotCurveTypes,  PRE_PRES_CONFIG  );
    load_one_config( PostImpactPlotCurveTypes, POST_PRES_CONFIG );

    read_the_fixed_pressures();

    type_mask = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;
    while ( axis_index< NOF_AXES && fixed_index < NofPressureTypesToCheck )
        {
        if ( type_mask & PressureTypesToCheck[fixed_index] )
            {
            FixedPressures.set_one_fixed_axis( axis_index, fixed_index );
            axis_index++;
            }
        fixed_index++;
        }
    FixedPressures.set_one_fixed_axis( VELOCITY_AXIS, FIXAXIS_VELOCITY_INDEX );
    FixedPressures.empty();
    }
else
    {
    load_one_config( PreImpactPlotCurveTypes,  PRE_CHAN_CONFIG  );
    load_one_config( PostImpactPlotCurveTypes, POST_CHAN_CONFIG );

    read_the_fixed_channels();

    type_mask = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;
    while ( axis_index< NOF_AXES && fixed_index < NofChannelTypesToCheck )
        {
        if ( type_mask & ChannelTypesToCheck[fixed_index] )
            {
            FixedChannels.set_one_fixed_axis( axis_index, fixed_index );
            axis_index++;
            }
        fixed_index++;
        }
    FixedChannels.set_one_fixed_axis( VELOCITY_AXIS, FIXAXIS_VELOCITY_INDEX );
    FixedChannels.empty();
    }

load_smooth( FALSE );

if ( Ini.find(CurvePenWidthKey) )
    CurvePenWidth = Ini.get_int();

Ini.cleanup();
}

/***********************************************************************
*                     TOGGLE_PRESSURE_DISPLAY_TYPE                     *
***********************************************************************/
void toggle_pressure_display_type()
{
BOOLEAN  b;
COMPUTER_CLASS c;
NAME_CLASS s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
if ( !s.file_exists() )
    s.get_exe_dir_file_name( c.whoami(), DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

/*
---------------------------
Default to pressure display
--------------------------- */
IsPressureDisplay = Ini.get_boolean( IsPresDisplayKey );

b = TRUE;
if ( IsPressureDisplay )
    b = FALSE;

Ini.put_boolean( IsPresDisplayKey, b );
init_global_curve_types();
}

/***********************************************************************
*                    CHECK_ONE_SET_OF_CHANNEL_BUTTONS                  *
***********************************************************************/
static void check_one_set_of_channel_buttons( BITSETYPE pressure_type, int button )
{
int     i;
int     n;
UINT    id;
BOOLEAN is_set;
HWND    w;

if ( CurrentMachine.is_ftii )
    n = NOF_FTII_ANALOGS;
else
    n = 4;

id = CHANNEL_STRING;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( MyWindowHandle, button );

    if ( i < NofPressureTypesToCheck )
        {
        set_text( w, resource_string(id) );
        id++;
        }
    else
        {
        if ( !IsWindowEnabled(w) )
            EnableWindow( w, TRUE );
        }

    if ( pressure_type & ChannelTypesToCheck[i] )
        is_set = TRUE;
    else
        is_set = FALSE;
    set_checkbox( w, is_set );
    button++;
    }

}

/***********************************************************************
*                      CHECK_THE_CHANNEL_BUTTONS                       *
***********************************************************************/
static void check_the_channel_buttons( void )
{
check_one_set_of_channel_buttons( PreChannelTypes,  PRE_CHANNEL_1_XBOX  );
check_one_set_of_channel_buttons( PostChannelTypes, POST_CHANNEL_1_XBOX );
}

/***********************************************************************
*                    CHECK_ONE_SET_OF_PRESSURE_BUTTONS                 *
***********************************************************************/
static void check_one_set_of_pressure_buttons( BITSETYPE pressure_type, int button )
{
int     i;
int     n;
UINT    id;
BOOLEAN is_set;
HWND     w;

if ( CurrentMachine.is_ftii )
    n = NOF_FTII_ANALOGS;
else
    n = 4;

id = PRESSURE_STRING;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( MyWindowHandle, button );

    if ( i < NofPressureTypesToCheck )
        {
        set_text( w, resource_string(id) );
        id++;
        if ( pressure_type & PressureTypesToCheck[i] )
            is_set = TRUE;
        else
            is_set = FALSE;

        set_checkbox( w, is_set );
        }
    else
        {
        set_checkbox( w, FALSE );
        if ( IsWindowEnabled(w) )
            EnableWindow( w, FALSE );
        }

    button++;
    }

}

/***********************************************************************
*                      CHECK_THE_PRESSURE_BUTTONS                      *
***********************************************************************/
static void check_the_pressure_buttons( void )
{
check_one_set_of_pressure_buttons( PrePressureTypes,  PRE_CHANNEL_1_XBOX  );
check_one_set_of_pressure_buttons( PostPressureTypes, POST_CHANNEL_1_XBOX );
}

/***********************************************************************
*                             INIT_CHOICES                             *
***********************************************************************/
static void init_choices()
{
BITSETYPE my_curve_types;
int       my_radio_button;
COMPUTER_CLASS c;
NAME_CLASS     s;

read_the_fixed_pressures();
read_the_fixed_channels();

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
if ( !s.file_exists() )
    s.get_exe_dir_file_name( c.whoami(), DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

my_curve_types = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;

if ( my_curve_types & PRESSURE_DISPLAY_MASK )
    {
    PrePressureTypes  = PreImpactPlotCurveTypes;
    PostPressureTypes = PostImpactPlotCurveTypes;
    my_radio_button   = PRESSURES_RADIO_BUTTON;
    check_the_pressure_buttons();
    }
else
    {
    PreChannelTypes  = PreImpactPlotCurveTypes;
    PostChannelTypes = PostImpactPlotCurveTypes;
    my_radio_button  = CHANNELS_RADIO_BUTTON;
    check_the_channel_buttons();
    }

CheckRadioButton( MyWindowHandle, PRESSURES_RADIO_BUTTON, CHANNELS_RADIO_BUTTON, my_radio_button );
load_the_other_config();

if ( my_radio_button == PRESSURES_RADIO_BUTTON )
    FixedPressures.put_to_screen();
else
    FixedChannels.put_to_screen();

load_smooth( TRUE );

if ( Ini.find(CurvePenWidthKey) )
    s = Ini.get_string();
else
    s = int32toasc( CurvePenWidth );

s.set_text( MyWindowHandle, CURVE_PEN_WIDTH_EBOX );

Ini.cleanup();
}

/***********************************************************************
*                            SET_ALL_CURVES                            *
***********************************************************************/
static void set_all_curves( void )
{
int  i;
HWND w;

for ( i=PRE_CHANNEL_1_XBOX; i<=POST_CHANNEL_4_XBOX; i++ )
    {
    w = GetDlgItem( MyWindowHandle, i );

    if ( IsWindowEnabled(w) )
        set_checkbox( w, TRUE );
    }
}

/***********************************************************************
*                  DISPLAY_CONFIGURATION_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK display_configuration_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindowHandle = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        init_choices();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_CHANNELS_BUTTON:
                set_all_curves();
                return TRUE;

            case CHANNELS_RADIO_BUTTON:
                get_pressure_types_from_screen( PrePressureTypes,  PRE_CHANNEL_1_XBOX );
                get_pressure_types_from_screen( PostPressureTypes, POST_CHANNEL_1_XBOX );
                check_the_channel_buttons();
                FixedPressures.get_from_screen();
                FixedChannels.put_to_screen();
                return FALSE;

            case PRESSURES_RADIO_BUTTON:
                get_channel_types_from_screen( PreChannelTypes,  PRE_CHANNEL_1_XBOX );
                get_channel_types_from_screen( PostChannelTypes, POST_CHANNEL_1_XBOX );
                check_the_pressure_buttons();
                FixedChannels.get_from_screen();
                FixedPressures.put_to_screen();
                return FALSE;

            case SAVE_DEFAULT_DISPLAY_CONFIG_PB:
                save_this_config( id );
                return TRUE;

            case IDOK:
                save_this_config( id );
                HaveHeader = FALSE;

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                FixedPressures.empty();
                FixedChannels.empty();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        DialogIsActive = FALSE;
        break;
    }

return FALSE;
}

