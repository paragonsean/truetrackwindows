#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\bitclass.h"
#include "..\include\wclass.h"
#include "..\include\ftclass.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\listbox.h"
#include "..\include\plotclas.h"
#include "..\include\runlist.h"
#include "..\include\stparam.h"
#include "..\include\ringcl.h"

#include "resource.h"
#include "extern.h"

extern BIT_CLASS Channel_A_Bits;
extern BIT_CLASS Channel_B_Bits;

static const DWORD MsForMonitorCycle = 500;
static const TCHAR CommaChar  = TEXT( ',' );

static const int32 OK_NULL_VOLTAGE_LEVEL       = 0;
static const int32 BALLPARK_NULL_VOLTAGE_LEVEL = 1;
static const int32 UNUSABLE_NULL_VOLTAGE_LEVEL = 2;
static const int32 NULL_VOLTAGE_READ_FAILURE   = 3;
static const BOARD_DATA MAX_ST_VARIABLE_NUMBER = 99;
static const BOARD_DATA DAC_BITS = 0x3FFF;                /* Mask for current dac command (SuretrakDacWord) */

/*
-------------------
SureTrak Parameters
------------------- */
const BOARD_DATA RD_ACCEL1           = 0x7F81;
const BOARD_DATA RD_ACCEL2           = 0x7F88;
const BOARD_DATA RD_ACCEL3           = 0x7F8F;
const BOARD_DATA RD_ACCEL4           = 0x7F96;
const BOARD_DATA RD_ACCEL5           = 0x7F9D;
const BOARD_DATA RD_ACCEL6           = 0x7FA4;
const BOARD_DATA RD_GOOSE1           = 0x7F82;
const BOARD_DATA RD_GOOSE2           = 0x7F89;
const BOARD_DATA RD_GOOSE3           = 0x7F90;
const BOARD_DATA RD_GOOSE4           = 0x7F97;
const BOARD_DATA RD_GOOSE5           = 0x7F9E;
const BOARD_DATA RD_GOOSE6           = 0x7FA5;
const BOARD_DATA RD_POS1             = 0x7F83;
const BOARD_DATA RD_POS2             = 0x7F8A;
const BOARD_DATA RD_POS3             = 0x7F91;
const BOARD_DATA RD_POS4             = 0x7F98;
const BOARD_DATA RD_POS5             = 0x7F9F;
const BOARD_DATA RD_POS6             = 0x7FA6;
const BOARD_DATA RD_VEL1             = 0x7F80;
const BOARD_DATA RD_VEL2             = 0x7F87;
const BOARD_DATA RD_VEL3             = 0x7F8E;
const BOARD_DATA RD_VEL4             = 0x7F95;
const BOARD_DATA RD_VEL5             = 0x7F9C;
const BOARD_DATA RD_VEL6             = 0x7FA3;
const BOARD_DATA RD_VAC1             = 0x7F86;
const BOARD_DATA RD_VAC2             = 0x7F8D;
const BOARD_DATA RD_VAC3             = 0x7F94;
const BOARD_DATA RD_VAC4             = 0x7F9B;
const BOARD_DATA RD_VAC5             = 0x7FA2;
const BOARD_DATA RD_VAC6             = 0x7FA9;
const BOARD_DATA RD_LICMD1           = 0x7F84;
const BOARD_DATA RD_LICMD2           = 0x7F8B;
const BOARD_DATA RD_LICMD3           = 0x7F92;
const BOARD_DATA RD_LICMD4           = 0x7F99;
const BOARD_DATA RD_LICMD5           = 0x7FA0;
const BOARD_DATA RD_LICMD6           = 0x7FA7;
const BOARD_DATA RD_LI_IO1           = 0x7F85;
const BOARD_DATA RD_LI_IO2           = 0x7F8C;
const BOARD_DATA RD_LI_IO3           = 0x7F93;
const BOARD_DATA RD_LI_IO4           = 0x7F9A;
const BOARD_DATA RD_LI_IO5           = 0x7FA1;
const BOARD_DATA RD_LI_IO6           = 0x7FA8;
const BOARD_DATA RD_RETR             = 0x7FB2;
const BOARD_DATA RD_FT               = 0x7FB3;
const BOARD_DATA RD_JS               = 0x7FB4;
const BOARD_DATA CRD_LS1             = 0x801B;
const BOARD_DATA CRD_LS2             = 0x801C;
const BOARD_DATA CRD_LS3             = 0x801D;
const BOARD_DATA CRD_LS4             = 0x801E;
const BOARD_DATA CRD_LS5             = 0x801F;
const BOARD_DATA CRD_LS6             = 0x8032;
const BOARD_DATA RD_V_LOOP_GAIN      = 0x7FE4;
const BOARD_DATA RD_BK_VEL1          = 0x7FE5;
const BOARD_DATA RD_BK_VEL2          = 0x7FE6;
const BOARD_DATA RD_BK_VEL3          = 0x7FE7;
const BOARD_DATA RD_BKGN1            = 0x7FE8;
const BOARD_DATA RD_BKGN2            = 0x7FE9;
const BOARD_DATA RD_BKGN3            = 0x7FEA;
const BOARD_DATA RD_DITHER_TIME      = 0x7FEB;
const BOARD_DATA RD_DITHER_AMPL      = 0x7FEC;
const BOARD_DATA RD_VEL_DIVIDEND     = 0x7FED;
const BOARD_DATA RD_P_CHECK_ZSPEED   = 0x7FEF;
const BOARD_DATA RD_COUNTS_PER_UNIT  = 0x7FF0;
const BOARD_DATA RD_JS_SETTING       = 0x7FF3;
const BOARD_DATA RD_RETR_SETTING     = 0x7FF4;
const BOARD_DATA RD_FT_SETTING       = 0x7FF5;
const BOARD_DATA RD_VEL_MUL          = 0x7FF6;

const BOARD_DATA CCUR_POS            = 0x8009;
const BOARD_DATA CCUR_VEL            = 0x800A;
const BOARD_DATA CCUR_ANA1           = 0x800B;
const BOARD_DATA CCUR_ANA2           = 0x800C;
const BOARD_DATA CCUR_ANA3           = 0x800D;
const BOARD_DATA CCUR_ANA4           = 0x800E;
const BOARD_DATA GET_C_VER_NUMBER    = 0x8012;
const BOARD_DATA SET_CONTROL_ACTIVE  = 0x8013;
const BOARD_DATA CLEAR_CONTROL_MODE  = 0x8014;
const BOARD_DATA UPDATE_PARAMETERS   = 0x8015;

