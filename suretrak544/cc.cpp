#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"
#include "..\include\fileclas.h"
#include "..\include\ftclass.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\plotclas.h"
#include "..\include\stparam.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "extern.h"

extern BOARD_CONTROL_CLASS SureTrakBoard;

static float PosDivisor = 0.0125;  /* Default setting. */

static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR ZeroChar  = TEXT( '0' );
static const TCHAR OneChar   = TEXT( '1' );
static const TCHAR TwoChar   = TEXT( '2' );
static const TCHAR ThreeChar = TEXT( '3' );
static const TCHAR FourChar  = TEXT( '4' );
static const TCHAR NineChar  = TEXT( '9' );
static const TCHAR AChar     = TEXT( 'A' );
static const TCHAR VChar     = TEXT( 'V' );
static const TCHAR Asterix   = TEXT( '*' );
static const TCHAR MinusChar = TEXT( '-' );
static const TCHAR CommaChar = TEXT( ',' );
static const TCHAR Period    = TEXT( '.' );
static const TCHAR SpaceChar = TEXT( ' ' );

static const TCHAR AccelCmdString[]         = TEXT("AC");
static const TCHAR EmptyString[]            = TEXT("");
static const TCHAR LowImpactDacCmdString[]  = TEXT("LD");
static const TCHAR LongDwellCmdString[]     = TEXT("LD");
static const TCHAR ShortDwellCmdString[]    = TEXT("SD");
static const TCHAR LowImpactCmdString[]     = TEXT("LI");
static const TCHAR InputCmdString[]         = TEXT("I");
static const TCHAR OutputCmdString[]        = TEXT("B");
static const TCHAR WhileCmdString[]         = TEXT("W");
static const TCHAR DuringCmdString[]        = TEXT("D");
static const TCHAR EndCmdString[]           = TEXT("E");
static const TCHAR PositionCmdString[]      = TEXT("P");
static const TCHAR GooseCmdString[]         = TEXT("G");

