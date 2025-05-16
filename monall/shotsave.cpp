#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\events.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\names.h"

#include "resource.h"
#include "extern.h"
#include "shotsave.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR CrChar        = TEXT( '\r' );
static TCHAR EtxChar       = TEXT( '\003' );
static TCHAR LfChar        = TEXT( '\n' );
static TCHAR LowerNChar    = TEXT( 'n' );
static TCHAR LowerRChar    = TEXT( 'r' );
static TCHAR DChar         = TEXT( 'D' );
static TCHAR MChar         = TEXT( 'M' );
static TCHAR NChar         = TEXT( 'N' );
static TCHAR OneChar       = TEXT( '1' );
static TCHAR PChar         = TEXT( 'P' );
static TCHAR SChar         = TEXT( 'S' );
static TCHAR StxChar       = TEXT( '\002' );
static TCHAR TChar         = TEXT( 'T' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR SpaceChar     = TEXT( ' ' );
static TCHAR TabChar       = TEXT( '\t' );
static TCHAR TwoChar       = TEXT( '2' );
static TCHAR YChar         = TEXT( 'Y' );
static TCHAR ZeroChar      = TEXT( '0' );
static TCHAR DelimitChar   = TEXT( '\t' );
static TCHAR TabString[]   = TEXT( "\\t" );
static TCHAR UnknownString[] = UNKNOWN;

static const TCHAR MonallIniFile[] = TEXT( "monall.ini" );
static const TCHAR ConfigSection[] = TEXT( "Config"     );

static BOOLEAN SaveAllAlarmShots = FALSE;

static BOOLEAN ShowMachine       = TRUE;
static BOOLEAN ShowPart          = TRUE;
static BOOLEAN ShowDate          = TRUE;
static BOOLEAN ShowTime          = TRUE;
static BOOLEAN ShowShot          = TRUE;
static BOOLEAN ShowParam[MAX_PARMS];

static TCHAR * EosComPort     = 0;
static TCHAR * EosComSetup    = 0;
static TCHAR   EosComDelimitChar = TEXT( ',' );
static TCHAR   EosComEol[3]      = TEXT( "\r\n" );
static int     EosFixedWidth     = 0;
static BOOLEAN EosNeedsStxEtx    = FALSE;
static TCHAR   EosPyramidLines   = ZeroChar;

static const float MaxFloatShort = float( 0xFFFF );

BOOLEAN get_eos_udp_address( sockaddr_in & dest,  TCHAR * computer, TCHAR * machine );
BOOLEAN nextshot( PROFILE_HEADER & ph );
void    send_udp_message( sockaddr_in & dest, TCHAR * message );
BOOLEAN write_parameters( PROFILE_HEADER & ph, float * parameters, short nof_parameters, BITSETYPE save_flags );
BOOLEAN write_profile( PROFILE_HEADER & ph, unsigned short points[MAX_CURVES][MAX_POINTS], float * parameters, short nof_parameters, BITSETYPE save_flags );

/***********************************************************************
*                             FREEBUF                                  *
***********************************************************************/
static void freebuf( int i )
{
SHOT_BUFFER_ENTRY * b;

b = BufPtr[i];
if ( b )
    {
    BufPtr[i] = 0;
    delete b;
    }
}

/***********************************************************************
*                        GET_DDE_PARAMETER_LIST                        *
***********************************************************************/
static void get_dde_parameter_list( void )
{
int32   i;
BOOLEAN have_comma;
TCHAR * cp;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowMachine") );
if ( *cp == NChar )
    ShowMachine = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowPart") );
if ( *cp == NChar )
    ShowPart = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowDate") );
if ( *cp == NChar )
    ShowDate = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowTime") );
if ( *cp == NChar )
    ShowTime = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowShot") );
if ( *cp == NChar )
    ShowShot = FALSE;

/*
---------------------------------
Default to showing all parameters
--------------------------------- */
for( i=0; i<MAX_PARMS; i++ )
    ShowParam[i] = TRUE;

cp = get_long_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamParmList") );
if ( lstrcmp(cp, UnknownString) != 0 )
    {
    for( i=0; i<MAX_PARMS; i++ )
        ShowParam[i] = FALSE;

    while ( TRUE )
        {
        have_comma = replace_char_with_null( cp, CommaChar );
        i = asctoint32( cp ) - 1;
        if ( i >= 0 && i < MAX_PARMS )
            ShowParam[i] = TRUE;
        if ( !have_comma )
            break;
        cp = nextstring( cp );
        }
    }
}

/***********************************************************************
*                             GET_INI_CHAR                             *
***********************************************************************/
TCHAR * get_ini_char( TCHAR * dest, TCHAR * sorc )
{
if ( *sorc == BackslashChar )
    {
    sorc++;
    if ( *sorc == LowerRChar )
        *dest = CrChar;
    else if ( *sorc == LowerNChar )
        *dest = LfChar;
    else
        *dest = TabChar;
    }
else
    {
    *dest = *sorc;
    }

sorc++;
return sorc;
}

/***********************************************************************
*                           INIT_SERIAL_PORT                           *
*   DWORD DCBlength            // sizeof( DCB )                        *
*   DWORD BaudRate;            // current baud rate                    *
*   DWORD fBinary: 1;          // binary mode, no EOF check            *
*   DWORD fParity: 1;          // enable parity checking               *
*   DWORD fOutxCtsFlow:1;      // CTS output flow control              *
*   DWORD fOutxDsrFlow:1;      // DSR output flow control              *
*   DWORD fDtrControl:2;       // DTR flow control type                *
*   DWORD fDsrSensitivity:1;   // DSR sensitivity                      *
*   DWORD fTXContinueOnXoff:1; // XOFF continues Tx                    *
*   DWORD fOutX: 1;            // XON/XOFF out flow control            *
*   DWORD fInX: 1;             // XON/XOFF in flow control             *
*   DWORD fErrorChar: 1;       // enable error replacement             *
*   DWORD fNull: 1;            // enable null stripping                *
*   DWORD fRtsControl:2;       // RTS flow control                     *
*   DWORD fAbortOnError:1;     // abort reads/writes on error          *
*   DWORD fDummy2:17;          // reserved                             *
*   WORD wReserved;            // not currently used                   *
*   WORD XonLim;               // transmit XON threshold               *
*   WORD XoffLim;              // transmit XOFF threshold              *
*   BYTE ByteSize;             // number of bits/byte, 4-8             *
*   BYTE Parity;               // 0-4=no,odd,even,mark,space           *
*   BYTE StopBits;             // 0,1,2 = 1, 1.5, 2                    *
*   char XonChar;              // Tx and Rx XON character              *
*   char XoffChar;             // Tx and Rx XOFF character             *
*   char ErrorChar;            // error replacement character          *
*   char EofChar;              // end of input character               *
*   char EvtChar;              // received event character             *
*   WORD wReserved1;           // reserved; do not use                 *
***********************************************************************/
void init_serial_port( void )
{
HANDLE  fh;
TCHAR * cp;
DCB     dcb;
int     i;
BOOL    status;

struct STOP_BIT_ENTRY {
    TCHAR s[4];
    unsigned int value;
    };
static STOP_BIT_ENTRY sbe[] = {
    { TEXT("1"),   ONESTOPBIT },
    { TEXT("1.5"), ONE5STOPBITS },
    { TEXT("2"),   TWOSTOPBITS  }
    };

const int nof_stops = sizeof(sbe)/sizeof(STOP_BIT_ENTRY);

struct PARITY_ENTRY {
    TCHAR character;
    unsigned int value;
    };

static PARITY_ENTRY  pe[] = {
    { TEXT('N'), 0 },
    { TEXT('n'), 0 },
    { TEXT('O'), 1 },
    { TEXT('o'), 1 },
    { TEXT('E'), 2 },
    { TEXT('e'), 2 },
    { TEXT('M'), 3 },
    { TEXT('m'), 3 },
    { TEXT('S'), 4 },
    { TEXT('s'), 4 }
    };

const int nof_parity_chars = sizeof(pe) / sizeof(PARITY_ENTRY);

if ( !EosComPort )
    return;

dcb.DCBlength = sizeof( DCB );
status = FALSE;

fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    status = GetCommState( fh, &dcb );
    CloseHandle( fh );
    }

if ( status )
    {
    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComSpeed") );
    if ( !unknown(cp) )
        dcb.BaudRate = (DWORD) asctoul( cp );

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComParity") );
    if ( !unknown(cp) )
        {
        for ( i=0; i<nof_parity_chars; i++ )
            {
            dcb.Parity  = 0;          /* No parity */
            if ( *cp == pe[i].character )
                {
                dcb.Parity  = pe[i].value;
                break;
                }
            }

        if ( dcb.Parity == 0 )
            dcb.fParity = 0;
        else
            dcb.fParity = 1;
        }

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComBits") );
    if ( !unknown(cp) )
        dcb.ByteSize = asctoul( cp );

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComStop") );
    if ( !unknown(cp) )
        {
        for ( i=0; i<nof_stops; i++ )
            {
            if ( strings_are_equal( sbe[i].s, cp) )
                {
                dcb.StopBits = sbe[i].value;
                break;
                }
            }
        }


    /*
    ---------------------------------------------------
    Tell the other computer I am always ready for chars
    --------------------------------------------------- */
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComCts") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutxCtsFlow = 1;
        else
            dcb.fOutxCtsFlow = 0;
        }

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComDsr") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutxDsrFlow = 1;
        else
            dcb.fOutxDsrFlow = 0;
        }

    dcb.fDsrSensitivity   = 0;
    dcb.fTXContinueOnXoff = 1;
    dcb.fInX              = 0;

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComXonXoff") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutX = 1;
        else
            dcb.fOutX = 0;
        }

    fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if ( fh != INVALID_HANDLE_VALUE )
        {
        SetCommState( fh, &dcb );
        CloseHandle( fh );
        }
    else
        {
        status = FALSE;
        }
    }

