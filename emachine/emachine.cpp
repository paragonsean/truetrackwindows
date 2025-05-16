#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\nameclas.h"
#include "..\include\ftii.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\verrors.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#include "resource.h"

#define _MAIN_
#include "..\include\events.h"

struct MACHINE_ENTRY
    {
    MACHINE_CLASS machine;
    BOOLEAN       has_changed;
    };

short           NofMachines    = 0;
short           CurrentMachine = 0;
MACHINE_ENTRY * Machines       = 0;

HINSTANCE MainInstance;
HWND   MainWindow;
HWND   MachSetupWindow;
HWND   NewMachineWindow    = 0;
HWND   EditEquationsWindow = 0;

static BOOLEAN HaveSureTrakControl = FALSE;
static BOOLEAN HavePressureControl = FALSE;
static BOOLEAN HaveDigitalServoAmp = FALSE;  // Set by show_machine

static TCHAR DisplayIniFile[]    = TEXT( "display.ini" );
static TCHAR VisiTrakIniFile[]   = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]     = TEXT( "Config" );
static TCHAR ChanPreConfigKey[]  = TEXT( "ChanPreConfig"  );
static TCHAR ChanPostConfigKey[] = TEXT( "ChanPostConfig" );
static TCHAR CurrentChannelModeKey[]  = TEXT( "CurrentChannelMode" );
static TCHAR HavePressureControlKey[] = TEXT( "HavePressureControl" );
static TCHAR HaveSureTrakKey[]   = TEXT( "HaveSureTrak" );
static TCHAR YChar               = TEXT( 'Y' );

short NofDCurves;
short * DCurves;

static  WNDPROC OldEditProc;

TCHAR MyClassName[]   = TEXT("EMachine");
STRING_CLASS MyWindowTitle;
TCHAR MyIconName[] = TEXT("EMACHINE_ICON" );

/*
---------------------------------------------
Global Variables for choose_servo_dialog_proc
--------------------------------------------- */
static HANDLE Mvo_06596_Bmp     = 0;
static HANDLE Digital_Servo_Bmp = 0;

BOOLEAN copymachine( TCHAR * computer, TCHAR * machine, MACHINE_CLASS & sorc );
BOOL CALLBACK edit_dcurve_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( void )
{
DWORD context;
HWND  w;

w = GetActiveWindow();
if ( w == NewMachineWindow )
    context = NEW_MACHINE_HELP;
else if ( w == EditEquationsWindow )
    context = EDIT_DIFF_EQUATIONS_HELP;
else
    context = MACHINE_EDIT_OVERVIEW_HELP;

gethelp( HELP_CONTEXT, context );
}

