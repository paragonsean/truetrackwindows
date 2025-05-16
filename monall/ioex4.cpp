#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\listbox.h"
#include "..\include\structs.h"
#include "..\include\part.h"
#include "..\include\subs.h"
#include "..\include\ringcl.h"
#include "..\include\machname.h"
#include "..\include\stparam.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"

static const TCHAR EmptyString[] = TEXT( "" );
static const TCHAR NullChar      = TEXT( '\0' );
static const TCHAR YChar         = TEXT( 'Y' );
static const TCHAR NChar         = TEXT( 'N' );

#include "extern.h"
#include "resource.h"
#include "boards.h"

extern double RealMaxPosition;
static double MinStrokeSubtract = 4.0;

static BOOL NeedTotalStrokeChecked     = FALSE;
static BOOL NeedMinimumStrokeChecked   = FALSE;

static const TCHAR MonallIniFile[]   = TEXT( "monall.ini" );
static const TCHAR VisiTrakIniFile[]       = TEXT( "visitrak.ini" );
static const TCHAR ConfigSection[]         = TEXT( "Config" );
static const TCHAR MinStrokeSubtractKey[]  = TEXT( "MinStrokeSubtractSave" );
static const TCHAR CheckTotalStrokeKey[]   = TEXT( "CheckTotalStrokeSave" );
static const TCHAR CheckMinimumStrokeKey[] = TEXT( "CheckMinimumStrokeSave" );
static const TCHAR AnalogCheckmarksKey[]   = TEXT( "AnalogCheckmarks" );

static const TCHAR CsvEnableKey[]          = TEXT( "CsvEnable" );
static const TCHAR CsvFileNameKey[]        = TEXT( "CsvFileName" );
static const TCHAR CsvChannelKey[]         = TEXT( "CsvChannel" );

static STRING_CLASS CsvChannel;
STRING_CLASS CsvFileName;
int          CsvChannelIndex = 0;
BOOLEAN      CsvEnable  = FALSE;

BOOL CALLBACK transducer_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam );

/***********************************************************************
*                              SEND_REQUEST                            *
***********************************************************************/
void send_request( BOARD_DATA request )
{
FasTrakCommand.push( ultohex(request) );
}

/***********************************************************************
*                          GRAY_IOEX4_CONTROLS                         *
***********************************************************************/
void gray_ioex4_controls( void )
{
static const UINT GrayButton[] = {
    TEST_RAM_BUTTON,
    TEST_OPTO_BUTTON,
    TEST_IO_BUTTON,
    COMMAND_AND_DATA_PORTS_BUTTON,
    CLEAR_ERROR_STATUS_BUTTON,
    CLEAR_DATA_READY_BUTTON
    };

const int32 NofGrayButtons = sizeof(GrayButton)/sizeof(UINT);

int32 i;
BOOL  is_enabled;

if ( !Ioex4IsActive )
    return;

if ( Ioex4GrayLevel <= CurrentPasswordLevel )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

for ( i=0; i<NofGrayButtons; i++ )
    EnableWindow( Ioex4Window.control(GrayButton[i]), is_enabled );
}