if ( !status )
    {
    delete[] EosComPort;
    EosComPort = 0;
    }
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( void )
{
TCHAR  * dp;
TCHAR  * cp;
int i;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    BufPtr[i] = 0;


cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("SaveAllAlarmShots") );
if ( *cp == YChar )
    SaveAllAlarmShots = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamDelimiter") );
if ( lstrcmp(cp,UNKNOWN) != 0 && lstrcmp(cp,TabString) != 0 )
    DelimitChar = *cp;

/*
----------------------------------------------
Get the name of the comm port "COM1" or "COM2"
---------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComPort") );
if ( lstrcmp(cp, UNKNOWN) != 0 && (*cp != NChar) && (*cp != LowerNChar) )
    EosComPort = maketext( cp );

/*
--------------------
See if I need an STX
-------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosNeedsStxEtx") );
if ( *cp == YChar )
    EosNeedsStxEtx = TRUE;

/*
--------------------------------------------------
See if the parameters should all be the same width
-------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosFixedWidth") );
if ( !unknown(cp) )
    {
    i = asctoint32( cp );
    if ( i > 0 && i < 50 )
        EosFixedWidth = i;
    }
/*
----------------
Look for Pyramid
---------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosPyramidLines") );
if ( *cp == OneChar || *cp == TwoChar )
    {
    EosPyramidLines = *cp;
    }

/*
---------------------------------------------------------
Get the delimiter to put after each field except the last
--------------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComDelimitChar") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    get_ini_char( &EosComDelimitChar, cp );

/*
----------------------------------------------------------
Get the end of line string (normally \r\n but may be none)
---------------------------------------------------------- */
dp = EosComEol;
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComEol") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    /*
    ------------------------------
    Allow up to two eol characters
    ------------------------------ */
    if ( *cp != NullChar )
        {
        cp = get_ini_char( dp, cp );
        dp++;
        }

    if ( *cp != NullChar )
        {
        get_ini_char( dp, cp );
        dp++;
        }
    }