/***********************************************************************
*                         CHECK_CURRENT_MACHINE                        *
***********************************************************************/
inline BOOLEAN check_current_machine( void )
{
if ( CurrentMachine >= 0 && CurrentMachine < NofMachines )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        FILL_COMPUTERS                                *
***********************************************************************/
void fill_computers( void )
{
LISTBOX_CLASS  lb;
COMPUTER_CLASS c;

lb.init( NewMachineWindow, COMPUTER_CBOX );
lb.empty();
lb.redraw_off();

c.rewind();
while( c.next() )
    lb.add( c.name() );

lb.setcursel( c.whoami() );
lb.redraw_on();
}

/***********************************************************************
*                            GET_NEW_TYPE                              *
***********************************************************************/
void get_new_type( void )
{

MACHINE_CLASS * m;

if ( check_current_machine() )
    {
    m = &Machines[CurrentMachine].machine;
    m->type &= ~(COLD_CHAMBER_TYPE | HOT_CHAMBER_TYPE );

    if ( IsDlgButtonChecked(MachSetupWindow, HOT_CHAMBER_RADIO_BUTTON) == BST_CHECKED )
        m->type |= HOT_CHAMBER_TYPE;
    else
        m->type |= COLD_CHAMBER_TYPE;

    Machines[CurrentMachine].has_changed = TRUE;
    }
}

/***********************************************************************
*                            GET_NEW_PITCH                             *
***********************************************************************/
void get_new_pitch( void )
{

MACHINE_CLASS * m;

if ( CurrentMachine < NofMachines )
    {
    m = &Machines[CurrentMachine].machine;

    /*
    ----------------------
    Get selected rod pitch
    ---------------------- */
    get_cb_select( MachSetupWindow, ROD_PITCH_COMBO_BOX, m->rodpitch );
    Machines[CurrentMachine].has_changed = TRUE;
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( void )
{
static TCHAR digital_curves[]  = TEXT( "1400" );
static TCHAR external_curves[] = TEXT( "3000" );

int             i;
INI_CLASS       ini;
TCHAR           c;
TCHAR         * cp;
MACHINE_CLASS * m;
BITSETYPE       pressure_control_mask;
BOOLEAN         have_digital_servo_amp_now;
NAME_CLASS      s;

pressure_control_mask = MA_HAS_PRESSURE_CONTROL;

m = &Machines[CurrentMachine].machine;
if ( m->monitor_flags & pressure_control_mask )
    {
    if ( !HavePressureControl )
        {
        m->monitor_flags &= ~pressure_control_mask;
        Machines[CurrentMachine].has_changed = TRUE;
        }
    }
else if ( HavePressureControl )
    {
    m->monitor_flags |= pressure_control_mask;
    Machines[CurrentMachine].has_changed = TRUE;
    }

if ( m->monitor_flags & MA_HAS_DIGITAL_SERVO )
    have_digital_servo_amp_now = TRUE;
else
    have_digital_servo_amp_now = FALSE;

if (have_digital_servo_amp_now != HaveDigitalServoAmp )
    {
    HaveDigitalServoAmp = have_digital_servo_amp_now;
    if ( HaveDigitalServoAmp )
        cp = digital_curves;
    else
        cp = external_curves;
    put_ini_string( DisplayIniFile, ConfigSection, ChanPreConfigKey, cp );
    put_ini_string( DisplayIniFile, ConfigSection, ChanPostConfigKey, cp );
    }
i = CHANNEL_MODE_7;
s.get_ft2_editor_settings_file_name( m->computer, m->name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( s.file_exists() )
    {
    if ( ini.find(CurrentChannelModeKey) )
        {
        s = ini.get_string();
        c = *s.text();
        if ( is_numeric(c) )
            i = s.int_value();
        }
    }
    
if ( i > CHANNEL_MODE_1 )
    {
    if ( HaveDigitalServoAmp )
        s = CHANNEL_MODE_7;
    else
        s = CHANNEL_MODE_5;
    ini.put_string( CurrentChannelModeKey, s.text() );
    }
 
for ( i=0; i<NofMachines; i++ )
    {
    if ( Machines[i].has_changed )
        {
        Machines[i].machine.save();
        Machines[i].has_changed = FALSE;
        poke_data( DDE_CONFIG_TOPIC,  ItemList[MACH_SETUP_INDEX].name,    Machines[i].machine.name );
        if ( Machines[i].machine.monitor_flags & MA_MONITORING_ON )
            poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, Machines[i].machine.name );
        }
    }
}

/***********************************************************************
*                     SET_DIFF_BUTTON_ENABLE_STATE                     *
***********************************************************************/
void set_diff_button_enable_state( short dcurve_number )
{
BOOL    is_enabled;
TCHAR   buf[MAX_STRING_LEN+1];

if ( dcurve_number == NO_DIFF_CURVE_NUMBER )
    is_enabled = FALSE;
else
    is_enabled = TRUE;

EnableWindow( GetDlgItem(MachSetupWindow,DIFF_PRESS_RADIO_BUTTON), is_enabled );
if ( !is_enabled && IsDlgButtonChecked(MachSetupWindow, DIFF_PRESS_RADIO_BUTTON) == BST_CHECKED )
    {
    LoadString( MainInstance, NEED_NEW_PARM_TYPE_STRING, buf, sizeof(buf) );
    MessageBox( NULL, buf, TEXT(""),  MB_OK | MB_ICONWARNING );
    }

}

/***********************************************************************
*                        SHOW_ADV_SETUP                                *
***********************************************************************/
void show_adv_setup( void )
{
short i;
short on_button;
MACHINE_CLASS * m;

if ( !check_current_machine() )
    return;

m = &Machines[CurrentMachine].machine;

if ( m->impact_pres_type[POST_IMPACT_INDEX] & ROD_PRESSURE_CURVE )
    on_button = ROD_PRESS_RADIO_BUTTON;

else if ( m->impact_pres_type[POST_IMPACT_INDEX] & HEAD_PRESSURE_CURVE )
    on_button = HEAD_PRESS_RADIO_BUTTON;

else
    on_button = DIFF_PRESS_RADIO_BUTTON;

CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, on_button );

SetDlgItemInt( MachSetupWindow, POS_IMPACT_POINTS_BOX,  m->impact_points[PRE_IMPACT_INDEX],  TRUE );
SetDlgItemInt( MachSetupWindow, TIME_IMPACT_POINTS_BOX, m->impact_points[POST_IMPACT_INDEX], TRUE );
SetDlgItemInt( MachSetupWindow, CYCLE_TIMEOUT_BOX,      m->cycle_timeout_seconds,            TRUE );
SetDlgItemInt( MachSetupWindow, AUTOSHOT_TIMEOUT_BOX,   m->autoshot_timeout_seconds,         TRUE );

for ( i=0; i<NofDCurves; i++)
   {
   /*
   ---------------------------------
   Find CB index for this curve type
   --------------------------------- */
   if ( DCurves[i] == m->diff_curve_number )
      {
      /*
      -----------------------------
      Select diff curve description
      ----------------------------- */
      SendDlgItemMessage( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX, CB_SETCURSEL, i, 0L);
      set_diff_button_enable_state( DCurves[i] );
      break;
      }
   }

}

/***********************************************************************
*                         SHOW_SERVO_CONTROLS                          *
***********************************************************************/
static void show_servo_controls()
{
BOOL    is_enabled;
int     on_button;
MACHINE_CLASS * m;

m = &Machines[CurrentMachine].machine;

if ( m->suretrak_controlled )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

EnableWindow( GetDlgItem(MachSetupWindow, MVO_06596_RADIO), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, DIGITAL_SERVO_RADIO), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, SERVO_AMP_BOARD_TYPE_STATIC), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, CHOOSE_SERVO_PB), is_enabled );