/***********************************************************************
*                           SAVE_TOTAL_STROKE_LEN                      *
***********************************************************************/
void save_total_stroke_len( HWND w )
{
TCHAR mychar[2];
PART_NAME_ENTRY pn;
PART_CLASS p;
SURETRAK_PARAM_CLASS s;
float dif;

TCHAR buf[MAX_FLOAT_LEN+1];
BOOLEAN have_changes;

have_changes = FALSE;

get_text( pn.computer, Ioex4Window.handle(), IOEX4_COMPUTER_TBOX, COMPUTER_NAME_LEN );
get_text( pn.machine,  Ioex4Window.handle(), IOEX4_MACHINE_TBOX,  MACHINE_NAME_LEN  );
get_text( pn.part,     Ioex4Window.handle(), IOEX4_PART_TBOX,     PART_NAME_LEN     );

if ( p.find(pn.computer, pn.machine, pn.part) )
    {
    NeedTotalStrokeChecked = is_checked(w, SAVE_TOTAL_STROKE_XBOX );
    if ( NeedTotalStrokeChecked )
        {
        if ( get_text(buf, w, TOTAL_STROKE_EBOX, MAX_FLOAT_LEN) )
            {
            p.total_stroke_length = extfloat( buf );
            have_changes = TRUE;
            }
        }

    NeedMinimumStrokeChecked = is_checked( w, SAVE_MINIMUM_STROKE_XBOX );
    if ( NeedMinimumStrokeChecked )
        {
        if ( get_text(buf, w, MINIMUM_STROKE_EBOX, MAX_FLOAT_LEN) )
            {
            p.min_stroke_length = extfloat( buf );
            dif = p.total_stroke_length - p.min_stroke_length;
            if ( dif > 0.0 && not_float_zero( MinStrokeSubtract-dif) )
                {
                MinStrokeSubtract = dif;
                put_ini_string( VisiTrakIniFile, ConfigSection, MinStrokeSubtractKey, ascii_float(dif) );
                }
            have_changes = TRUE;
            }
        }

    if ( have_changes )
        p.save();


    if ( is_checked(w, SAVE_ZERO_SPEED_CHECK_XBOX) )
        {
        if ( s.find(pn.computer) )
            {
            if ( get_text(buf, w, ZERO_SPEED_CHECK_EBOX, MAX_FLOAT_LEN) )
                {
                s.set_zero_speed_check_pos( extfloat(buf) );
                s.save(pn.computer);
                have_changes = TRUE;
                }
            }
        }

    if ( have_changes )
        {
        poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, pn.machine );
        PostMessage( MainWindow, WM_NEWSETUP, 0, 0L );
        }

    }

mychar[1] = NullChar;

if ( NeedTotalStrokeChecked )
    *mychar = YChar;
else
    *mychar = NChar;
put_ini_string( VisiTrakIniFile, ConfigSection, CheckTotalStrokeKey, mychar );

if ( NeedMinimumStrokeChecked )
    *mychar = YChar;
else
    *mychar = NChar;

put_ini_string( VisiTrakIniFile, ConfigSection, CheckMinimumStrokeKey, mychar );
}

/***********************************************************************
*                       SAVE_TOTAL_STROKE_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK save_total_stroke_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_total_stroke_len( hWnd );
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_INITDIALOG:
        EnableWindow( GetDlgItem(hWnd, ZERO_SPEED_CHECK_EBOX),      (BOOL) HaveSureTrakControl );
        EnableWindow( GetDlgItem(hWnd, SAVE_ZERO_SPEED_CHECK_XBOX), (BOOL) HaveSureTrakControl );

        set_text( hWnd, TOTAL_STROKE_EBOX,     fixed_string(RealMaxPosition, LeftPosPlaces, RightPosPlaces)                   );
        set_text( hWnd, MINIMUM_STROKE_EBOX,   fixed_string(RealMaxPosition-MinStrokeSubtract, LeftPosPlaces, RightPosPlaces) );
        set_text( hWnd, ZERO_SPEED_CHECK_EBOX, fixed_string(RealMaxPosition/2.0, LeftPosPlaces, RightPosPlaces)               );

        set_checkbox( hWnd, SAVE_TOTAL_STROKE_XBOX,     NeedTotalStrokeChecked    );
        set_checkbox( hWnd, SAVE_MINIMUM_STROKE_XBOX,   NeedMinimumStrokeChecked  );
        set_checkbox( hWnd, SAVE_ZERO_SPEED_CHECK_XBOX, FALSE );

        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        INIT_ANALOG_TEXTBOXES                         *
***********************************************************************/
void init_analog_textboxes( void )
{
static TCHAR default_settings[] = TEXT( "YNNNNNNNN" );
int     i;
int     id;
TCHAR * cp;
BOOLEAN need_check;

for ( i=0; i<MAX_FT_CHANNELS; i++ )
    {
    set_text( FtCurrentValuesWindow, VOLTS_1_TBOX+i,  EmptyString );
    set_text( FtCurrentValuesWindow, ANALOG_1_TBOX+i, EmptyString );
    }

cp = get_ini_string( MonallIniFile, ConfigSection, AnalogCheckmarksKey );
if ( !cp || unknown(cp) )
    cp = default_settings;

id = FT_A1_XBOX;
for ( i=0; i<MAX_FT_CHANNELS; i++ )
    {
    if ( *cp == YChar )
        need_check = TRUE;
    else
        need_check = FALSE;

    set_checkbox( FtCurrentValuesWindow, id, need_check );
    cp++;
    id++;
    }
}