static const TCHAR CcErrorString[]          = TEXT( "CC Error" );
static const TCHAR CcBoardString[]          = TEXT( "CC Board Error, status = " );

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status( TCHAR * s )
{
set_text( MainScreenWindow.handle(), WAITING_FOR_TEXTBOX, s );
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status( UINT id )
{
show_status( resource_string(id) );
}

/***********************************************************************
*                              SHOW_ERROR                              *
***********************************************************************/
static void show_error( UINT id )
{
resource_message_box( MainInstance, id, ERROR_TITLE_ID_STRING );
}

/***********************************************************************
*                              SHOW_ERROR                              *
***********************************************************************/
static void show_error( TCHAR * s )
{

MessageBox( NULL, s, CcErrorString, MB_OK | MB_SYSTEMMODAL );

}

/***********************************************************************
*                              SHOW_ERROR                              *
***********************************************************************/
static void show_error( int status, TCHAR * s )
{
STRING_CLASS sc;

sc = CcBoardString;
sc += int32toasc( (int32) status );

MessageBox( NULL, s, sc.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                              SHOW_ERROR                              *
***********************************************************************/
static void show_error( int status, UINT id )
{
STRING_CLASS sc;

sc = CcBoardString;
sc += int32toasc( (int32) status );

MessageBox( NULL, resource_string(id), sc.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                               GET_INT                                *
* This will extract only positive intergers, signs or decimal point    *
* will terminate the extraction.                                       *
***********************************************************************/
static int32 get_int( TCHAR * sorc )
{
TCHAR buf[MAX_INTEGER_LEN+1];
TCHAR * dest;
int32 i;

i = 0;
dest = buf;

while ( *sorc >= ZeroChar && *sorc <= NineChar )
    *dest++ = *sorc++;

*dest = NullChar;

if ( lstrlen(buf) > 0 )
    i = asctoint32( buf );

return i;
}

/***********************************************************************
*                            GET_BOARD_DATA                            *
***********************************************************************/
static BOARD_DATA get_board_data( TCHAR * sorc )
{
return (BOARD_DATA) get_int( sorc );
}

/***********************************************************************
*                        GET_SIGNED_BOARD_DATA                         *
***********************************************************************/
static BOARD_DATA get_signed_board_data( TCHAR * sorc )
{
TCHAR buf[MAX_INTEGER_LEN+1];
TCHAR * dest;
int32 i;
short si;

i = 0;
dest = buf;

while ( (*sorc == MinusChar) || (*sorc >= ZeroChar && *sorc <= NineChar) )
    *dest++ = *sorc++;

*dest = NullChar;

if ( lstrlen(buf) > 0 )
    i = asctoint32( buf );

si = (short) i;

return (BOARD_DATA) si;
}

/***********************************************************************
*                               GET_REAL                               *
***********************************************************************/
float get_real( TCHAR * sorc )
{
TCHAR buf[MAX_FLOAT_LEN+1];
TCHAR * dest;
float   y;

dest = buf;

while ( *sorc >= MinusChar && *sorc <= NineChar )
    *dest++ = *sorc++;
*dest = NullChar;

y = 0.0;

if ( lstrlen(buf) > 0 )
    y = extfloat( buf );

return y;
}

/***********************************************************************
*                        SEND_DATA_AND_COMMAND                         *
***********************************************************************/
static int send_data_and_command( BOARD_DATA data, BOARD_DATA command )
{
int          status;

SureTrakBoard.send_data( data );
status = SureTrakBoard.send_command_busy( command  );

if ( status != FT_OK )
    show_error( status, SEND_DATA_AND_CMD_ERROR_STRING );

return status;
}

/***********************************************************************
*                        SEND_DATA_AND_COMMAND                         *
***********************************************************************/
static int send_data_and_command( TCHAR * s, BOARD_DATA command )
{
return send_data_and_command( get_board_data(s), command );
}

/***********************************************************************
*                           VARIABLE_ADDRESS                           *
***********************************************************************/
static BOARD_DATA variable_address( TCHAR * sorc )
{
BOARD_DATA v;

v = (BOARD_DATA) get_int( sorc );
v *= 2;
v += 0xff00;
if ( v > 0xffc6 || v < 0xff00 )
    {
    v = 0;
    show_error( VAR_OUT_OF_RANGE_ERROR_STRING );
    }

return v;
}

/***********************************************************************
*                      SEND_VARIABLE_ADDRESS                           *
***********************************************************************/
static int send_variable_address( TCHAR * sorc, BOARD_DATA command )
{
int        status;
BOARD_DATA v;

v = variable_address( sorc );

if ( v )
    status = send_data_and_command( v, command );
else
    status = 96;
return status;
}

/***********************************************************************
*                          SEND_INPUT_NUMBER                           *
***********************************************************************/
int send_input_number( BOARD_DATA & var_indicator, BOARD_DATA mask, TCHAR ** sorc, BOARD_DATA command )
{
int32      i;
int        status;
TCHAR    * ptr;
BOARD_DATA x;

ptr = *sorc;

if (*ptr == VChar )
    {
    var_indicator |= mask;
    ptr++;

    status = send_variable_address( ptr, command );
    }

else if ( *ptr == Asterix )  /* Direct value to send, dont convert */
    {
    ptr++;
    status = send_data_and_command( ptr, command );
    }
else
    {
    i = get_int( ptr );

    x = 0;

    /* Now convert the input number to the correct value for the
       SureTrak board.  17 - 31 are valid input numbers. */

    switch ( i )
        {
        case 17:
            x = 0x0001;
            break;

        case 18:
            x = 0x0002;
            break;

        case 19:
            x = 0x0004;
            break;

        case 20:
            x = 0x0008;
            break;

        case 21:
            x = 0x0010;
            break;

        case 22:
            x = 0x0020;
            break;

        case 23:
            x = 0x0040;
            break;

        case 24:
            x = 0x0080;
            break;

        case 25:
            x = 0x0100;
            break;

        case 26:
            x = 0x0200;
            break;

        case 27:
            x = 0x0400;
            break;

        case 28:
            x = 0x0800;
            break;

        case 29:
            x = 0x1000;
            break;

        case 30:
            x = 0x2000;
            break;

        case 31:
            x = 0x4000;
            break;

        default:
            show_error( INPUT_OUT_OF_RANGE_ERROR_STRING );
            status = 99;
            break;
        }

    if ( x > 0 )
        status = send_data_and_command( x, command );
    }


*sorc = ptr;
return status;
}

/***********************************************************************
*                          PARSE_CONDITIONAL                           *
***********************************************************************/
int parse_conditional( int blk_no, TCHAR * conditional_str, BOARD_DATA & var_indicator )
{
TCHAR * ptr;

BOARD_DATA   base_command;
BOARD_DATA   input_check_word;
BOARD_DATA   x;
STRING_CLASS s;
int32        i;
int32        status;
BOOLEAN      set_out_flg;
BOOLEAN      test_in_flg;

input_check_word = 0;
test_in_flg      = FALSE;
set_out_flg      = FALSE;

/*
------------------------
Compute the base command
------------------------ */
base_command = ((blk_no * 15) - 15) + 0xf800;

/*
-----------------------------------------------------------------
Disable both testing inputs and setting outputs by writing 999 to
the approiate locations.  As the string is parsed, these will be
overwritten if testing inputs / setting outputs is needed.
----------------------------------------------------------------- */
status = send_data_and_command( (BOARD_DATA) 999, base_command + 11 );

if ( status == FT_OK )
    status = SureTrakBoard.send_command_busy( base_command + 13 );

if ( status != FT_OK )
    goto ext;

/*
------------
Check inputs
------------ */
ptr = findstring( InputCmdString, conditional_str );
if ( ptr )
    {
    ptr++;
    if ( *ptr == ZeroChar )  /* IF INPUT OFF condition */
        {
        test_in_flg = TRUE;
        status = send_data_and_command( (BOARD_DATA) 0, base_command + 11 );
        if ( status != FT_OK )
            goto ext;
        }

    else if (*ptr == OneChar )  /* IF INPUT ON condition */
        {
        test_in_flg = TRUE;
        status = send_data_and_command( (BOARD_DATA) 1, base_command + 11 );
        if ( status != FT_OK )
            goto ext;
        }

    else if (*ptr == AChar )  /* Check analog input */
        {
        ptr++;

        /*
        ----------------------------
        Determine the analog channel
        ---------------------------- */
        if ( *ptr == OneChar ) input_check_word |= 0;  /* No change */
        else if ( *ptr == TwoChar   ) input_check_word |= 0x0004;
        else if ( *ptr == ThreeChar ) input_check_word |= 0x0008;
        else if ( *ptr == FourChar  ) input_check_word |= 0x000c;
        else  /* Syntax error */
            {
            status = 99;
            goto ext;
            }

        /*
        ----------------
        Get the operator
        ---------------- */
        ptr++;
        if (*ptr == TEXT('<') )      {input_check_word |= 0x0010; ptr += 2;} /* <= */
        else if (*ptr == TEXT('>') ) {input_check_word |= 0x0020; ptr += 2;} /* >= */
        else if (*ptr == TEXT('=') ) {input_check_word |= 0x0030; ptr++;}    /* =  */
        else if (*ptr == TEXT('!') ) {input_check_word |= 0x0040; ptr += 2;} /* != */
        else
            {
            status = 99;
            goto ext;
            }

        /*
        -------------------------
        Send the input check word
        ------------------------- */
        status = send_data_and_command( input_check_word, base_command + 11 );
        if ( status != FT_OK )
            goto ext;

        /*
        --------------------------
        Get the analog check value
        -------------------------- */
        if ( *ptr == VChar )
            {
            var_indicator |= 0x0200;
            ptr++;

            status = send_variable_address( ptr, base_command + 9 );
            }
        else
            {
            x = get_board_data( ptr );
            if ( x > 4095 )
                {
                status = 99;
                goto ext;
                }

            status = send_data_and_command( x, base_command + 9 );
            }

        if ( status != FT_OK )
            goto ext;
        }
    else
        {
        status = 99;
        goto ext;
        }
    }
else
    {
    ptr = findstring( WhileCmdString, conditional_str );
    if ( ptr )
        {
        test_in_flg = TRUE;
        ptr++;
        if ( *ptr == ZeroChar )  /* WHILE INPUT OFF condition */
            {
            status = send_data_and_command( (BOARD_DATA) 2, base_command + 11 );
            }

        else if ( *ptr == OneChar )  /* WHILE INPUT ON condition */
            {
            status = send_data_and_command( (BOARD_DATA) 3, base_command + 11 );
            }

        else
            {
            show_error( WHILE_WO_1_OR_0_ERROR_STRING );
            status = 99;
            }

        if ( status != FT_OK )
            goto ext;
        }
    }

/*
-----------------------------
Get and send the input number
----------------------------- */
if ( test_in_flg )
    {
    ptr++;
    if ( *ptr == Period )
        {
        ptr++;
        status = send_input_number( var_indicator, 0x1000, &ptr, base_command + 12 );
        }
    else
        {
        show_error( NO_INPUT_DOT_ERROR_STRING );
        status = 99;
        }

    if ( status != FT_OK )
        goto ext;
    }

/*
-----------------
Check set outputs
----------------- */
ptr = findstring( OutputCmdString, conditional_str );
if ( ptr )
    {
    set_out_flg = TRUE;
    ptr++;
    if ( *ptr == ZeroChar )
        {
        show_status( BEGIN_TURN_OFF_STRING );
        status = send_data_and_command( (BOARD_DATA) 0, base_command + 13 );
        }

    else if ( *ptr == OneChar )
        {
        show_status( BEGIN_TURN_ON_STRING );
        status = send_data_and_command( (BOARD_DATA) 1, base_command + 13 );
        }

    else
        {
        show_error( B_WO_1_OR_0_ERROR_STRING );
        status = 99;
        }

    if ( status != FT_OK )
        goto ext;
    }

else if ( (ptr = findstring(DuringCmdString, conditional_str)) != 0 )
    {
    set_out_flg = TRUE;
    ptr++;
    if ( *ptr == OneChar )
        {
        show_status( DURING_TURN_ON_STRING );
        status = send_data_and_command( (BOARD_DATA) 3, base_command + 13 );
        }
    else
        {
        show_error( D_WO_1_ERROR_STRING );
        status = 99;
        }

    if ( status != FT_OK )
        goto ext;
    }
else if ( (ptr = findstring(EndCmdString, conditional_str)) != 0 )
    {
    set_out_flg = TRUE;
    ptr++;
    if (*ptr == ZeroChar )
        {
        show_status( AT_END_TURN_OFF_STRING );
        status = send_data_and_command( (BOARD_DATA) 4, base_command + 13 );
        }

    else if (*ptr == OneChar )
        {
        show_status( AT_END_TURN_ON_STRING );
        status = send_data_and_command( (BOARD_DATA) 5, base_command + 13 );
        }

    else
        {
        show_error( E_WO_1_OR_0_ERROR_STRING );
        status = 99;
        }

    if ( status != FT_OK )
        goto ext;
    }

/*
------------------------------
Get and send the output number
------------------------------ */
if ( set_out_flg )
    {
    ptr++;
    if ( *ptr != Period )
        {
        show_error( NO_OUTPUT_DOT_ERROR_STRING );
        status = 99;
        goto ext;
        }

    ptr++;

    if ( *ptr == VChar )
        {
        var_indicator |= 0x4000;
        ptr++;
        status = send_variable_address( ptr, base_command + 14 );
        }

    else if ( *ptr == Asterix )  /* Direct value to send, dont convert */
        {
        ptr++;
        status = send_data_and_command( ptr, base_command + 14 );
        }
    else
        {
        i = get_int( ptr );

        /*
        ------------------------------------------------------
        Convert the output number to the correct value for the
        SureTrak board.  1 - 16 are valid output numbers, but
        13 - 16 are reserved for limit switches.
        ------------------------------------------------------ */
        switch ( i )
            {
            case 1:
                x = 0x0001;
                break;

            case 2:
                x = 0x0002;
                break;

            case 3:
                x = 0x0004;
                break;

            case 4:
                x = 0x0008;
                break;

            case 5:
                x = 0x0010;
                break;

            case 6:
                x = 0x0020;
                break;

            case 7:
                x = 0x0040;
                break;

            case 8:
                x = 0x0080;
                break;

            case 9:
                x = 0x0100;
                break;

            case 10:
                x = 0x0200;
                break;

            case 11:
                x = 0x0400;
                break;

            case 12:
                x = 0x0800;
                break;

            /* Outputs 13, 14, 15 and 16 reserved for limit switches. */

            default:
                s = resource_string( BLOCK_NUMBER_STRING );
                s += int32toasc( blk_no );
                s += resource_string( OUTPUT_OUT_OF_RANGE_STRING );
                show_error( s.text() );
                status = 99;
                goto ext;
            }

        status = send_data_and_command( x, base_command + 14 );
        }

    if ( status != FT_OK )
        goto ext;
    }

ext:
return status;
}

/***************** PROGRAM BLOCK FUNCTIONS **********************/

/***********************************************************************
*                               VAL_STEP                               *
***********************************************************************/
int val_step( int blk_no, TCHAR * blkstr, TCHAR * conditional_str )
{
TCHAR    * ptr;
BOARD_DATA var_indicator;
BOARD_DATA base_command;
int        status;
BOARD_DATA x;
float      rx;

var_indicator = 0;

/*
------------------------
Compute the base command
------------------------ */
base_command = ((blk_no * 15) - 15) + 0xf800;

/*
-------------------
Send the block type
------------------- */
status = send_data_and_command( (BOARD_DATA) 0, base_command );
if ( status != FT_OK )
    goto ext;

ptr = blkstr;

/*
-----------------------
Extract the DAC Command
----------------------- */
ptr++;
if ( *ptr == VChar )
    {
    var_indicator |= 0x0002;
    ptr++;

    status = send_variable_address( ptr, base_command + 1 );
    if ( status != FT_OK )
        goto ext;
    }
else
    {
    x = get_signed_board_data( ptr );
    status = send_data_and_command( x, base_command + 1 );
    if ( status != FT_OK )
        goto ext;
    }

/*
-----------------------
Extract the accel value
----------------------- */
ptr = findstring( AccelCmdString, blkstr );
if ( ptr )
    {
    ptr += 2; /* Get past the AC */
    if ( *ptr == VChar )
        {
        var_indicator |= 0x0004;
        ptr++;
        status = send_variable_address( ptr, base_command + 2 );
        }
    else
        {
        status = send_data_and_command( ptr, base_command + 2 );
        }
    }
else
    {
    /*
    ----------------------------
    Disable function with 0 data
    ---------------------------- */
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 2 );
    }

if ( status != FT_OK )
    goto ext;

/*
--------------------------
Extract the position value
-------------------------- */
ptr = findstring( PositionCmdString, blkstr );
if ( ptr )
    {
    ptr++; /* Get past the P */
    if ( *ptr == VChar )
        {
        var_indicator |= 0x0008;
        ptr++;

        status = send_variable_address( ptr, base_command + 3 );
        if ( status != FT_OK )
            goto ext;
        }

    else
        {
        rx = get_real( ptr );
        rx /= PosDivisor;
        x = (BOARD_DATA) rx;
        status = send_data_and_command( x, base_command + 3 );
        if ( status != FT_OK )
            goto ext;
        }
    }
else
    {
    /*
    ----------------------------
    Disable function with 0 data
    ---------------------------- */
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 3 );
    if ( status != FT_OK )
        goto ext;
    }

/*
---------------------------------
Extract the long dwell time value
--------------------------------- */
ptr = findstring( LongDwellCmdString, blkstr );
if ( ptr )
    {
    ptr += 2; /* Get past the LD */
    if ( *ptr == VChar )
        {
        var_indicator |= 0x0010;
        ptr++;
        status = send_variable_address( ptr, base_command + 4 );
        }
    else
        {
        rx = get_real( ptr );
        if ( rx < 0.088 || rx > 5725.0 )
            {
            status = 95;
            show_error( LD_OUT_OF_RANGE_ERROR_STRING );
            }
        else
            {
            rx /= 0.0873813;
            x = (BOARD_DATA) rx;
            status = send_data_and_command( x, base_command + 4 );
            }
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 4 );
    }

if ( status != FT_OK )
    goto ext;

/*
----------------------------
Extract the short dwell time
---------------------------- */
ptr = findstring( ShortDwellCmdString, blkstr );
if ( ptr )
    {
    ptr += 2;
    if ( *ptr == VChar )
        {
        var_indicator |= 0x0020;
        ptr++;
        status = send_variable_address( ptr, base_command + 5 );
        }
    else
        {
        rx = get_real( ptr );
        if ( rx < 0.019 || rx > 87.0 )
            {
            status = 95;
            show_error( SD_OUT_OF_RANGE_ERROR_STRING );
            }
        else
            {
            rx *= 1000.0;
            rx /= 1.333333;
            x = (BOARD_DATA) rx;
            status = send_data_and_command( x, base_command + 5 );
            }
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 5 );
    }

if ( status != FT_OK )
    goto ext;

/* Now parse the conditional string. The variable indicator will be changed
   as required by the parse_conditional() function. */
status = parse_conditional( blk_no, conditional_str, var_indicator );
if ( status != FT_OK )
   goto ext;

/* Now send the variable indicator */
status = send_data_and_command( var_indicator, base_command + 10 );
if ( status != FT_OK )
   goto ext;

ext:
return status;
}

/***********************************************************************
*                             VEL_CONTROL                              *
***********************************************************************/
static int vel_control( int blk_no, TCHAR * blkstr, TCHAR * conditional_str )
{
TCHAR    * ptr;
BOARD_DATA var_indicator;
BOARD_DATA base_command;
int        status;
float      rx;
BOARD_DATA x;

var_indicator = 0;

base_command = ((blk_no * 15) - 15) + 0xf800;

/*
-------------------
Send the block type
------------------- */
status = send_data_and_command( (BOARD_DATA) 3, base_command );
if ( status != FT_OK )
   goto ext;

ptr = blkstr;

/*
----------------------------
Extract the velocity command
---------------------------- */
ptr++;
if ( *ptr == VChar )
    {
    var_indicator |= 0x0002;
    ptr++;
    status = send_variable_address( ptr, base_command + 1 );
    }
else
    {
    rx = get_real( ptr );
    rx *= 100.0;
    x = (BOARD_DATA) rx;
    status = send_data_and_command( x, base_command + 1 );
    }

if ( status != FT_OK )
    goto ext;

/*
-----------------------
Extract the accel value
----------------------- */
ptr = findstring( AccelCmdString, blkstr );
if ( ptr )
    {
    ptr += 2; /* Get past the AC */
    if (*ptr == 'V')
        {
        var_indicator |= 0x0004;
        ptr++;
        status = send_variable_address( ptr, base_command + 2 );
        }
    else
        {
        status = send_data_and_command( ptr, base_command + 2 );
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 2 );
    }

if ( status != FT_OK )
    goto ext;

/*
-----------------------
Extract the goose value
----------------------- */
ptr = findstring( GooseCmdString, blkstr );
if ( ptr )
    {
    ptr ++; /* Get past the G */
    if (*ptr == 'V')
        {
        var_indicator |= 0x0040;
        ptr++;
        status = send_variable_address( ptr, base_command + 6 );
        }
    else
        {
        status = send_data_and_command( ptr, base_command + 6 );
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 6 );
    }

if ( status != FT_OK )
    goto ext;

/*
--------------------------
Extract the position value
-------------------------- */
ptr = findstring( PositionCmdString, blkstr );
if ( ptr )
    {
    ptr++;
    if (*ptr == 'V')
        {
        var_indicator |= 0x0008;
        ptr++;

        status = send_variable_address( ptr, base_command + 3 );
        }
    else
        {
        rx = get_real( ptr );
        rx /= PosDivisor;
        x = (BOARD_DATA) rx;
        status = send_data_and_command( x, base_command + 3 );
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 3 );
    }

if ( status != FT_OK )
    goto ext;

/*
---------------------------------
Extract the low impact I/O number
--------------------------------- */
ptr = findstring( LowImpactCmdString, blkstr );
if ( ptr )
    {
    ptr += 2; /* Get past the LI */
    status = send_input_number( var_indicator, 0x0020, &ptr, base_command + 5 );
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 5 );
    }

if ( status != FT_OK )
    goto ext;

/*
----------------------------------
Extract the Low impact DAC command
---------------------------------- */
ptr = findstring( LowImpactDacCmdString, blkstr );
if ( ptr )
    {
    ptr += 2; /* Get past the LD */

    if ( *ptr == VChar )
        {
        var_indicator |= 0x0010;
        ptr++;
        status = send_variable_address( ptr, base_command + 4 );
        }
    else
        {
        status = send_data_and_command( ptr, base_command + 4 );
        }
    }
else
    {
    status = send_data_and_command( (BOARD_DATA) 0, base_command + 4 );
    }

if ( status != FT_OK )
    goto ext;

/*
------------------------------------------------------------------------
Now parse the conditional string. The variable indicator will be changed
as required by the parse_conditional() function.
------------------------------------------------------------------------ */
status = parse_conditional( blk_no, conditional_str, var_indicator );
if ( status != FT_OK )
    goto ext;

/*
---------------------------
Send the variable indicator
--------------------------- */
status = send_data_and_command( var_indicator, base_command + 10 );
if ( status != FT_OK )
    goto ext;

ext:
return status;
}

/***********************************************************************
*                                 JUMP                                 *
***********************************************************************/
static int jump( int blk_no, TCHAR * blkstr, TCHAR * conditional_str, int rel_indicator )
{
TCHAR * ptr;
TCHAR   type;
BOARD_DATA var_indicator = 0;
BOARD_DATA base_command;
BOARD_DATA x;
STRING_CLASS sc;

int        status;

sc = TEXT( "Conditional str[" );

var_indicator = 0;
sc            += conditional_str;
sc            += TEXT( "] " );

base_command = ((blk_no * 15) - 15) + 0xf800;

/*
-------------------
Send the block type
------------------- */
status = send_data_and_command( (BOARD_DATA) 1, base_command ); /* 1 = jump block */
if ( status != FT_OK )
   {
   show_error( status, TEXT("Send block type") );
   goto ext;
   }

ptr = blkstr;

if ( rel_indicator )
    {
    ptr += 2; /* Get past the -J */

    /*
    ------------
    Get the type
    ------------ */
    type = *ptr;

    ptr += 2;  /* Move past the (type). characters */
    if ( *ptr == MinusChar )
        {
        status = send_data_and_command( (BOARD_DATA) 2, base_command + 1 );
        if ( status != FT_OK )
           show_error( status, TEXT("Minus type if rel indicator") );
        ptr++;  /* get past the minus sign */
        }
    else   /* Relative forward jump */
        {
        status = send_data_and_command( (BOARD_DATA) 1, base_command + 1 );
        if ( status != FT_OK )
           show_error( status, TEXT("Rel forward type if rel indicator") );
        }

    }
else
    {
    ptr++;  /* Get past the J */

    /*
    ------------
    Get the type
    ------------ */
    type = *ptr;
    ptr += 2;  /* Move past the (type). characters */

    status = send_data_and_command( (BOARD_DATA) 0, base_command + 1 );
    if ( status != FT_OK )
       show_error( status, TEXT("Send 0 type if no rel indicator") );
    }

if ( status != FT_OK )
    goto ext;

/*
-------------------------------------------
Get the block to jump to (or relative move)
------------------------------------------- */
if (*ptr == VChar )
    {
    var_indicator |= 0x0004;
    ptr++;
    status = send_variable_address( ptr, base_command + 2 );
    if ( status != FT_OK )
       show_error( status, TEXT("Bad variable address to jump to") );
    }
else
    {
    status = send_data_and_command( ptr, base_command + 2 );
    if ( status != FT_OK )
       show_error( status, TEXT("Bad block to jump to") );
    }

if ( status != FT_OK )
    goto ext;

/*
-----------------
Extract jump type
----------------- */
switch ( type )
    {
    case ZeroChar:
        x = 0;
        break;

    case OneChar:
        x = 1;
        break;

    case TwoChar:
        x = 2;
        break;

    default:
        show_error( TEXT("Syntax error, Jump type NOT {0,1,2}") );
        status = 99;
        goto ext;
    }

status = send_data_and_command( x, base_command + 3 );
if ( status != FT_OK )
    goto ext;

/*
--------------------------------------------------------------------
Parse the conditional string. The variable indicator will be changed
as required by the parse_conditional() function.
-------------------------------------------------------------------- */
status = parse_conditional( blk_no, conditional_str, var_indicator );
if ( status != FT_OK )
    {
    show_error( status, TEXT("Can't parse conditional") );
    goto ext;
    }

/*
---------------------------
Send the variable indicator
--------------------------- */
status = send_data_and_command( var_indicator, base_command + 10 );
if ( status != FT_OK )
    {
    show_error( status, TEXT("Can't send variable indicator") );
    goto ext;
    }

ext:
return status;
}

/***********************************************************************
*                                SET_POS                               *
***********************************************************************/
static int set_pos( int blk_no, TCHAR * blkstr, TCHAR * conditional_str )
{
TCHAR * ptr;
BOARD_DATA var_indicator;
BOARD_DATA base_command;
int        status;

base_command  = ((blk_no * 15) - 15) + 0xf800;
var_indicator = 0;

/*
----------------------------------------------
Send the block type ( 2 = set position block )
---------------------------------------------- */
status = send_data_and_command( (BOARD_DATA) 2, base_command );
if ( status != FT_OK )
   goto ext;

ptr = blkstr;

/*
---------------------------------------
Extract the position to set the axis to
--------------------------------------- */
ptr++;
if ( *ptr == VChar )
    {
    var_indicator |= 0x0002;
    ptr++;
    status = send_variable_address( ptr, base_command + 1 );
    }
else
    {
    status = send_data_and_command( ptr, base_command + 1 );
    }

if ( status != FT_OK )
    goto ext;


/*
--------------------------------------------------------------------
Parse the conditional string. The variable indicator will be changed
as required by the parse_conditional() function.
-------------------------------------------------------------------- */
status = parse_conditional( blk_no, conditional_str, var_indicator );
if ( status != FT_OK )
   goto ext;

/* Now send the variable indicator */
status = send_data_and_command( var_indicator, base_command + 10 );
if ( status != FT_OK )
    goto ext;

ext:
return status;
}

/***********************************************************************
*                               PROG_END                               *
***********************************************************************/
static int prog_end( int blk_no )
{
BOARD_DATA base_command;

base_command = ((blk_no * 15) - 15) + 0xf800;

/*
---------------------------------------------
Send the block type ( 999 == End of program )
--------------------------------------------- */
return send_data_and_command( (BOARD_DATA) 999, base_command );
}

/***********************************************************************
*                         COMPILE_AND_SEND                             *
***********************************************************************/
int compile_and_send( TCHAR * filename )
{
FILE_CLASS   f;
TCHAR      * ptr;
TCHAR        blkstr[81];
TCHAR        conditional_str[40];

int          blk_no;
int          status;
BOARD_DATA   par_val;
BOARD_DATA   par_num;
int          c;
BOOLEAN      have_conditional;
STRING_CLASS s;

/* Status return values

   1  == No problems encountered
   2 - ?? SureTrak card interface problems
   99 == Syntax error in program string
   98 == Too many blocks in program
   97 == Too many parameters
   96 == Variable value out of range
   95 == Time delay value out of range
   80 == Unable to open program file
   81 == Unable to open parameter file
*/

status = FT_ERROR;

s = resource_string( DOWNLOADING_STRING );

s += filename;
set_text( MainScreenWindow.handle(), WAITING_FOR_TEXTBOX, s.text() );

if ( !f.open_for_read(filename)  )
    {
    status = 80;
    MessageBox( MainWindow.handle(), ultohex(f.error()), resource_string(CONTROL_PROG_OPEN_ERROR_STRING), MB_OK | MB_SYSTEMMODAL );
    goto ext;
    }

while ( TRUE  )
    {
    ptr = f.readline();
    if ( !ptr )
        break;

    show_status( ptr );

    c = (int) *ptr;
    switch( c )
        {
        /*
        ----------------
        Position Divisor
        ---------------- */
        case 'P':
            ptr++;
            PosDivisor = get_real( ptr );
            break;

        /*
        ---------
        Parameter
        --------- */
        case CommaChar:

            ptr++;

            if ( replace_char_with_null(ptr, SpaceChar) )
                {
                par_num = (BOARD_DATA) asctoul( ptr );

                if (par_num > 38 || par_num < 1)
                    {
                    show_error( TOO_MANY_PARAMS_ERROR_STRING );
                    status = 97;
                    goto ext;
                    }

                ptr = nextstring( ptr );
                par_val = (BOARD_DATA) asctoul( ptr );

                /*
                -------------------------------------------------
                Generate the command to send based on the par_num
                ------------------------------------------------- */
                par_num *= 2;        /* Byte oriented word */
                par_num -= 2;        /* Parameters start at 1, not zero */
                par_num += 0xffc8;   /* Add offset, params begin at 0xffc8, hi ram */
                par_num >>= 1;       /* Shift right */
                par_num |= 0x8000;   /* Set MSB, par_num is now command to send */

                /*
                -----------------
                Send to the board
                ----------------- */
                status = send_data_and_command( par_val, par_num );
                if ( status != FT_OK )
                    goto ext;
                }

            break;

        /*
        -------------
        Program block
        ------------- */
        case Period:
            ptr++;
            if ( replace_char_with_null(ptr, SpaceChar) )
                {
                blk_no = asctoint32( ptr );

                if (blk_no > 128 || blk_no < 1)
                   {
                   show_error( TOO_MANY_BLOCKS_ERROR_STRING );
                   status = 98;
                   goto ext;
                   }

                ptr = nextstring( ptr );
                have_conditional = replace_char_with_null(ptr, SpaceChar );
                lstrcpy( blkstr, ptr );

                if ( have_conditional )
                    {
                    ptr = nextstring( ptr );
                    replace_char_with_null(ptr, SpaceChar );
                    lstrcpy( conditional_str, ptr );
                    }
                else
                    {
                    *conditional_str = NullChar;
                    }

                /*
                ------------------------
                Determine the block type
                ------------------------ */
                c = (int) *blkstr;
                switch ( c )
                    {
                    case 'T':  /* Valve step */
                        status = val_step( blk_no, blkstr, conditional_str );
                        if ( status != FT_OK )
                            {
                            show_error( status, VALVE_STEP_ERROR_STRING );
                            goto ext;
                            }
                        break;

                    case 'V':  /* Velocity control */
                        status = vel_control( blk_no, blkstr, conditional_str );
                        if ( status != FT_OK )
                            {
                            show_error( status, VEL_CONTROL_STEP_ERROR_STRING );
                            goto ext;
                            }
                        break;

                    case MinusChar:  /* Relative jump */
                        status = jump( blk_no, blkstr, conditional_str, 1 );
                        if ( status != FT_OK )
                            goto ext;
                        break;

                    case 'J':  /* Absolute jump */
                        status = jump( blk_no, blkstr, conditional_str, 0 );
                        if ( status != FT_OK )
                            goto ext;
                        break;

                    case 'S':  /* Set position */
                        status = set_pos( blk_no, blkstr, conditional_str );
                        if ( status != FT_OK )
                            {
                            show_error( status, SET_POSITION_STEP_ERROR_STRING );
                            goto ext;
                            }
                        break;

                    case 'E':  /* Program end */
                        prog_end( blk_no );
                        status = FT_OK;
                        break;

                    default:  /* Syntax error */
                        show_error( UNKNOWN_BLOCK_TYPE_ERROR_STRING );
                        status = 99;
                        break;
                    }

                if ( status != FT_OK )
                    goto ext;
                }

            break;

        default:
            break;
        }
    }

ext:
f.close();

if ( status == FT_OK )
    {
    status = SureTrakBoard.send_command_busy( SET_PROG_DOWNLOADED );
    SureTrakHasProgram = TRUE;
    }

set_text( MainScreenWindow.handle(), WAITING_FOR_TEXTBOX, EmptyString );

return status;
}