if ( is_enabled && (m->monitor_flags & MA_HAS_DIGITAL_SERVO) )
    on_button = DIGITAL_SERVO_RADIO;
else
    on_button = MVO_06596_RADIO;

CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, on_button );
}

/***********************************************************************
*                            SHOW_MACHINE                              *
***********************************************************************/
static void show_machine()
{
int     on_button;
MACHINE_CLASS * m;
BOOLEAN         b;
WINDOW_CLASS    w;

if ( !check_current_machine() )
    return;

m = &Machines[CurrentMachine].machine;

set_current_cb_item( MachSetupWindow, ROD_PITCH_COMBO_BOX, m->rodpitch );

if ( m->type & COLD_CHAMBER_TYPE )
    on_button  = COLD_CHAMBER_RADIO_BUTTON;
else
    on_button  = HOT_CHAMBER_RADIO_BUTTON;

CheckRadioButton( MachSetupWindow, HOT_CHAMBER_RADIO_BUTTON, COLD_CHAMBER_RADIO_BUTTON, on_button );

set_checkbox( MachSetupWindow, SURETRAK2_XBOX, m->suretrak_controlled );

w = MachSetupWindow;
w = w.control( MEAS_BISCUIT_AT_DELAY_XBOX );
if ( m->is_ftii )
    {
    if ( !w.is_enabled() )
        w.enable();
    }

b = FALSE;
if ( m->is_ftii && (m->monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT) )
    b = TRUE;
set_checkbox( MachSetupWindow, MEAS_BISCUIT_AT_DELAY_XBOX, b );

if ( !m->is_ftii )
    {
    if ( w.is_enabled() )
        w.disable();
    }

show_servo_controls();

/*
-----------------------------------------------------------------------------------
Save the initial setting of the servo amp board type. If this changes then I should
change the curves displayed on the plot screen.
----------------------------------------------------------------------------------- */
if ( m->monitor_flags & MA_HAS_DIGITAL_SERVO )
    HaveDigitalServoAmp = TRUE;
else
    HaveDigitalServoAmp = FALSE;

show_adv_setup();
}

/***********************************************************************
*                           CLEANUP_MACHINES                           *
***********************************************************************/
static void cleanup_machines()
{

if ( Machines )
    {
    delete[] Machines;
    Machines    = 0;
    NofMachines = 0;
    }
}