*dp = NullChar;


cp = get_long_ini_string( MonallIniFile, ConfigSection, TEXT("EosComSetup") );
if ( !unknown(cp) )
    {
    EosComSetup = maketext( cp );
    }
else if ( EosComPort )
    {
    delete[] EosComPort;
    EosComPort = 0;
    }
if ( EosComPort )
    init_serial_port();

get_dde_parameter_list();
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
int i;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    freebuf( i );

if ( EosComPort )
    {
    delete[] EosComPort;
    EosComPort = 0;
    }

if ( EosComSetup )
    {
    delete[] EosComSetup;
    EosComSetup = 0;
    }

}

/***********************************************************************
*                         COPY_DATA_TO_BUFFER                          *
***********************************************************************/
static void copy_data_to_buffer( SHOT_BUFFER_ENTRY * b, PROFILE_HEADER & new_ph, float new_points[MAX_CURVES][MAX_POINTS], float * new_parameters, short nof_new_parameters )
{
int   c;
int   i;
float y;
float yrange;
float ymin;

b->ph         = new_ph;
b->nof_parameters = nof_new_parameters;
for ( i=0; i<nof_new_parameters; i++ )
    b->parameters[i] = new_parameters[i];

b->nof_curves = 0;

for ( c=0; c<MAX_CURVES; c++ )
    {
    if ( new_ph.curves[c].type == NO_CURVE_TYPE )
        break;

    b->nof_curves++;

    ymin   = new_ph.curves[c].min;
    yrange = new_ph.curves[c].max - ymin;

    for ( i=0; i<new_ph.n; i++ )
        {
        /*
        --------------------
        Normalize each point
        -------------------- */
        y = new_points[c][i];
        y -= ymin;
        y /= yrange;
        y *= MaxFloatShort;
        b->points[c][i] = (unsigned short) y;
        }
    }

}