/**********************************************************************
*                         update_csv_file_name                        *
**********************************************************************/
static void update_csv_file_name()
{
BOOLEAN      b;
STRING_CLASS s;

s.get_text( FtCurrentValuesWindow, CSV_FILE_NAME_EBOX );
if ( s != CsvFileName )
    {
    b = CsvEnable;
    CsvEnable = FALSE;
    CsvFileName = s;
    CsvEnable = b;
    }    
}

/**********************************************************************
*                          update_csv_channel                         *
**********************************************************************/
static void update_csv_channel()
{
int          ch;
STRING_CLASS s;

s.get_text( FtCurrentValuesWindow, CSV_CHANNEL_EBOX );
if ( s != CsvChannel )
    {
    ch = s.int_value();
    if ( ch > 0 && ch < 5 )
        {
        CsvChannel = s;
        CsvChannelIndex = ch - 1;
        } 
    else
        {
        CsvChannel.null();
        CsvChannel.set_text(  FtCurrentValuesWindow, CSV_CHANNEL_EBOX );
        }
    }    
}

/**********************************************************************
*                            save_csv_controls                        *
**********************************************************************/
static void save_csv_controls()
{
put_ini_string( MonallIniFile, ConfigSection, CsvFileNameKey, CsvFileName.text() );
put_ini_string( MonallIniFile, ConfigSection, CsvChannelKey,  CsvChannel.text()  );
boolean_to_ini( MonallIniFile, ConfigSection, CsvEnableKey,   CsvEnable );
}

/**********************************************************************
*                            init_csv_controls                        *
**********************************************************************/
static void init_csv_controls()
{
int ch;

CsvFileName = get_ini_string( MonallIniFile, ConfigSection, CsvFileNameKey );
CsvFileName.set_text( FtCurrentValuesWindow, CSV_FILE_NAME_EBOX );

CsvChannelIndex = 0;
CsvChannel  = get_ini_string( MonallIniFile, ConfigSection, CsvChannelKey );
ch = CsvChannel.int_value();
if ( ch > 0 && ch < 5 )
    CsvChannelIndex = ch - 1; 
else
    CsvChannel.null();
CsvChannel.set_text(  FtCurrentValuesWindow, CSV_CHANNEL_EBOX );

//CsvEnable = boolean_from_ini( MonallIniFile, ConfigSection, CsvEnableKey );
//set_checkbox( FtCurrentValuesWindow, CSV_ENABLE_XBOX, CsvEnable );
}

/***********************************************************************
*                        SAVE_ANALOG_CHECKMARKS                        *
***********************************************************************/
static void save_analog_checkmarks( void )
{
int    i;
int    id;
TCHAR  buf[MAX_FT_CHANNELS+1];
TCHAR * cp;

id = FT_A1_XBOX;
cp = buf;
for ( i=0; i<MAX_FT_CHANNELS; i++ )
    {
    if ( is_checked(FtCurrentValuesWindow, id) )
        *cp = YChar;
    else
        *cp = NChar;
    cp++;
    id++;
    }
*cp = NullChar;

put_ini_string( MonallIniFile, ConfigSection, AnalogCheckmarksKey, buf );
}