/***********************************************************************
*                            CLEANUP_DCURVES                           *
***********************************************************************/
void cleanup_dcurves( void )
{

if ( DCurves )
    {
    delete[] DCurves;
    DCurves    = 0;
    NofDCurves = 0;
    }

}

/***********************************************************************
*                            FILL_MACHINES                             *
***********************************************************************/
void fill_machines( TCHAR * current_machine_name )
{
short   i;
DB_TABLE t;
MACHINE_ENTRY * m;
BOOLEAN have_current_machine;
TCHAR   mymachine[MACHINE_NAME_LEN+1];

LISTBOX_CLASS lb;
COMPUTER_CLASS c;

if ( current_machine_name )
    lstrcpy( mymachine, current_machine_name );
else if ( CurrentMachine >= 0 && CurrentMachine < NofMachines )
    lstrcpy( mymachine, Machines[CurrentMachine].machine.name );
else
    lstrcpy( mymachine, NO_MACHINE );

lb.init( MachSetupWindow, MACHINE_LIST_BOX );
lb.empty();
lb.redraw_off();

cleanup_machines();

NofMachines = 0;
c.rewind();
while( c.next() )
    {
    if ( c.is_present() )
        {
        if ( t.open( machset_dbname(c.name()), MACHSET_RECLEN, PFL) )
            {
            NofMachines += t.nof_recs();
            t.close();
            }
        }
    }

if ( NofMachines > 0 )
    {
    Machines = new MACHINE_ENTRY[NofMachines];
    }

i = 0;
m = Machines;

c.rewind();
while( c.next() )
    {
    if ( c.is_present() )
        {
        if ( t.open( machset_dbname(c.name()), MACHSET_RECLEN, PFL) )
            {
            while ( t.get_next_record(FALSE) && i < NofMachines )
                {
                lstrcpy( m->machine.computer, c.name() );
                t.get_alpha( m->machine.name, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
                i++;
                m++;
                }
            t.close();
            }
        }
    }

NofMachines = i;

have_current_machine = FALSE;
m = Machines;
for ( i=0; i<NofMachines; i++ )
    {
    m->machine.find( m->machine.computer, m->machine.name );
    lb.add( m->machine.name );
    if ( lstrcmp(m->machine.name, mymachine) == 0 )
        {
        CurrentMachine = i;
        have_current_machine = TRUE;
        }
    m++;
    }

if ( !have_current_machine )
    CurrentMachine = 0;

if ( NofMachines )
    lb.setcursel( Machines[CurrentMachine].machine.name );

lb.redraw_on();
show_machine();
}

/***********************************************************************
*                            FILL_ROD_PITCHES                          *
***********************************************************************/
void fill_rod_pitches( void )
{
DB_TABLE t;
TCHAR s[RODPITCH_NAME_LEN+1];
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, ROD_PITCH_COMBO_BOX );
lb.empty();
lb.redraw_off();

if ( t.open(rodpitch_dbname(), RODPITCH_RECLEN, PFL) )
    {
    while ( t.get_next_record(FALSE) )
        {
        t.get_alpha( s, RODPITCH_NAME_OFFSET, RODPITCH_NAME_LEN );
        lb.add( s );
        }
    t.close();
    }

lb.redraw_on();
}

/***********************************************************************
*                             ADD_MACHINE                              *
***********************************************************************/
void add_machine( TCHAR * machine )
{
TCHAR computer[COMPUTER_NAME_LEN+1];

if ( !check_current_machine() )
    return;

if ( get_cb_select(NewMachineWindow, COMPUTER_CBOX, computer) )
    copymachine( computer, machine, Machines[CurrentMachine].machine );

fill_machines( machine );
}