/***********************************************************************
*                         ADD_TO_SHOTSAVE                              *
*                                                                      *
* This is called by getdata to add the shot to the buffer. Once I      *
* set the BufPtr, I should no longer make any changes to it.           *
* This is the only routine that is known globally.                     *
***********************************************************************/
BOOLEAN add_to_shotsave( PROFILE_HEADER & new_ph, float new_points[MAX_CURVES][MAX_POINTS], float * new_parameters, short nof_new_parameters, BOOLEAN has_alarm )
{
int i;

SHOT_BUFFER_ENTRY * b;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    {
    if ( BufPtr[i] == 0 )
        {
        b = new SHOT_BUFFER_ENTRY;
        if ( b )
            {
            copy_data_to_buffer( b, new_ph, new_points, new_parameters, nof_new_parameters );
            b->save_failed = FALSE;
            b->has_alarm   = has_alarm;
            BufPtr[i] = b;
            return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                      MAKE_PARAMETER_EVENT_STRING                     *
***********************************************************************/
static TCHAR * make_parameter_event_string( SHOT_BUFFER_ENTRY * b )
{
static const int DDE_LINE_LEN  = MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + SHOT_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + MAX_PARMS*(SHOTPARM_FLOAT_LEN + 1);

int        i;
TCHAR    * buf;
TCHAR    * cp;
SYSTEMTIME st;

buf = new TCHAR[DDE_LINE_LEN+1];
if ( !buf )
    {
    return 0;
    }

FileTimeToSystemTime( &b->ph.time_of_shot, &st );

cp = buf;


if ( ShowMachine )
    cp = copy_w_char( cp, b->ph.machine_name, DelimitChar );

if ( ShowPart )
    cp = copy_w_char( cp, b->ph.part_name,    DelimitChar );

if ( ShowDate )
    cp = copy_w_char( cp, alphadate(st), DelimitChar );

if ( ShowTime )
    cp = copy_w_char( cp, alphatime(st), DelimitChar );

/*
-----------
Shot Number
----------- */
if ( ShowShot )
    {
    int32toasc( cp, b->ph.shot_number, DECIMAL_RADIX );
    cp += lstrlen( cp );
    }

/*
----------
Parameters
---------- */
for ( i=0; i<b->nof_parameters; i++ )
    {
    if ( ShowParam[i] )
        {
        *cp = DelimitChar;
        cp++;
        ftoasc( cp, b->parameters[i] );
        cp += lstrlen(cp);
        }
    }

*cp = NullChar;

return buf;
}

/***********************************************************************
*                         WRITE_SERIAL_PORT_INFO                       *
***********************************************************************/
void write_serial_port_info( SHOT_BUFFER_ENTRY * b )
{
const int32 SLEN = 1 + MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + MAX_INTEGER_LEN + ALPHADATE_LEN + 1 + ALPHATIME_LEN + (MAX_PARMS*MAX_FLOAT_LEN) + MAX_PARMS +1;

HANDLE  fh;
int32   n;
TCHAR * bp;
TCHAR * buf;
TCHAR * cp;
BOOLEAN firstfield;
DWORD   bytes_to_write;
DWORD   bytes_written;
SYSTEMTIME st;
sockaddr_in sa;

buf = maketext( SLEN );
if ( !buf )
    return;

FileTimeToSystemTime( &b->ph.time_of_shot, &st );

firstfield = TRUE;
cp = EosComSetup;
bp = buf;

if ( EosPyramidLines == OneChar )
    {
    lstrcpy( bp, alphadate(st) );
    *(bp+2) = *(bp+3);
    *(bp+3) = *(bp+4);
    *(bp+4) = *(bp+8);
    *(bp+5) = *(bp+9);
    *(bp+6) = SpaceChar;
    bp += 7;
    insalph( bp, b->ph.shot_number, 6, ZeroChar, DECIMAL_RADIX );
    bp += 6;
    *bp = CrChar;
    bp++;
    *bp = NullChar;
    }
else if ( EosPyramidLines == TwoChar )
    {
    bp = copystring( bp, TEXT("V01") );
    lstrcpy( bp, alphadate(st) );
    *(bp+2) = *(bp+3);
    *(bp+3) = *(bp+4);
    *(bp+4) = *(bp+8);
    *(bp+5) = *(bp+9);
    *(bp+6) = CrChar;
    bp += 7;
    bp = copystring( bp, TEXT("V02") );
    insalph( bp, b->ph.shot_number, 6, ZeroChar, DECIMAL_RADIX );
    bp += 6;
    *bp = CrChar;
    bp++;
    *bp = NullChar;
    }
else
    {
    if ( EosNeedsStxEtx )
        {
        *bp = StxChar;
        bp++;
        }

    while ( TRUE )
        {
        if ( *cp == NullChar )
            break;

        if ( firstfield )
            {
            firstfield = FALSE;
            }
        else
            {
            *bp = EosComDelimitChar;
            bp++;
            }

        if ( is_numeric(*cp) )
            {
            n = asctoint32( cp );
            if ( n >= 0 && n <= MAX_PARMS )
                {
                rounded_double_to_tchar( bp, (double) b->parameters[n] );
                if ( EosFixedWidth > 0 )
                    rjust( bp, (short) EosFixedWidth, SpaceChar );
                }
            else
                {
                lstrcpy( bp, UnknownString );
                }
            }
        else if ( *cp == MChar )
            {
            lstrcpy( bp, b->ph.machine_name );
            }
        else if ( *cp == PChar )
            {
            lstrcpy( bp, b->ph.part_name );
            }
        else if ( *cp == SChar )
            {
            n = 0;
            if ( is_numeric(*(cp+1)) )
                n = asctoint32(cp+1);
            if ( n > 0 )
                {
                insalph( bp, b->ph.shot_number, n, ZeroChar, DECIMAL_RADIX );
                bp += n;
                *bp = NullChar;
                }
            else
                {
                lstrcpy( bp, int32toasc(b->ph.shot_number) );
                }
            }
        else if ( *cp == DChar )
            {
            lstrcpy( bp, alphadate(st) );
            /*
            --------------------------
            If "D0" remove the slashes
            -------------------------- */
            cp++;
            if ( *cp == ZeroChar )
                {
                *(bp+2) = *(bp+3);
                *(bp+3) = *(bp+4);
                *(bp+4) = *(bp+8);
                *(bp+5) = *(bp+9);
                *(bp+6) = NullChar;
                }
            else
                {
                cp--;
                }
            }
        else if ( *cp == TChar )
            {
            lstrcpy( bp, alphatime(st) );
            /*
            -------------------------
            If "T0" remove the colons
            ------------------------- */
            cp++;
            if ( *cp == ZeroChar )
                {
                *(bp+2) = *(bp+3);
                *(bp+3) = *(bp+4);
                *(bp+4) = *(bp+6);
                *(bp+5) = *(bp+7);
                *(bp+6) = NullChar;
                }
            else
                {
                cp--;
                }
            }

        if ( *bp != NullChar )
            bp += lstrlen(bp);

        while ( *cp != NullChar )
            {
            if ( *cp == CommaChar )
                {
                cp++;
                break;
                }
            cp++;
            }
        }


    if ( *EosComEol != NullChar )
        bp = copystring( bp, EosComEol );

    if ( EosNeedsStxEtx )
        {
        *bp = EtxChar;
        bp++;
        *bp = NullChar;
        }
    }

if ( HaveEosUdp )
    {
    if ( get_eos_udp_address(sa, ComputerName, b->ph.machine_name) )
        send_udp_message( sa, buf );
    }
else
    {
    bytes_to_write = lstrlen(buf) * sizeof( TCHAR );
    bytes_written  = 0;
    fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if ( fh != INVALID_HANDLE_VALUE )
        {
        WriteFile( fh, buf, bytes_to_write, &bytes_written, 0 );
        CloseHandle( fh );
        }

    }

delete[] buf;
}

/***********************************************************************
*                               SAVE_SHOT                              *
***********************************************************************/
void save_shot( SHOT_BUFFER_ENTRY * b )
{
static BOOLEAN have_enough_disk_space = TRUE;
BITSETYPE save_flags;
TCHAR     root[MAX_PATH+1];
TCHAR   * buf;
TCHAR   * cp;

if ( get_root_directory(root) )
    {
    if ( free_kilobytes(root) < MIN_FREE_DISK_KB )
        {
        if ( have_enough_disk_space )
            {
            have_enough_disk_space = FALSE;
            resource_message_box( MainInstance, LOW_DISK_SPACE_STRING, NO_SHOTS_SAVED_STRING, MB_OK | MB_SYSTEMMODAL );
            }
        return;
        }
    else
        {
        have_enough_disk_space = TRUE;
        }
    }

nextshot( b->ph );
save_flags = skip_new_shot_notify( b->ph.machine_name, b->ph.time_of_shot);

if ( SaveAllAlarmShots && b->has_alarm )
    {
    save_flags |= SAVE_PARM_DATA;
    save_flags |= SAVE_PROFILE_DATA;
    }

write_parameters( b->ph, b->parameters, b->nof_parameters, save_flags );
write_profile( b->ph, b->points, b->parameters, b->nof_parameters, save_flags );

buf = new TCHAR[MACHINE_NAME_LEN+1+PART_NAME_LEN+1];
if ( buf )
    {
    cp = copy_w_char( buf, b->ph.machine_name, CommaChar );
    lstrcpy( cp, b->ph.part_name );
    PostMessage( MainWindow, WM_NEWSHOT, (WPARAM) b->ph.shot_number, (LPARAM) buf );
    }

buf = make_parameter_event_string( b );
if ( buf )
    PostMessage( MainWindow, WM_POKEMON, (WPARAM) PARAM_DATA_INDEX, (LPARAM) buf );

if ( EosComPort || HaveEosUdp )
    write_serial_port_info( b );
}

/***********************************************************************
*                          SAVE_THIS_MACHINE                           *
***********************************************************************/
static void save_this_machine( TCHAR * machine_to_save )
{
int  i;
int  i_to_save;
FILETIME time_to_save;
FILETIME new_time;
SHOT_BUFFER_ENTRY * b;

/*
-------------------------------------
Find the oldest shot for this machine
------------------------------------- */
i_to_save = NO_INDEX;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    {
    if ( BufPtr[i] )
        {
        b = BufPtr[i];
        if ( lstrcmp(b->ph.machine_name, machine_to_save) == 0 )
            {
            new_time = b->ph.time_of_shot;
            if ( i_to_save == NO_INDEX )
                {
                i_to_save = i;
                time_to_save = new_time;
                }
            else if ( seconds_difference(new_time, time_to_save) < 0 )
                {
                i_to_save = i;
                time_to_save = new_time;
                }
            }
        }

    }

if ( i_to_save != NO_INDEX )
    {
    b = BufPtr[i_to_save];
    if ( get_shotsave_semaphore(ComputerName, b->ph.machine_name) )
        {
        save_shot( b );
        free_shotsave_semaphore();
        freebuf( i_to_save );
        }
    else
        {
        /*
        --------------------------------------------------
        Mark all the shots for this machine as save failed
        -------------------------------------------------- */
        for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
            {
            if ( BufPtr[i] )
                {
                b = BufPtr[i];
                if ( lstrcmp(b->ph.machine_name, machine_to_save) == 0 )
                    b->save_failed = TRUE;
                }
            }
        }
    }
}

/***********************************************************************
*                            SAVE_ALL_SHOTS                            *
***********************************************************************/
void save_all_shots( void )
{
static int current_index = 0;
int                 first_index;
SHOT_BUFFER_ENTRY * b;

first_index = current_index;
while ( TRUE )
    {
    if ( BufPtr[current_index] )
        {
        b = BufPtr[current_index];
        if ( !b->save_failed )
            save_this_machine( b->ph.machine_name );
        }
    current_index++;
    if ( current_index >= MAX_SHOTSAVE_BUFFERS )
        current_index = 0;
    if ( current_index == first_index )
        break;
    }
}

/***********************************************************************
*                       SHOTSAVE_THREAD                                *
***********************************************************************/
DWORD shotsave_thread( int * notused )
{
DWORD status;
DWORD ms_to_wait;
int   i;

startup();

while ( TRUE )
    {
    ms_to_wait = INFINITE;
    for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
        {
        if ( BufPtr[i] )
            {
            ms_to_wait = SHOTSAVE_WAIT_TIMEOUT;
            BufPtr[i]->save_failed = FALSE;
            }
        }

    status = WaitForSingleObject( ShotSaveEvent, ms_to_wait );

    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance, UNABLE_TO_CONTINUE_STRING, SHOTSAVE_WAIT_FAIL_STRING, MB_OK | MB_SYSTEMMODAL );

    save_all_shots();

    if ( ShuttingDown )
        {
        ShotSaveIsRunning = FALSE;
        PostMessage( MainWindow, WM_CLOSE, 0, 0L );
        break;
        }

    }


shutdown();

return 0;
}

/***********************************************************************
*                        START_SHOTSAVE_THREAD                         *
***********************************************************************/
void start_shotsave_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                     /* no security attributes        */
    0,                                        /* use default stack size        */
    (LPTHREAD_START_ROUTINE) shotsave_thread, /* thread function       */
    0,                                        /* argument to thread function   */
    0,                                        /* use default creation flags    */
    &id );                                    /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, SHOTSAVE_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}