/***********************************************************************
*                      FT_CURRENT_VALUES_DIALOG_PROC                   *
*                                modal dialog                          *
***********************************************************************/
BOOL CALLBACK ft_current_values_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case CSV_FILE_NAME_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_csv_file_name();
                break;
                
            case CSV_CHANNEL_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_csv_channel();
                break;

            case CSV_ENABLE_XBOX:
                    CsvEnable = is_checked( hWnd, CSV_ENABLE_XBOX );
                break;

            case MAX_POSITION_RESET_BUTTON:
                send_request( RESET_MAX_POSITION_REQUEST );
                return TRUE;

            case RESET_FT_POSITION_BUTTON:
                send_request( RESET_REQUEST );
                return TRUE;

            case SAVE_TOTAL_STROKE_BUTTON:
                DialogBox( MainInstance, TEXT("SAVE_TOTAL_STROKE_DIALOG"), hWnd, (DLGPROC) save_total_stroke_dialog_proc );
                return TRUE;

            case IDCANCEL:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;

    case WM_INITDIALOG:
        FtCurrentValuesWindow = hWnd;
        init_csv_controls();
        init_analog_textboxes();
        return TRUE;

    case WM_CLOSE:
        save_analog_checkmarks();
        save_csv_controls();
        FtCurrentValuesWindow = 0;
        if ( ShowCurrentFtValuesOnly )
            {
            ShowCurrentFtValuesOnly = FALSE;
            Ioex4Window.close();
            }
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_DESTROY:
        FtCurrentValuesWindow = 0;
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
const  int NOF_TABS  = 1;
static int tabs[NOF_TABS];
int        x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = PARAMETER_NAME_LEN * x;