/***********************************************************************
*                            DELETE_MACHINE                            *
***********************************************************************/
void delete_machine( void )
{
INT      answer_button;
BOOLEAN  need_update;
TCHAR    machine[MACHINE_NAME_LEN+1];
TCHAR  * computer;
DB_TABLE t;
MACHINE_CLASS * m;

need_update = FALSE;

if ( NofMachines < 2 )
    {
    resource_message_box( MainInstance, LAST_MACHINE_STRING, UNABLE_TO_COMPLY_STRING, MB_OK );
    return;
    }

/*
----------------------------------------------------------
Don't allow deletion of a machine that might be monitoring
---------------------------------------------------------- */
m = &Machines[CurrentMachine].machine;
m->refresh_monitor_flags();
if ( m->monitor_flags & MA_MONITORING_ON )
    {
    resource_message_box( MainInstance, MONITORING_OFF_FIRST_STRING, UNABLE_TO_COMPLY_STRING, MB_OK );
    return;
    }

answer_button = resource_message_box( MainInstance, CONTINUE_STRING, ABOUT_TO_DELETE_STRING, MB_OKCANCEL | MB_ICONWARNING );
if ( answer_button != IDOK )
    return;

if ( !check_current_machine() )
    return;

lstrcpy( machine, Machines[CurrentMachine].machine.name );
computer = Machines[CurrentMachine].machine.computer;

if ( t.open( machset_dbname(computer), MACHSET_RECLEN, WL ) )
   {
   t.put_alpha(MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN);
   t.reset_search_mode();
   if ( t.get_next_key_match(1, FALSE) )
      {
      t.rec_delete();
      need_update = TRUE;
      }
   t.close();
   }

if ( need_update )
    {
    kill_directory( codir_name(computer, machine) );
    fill_machines(NULL);
    }

poke_data( DDE_CONFIG_TOPIC, ItemList[MACH_SETUP_INDEX].name, machine );
}

/***********************************************************************
*                              FILL_DCURVES                            *
***********************************************************************/
void fill_dcurves( void )
{
short i;
TCHAR s[DCURVE_DESC_LEN];
DB_TABLE t;
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX );
lb.empty();
lb.redraw_off();

cleanup_dcurves();

if ( t.open( diffcurve_dbname (), DCURVE_RECLEN, PFL ) )
    {
    NofDCurves = t.nof_recs();
    if ( NofDCurves > 0 )
        {
            DCurves = new short[NofDCurves];
        }

    if ( NofDCurves > 0 )
        {
        i = 0;
        while ( t.get_next_record(FALSE) && i < NofDCurves )
            {
            DCurves[i] = t.get_short( DCURVE_CURVE_NUMBER_OFFSET );
            i++;
            t.get_alpha( s, DCURVE_DESC_OFFSET, DCURVE_DESC_LEN );
            lb.add( s );
            }
        }
   t.close();
   }

lb.setcursel( 0 );
lb.redraw_on();
}

/***********************************************************************
*                            UPDATE_DCURVE                             *
***********************************************************************/
void update_dcurve( void )
{
LRESULT x;
short   i;

if ( !check_current_machine() )
    return;

x = SendDlgItemMessage( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX, CB_GETCURSEL, 0, 0L );
if ( x != CB_ERR )
    {
    i = short( x );
    if ( i >= 0 && i < NofDCurves )
        {
        Machines[CurrentMachine].machine.diff_curve_number = DCurves[i];
        Machines[CurrentMachine].has_changed = TRUE;
        set_diff_button_enable_state( i );
        }
    }
}

/***********************************************************************
*                         POSITION_ADV_SETUP                           *
***********************************************************************/
void position_adv_setup( void )
{

RECT  r;
long  x;
long  y;

GetWindowRect( MainWindow, &r );
x = r.right;
y = r.top;

if ( MachSetupWindow )
    {
    GetWindowRect( MachSetupWindow, &r );
    MoveWindow( MachSetupWindow, x, y, r.right-r.left, r.bottom-r.top, TRUE );
    }

}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
const INT NOF_CONTROLS = 7;
const INT NOF_STATIC_CONTROLS = 7;

static UINT myid[] = {
    MACHINE_LIST_BOX,
    HOT_CHAMBER_RADIO_BUTTON,
    COLD_CHAMBER_RADIO_BUTTON,
    DIFF_CURVE_TYPE_COMBO_BOX,
    ROD_PITCH_COMBO_BOX,
    SURETRAK2_XBOX,
    MEAS_BISCUIT_AT_DELAY_XBOX };

static UINT static_id[] = {
    MACHINE_NUMBER_STATIC,
    ROD_PITCH_STATIC,
    MACHINE_TYPE_STATIC,
    DIFF_CURVE_TYPE_STATIC,
    TIMEOUTS_STATIC,
    IMPACT_POINTS_STATIC,
    CALC_PARAMS_STATIC };