const BOARD_DATA GET_BLOCK_PTR       = 0x8020;

const BOARD_DATA RD_PROG_DOWNLOADED  = 0x802E;

static const uint32 LowWordMask       = 0x0000FFFFL;
static const DWORD  MsForSettling     = 250;
static const DWORD  MsForHoldingPoint = 500;

BOARD_CONTROL_CLASS SureTrakBoard;

static BOOLEAN     TestingValve        = FALSE;
static BOOLEAN     ControlWasActive    = FALSE;
static int32       VTCycleCount        = 0;

static const DWORD MsAfterVT           = 1000;
static const DWORD MsForVTCycle        = 1500;
static const DWORD MsForVTSettling     =  500;
static const DWORD MsForTransducerTest =  500;
static const DWORD MsForRamTest        =  5;
static const DWORD MsForOptoPortTest   =  5;
static const TCHAR EmptyString[]       = TEXT("");

int send_profile_to_board( void );
int compile_and_send( TCHAR * filename );

const TCHAR TabStr[] = TEXT( "\t" );

const short UNSIGNED_SHORT_TYPE    = 1;
const short SIGNED_SHORT_TYPE      = 2;
const short DIV_BY_100_FLOAT_TYPE  = 3;
const short DIV_BY_1000_FLOAT_TYPE = 4;
const short DIV_BY_COUNTS_TYPE     = 5;
const short PERCENTAGE_TYPE        = 6;
const short VEL_DIVIDEND_TYPE      = 7;