SendDlgItemMessage( FtShotParamsWindow, SHOT_PARAM_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                      FT_SHOT_PARAMETER_DIALOG_PROC                   *
*                                modal dialog                          *
***********************************************************************/
BOOL CALLBACK ft_shot_parameter_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch (msg)
    {
    case WM_INITDIALOG:
        FtShotParamsWindow = hWnd;
        send_request( FILL_SHOT_PARAMS_REQUEST );
        set_tab_stops();
        return TRUE;

    case WM_CLOSE:
        FtShotParamsWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_DESTROY:
        FtShotParamsWindow = 0;
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      FT_SETUP_PARAMS_DIALOG_PROC                     *
*                              modal dialog                            *
***********************************************************************/
BOOL CALLBACK ft_setup_params_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;

cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        if ( cmd == EN_CHANGE )
            {
            /* EnableWindow( GetDlgItem(hWnd, IDOK), TRUE ); */
            return TRUE;
            }
        break;

    case WM_INITDIALOG:
        FtSetupParamsWindow = hWnd;
        send_request( FILL_SETUP_PARAMS_REQUEST );
        EnableWindow( GetDlgItem(hWnd, IDOK), FALSE );
        return TRUE;

    case WM_CLOSE:
        FtSetupParamsWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_DESTROY:
        FtSetupParamsWindow = 0;
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      FT_COMMAND_PORT_DIALOG_PROC                     *
*                              modal dialog                            *
***********************************************************************/
BOOL CALLBACK ft_command_port_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        FtCommandPortWindow = hWnd;
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case READ_DATA_PORT_BUTTON:
                set_text( hWnd, DATA_PORT_VALUE_EBOX, EmptyString );
                send_request( READ_DATA_PORT_REQUEST );
                return TRUE;

            case SEND_DATA_PORT_BUTTON:
                send_request( SEND_DATA_PORT_REQUEST );
                return TRUE;

            case CLEAR_COMMAND_PORT_BUTTON:
                send_request( CLEAR_COMMAND_PORT_REQUEST );
                return TRUE;

            case SEND_COMMAND_BUTTON:
                send_request( SEND_COMMAND_PORT_REQUEST );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        FtCommandPortWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_DESTROY:
        FtSetupParamsWindow = 0;
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          OPTO_TEST_DIALOG_PROC                       *
*                              modal dialog                            *
***********************************************************************/
BOOL CALLBACK opto_test_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOARD_DATA last_command;

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        if ( is_checked(Ioex4Window.handle(),SINGLE_BOARD_XBOX) )
            send_request( SET_MUX_BOARD_REQUEST );
        last_command = CLEAR_OUTPUT_OPTOS_REQUEST;
        send_request( last_command );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( last_command == CLEAR_OUTPUT_OPTOS_REQUEST )
                    last_command = SET_OUTPUT_OPTOS_REQUEST;
                else
                    last_command = CLEAR_OUTPUT_OPTOS_REQUEST;
                send_request( last_command );
                return TRUE;

            case IDCANCEL:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        if ( is_checked(Ioex4Window.handle(),SINGLE_BOARD_XBOX) )
            send_request( SET_SINGLE_BOARD_REQUEST );
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         INIT_TOTAL_STROKE_GLOBALS                    *
***********************************************************************/
static void init_total_stroke_globals( void )
{
TCHAR s[MAX_INI_STRING_LEN+1];

if ( *get_ini_string( VisiTrakIniFile, ConfigSection, CheckTotalStrokeKey) == YChar )
    NeedTotalStrokeChecked     = TRUE;
else
    NeedTotalStrokeChecked     = FALSE;

if ( *get_ini_string( VisiTrakIniFile, ConfigSection, CheckMinimumStrokeKey) == YChar )
    NeedMinimumStrokeChecked   = TRUE;
else
    NeedMinimumStrokeChecked   = FALSE;

lstrcpy( s, get_ini_string(VisiTrakIniFile, ConfigSection, MinStrokeSubtractKey) );
if ( lstrcmp(s, UNKNOWN) != 0 )
    MinStrokeSubtract = extfloat( s );
else
    MinStrokeSubtract = 4.0;

}

/***********************************************************************
*                            IOEX4_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK ioex4_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF GreenColor = RGB( 0, 255, 0 );
static HBRUSH BackgroundBrush = 0;
static bool firstime = true;

int  id;
int  cmd;
id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case BOARD_1_RADIO:
            case BOARD_2_RADIO:
            case BOARD_3_RADIO:
            case BOARD_4_RADIO:
            case BOARD_5_RADIO:
            case BOARD_6_RADIO:
            case BOARD_7_RADIO:
            case BOARD_8_RADIO:
                send_request( SET_BOARD_REQUEST );
                return TRUE;

            case MUX_CHANNEL_1_RADIO:
            case MUX_CHANNEL_2_RADIO:
            case MUX_CHANNEL_3_RADIO:
            case MUX_CHANNEL_4_RADIO:
            case MUX_CHANNEL_5_RADIO:
            case MUX_CHANNEL_6_RADIO:
            case MUX_CHANNEL_7_RADIO:
            case MUX_CHANNEL_8_RADIO:
                send_request( SET_MUX_CHANNEL_REQUEST );
                return TRUE;

            case MUX_ENABLE_1_XBOX:
                send_request( SET_MUX_MASK_REQUEST );
                return TRUE;

            case SET_DATA_COLL_MODE_BUTTON:
                send_request( DATA_COLLECTION_MODE_REQUEST );
                return TRUE;

            case XDCR_TEST_BUTTON:
                DialogBox( MainInstance, TEXT("TRANSDUCER_TEST_DIALOG"), hWnd, (DLGPROC) transducer_test_dialog_proc );
                return TRUE;

            case SET_COMMAND_MODE_BUTTON:
                send_request( COMMAND_MODE_REQUEST );
                return TRUE;

            case SET_SCAN_MODE_BUTTON:
                send_request( SCAN_MODE_REQUEST );
                return TRUE;

            case GET_PROM_VERSION_BUTTON:
                set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, EmptyString );
                send_request( GET_VERSION_REQUEST );
                return TRUE;

            case COMMAND_AND_DATA_PORTS_BUTTON:
                DialogBox( MainInstance, TEXT("FT_COMMAND_PORT_DIALOG"), hWnd, (DLGPROC) ft_command_port_dialog_proc );
                return TRUE;

            case CLEAR_ERROR_STATUS_BUTTON:
                send_request( CLEAR_ERROR_BITS_REQUEST );
                return TRUE;

            case LATCH_SHOT_COUNTERS_BUTTON:
                send_request( LATCH_COUNTERS_REQUEST );
                return TRUE;

            case CLEAR_SHOT_COUNTERS_BUTTON:
                send_request( CLEAR_COUNTERS_REQUEST );
                return TRUE;

            case TEST_RAM_BUTTON:
                set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, resource_string(MainInstance, TESTING_RAM_STRING) );
                send_request( RAM_TEST_REQUEST );
                return TRUE;

            case TEST_OPTO_BUTTON:
                if ( GetKeyState(VK_SHIFT) < 0 )
                    {
                    DialogBox( MainInstance, TEXT("OPTO_TEST_DIALOG"), hWnd, (DLGPROC) opto_test_dialog_proc );
                    }
                else
                    {
                    set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, resource_string(MainInstance, TESTING_OPTO_PORT_STRING) );
                    send_request( OPTO_PORT_TEST_REQUEST );
                    }
                return TRUE;

            case TEST_IO_BUTTON:
                set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, resource_string(MainInstance, TESTING_IO_PORT_STRING) );
                send_request( IO_PORT_TEST_REQUEST );
                return TRUE;

            case SINGLE_BOARD_XBOX:
                send_request( SET_SINGLE_BOARD_REQUEST );
                return TRUE;

            case MUX_BOARD_XBOX:
                send_request( SET_MUX_BOARD_REQUEST );
                return TRUE;

            case CLEAR_DATA_READY_BUTTON:
                send_request( CLEAR_DATA_READY_BIT_REQUEST );
                return TRUE;

            case READ_CURRENT_VALUES_BUTTON:
                DialogBox( MainInstance, TEXT("FT_CURRENT_VALUES_DIALOG"), hWnd, (DLGPROC) ft_current_values_dialog_proc );
                return TRUE;

            case READ_SHOT_PARAM_BUTTON:
                DialogBox( MainInstance, TEXT("FT_SHOT_PARAMETER_DIALOG"), hWnd, (DLGPROC) ft_shot_parameter_dialog_proc );
                return TRUE;

            case SETUP_PARAMS_BUTTON:
                DialogBox( MainInstance, TEXT("FT_SETUP_PARAMS_DIALOG"), hWnd, (DLGPROC) ft_setup_params_dialog_proc );
                return TRUE;

            case IDCANCEL:
            case IDCLOSE:
                Ioex4Window.close();
                return TRUE;

            }
        break;

    case WM_CTLCOLORSTATIC:
        HWND ws;

        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id >= SCAN_MODE_BIT_TBOX && id <= BUSY_BIT_TBOX )
            {
            if ( IsWindowEnabled(ws) )
                {
                SetBkColor((HDC) wParam, GreenColor );
                return (int) BackgroundBrush;
                }
            }
        break;

    case WM_INITDIALOG:
        Ioex4Window = hWnd;
        BackgroundBrush = CreateSolidBrush ( GreenColor );
        set_text( hWnd, IOEX4_COMPUTER_TBOX, ComputerName );
        init_total_stroke_globals();
        for ( id = SCAN_MODE_BIT_TBOX; id<=BUSY_BIT_TBOX; id++ )
            ShowWindow( GetDlgItem(hWnd, id), SW_HIDE );
        return TRUE;

    case WM_DBINIT:
        if ( ShowCurrentFtValuesOnly )
            {
            send_request( COMMAND_MODE_REQUEST );
            DialogBox( MainInstance, TEXT("FT_CURRENT_VALUES_DIALOG"), hWnd, (DLGPROC) ft_current_values_dialog_proc );
            }
        return TRUE;

    case WM_SHOWWINDOW:
        if ( ((BOOL) wParam) && lParam == 0 )  //wParam is TRUE if show, lParam is 0 of caused by ShowWindow
            {
            if ( firstime )
                firstime = false;
            else
                shrinkwrap( MainWindow, hWnd );

            if ( ShowCurrentFtValuesOnly )
                Ioex4Window.post( WM_DBINIT );
            else
                Ioex4Window.set_focus( SET_COMMAND_MODE_BUTTON );
            }
        break;

    case WM_CLOSE:
        SetMenu( MainWindow, MainMenu );
        ShowWindow( HardwareStatusWindow, SW_SHOW );
        Ioex4Window.hide();
        Ioex4IsActive = FALSE;
        return TRUE;

    case WM_DESTROY:
        if ( BackgroundBrush )
            {
            DeleteObject( BackgroundBrush );
            BackgroundBrush = 0;
            }
        break;
    }

return FALSE;
}