static DWORD mycontext[] = {
    MACHINE_NUMBER_HELP,
    MACHINE_TYPE_HELP,
    MACHINE_TYPE_HELP,
    DIFF_CURVE_TYPE_HELP,
    ROD_PITCH_HELP,
    SURETRAK2_XBOX_HELP,
    MEAS_BISCUIT_AT_DELAY_XBOX_HELP };

static DWORD static_context[] = {
    MACHINE_NUMBER_HELP,
    ROD_PITCH_HELP,
    MACHINE_TYPE_HELP,
    DIFF_CURVE_TYPE_HELP,
    TIMEOUTS_HELP,
    IMPACT_POINTS_HELP,
    CALC_PARAMS_HELP };


INT   i;
POINT p;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(MachSetupWindow, static_id[i] );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_context[i] );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(MachSetupWindow, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

gethelp( HELP_CONTEXT, MACHINE_EDIT_OVERVIEW_HELP );
return TRUE;
}

/***********************************************************************
*                         GET_SELECTED_MACHINE                         *
***********************************************************************/
static void get_selected_machine( void )
{
short         i;
TCHAR       * cp;
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, MACHINE_LIST_BOX );
cp = lb.selected_text();

for ( i=0; i<NofMachines; i++ )
    {
    /*
    -----------------------------------------------
    The first thing on the line is the machine name
    ----------------------------------------------- */
    if ( strings_are_equal(cp, Machines[i].machine.name, MACHINE_NAME_LEN) )
        {
        CurrentMachine = i;
        show_machine();
        }
    }

}

/***********************************************************************
*                     END_CHOOSE_SERVO_DIALOG_PROC                     *
***********************************************************************/
void end_choose_servo_dialog_proc( HWND hWnd )
{
HWND w;

if ( Mvo_06596_Bmp )
    {
    w = GetDlgItem( hWnd, MVO_06596_PB );
    SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) 0 );
    DeleteObject( Mvo_06596_Bmp );
    Mvo_06596_Bmp = 0;
    }

if ( Digital_Servo_Bmp )
    {
    w = GetDlgItem( hWnd, DIGITAL_SERVO_PB );
    SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) 0 );
    DeleteObject( Digital_Servo_Bmp );
    Digital_Servo_Bmp = 0;
    }

EndDialog( hWnd, 0 );
}

/***********************************************************************
*                       CHOOSE_SERVO_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK choose_servo_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int   id;
HWND  w;

id  = LOWORD( wParam );
switch (msg)
    {
    case WM_INITDIALOG:
        w = GetDlgItem( hWnd, MVO_06596_PB );
        Mvo_06596_Bmp   = LoadBitmap( MainInstance, TEXT("Mvo_06596_Bmp") );
        SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) Mvo_06596_Bmp );
        w = GetDlgItem( hWnd, DIGITAL_SERVO_PB );
        Digital_Servo_Bmp   = LoadBitmap( MainInstance, TEXT("Digital_Servo_Bmp") );
        SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) Digital_Servo_Bmp );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case MVO_06596_PB:
                CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, MVO_06596_RADIO );
                Machines[CurrentMachine].machine.monitor_flags &= ~MA_HAS_DIGITAL_SERVO;
                Machines[CurrentMachine].has_changed = TRUE;
                end_choose_servo_dialog_proc( hWnd );
                break;

            case DIGITAL_SERVO_PB:
                CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, DIGITAL_SERVO_RADIO );
                Machines[CurrentMachine].machine.monitor_flags |= MA_HAS_DIGITAL_SERVO;
                Machines[CurrentMachine].has_changed = TRUE;
                end_choose_servo_dialog_proc( hWnd );
                break;

            case CANCEL_BUTTON:
                end_choose_servo_dialog_proc( hWnd );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             MachSetupProc                            *
***********************************************************************/
BOOL CALLBACK MachSetupProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   i;
int   id;
int   cmd;
MACHINE_CLASS * m;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );
m   = 0;