struct CONTROL_PARAMETER_ENTRY {
    BOARD_DATA cmd;
    short      type;
    TCHAR    * name;
    };

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                              SHOW_ERROR                              *
***********************************************************************/
static void show_error( STRING_CLASS & s, int status, UINT id )
{

s += resource_string( STATUS_SUFFIX_STRING );
s += int32toasc( status );

MessageBox( 0, s.text(), resource_string(id), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                  DO_READ_CONTROL_PARAMETERS_REQUEST                  *
***********************************************************************/
static void do_read_control_parameters_request( void )
{

static CONTROL_PARAMETER_ENTRY parameters[] = {
    { RD_ACCEL1,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL1") },
    { RD_ACCEL2,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL2") },
    { RD_ACCEL3,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL3") },
    { RD_ACCEL4,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL4") },
    { RD_ACCEL5,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL5") },
    { RD_ACCEL6,           UNSIGNED_SHORT_TYPE,    TEXT("ACCEL6") },
    { RD_GOOSE1,           SIGNED_SHORT_TYPE,      TEXT("GOOSE1") },
    { RD_GOOSE2,           SIGNED_SHORT_TYPE,      TEXT("GOOSE2") },
    { RD_GOOSE3,           SIGNED_SHORT_TYPE,      TEXT("GOOSE3") },
    { RD_GOOSE4,           SIGNED_SHORT_TYPE,      TEXT("GOOSE4") },
    { RD_GOOSE5,           SIGNED_SHORT_TYPE,      TEXT("GOOSE5") },
    { RD_GOOSE6,           SIGNED_SHORT_TYPE,      TEXT("GOOSE6") },
    { RD_POS1,             DIV_BY_COUNTS_TYPE,     TEXT("POS1") },
    { RD_POS2,             DIV_BY_COUNTS_TYPE,     TEXT("POS2") },
    { RD_POS3,             DIV_BY_COUNTS_TYPE,     TEXT("POS3") },
    { RD_POS4,             DIV_BY_COUNTS_TYPE,     TEXT("POS4") },
    { RD_POS5,             DIV_BY_COUNTS_TYPE,     TEXT("POS5") },
    { RD_POS6,             DIV_BY_COUNTS_TYPE,     TEXT("POS6") },
    { RD_VEL1,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL1") },
    { RD_VEL2,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL2") },
    { RD_VEL3,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL3") },
    { RD_VEL4,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL4") },
    { RD_VEL5,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL5") },
    { RD_VEL6,             DIV_BY_100_FLOAT_TYPE,  TEXT("VEL6") },
    { RD_VAC1,             UNSIGNED_SHORT_TYPE,    TEXT("VAC1") },
    { RD_VAC2,             UNSIGNED_SHORT_TYPE,    TEXT("VAC2") },
    { RD_VAC3,             UNSIGNED_SHORT_TYPE,    TEXT("VAC3") },
    { RD_VAC4,             UNSIGNED_SHORT_TYPE,    TEXT("VAC4") },
    { RD_VAC5,             UNSIGNED_SHORT_TYPE,    TEXT("VAC5") },
    { RD_VAC6,             UNSIGNED_SHORT_TYPE,    TEXT("VAC6") },
    { RD_LICMD1,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD1") },
    { RD_LICMD2,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD2") },
    { RD_LICMD3,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD3") },
    { RD_LICMD4,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD4") },
    { RD_LICMD5,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD5") },
    { RD_LICMD6,           UNSIGNED_SHORT_TYPE,    TEXT("LICMD6") },
    { RD_LI_IO1,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO1") },
    { RD_LI_IO2,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO2") },
    { RD_LI_IO3,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO3") },
    { RD_LI_IO4,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO4") },
    { RD_LI_IO5,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO5") },
    { RD_LI_IO6,           UNSIGNED_SHORT_TYPE,    TEXT("LI_IO6") },
    { RD_RETR,             UNSIGNED_SHORT_TYPE,    TEXT("RETR") },
    { RD_FT,               UNSIGNED_SHORT_TYPE,    TEXT("FT") },
    { RD_JS,               UNSIGNED_SHORT_TYPE,    TEXT("JS") },
    { CRD_LS1,             UNSIGNED_SHORT_TYPE,    TEXT("LS1") },
    { CRD_LS2,             UNSIGNED_SHORT_TYPE,    TEXT("LS2") },
    { CRD_LS3,             UNSIGNED_SHORT_TYPE,    TEXT("LS3") },
    { CRD_LS4,             UNSIGNED_SHORT_TYPE,    TEXT("LS4") },
    { RD_V_LOOP_GAIN,      UNSIGNED_SHORT_TYPE,    TEXT("V_LOOP_GAIN") },
    { RD_DITHER_TIME,      SIGNED_SHORT_TYPE,      TEXT("DITHER_TIME") },
    { RD_DITHER_AMPL,      SIGNED_SHORT_TYPE,      TEXT("DITHER_AMPL") },
    { RD_BK_VEL1,          DIV_BY_100_FLOAT_TYPE,  TEXT("BREAK_VEL1 ") },
    { RD_BK_VEL2,          DIV_BY_100_FLOAT_TYPE,  TEXT("BREAK_VEL2 ") },
    { RD_BK_VEL3,          DIV_BY_100_FLOAT_TYPE,  TEXT("BREAK_VEL3 ") },
    { RD_BKGN1,            DIV_BY_1000_FLOAT_TYPE, TEXT("BKGN1 ") },
    { RD_BKGN2,            DIV_BY_1000_FLOAT_TYPE, TEXT("BKGN2 ") },
    { RD_BKGN3,            DIV_BY_1000_FLOAT_TYPE, TEXT("BKGN3 ") },
    { RD_VEL_DIVIDEND,     VEL_DIVIDEND_TYPE,      TEXT("VEL_DIVIDEND ") },
    { RD_COUNTS_PER_UNIT,  DIV_BY_100_FLOAT_TYPE,  TEXT("COUNTS_PER_UNIT ") },
    { RD_P_CHECK_ZSPEED,   DIV_BY_COUNTS_TYPE,     TEXT("P_CHECK_ZSPEED ") },
    { RD_JS_SETTING,       PERCENTAGE_TYPE,        TEXT("JS_PERCENTAGE ") },
    { RD_RETR_SETTING,     PERCENTAGE_TYPE,        TEXT("RETR_PERCENTAGE ") },
    { RD_FT_SETTING,       PERCENTAGE_TYPE,        TEXT("FT_PERCENTAGE ") },
    { RD_VEL_MUL,          DIV_BY_100_FLOAT_TYPE,  TEXT("VELOCITY_MUL ") }
    };

const int32 nof_parameters = sizeof(parameters)/sizeof(CONTROL_PARAMETER_ENTRY);

unsigned long vel_dividend;
int32         i;
BOARD_DATA    x;
float         y;
TCHAR       * cp;
float         counts_per_unit;
TCHAR         buf[100];
LISTBOX_CLASS lb;

if ( !CurrentParametersWindow )
    return;

counts_per_unit = 1.0;

lb.init( CurrentParametersWindow, CURRENT_PARAMETER_VALUE_LISTBOX );
lb.empty();
lb.redraw_off();

for ( i=0; i<nof_parameters; i++ )
    {
    SureTrakBoard.read_parameter( parameters[i].cmd, &x );
    lstrcpy( buf, parameters[i].name );
    lstrcat( buf, TabStr );
    switch ( parameters[i].type )
        {
        case UNSIGNED_SHORT_TYPE:
            cp = int32toasc( (int32) x );
            break;

        case SIGNED_SHORT_TYPE:
            cp = int32toasc( (int32)(short)x );
            break;

        case DIV_BY_100_FLOAT_TYPE:
            y = (float) x;
            y /= 100.0;

            if ( parameters[i].cmd == RD_COUNTS_PER_UNIT )
                {
                if ( y > 1.0 )
                    counts_per_unit = y;
                }
            cp = ascii_float( y );
            break;

        case DIV_BY_1000_FLOAT_TYPE:
            y = (float) x;
            y /= 1000.0;
            cp = ascii_float( y );
            break;

        case DIV_BY_COUNTS_TYPE:
            y = (float) x;
            y /= counts_per_unit;
            cp = ascii_float( y );
            break;

        case PERCENTAGE_TYPE:
            y = (float) x;
            y /= (float) 0x2000;
            y -= 1.0;
            y *= 100.0;
            cp = ascii_float( y );
            break;

        case VEL_DIVIDEND_TYPE:
            vel_dividend = x;
            SureTrakBoard.read_parameter( parameters[i].cmd+1, &x );
            vel_dividend += (unsigned long) 0x10000 * (unsigned long) x;
            cp = ultoascii( vel_dividend );
            break;

        default:
            cp = int32toasc( (int32) x );
            break;
        }

    lstrcat( buf, cp );
    lb.add( buf );
    }

lb.redraw_on();
}

/***********************************************************************
*                         DO_SET_PARAMETER_REQUEST                     *
*                             "4,3A43,D123"                            *
*                              |   |   +---- Parameter Value           *
*                              |   +-------- Parameter command.        *
*                              +------------ SET_PARAMETER_REQUEST     *
***********************************************************************/
int do_set_parameter_request( TCHAR * s )
{
int        status;
BOARD_DATA command;
BOARD_DATA data;
STRING_CLASS sc;

sc = s;
status = FT_ERROR;
if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    if ( replace_char_with_null(s, CommaChar) )
        {
        command = (BOARD_DATA) hextoul( s );
        s = nextstring( s );
        data = (BOARD_DATA) hextoul( s );
        status = SureTrakBoard.set_parameter( command, data );
        if ( status == FT_OK )
            status = SureTrakBoard.send_command_busy( UPDATE_PARAMETERS );
        }
    }

if ( status != FT_OK )
    show_error( sc, status, SET_PARAMETER_ERROR_STRING );

return status;
}

/***********************************************************************
*                           SET_VEL_DIVIDEND                           *
*                             "5,3A43D123"                             *
*                              |     +---- Velocity dividend           *
*                              +---------- SET_VEL_DIVIDEND_REQUEST    *
***********************************************************************/
int set_vel_dividend( TCHAR * s )
{
uint32 dividend;
int status;
BOARD_DATA x;
STRING_CLASS sc;

sc = s;

status = FT_ERROR;
if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    dividend = hextoul( s );
    x = (BOARD_DATA) ( LowWordMask & dividend );
    status = SureTrakBoard.set_parameter( SET_VEL_DIVIDEND, x );
    if ( status == FT_OK )
        {
        x = (BOARD_DATA) (dividend >> 16);
        status = SureTrakBoard.set_parameter( SET_VEL_DIVIDEND+1, x );
        if ( status == FT_OK )
            status = SureTrakBoard.send_command_busy( UPDATE_PARAMETERS );
        }
    }

if ( status != FT_OK )
    show_error( sc, status, SET_VEL_DIVIDEND_STRING );

return status;
}

/***********************************************************************
*                         DO_SEND_DATA_REQUEST                         *
***********************************************************************/
BOOLEAN do_send_data_request( TCHAR * s )
{
BOARD_DATA x;
STRING_CLASS sc;

sc = s;

if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    x = (BOARD_DATA) hextoul( s );
    SureTrakBoard.send_data( x );
    return FT_OK;
    }

MessageBox( 0, sc.text(), resource_string(SEND_DATA_ERROR_STRING), MB_OK | MB_SYSTEMMODAL );
return FT_ERROR;
}

/***********************************************************************
*                            CHECK_RUNLIST                             *
***********************************************************************/
static void check_runlist()
{
TCHAR * cp;
TCHAR * buf;

NeedRunlistCheck = FALSE;

if ( !HaveRunlist )
    return;

cp = Runlist.find_part( SureTrakInputWord );

if ( *cp != TEXT('\0') && !strings_are_equal(cp, SureTrakPart.part) )
    {
    if ( part_exists( SureTrakPart.computer, SureTrakPart.machine, cp) )
        {
        buf = maketext( cp );
        if ( buf )
            MainWindow.post( WM_POKEMON, (WPARAM) 0, (LPARAM) buf );
        }
    else
        {
        MessageBox( 0, resource_string(NO_WARMUP_PART_STRING), cp, MB_OK | MB_SYSTEMMODAL );
        }
    }
}

/***********************************************************************
*                    DO_SEND_COMMAND_BUSY_REQUEST                      *
***********************************************************************/
BOOLEAN do_send_command_busy_request( TCHAR * s )
{
BOARD_DATA x;
STRING_CLASS sc;
int status;

sc = s;

status = FT_ERROR;

if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    x = (BOARD_DATA) hextoul( s );
    status = SureTrakBoard.send_command_busy( x );
    }

if ( status != FT_OK )
    show_error( sc, status, SEND_CMD_BUSY_ERROR_STRING );

return status;
}

/***********************************************************************
*                       PARSE_COMMAND_THEN_DATA                        *
***********************************************************************/
BOOLEAN parse_command_then_data( BOARD_DATA & command, BOARD_DATA & data, TCHAR * s )
{

/*
-----------------------
Skip the request string
----------------------- */
if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    if ( replace_char_with_null(s, CommaChar) )
        {
        command = (BOARD_DATA) hextoul( s );
        s = nextstring( s );
        data = (BOARD_DATA) hextoul( s );
        return TRUE;
        }
    }
return FALSE;
}

/***********************************************************************
*                  DO_SEND_DATA_COMMAND_BUSY_REQUEST                   *
***********************************************************************/
static BOOLEAN do_send_data_command_busy_request( TCHAR * cp )
{
BOARD_DATA command;
BOARD_DATA data;
STRING_CLASS sc;
int          status;

sc     = cp;
status = FT_OK;

if ( parse_command_then_data(command, data, cp) )
    {
    SureTrakBoard.send_data( data );
    status = SureTrakBoard.send_command_busy( command );
    if ( status == FT_OK )
        return TRUE;
    }

show_error( sc, status, SEND_DATA_AND_CMD_ERROR_STRING );

return FALSE;
}

/***********************************************************************
*                              SET_OPTO                                *
***********************************************************************/
int set_opto( BOARD_DATA x )
{

SureTrakBoard.send_data( x );
return SureTrakBoard.send_command_busy( WR_CONT_OPTO );
}

/***********************************************************************
*                   DO_PROFILE_DOWNLOAD_REQUEST                        *
***********************************************************************/
BOOLEAN do_profile_download_request( void )
{
int status;

Downloading = true;
status = send_profile_to_board();
Downloading = false;
if ( status == FT_OK )
    return TRUE;

MessageBox( 0, int32toasc((int32) status), resource_string(PROFILE_DOWNLOAD_ERROR_STRING), MB_OK | MB_SYSTEMMODAL );

return FALSE;
}

/***********************************************************************
*                   DO_PROGRAM_DOWNLOAD_REQUEST                        *
***********************************************************************/
BOOLEAN do_program_download_request( void )
{
const DWORD MS_TO_WAIT    = 100;
const int32 MAX_LOOPS     = 10;

BOOLEAN control_was_active;
BOOLEAN status;
BOARD_DATA x;
int32 loop_count;

STRING_CLASS s;

control_was_active = FALSE;
status             = FALSE;

Downloading = true;
if ( SureTrakBoard.read_status() & CVIS_CONTROL_ACTIVE )
    {
    control_was_active = TRUE;
    SureTrakBoard.send_command_busy( CLEAR_CONTROL_MODE );
    loop_count = MAX_LOOPS;
    while ( TRUE )
        {
        Sleep( MS_TO_WAIT );
        x = SureTrakBoard.read_status();
        if ( !(x & CVIS_CONTROL_ACTIVE) )
            {
            SureTrakStatus = x;
            MainScreenWindow.post( WM_STATUS_CHANGE );
            break;
            }
        loop_count--;
        if ( !loop_count )
            {
            resource_message_box( MainInstance, CLEAR_CONTROL_MODE_ERROR_STRING, SURETRAK_STRING );
            break;
            }

        }

    }

s = control_program_name( SureTrakPart.computer );
if ( compile_and_send(s.text()) == FT_OK )
    status = TRUE;
else
    resource_message_box( MainInstance, CC_ERROR_STRING, SURETRAK_STRING );

if ( control_was_active )
    SureTrakBoard.send_command_busy( SET_CONTROL_ACTIVE );

Downloading = false;
return status;
}

/***********************************************************************
*                     DO_SEND_COMMAND_REQUEST                          *
***********************************************************************/
BOOLEAN do_send_command_request( TCHAR * s )
{
BOARD_DATA x;
int status;

status = FT_ERROR;

if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    x = (BOARD_DATA) hextoul( s );
    status = SureTrakBoard.send_command( x );
    if ( status == FT_OK )
        return TRUE;
    }

MessageBox( 0, ultohex( (unsigned long) status ), resource_string(SEND_CMD_ERROR_STRING), MB_OK | MB_SYSTEMMODAL );
return FALSE;
}

/***********************************************************************
*                      DO_NULL_VALVE_INIT_REQUEST                      *
***********************************************************************/
void do_null_valve_init_request( void )
{

if ( NullValveWindow )
    set_text( NullValveWindow, NULL_STATUS_TBOX, resource_string( MainInstance, NULL_VALVE_INIT_STRING) );

Sleep( MsForHoldingPoint );

/*
------------------------------
Set the valve test output wire
------------------------------ */
set_opto( VALVE_TEST_WIRE_BIT );

SureTrakBoard.send_command_busy( NULL_VALVE );

Sleep( MsForSettling );

if ( NullValveWindow )
    set_text( NullValveWindow, NULL_STATUS_TBOX, EmptyString );
}

/***********************************************************************
*                        READ_VELOCITY_DIVIDEND                        *
***********************************************************************/
static BOOLEAN read_velocity_dividend( uint32 * dest )
{
BOARD_DATA x;
uint32     i;

if ( SureTrakBoard.read_parameter(RD_VEL_DIVIDEND+1, &x) == FT_OK )
    {
    i = (uint32) x;
    i <<= 16;
    if ( SureTrakBoard.read_parameter(RD_VEL_DIVIDEND, &x) == FT_OK )
        {
        i += (uint32) x;
        if ( i == 0 )
            i = 1;
        *dest = i;
        return TRUE;
        }
    }

*dest = 1;
return FALSE;
}

/***********************************************************************
*                       AVERAGE_CHANNEL_1_VALUE                        *
***********************************************************************/
static BOOLEAN average_channel_1_value( float * dest )
{
const double divisor = 4094.0;   /* sum/10 * 5 / 2047 */
int        i;
BOARD_DATA x;
double     sum;

*dest = 0.0;
sum   = 0.0;

for ( i=0; i<10; i++ )
    {
    if ( SureTrakBoard.read_parameter(CCUR_ANA1, &x) != FT_OK )
        return FALSE;

    sum  += (double) (short) x;
    }

sum /= divisor;


*dest = (float) sum;

return TRUE;
}

/***********************************************************************
*                            VOLTAGE_LEVEL                             *
***********************************************************************/
static int32 voltage_level( float & v )
{

if ( !average_channel_1_value(&v) )
    return NULL_VOLTAGE_READ_FAILURE;

if ( v <= .035 && v >= -.035 )
    return OK_NULL_VOLTAGE_LEVEL;
else if ( v <= 0.500 && v >= -0.500 )
    return BALLPARK_NULL_VOLTAGE_LEVEL;

return UNUSABLE_NULL_VOLTAGE_LEVEL;
}

/***********************************************************************
*                            VOLTAGE_LEVEL                             *
***********************************************************************/
static int32 voltage_level( void )
{
float v;
return voltage_level(v);
}

/***********************************************************************
*                          DO_TRANSDUCER_TEST                          *
***********************************************************************/
void do_transducer_test( void )
{
BOARD_DATA cmd;
BOARD_DATA x;
int32      i;

if ( SureTrakBoard.send_command(CHECK_XDCR) == FT_OK )
     {
     Sleep( MsForTransducerTest );
     SureTrakBoard.wait_for_busy_bit_clear();
     }

cmd = 0x6800 + 4;

for ( i=0; i<500; i++ )
    {
    SureTrakBoard.read_parameter( cmd, &x );
    if ( x & 0x0001)
        Channel_A_Bits.set( i );
    else
        Channel_A_Bits.clear( i );

    if ( x & 0x0020)
        Channel_B_Bits.set( i );
    else
        Channel_B_Bits.clear( i );
    cmd++;
    }

if ( TransducerTestWindow )
    InvalidateRect( TransducerTestWindow, 0, TRUE );

}

/***********************************************************************
*                           SHOW_NULL_STATUS                           *
***********************************************************************/
static void show_null_status( UINT id )
{
if ( NullValveWindow )
    set_text( NullValveWindow, NULL_STATUS_TBOX, resource_string( MainInstance, id) );

}


/***********************************************************************
*                        DO_IO_PORT_TEST_REQUEST                       *
***********************************************************************/
void do_io_port_test_request( void )
{
const BOARD_DATA MASK_ONE = 0x5555;
const BOARD_DATA MASK_TWO = 0xAAAA;
const DWORD MS_TO_WAIT    = 100;

UINT id;

id = IO_PORT_OK_STRING;

SureTrakBoard.send_data( MASK_ONE );
SureTrakBoard.send_command_wo_check( CDATA_REG_WR_TST );
Sleep( MS_TO_WAIT );

if ( SureTrakBoard.read_status() != MASK_ONE || SureTrakBoard.read_data() != MASK_ONE )
    {
    id = IO_PORT_DATA_ERROR_STRING;
    }
else
    {
    SureTrakBoard.send_data( MASK_TWO );
    SureTrakBoard.send_command_wo_check( CDATA_REG_WR_TST );
    Sleep( MS_TO_WAIT );
    if ( SureTrakBoard.read_status() != MASK_TWO || SureTrakBoard.read_data() != MASK_TWO )
        id = IO_PORT_DATA_ERROR_STRING;
    }

if ( id == IO_PORT_OK_STRING )
    {
    /*
    -------------------------------------
    Put card in mode to test command port
    ------------------------------------- */
    SureTrakBoard.send_command_wo_check( CSET_REG_TST_FLG );
    Sleep( MS_TO_WAIT );

    /*
    ----------------
    Send bit pattern
    ---------------- */
    SureTrakBoard.send_command_wo_check( MASK_ONE );
    Sleep( MS_TO_WAIT );
    if ( SureTrakBoard.read_status() != MASK_ONE || SureTrakBoard.read_data() != MASK_ONE )
        id = IO_PORT_COMMAND_ERROR_STRING;

    SureTrakBoard.send_command_wo_check( MASK_TWO );
    Sleep( MS_TO_WAIT );
    if ( SureTrakBoard.read_status() != MASK_TWO || SureTrakBoard.read_data() != MASK_TWO )
        id = IO_PORT_COMMAND_ERROR_STRING;
     }

if ( ServiceToolsWindow )
    set_text( ServiceToolsWindow, ACTION_STATUS_TEXTBOX, resource_string(MainInstance, id) );

/*
--------------------------
Send a NMI to restart card
-------------------------- */
SureTrakBoard.set_command_mode();
}

/***********************************************************************
*                      DO_OPTO_PORT_TEST_REQUEST                       *
***********************************************************************/
void do_opto_port_test_request( void )
{
BOARD_DATA x;
UINT       id;

SureTrakBoard.send_command_wo_check( TEST_C_DIO );

Sleep( MsForOptoPortTest );

while ( SureTrakBoard.read_status() & VIS_BUSY )
    ;

x = SureTrakBoard.read_data();

if ( x == 0 )
    id = OPTO_PORT_OK_STRING;
else
    id = OPTO_PORT_ERROR_STRING;

if ( ServiceToolsWindow )
    set_text( ServiceToolsWindow, ACTION_STATUS_TEXTBOX, resource_string(MainInstance, id) );
}

/***********************************************************************
*                         DO_RAM_TEST_REQUEST                          *
***********************************************************************/
void do_ram_test_request( void )
{
BOARD_DATA x;
STRING_CLASS s;

SureTrakBoard.send_command_wo_check( CRAM_TEST );
Sleep( MsForRamTest );

while ( SureTrakBoard.read_status() & VIS_BUSY )
    ;

x = SureTrakBoard.read_data();

if ( x == 0 )
    {
    s = resource_string( MainInstance, NO_RAM_ERRORS_STRING );
    }
else if ( x & 0x0001 )
    {
    s = resource_string( MainInstance, HIGH_RAM_ERROR_STRING );
    s += ultohex( (unsigned long) x );
    }
else
    {
    s = resource_string( MainInstance, LOW_RAM_ERROR_STRING );
    s += ultohex( (unsigned long) x );
    }

if ( ServiceToolsWindow )
    set_text( ServiceToolsWindow, ACTION_STATUS_TEXTBOX, s.text() );
}

/***********************************************************************
*                         DO_AUTO_NULL_REQUEST                         *
***********************************************************************/
static void do_auto_null_request( void )
{
int32 current_null_offset;
int32 level;
int32 i;
short x;
short dx;
UINT  id;
float v;
STRING_CLASS s;

current_null_offset = SureTrakParameters.null_dac_offset();

level = voltage_level(v);
switch ( level )
    {
    case OK_NULL_VOLTAGE_LEVEL:
        show_null_status( NULL_ALREADY_GOOD_STRING );
        return;

    case BALLPARK_NULL_VOLTAGE_LEVEL:
        if ( v < 0.0 )
            dx = 1;
        else
            dx = -1;

        x = 0;

        for( i=0; i<=400; i++ )
            {
            s = resource_string( MainInstance, AUTO_NULL_SETTING_STRING );
            s += int32toasc( x );
            if ( NullValveWindow )
                set_text( NullValveWindow, NULL_STATUS_TBOX, s.text() );

            SureTrakBoard.set_parameter( SET_NULL, (BOARD_DATA) x );
            SureTrakBoard.send_command_busy( UPDATE_PARAMETERS );
            SureTrakBoard.send_command_busy( NULL_VALVE );

            Sleep(25);

            level = voltage_level();
            if ( level == OK_NULL_VOLTAGE_LEVEL )
                {
                if ( x == 0 )
                    id = AUTO_NULL_ZERO_SUCCESS_STRING;
                else
                    id = AUTO_NULL_SUCCESS_STRING;

                show_null_status( id );
                SureTrakParameters.set_null_dac_offset( x );
                return;
                }
            else if ( level == NULL_VOLTAGE_READ_FAILURE )
                break;

            x += dx;
            }
        break;
    }

/*
------------------------------------------------------------------
I couldn't set the level, return the board to the original setting
------------------------------------------------------------------ */
x = current_null_offset;

SureTrakBoard.set_parameter( SET_NULL, (BOARD_DATA) x );
SureTrakBoard.send_command_busy( UPDATE_PARAMETERS );
SureTrakBoard.send_command_busy( NULL_VALVE );

switch ( level )
    {
    case BALLPARK_NULL_VOLTAGE_LEVEL:
        id = AUTO_NULL_FAIL_STRING;
        break;

    case UNUSABLE_NULL_VOLTAGE_LEVEL:
        id = UNUSABLE_NULL_LEVEL_STRING;
        break;

    default:
        id = PARAMETER_READ_FAIL_STRING;
        break;
    }

show_null_status( id );
}

/***********************************************************************
*                       DO_SET_VARIABLE_REQUEST                        *
***********************************************************************/
void do_set_variable_request( TCHAR * s )
{
BOARD_DATA command;
BOARD_DATA data;

if ( parse_command_then_data(command, data, s) )
    {
    if ( command <= MAX_ST_VARIABLE_NUMBER )
        {
        command += 0xff80;
        SureTrakBoard.set_parameter( command, data );
        }
    }
}

/***********************************************************************
*                       DO_READ_VARIABLE_REQUEST                        *
***********************************************************************/
void do_read_variable_request( TCHAR * s )
{
BOARD_DATA command;
BOARD_DATA x;

if ( replace_char_with_null(s, CommaChar) )
    {
    s = nextstring( s );
    command = (BOARD_DATA) hextoul( s );
    if ( command <= MAX_ST_VARIABLE_NUMBER )
        {
        command += 0x7f80;
        if ( SureTrakBoard.read_parameter(command, &x) == FT_OK )
            {
            if ( VariablesWindow )
                {
                set_text( VariablesWindow, VARIABLE_VALUE_EBOX, ultoascii((unsigned long) x) );
                }
            }
        }
    }
}


/***********************************************************************
*                              VALVE_TEST                              *
***********************************************************************/
void valve_test( void )
{

if ( SureTrakBoard.send_command_busy(START_VALVE_TEST) == FT_OK )
    {
    Sleep( MsForVTSettling );
    SureTrakBoard.wait_for_busy_bit_clear();
    }

VTCycleCount = MsForVTCycle / MsForMonitorCycle;
}

/***********************************************************************
*                           DO_VALVE_TEST_REQUEST                      *
***********************************************************************/
static void do_valve_test_request( void )
{

int32 count;
BOARD_DATA x;

count = 0;

/*
--------------------
Wait for manual mode
-------------------- */
while ( TRUE )
    {
    if ( SureTrakBoard.read_parameter(RD_INPUT_WORD, &x) != FT_OK )
        return;

    if ( count > 3 )
        break;

    if ( x & MANUAL_MODE_BIT )
        break;

    if ( !ValveTestWindow.handle() )
        return;

    if ( resource_message_box(ValveTestWindow.handle(), MainInstance, PRESS_OK_WHEN_READY_STRING, NOT_MANUAL_MODE_STRING, MB_OKCANCEL | MB_SYSTEMMODAL) == IDCANCEL )
        {
        ValveTestWindow.close();
        return;
        }

    count++;
    }

ControlWasActive = FALSE;

if ( SureTrakBoard.read_status() & CVIS_CONTROL_ACTIVE )
    {
    ControlWasActive = TRUE;
    SureTrakBoard.send_command_busy( CLEAR_CONTROL_MODE );
    }

set_opto( VALVE_TEST_WIRE_BIT );

/*
-----------------------------------
Tell the monitor loop to test valve
----------------------------------- */
TestingValve = TRUE;
}

/***********************************************************************
*                         SEND_VT_DATA_TO_PLOT                         *
***********************************************************************/
static void send_vt_data_to_plot( void )
{
int32 i;
float pos;
BOARD_DATA x;
BOARD_DATA a;

a   = 0x7000;
pos = 0.0;
for ( i=0; i<NofValveTestPoints; i++ )
    {
    SureTrakBoard.read_parameter(a, &x);
    ValveTestData[i].y = (float) x;
    ValveTestData[i].y /= 409.5;
    ValveTestData[i].x = pos;
    pos += 2.0;
    a++;
    }

ValveTestWindow.post( WM_NEW_DATA );
}

/***********************************************************************
*                         CHECK_FOR_VALVE_TEST                         *
***********************************************************************/
static void check_for_valve_test( void )
{

if ( !TestingValve )
    return;

if ( !ValveTestWindow.handle() )
    {
    Sleep( MsAfterVT );
    set_opto( 0 );
    if ( ControlWasActive )
        SureTrakBoard.send_command_busy( SET_CONTROL_ACTIVE );
    ControlWasActive = FALSE;
    TestingValve = FALSE;
    return;
    }

VTCycleCount--;
if  ( VTCycleCount <= 0 )
    {
    send_vt_data_to_plot();
    valve_test();
    }

}

/***********************************************************************
*                          CHECK_FOR_COMMAND                           *
***********************************************************************/
static void check_for_command( void )
{
TCHAR    * cp;
BOARD_DATA x;
int        i;

while ( TRUE )
    {
    cp = SureTrakCommand.pop();
    if ( !cp )
        break;

    x = (BOARD_DATA) hextoul( cp );
    if ( x > 0x0100 )
        {
        SureTrakBoard.send_command( x );
        }
    else
        {
        i = x;
        switch ( i )
            {
            case DEACTIVATE_CONTROL_REQUEST:
                SureTrakBoard.send_command_busy( CLEAR_CONTROL_MODE );
                break;

            case ACTIVATE_CONTROL_REQUEST:
                SureTrakBoard.send_command_busy( SET_CONTROL_ACTIVE );
                break;

            case CONTROLLER_RESET_REQUEST:
                SureTrakBoard.set_command_mode();
                break;

            case SET_VEL_DIVIDEND_REQUEST:
                set_vel_dividend( cp );
                break;

            case SET_PARAMETER_REQUEST:
                do_set_parameter_request( cp );
                break;

            case SEND_DATA_REQUEST:
                do_send_data_request( cp );
                break;

            case SEND_COMMAND_REQUEST:
                do_send_command_request( cp );
                break;

            case SEND_COMMAND_BUSY_REQUEST:
                do_send_command_busy_request( cp );
                break;

            case SEND_DATA_COMMAND_BUSY_REQUEST:
                do_send_data_command_busy_request( cp );
                break;

            case NULL_VALVE_INIT_REQUEST:
                do_null_valve_init_request();
              break;

            case NULL_VALVE_FINISH_REQUEST:
                set_opto( 0 );
                break;

            case AUTO_NULL_REQUEST:
                do_auto_null_request();
                break;

            case VALVE_TEST_REQUEST:
                do_valve_test_request();
                break;

            case READ_VARIABLE_REQUEST:
                do_read_variable_request( cp );
                break;

            case RAM_TEST_REQUEST:
                do_ram_test_request();
                break;

            case SET_VARIABLE_REQUEST:
                do_set_variable_request( cp );
                break;

            case PROFILE_DOWNLOAD_REQUEST:
                do_profile_download_request();
                break;

            case PROGRAM_DOWNLOAD_REQUEST:
                do_program_download_request();
                break;

            case READ_CONTROL_PARAMETERS_REQUEST:
                do_read_control_parameters_request();
                break;

            case TRANSDUCER_TEST_REQUEST:
                do_transducer_test();
                break;

            case OPTO_PORT_TEST_REQUEST:
                do_opto_port_test_request();
                break;

            case IO_PORT_TEST_REQUEST:
                do_io_port_test_request();
                break;
            }
        }
    }

}

/***********************************************************************
*                         UPDATE_SURETRAK_POS                          *
***********************************************************************/
static BOOLEAN update_suretrak_pos( void )
{
BOARD_DATA x;

if ( SureTrakBoard.read_parameter(CCUR_POS, &x) == FT_OK )
    {
    if ( x != SureTrakRawPosition )
        {
        SureTrakRawPosition = x;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                     CHECK_FOR_NULL_VALVE_DISPLAY                     *
***********************************************************************/
static void check_for_null_valve_display( void )
{
static bool first_time = true;
BOARD_DATA x;

if ( NullValveWindow )
    {
    if ( SureTrakBoard.read_parameter(CCUR_ANA1, &x) == FT_OK )
        {
        if ( x != SureTrakAnalogValue[0] || first_time )
            {
            SureTrakAnalogValue[0] = x;
            PostMessage( NullValveWindow, WM_NEW_DATA, 0, 0L );
            first_time = false;
            }
        }
    }
else
    {
    first_time = true;
    }
}


/***********************************************************************
*                       CHECK_FOR_ANALOG_DISPLAY                       *
***********************************************************************/
static void check_for_analog_display( void )
{
struct ANALOG_COMMAND_ENTRY {
    BOARD_DATA command;
    UINT control;
    };

const ANALOG_COMMAND_ENTRY analog[MAX_ST_CHANNELS]=
    {
    {CCUR_ANA1, CURRENT_NULL_VOLTAGE_TEXTBOX},
    {CCUR_ANA2, CURRENT_ANALOG_2_TEXTBOX},
    {CCUR_ANA3, CURRENT_ANALOG_3_TEXTBOX},
    {CCUR_ANA4, CURRENT_ANALOG_4_TEXTBOX}
    };

static BOOLEAN first_time = TRUE;

int32      i;
BOARD_DATA x;
bool       need_update;

need_update = false;

if ( !CurrentAnalogWindow )
    {
    first_time = TRUE;
    return;
    }

for ( i=0; i<MAX_ST_CHANNELS; i++ )
    {
    if ( SureTrakBoard.read_parameter(analog[i].command, &x) == FT_OK )
        {
        if ( first_time || SureTrakAnalogValue[i] != x )
            {
            SureTrakAnalogValue[i] = x;
            need_update = true;
            }
        }
    else
        {
        set_text( CurrentAnalogWindow, analog[i].control, resource_string(MainInstance, BAD_READ_STRING) );
        }
    }

if ( SureTrakBoard.read_parameter(CCUR_VEL, &x) == FT_OK )
    {
    if ( SureTrakVelocity != x )
        {
        SureTrakVelocity = x;
        need_update = true;
        }
    }
else
    {
    set_text( CurrentAnalogWindow, CURRENT_VELOCITY_TEXTBOX, resource_string(MainInstance, BAD_READ_STRING) );
    }

if ( need_update )
    PostMessage( CurrentAnalogWindow, WM_NEW_DATA, 0, 0L );

first_time = FALSE;
}

/***********************************************************************
*                           UPDATE_RAM_BLOCK                           *
***********************************************************************/
static void update_ram_block( void )
{
BOARD_DATA sorc;
BOARD_DATA x;
int32      i;
uint32   * dest;
uint32   * dp;

dest = new uint32[2000];
if ( !dest )
    {
    resource_message_box( MainInstance, OUT_OF_MEMORY, RAM_BLOCK_ALLOCATE );
    return;
    }

dp   = dest;

sorc = 0x800 * (BOARD_DATA) RamBlockNumber;

if ( is_checked(RamBlockWindow, HIGH_RAM_BUTTON) )
    sorc += 0x4000;

for ( i=0; i<2000; i++ )
    {
    if ( RamBlockNumber < 0 )
        {
        delete[] dest;
        return;
        }

    if ( LoadStatusWindow )
        set_text( LoadStatusWindow, RAM_LOC_TBOX, ultoascii((unsigned long) sorc) );

    if ( SureTrakBoard.read_parameter( sorc, &x) == FT_OK )
        *dp = (uint32) x;
    else
        *dp = 0xFFFFL;

    if ( LoadStatusWindow )
        set_text( LoadStatusWindow, RAM_VALUE_TBOX, ultoascii(*dp) );

    sorc++;
    dp++;
    }

if ( RamBlockNumber < 0 )
    {
    delete[] dest;
    return;
    }

PostMessage( RamBlockWindow, WM_NEW_DATA, 0, (LPARAM) dest );
}

/***********************************************************************
*                       SURETRAK_MONITOR_THREAD                        *
***********************************************************************/
DWORD suretrak_monitor_thread( int * notused )
{

DWORD      status;
BOARD_DATA x;
BOOLEAN    need_update;
BOOLEAN    has_program;
TCHAR    * cp;

SureTrakMonitorIsRunning = TRUE;
SureTrakBoard.setports( DEFAULT_SURETRAK_PORT_ADDRESS );
SureTrakStatus      = 0;
SureTrakInputWord   = 0;
SureTrakOutputWord  = 0;
SureTrakBlockNumber = 0;
SureTrakHasProgram  = FALSE;
need_update         = TRUE;

SureTrakBoard.startup();

cp = get_ini_string( TEXT("monall.ini"), TEXT("Config"), TEXT("FtClearCmdWait") );
if ( !unknown(cp) )
    SureTrakBoard.set_wait_count( (int) asctoint32(cp) );

if ( SureTrakBoard.read_parameter(RD_PROG_DOWNLOADED, &x) == FT_OK )
    {
    if ( x == 0 )
        do_program_download_request();
    else
        SureTrakHasProgram = TRUE;
    }
else
    {
    resource_message_box( MainInstance, NO_SURETRAK_BOARD_STRING, PARAMETER_READ_FAIL_STRING );
    }

while ( TRUE )
    {
    if ( ShuttingDown )
        {
        SureTrakMonitorIsRunning = FALSE;
        MainWindow.close();
        break;
        }

    x = SureTrakBoard.read_status();
    if ( x != SureTrakStatus )
        {
        /*
        --------------------------------------------------
        Read the error number if this is a new fatal error
        -------------------------------------------------- */
        if ( (x & CVIS_CONTROL_ERR) && !(SureTrakStatus & CVIS_CONTROL_ERR) )
            SureTrakBoard.read_parameter( RD_CONT_ERR_WORD, &SureTrakFatalErrorNumber );
        SureTrakStatus = x;
        need_update    = TRUE;
        }

    check_for_command();

    check_for_valve_test();

    /*
    ---------------------------------------------------------------
    Only update the parameters if the service tools are not running
    --------------------------------------------------------------- */
    if ( NeedMonitorUpdate )
        {
        if ( SureTrakBoard.read_parameter(RD_PROG_DOWNLOADED, &x) == FT_OK )
            {
            if ( x == 0 )
                has_program = FALSE;
            else
                has_program = TRUE;

            if ( has_program != SureTrakHasProgram )
                {
                SureTrakHasProgram = has_program;
                need_update    = TRUE;
                }
            }

        if ( SureTrakBoard.read_parameter(RD_INPUT_WORD, &x) == FT_OK )
            {
            if ( x != SureTrakInputWord )
                {
                NeedRunlistCheck = TRUE;
                SureTrakInputWord = x;
                need_update    = TRUE;
                }

            if ( NeedRunlistCheck )
                check_runlist();

            }

        if ( SureTrakBoard.read_parameter(RD_OUTPUT_WORD, &x) == FT_OK )
            {
            if ( x != SureTrakOutputWord )
                {
                SureTrakOutputWord = x;
                need_update    = TRUE;
                }
            }

        if ( SureTrakBoard.read_parameter(GET_BLOCK_PTR, &x) == FT_OK )
            {
            x -= 0xF000;
            x /= 30;
            x++;

            if ( SureTrakBlockNumber != x )
                {
                SureTrakBlockNumber = x;
                need_update = TRUE;
                }
            }

        if ( update_suretrak_pos() )
            need_update = TRUE;


        if ( SureTrakBoard.read_parameter(RD_CURRENT_DAC, &x) == FT_OK )
            {
            x &= DAC_BITS;
            if ( SureTrakDacWord != x )
                {
                SureTrakDacWord = x;
                need_update = TRUE;
                }
            }
        }

    check_for_analog_display();
    check_for_null_valve_display();

    if ( need_update )
        {
        MainScreenWindow.post( WM_STATUS_CHANGE );
        need_update = FALSE;
        }

    if ( PromNumberWindow )
        {
        if ( SureTrakBoard.read_parameter(GET_C_VER_NUMBER, &x) == FT_OK )
            cp = ultoascii( (unsigned long) x );
        else
            cp = resource_string( MainInstance, BAD_READ_STRING );

        if ( cp )
            set_text( PromNumberWindow, cp );

        PromNumberWindow = 0;
        }

    if ( DataPortWindow )
        {
        x = SureTrakBoard.read_data();
        set_text( DataPortWindow, ultohex((unsigned long) x) );
        DataPortWindow = 0;
        }

    if ( RamBlockNumber >= 0 )
        {
        update_ram_block();
        RamBlockNumber = -1;
        }

    status = WaitForSingleObject( SureTrakMonitorEvent, MsForMonitorCycle );
    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, SURETRAK_WAIT_FAIL_STRING );
    }

SureTrakBoard.shutdown();
return 0;
}

/***********************************************************************
*                     START_SURETRAK_MONITOR_THREAD                    *
***********************************************************************/
void start_suretrak_monitor_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) suretrak_monitor_thread, /* thread function       */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, SURETRAK_THREAD_STRING );

}