switch (msg)
    {
    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_INITDIALOG:
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_NEWSETUP:
        fill_dcurves();
        show_adv_setup();
        return TRUE;

    case WM_DBINIT:
        for ( i=0; i<4; i++ )
            SendDlgItemMessage( MachSetupWindow, POS_IMPACT_POINTS_BOX+i, EM_LIMITTEXT, 5, 0L );
        return TRUE;

    case WM_COMMAND:
        if ( check_current_machine() )
            m = &Machines[CurrentMachine].machine;

        switch ( id )
            {
            case MACHINE_LIST_BOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    get_selected_machine();
                    }
                break;

            case ROD_PITCH_COMBO_BOX:
                if ( cmd == LBN_SELCHANGE )
                    get_new_pitch();
                break;

            case HOT_CHAMBER_RADIO_BUTTON:
            case COLD_CHAMBER_RADIO_BUTTON:
                get_new_type();
                break;

            case DIFF_CURVE_TYPE_COMBO_BOX:
                if ( cmd == LBN_SELCHANGE )
                    update_dcurve();
                break;

            case SURETRAK2_XBOX:
                if ( m )
                    {
                    m->suretrak_controlled = is_checked( hWnd, SURETRAK2_XBOX );
                    show_servo_controls();
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case MEAS_BISCUIT_AT_DELAY_XBOX:
                if ( m )
                    {
                    if ( is_checked(hWnd, MEAS_BISCUIT_AT_DELAY_XBOX) )
                        m->monitor_flags |= MA_USE_TIME_TO_MEASURE_BISCUIT;
                    else
                        m->monitor_flags &= ~MA_USE_TIME_TO_MEASURE_BISCUIT;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case MVO_06596_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    m->monitor_flags &= ~MA_HAS_DIGITAL_SERVO;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case DIGITAL_SERVO_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    m->monitor_flags |= MA_HAS_DIGITAL_SERVO;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case CHOOSE_SERVO_PB:
                DialogBox(
                    MainInstance,
                    TEXT("CHOOSE_SERVO_DIALOG"),
                    hWnd,
                    (DLGPROC) choose_servo_dialog_proc );
                break;

            case EDIT_EQUATIONS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_DCURVE_DIALOG"),
                    hWnd,
                    (DLGPROC) edit_dcurve_dialog_proc );
                break;

            case HEAD_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = HEAD_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case ROD_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = ROD_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case DIFF_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = DIFF_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case POS_IMPACT_POINTS_BOX:
            case TIME_IMPACT_POINTS_BOX:
            case CYCLE_TIMEOUT_BOX:
            case AUTOSHOT_TIMEOUT_BOX:
                if ( cmd == EN_KILLFOCUS && m )
                    {
                    i = GetDlgItemInt( MachSetupWindow, id, NULL, TRUE );
                    switch ( id )
                        {
                        case POS_IMPACT_POINTS_BOX:
                            m->impact_points[PRE_IMPACT_INDEX] = i;
                            break;
                        case TIME_IMPACT_POINTS_BOX:
                            m->impact_points[POST_IMPACT_INDEX] = i;
                            break;
                        case CYCLE_TIMEOUT_BOX:
                            m->cycle_timeout_seconds = i;
                            break;
                        case AUTOSHOT_TIMEOUT_BOX:
                            m->autoshot_timeout_seconds = i;
                            break;
                        }
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case IDOK:
               save_changes();
               break;

            case CANCEL_BUTTON:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                break;
            }
        return TRUE;

    }

return FALSE;
}

/***********************************************************************
*                              SUB_PROC                                *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
TCHAR c;
if ( msg == WM_CHAR )
    {
    c = (TCHAR) wParam;
    if ( c != VK_BACK )
        {
        if ( !IsCharAlphaNumeric(c) )
            {
            MessageBeep( 0xFFFFFFFF );
            return 0;
            }
        }
    }

return CallWindowProc( OldEditProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                            NEW_MACHINE_PROC                          *
***********************************************************************/
BOOL CALLBACK new_machine_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
HWND  w;
TCHAR name[MACHINE_NAME_LEN+1];

id  = LOWORD( wParam );
switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        w = GetDlgItem( NewMachineWindow, MACHINE_EDIT );
        OldEditProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        SendDlgItemMessage( NewMachineWindow, MACHINE_EDIT, EM_LIMITTEXT, MACHINE_NAME_LEN,0L );
        fill_computers();
        SetFocus( GetDlgItem(NewMachineWindow, MACHINE_EDIT) );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                /*
                ----------------
                Get machine name
                ---------------- */
                GetDlgItemText( NewMachineWindow, MACHINE_EDIT, name, MACHINE_NAME_LEN+1 );

                if ( is_empty(name) )
                    ShowWindow( NewMachineWindow, SW_HIDE );

                /*
                ------------------------------------------------
                Machine name must be unique across all computers
                ------------------------------------------------ */
                else if (SendDlgItemMessage(MachSetupWindow, MACHINE_LIST_BOX, LB_FINDSTRINGEXACT, 0, (LPARAM) ((LPSTR) name)) == LB_ERR)
                    {
                    add_machine( name );
                    ShowWindow( NewMachineWindow, SW_HIDE );
                    poke_data( DDE_CONFIG_TOPIC, ItemList[MACH_SETUP_INDEX].name, name );
                    }
                else
                    MessageBox( NewMachineWindow, TEXT("Machine Already Exists"), TEXT("Error"), MB_OK );

                break;

            case CANCEL_BUTTON:
                ShowWindow( NewMachineWindow, SW_HIDE );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          CREATE_NEW_MACHINE                          *
***********************************************************************/
void create_new_machine ( void )
{
if ( HaveSureTrakControl )
    resource_message_box( MainInstance, ONLY_ONE_MACHINE_STRING, SURETRAK_CONTROLLED_STRING, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL );

if (NewMachineWindow == NULL)
   {
   NewMachineWindow = CreateDialog(
       MainInstance,
       TEXT("NEW_MACHINE"),
       MainWindow,
       (DLGPROC) new_machine_proc );
   }

ShowWindow( NewMachineWindow, SW_SHOW );
}
/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_HELP:
        gethelp();
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case IDCANCEL:
            case EXIT_CHOICE:
               SendMessage( hWnd, WM_CLOSE, 0, 0L );
               return 0;

            case MINIMIZE_MENU_CHOICE:
               CloseWindow( MainWindow );
               return 0;

            case CREATE_CHOICE:
               create_new_machine();
               return 0;

            case DELETE_CHOICE:
                delete_machine();
                return 0;
             }
        break;

    case WM_DBINIT:
        fill_rod_pitches();
        fill_dcurves();
        CurrentMachine = 0;
        fill_machines(NULL);
        client_dde_startup( hWnd );
        SetFocus( GetDlgItem(MachSetupWindow, MACHINE_LIST_BOX) );
        return 0;

    case WM_SETFOCUS:
        if ( MachSetupWindow )
            SetFocus( GetDlgItem(MachSetupWindow, MACHINE_LIST_BOX) );
        return 0;
        
    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( HINSTANCE this_instance, int cmd_show )
{

WNDCLASS wc;
COMPUTER_CLASS c;

MainInstance = this_instance;
MachSetupWindow = 0;

names_startup ();
c.startup();

if ( *get_ini_string( VisiTrakIniFile, ConfigSection, HaveSureTrakKey ) == YChar )
    HaveSureTrakControl = TRUE;
if ( *get_ini_string( VisiTrakIniFile, ConfigSection, HavePressureControlKey ) == YChar )
    HavePressureControl = TRUE;

wc.lpszClassName = MyClassName;
wc.hInstance     = this_instance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(this_instance, MyIconName );
wc.lpszMenuName  = TEXT("MachineMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    610, 325,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MachSetupWindow  = CreateDialog(
    MainInstance,
    TEXT("EDIT_MACHINES"),
    MainWindow,
    (DLGPROC) MachSetupProc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
COMPUTER_CLASS c;

client_dde_shutdown();
c.shutdown();
cleanup_machines();
cleanup_dcurves();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

MyWindowTitle = resource_string( this_instance, MAIN_WINDOW_TITLE );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( this_instance, cmd_show);

while ( GetMessage(&msg, NULL, 0, 0) )
   {
   status = FALSE;

   if (MachSetupWindow )
      status = IsDialogMessage( MachSetupWindow, &msg );

   if (!status && NewMachineWindow )
      status = IsDialogMessage( NewMachineWindow, &msg );

   if ( !status )
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
   }

shutdown();
return(msg.wParam);
}
