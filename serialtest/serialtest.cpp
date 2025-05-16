#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;

static TCHAR * StopBitChoice[] =
    {
    { TEXT("1")   },
    { TEXT("1.5") },
    { TEXT("2")   }
    };
static const int NofStopBitChoices = 3;

static TCHAR ParityChoice[] = TEXT( "NOEMS" );
static const int NofParityChoices = 5;

static TCHAR MyClassName[]           = TEXT( "SerialTest" );
static TCHAR MonallIniFile[]         = TEXT( "monall.ini" );
static TCHAR SerialtestIniFile[]     = TEXT( "serialtest.ini" );
static TCHAR ConfigSection[]         = TEXT( "Config"     );
static TCHAR EosComBitsKey[]         = TEXT( "EosComBits" );
static TCHAR EosComCtsKey[]          = TEXT( "EosComCts" );
static TCHAR EosComDelimitCharKey[]  = TEXT( "EosComDelimitChar" );
static TCHAR EosComDsrKey[]          = TEXT( "EosComDsr" );
static TCHAR EosComEolKey[]          = TEXT( "EosComEol"  );
static TCHAR EosComParityKey[]       = TEXT( "EosComParity" );
static TCHAR EosComPortKey[]         = TEXT( "EosComPort" );
static TCHAR EosComSetupKey[]        = TEXT( "EosComSetup" );
static TCHAR EosComSpeedKey[]        = TEXT( "EosComSpeed" );
static TCHAR EosHaveAlarmShotKey[]   = TEXT( "EosHaveAlarmShot" );
static TCHAR EosHaveGoodShotKey[]    = TEXT( "EosHaveGoodShot" );
static TCHAR EosHaveWarningShotKey[] = TEXT( "EosHaveWarningShot" );
static TCHAR EosEndFileKey[]         = TEXT( "EosEndFile" );
static TCHAR EosStartFileKey[]       = TEXT( "EosStartFile" );
static TCHAR EosComStopKey[]         = TEXT( "EosComStop" );
static TCHAR EosComXonXoffKey[]      = TEXT( "EosComXonXoff" );
static TCHAR EosFixedWidthKey[]      = TEXT( "EosFixedWidth" );
static TCHAR EosNeedsStxEtxKey[]     = TEXT( "EosNeedsStxEtx" );
static TCHAR NullCharString[]        = TEXT( "\\0" );
static TCHAR TimerMsKey[]            = TEXT( "TimerMs" );

static TCHAR AChar         = TEXT( 'A' );
static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR CrChar        = TEXT( '\r' );
static TCHAR CChar         = TEXT( 'C' );
static TCHAR DChar         = TEXT( 'D' );
static TCHAR DoubleQuoteChar = TEXT( '\"' );
static TCHAR GChar         = TEXT( 'G' );
static TCHAR GTChar        = TEXT( '>' );
static TCHAR JChar         = TEXT( 'J' );
static TCHAR LChar         = TEXT( 'L' );
static TCHAR LfChar        = TEXT( '\n' );
static TCHAR LowerNChar    = TEXT( 'n' );
static TCHAR LowerRChar    = TEXT( 'r' );
static TCHAR LowerTChar    = TEXT( 't' );
static TCHAR LTChar        = TEXT( '<' );
static TCHAR MChar         = TEXT( 'M' );
static TCHAR NineChar      = TEXT( '9' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR OneChar       = TEXT( '1' );
static TCHAR PChar         = TEXT( 'P' );
static TCHAR QChar         = TEXT( 'Q' );
static TCHAR RChar         = TEXT( 'R' );
static TCHAR SChar         = TEXT( 'S' );
static TCHAR SpaceChar     = TEXT( ' ' );
static TCHAR TabChar       = TEXT( '\t' );
static TCHAR TChar         = TEXT( 'T' );
static TCHAR TwoChar       = TEXT( '2' );
static TCHAR YChar         = TEXT( 'Y' );
static TCHAR WChar         = TEXT( 'W' );
static TCHAR ZeroChar      = TEXT( '0' );

static char EtxBuf[]   = "\003";
static char StxBuf[]   = "\002";

STRING_CLASS EosComBits;
STRING_CLASS EosComDelimitChar;
STRING_CLASS EosComEol;
STRING_CLASS EosComParity;
STRING_CLASS EosComPort;
STRING_CLASS EosComSetup;
STRING_CLASS EosComSpeed;
STRING_CLASS EosComStop;
STRING_CLASS EosFixedWidth;
STRING_CLASS EosHaveAlarmShot;
STRING_CLASS EosHaveWarningShot;
STRING_CLASS EosHaveGoodShot;
STRING_CLASS EosStartFile;
STRING_CLASS EosEndFile;

BOOLEAN EosComCts      = FALSE;
BOOLEAN EosComDsr      = FALSE;
BOOLEAN EosComXonXoff  = FALSE;
BOOLEAN EosNeedsStxEtx = FALSE;

int EosComParityIndex = 0;
int EosComBitsIndex   = 0;
int EosComStopIndex   = 0;

static UINT DelayTimerId = 2;
static UINT TimerId = 1;
static UINT TimerMs = 0;
static bool TimerIsActive = false;
static int  MessageCount  = 0;

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          CONVERT_CONTROL_CHARS                       *
*                This is save to use on STRING_CLASS vars.             *
***********************************************************************/
static void convert_control_chars( TCHAR * sorc )
{
TCHAR * cp;

if ( !sorc )
    return;

while ( *sorc != NullChar )
    {
    if ( *sorc == BackslashChar )
        {
        cp = sorc;
        cp++;
        if ( *cp == LowerRChar )
            {
            *sorc = CrChar;
            remove_char( cp );
            }
        else if ( *cp == LowerNChar )
            {
            *sorc = LfChar;
            remove_char( cp );
            }
        else if ( *cp == LowerTChar )
            {
            *sorc = TabChar;
            remove_char( cp );
            }
        else if ( *cp == ZeroChar )
            {
            *sorc = NullChar;
            remove_char( cp );
            }
        else if ( ZeroChar < *cp && *cp <= NineChar )
            {
            *sorc = ( *cp - ZeroChar);
            remove_char( cp );
            }
        }
    sorc++;
    }
}

/***********************************************************************
*                         SET_COM_PORT_INDEXES                         *
***********************************************************************/
static void set_com_port_indexes()
{
int i;
TCHAR c;

i = EosComBits.int_value();
if ( i < 7 || i > 8 )
    i = 8;
EosComBitsIndex = i - 7;

EosComStopIndex   = 0;
for ( i=0; i<NofStopBitChoices; i++ )
    {
    if ( EosComStop == StopBitChoice[i] )
        {
        EosComStopIndex = i;
        break;
        }
    }

EosComParityIndex = 0;
c = *EosComParity.text();
for ( i=0; i<NofParityChoices; i++ )
    {
    if ( c == ParityChoice[i] )
        {
        EosComParityIndex = i;
        break;
        }
    }
}

/***********************************************************************
*                      WRITE_BYTES_TO_SERIAL_PORT                      *
***********************************************************************/
static BOOLEAN write_bytes_to_serial_port( void * sorc, DWORD bytes_to_write )
{
HANDLE  fh;
DWORD   bytes_written;
STRING_CLASS msg;
STRING_CLASS s;

bytes_written  = 0;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );
    CloseHandle( fh );
    }

if ( bytes_written != bytes_to_write )
    {
    s = TEXT( "Error:bytes to write = " );
    s += unsigned( bytes_to_write );
    msg = TEXT( "bytes written = " );
    msg += unsigned( bytes_written );
    MessageBox( 0, s.text(), msg.text(), MB_OK );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                    WRITE_HEX_FILE_TO_SERIAL_PORT                     *
* This is just the test program. The file should be in the same dir    *
* as I am.                                                             *
***********************************************************************/
static BOOLEAN write_hex_file_to_serial_port( STRING_CLASS & fn )
{
FILE_CLASS f;
STRING_CLASS s;
TCHAR        c1;
TCHAR        c2;
TCHAR      * cp;
TCHAR        hex[3];
char       * buf;
int          n;
DWORD        bytes_to_write;

if ( fn.isempty() )
    return FALSE;

if ( !file_exists(fn.text()) )
    return FALSE;

n = file_size( fn.text() );
if ( n < 3 )
    return FALSE;

buf = new char[n];
if ( !buf )
    return FALSE;

bytes_to_write = 0;

if ( !f.open_for_read(fn.text()) )
    return FALSE;

while ( true )
    {
    s = f.readline();
    if ( s.isempty() )
        break;
    cp = s.text();
    while ( true )
        {
        c1 = *cp;
        if ( !is_hex_char(c1) )
            break;
        c2 = *(cp+1);
        if ( !is_hex_char(c2) )
            break;
        hex[0] = c1;
        hex[1] = c2;
        hex[2] = NullChar;
        buf[bytes_to_write] = ul_to_char( hextoul(hex) );
        bytes_to_write++;
        cp += 2;
        if ( *cp != SpaceChar )
            break;
        cp++;
        }
    }

f.close();

if ( bytes_to_write )
    write_bytes_to_serial_port( buf, bytes_to_write );

delete[] buf;
return TRUE;
}

/***********************************************************************
*                     SEND_MESSAGE_TO_SERIAL_PORT                      *
***********************************************************************/
static void send_message_to_serial_port( TCHAR * sorc )
{
STRING_CLASS s;
STRING_CLASS eol;
DWORD   bytes_to_write;

if ( EosNeedsStxEtx )
    write_bytes_to_serial_port( StxBuf, 1 );

write_hex_file_to_serial_port( EosStartFile );

eol = EosComEol;
convert_control_chars( eol.text() );

s = sorc;
if ( s.isempty() )
    return;

if ( !eol.isempty() )
    s += eol;

bytes_to_write = s.len();
bytes_to_write *= sizeof( TCHAR );

write_bytes_to_serial_port( s.text(), bytes_to_write );
write_hex_file_to_serial_port( EosEndFile );

if ( EosNeedsStxEtx )
    write_bytes_to_serial_port( EtxBuf, 1 );
}

/***********************************************************************
*                     SEND_MESSAGE_TO_SERIAL_PORT                      *
  It is possible that the message contains a wait tag "<250>".
  If this is the case I need to send the first part and then que up
  a DelayTimerId timer, after which resume_sending_message_to_serial_
  port will send the rest.
***********************************************************************/
static void send_message_to_serial_port()
{
STRING_CLASS s;
TCHAR * cp;
TCHAR * wp;
UINT    ms;

s.get_text( MainDialog.control(MESSAGE_TBOX) );
if ( s.isempty() )
    return;

ms = 0;
cp = s.text();
while ( *cp != NullChar )
    {
    if ( *cp == LTChar )
        {
        cp++;
        wp = cp;
        while ( ZeroChar<=*cp && *cp<=NineChar )
            cp++;
        if ( cp != wp )
            {
            *cp = NullChar;
            ms = (UINT) asctoint32( wp );
            wp--;
            *wp = NullChar;
            }
        break;
        }
    cp++;
    }

if ( !s.isempty() )
    send_message_to_serial_port( s.text() );

if ( ms > 0 )
    SetTimer( MainDialog.handle(), DelayTimerId, ms , 0 );
}

/***********************************************************************
*                RESUME_SENDING_MESSAGE_TO_SERIAL_PORT                 *
* The delay timer has been received. I need to send everything after   *
* the delay tag <250>.                                                 *
***********************************************************************/
static void resume_sending_message_to_serial_port()
{
STRING_CLASS s;
TCHAR * cp;

KillTimer( MainDialog.handle(), DelayTimerId );

s.get_text( MainDialog.control(MESSAGE_TBOX) );
if ( s.isempty() )
    return;

cp = s.text();
while ( *cp != NullChar )
    {
    if ( *cp == GTChar )
        {
        cp++;
        if ( *cp != NullChar )
            send_message_to_serial_port( cp );
        break;
        }
    cp++;
    }
}

/***********************************************************************
*                             TOGGLE_TIMER                             *
***********************************************************************/
static void toggle_timer()
{
if ( TimerIsActive )
    {
    TimerIsActive = false;
    KillTimer( MainDialog.handle(), TimerId );
    set_text( MainDialog.control(TIMED_MESSAGES_PB), TEXT("Begin") );
    }
else if ( TimerMs > 0 )
    {
    TimerIsActive = true;
    MessageCount = 0;
    set_text( MainDialog.control(MESSAGE_COUNT_TBOX), TEXT("0") );

    SetTimer( MainDialog.handle(), TimerId, TimerMs, 0 );
    set_text( MainDialog.control(TIMED_MESSAGES_PB), TEXT("Stop") );
    }
else
    {
    MessageBox( MainDialog.handle(), TEXT("Seconds must be > 0"), TEXT("Timer Error"), MB_OK );
    }
}

/***********************************************************************
*                             MAKE_MESSAGE                             *
***********************************************************************/
static void make_message()
{
static int goodness    = 0;
static int shot_number = 0;
TCHAR buf[100];
STRING_CLASS s;
STRING_CLASS delimiter;
TCHAR   c;
TCHAR * bp;
TCHAR * cp;
TCHAR * dp; /* Used for replacing the comma with a cr or lf */
int     fixed_width;
int     j;
int     n;
bool    firstfield;
bool    have_delimiter;
SYSTEMTIME st;

GetLocalTime( &st );

fixed_width = EosFixedWidth.int_value();
firstfield  = true;

have_delimiter = true;
delimiter = EosComDelimitChar;
if ( delimiter == NullCharString )
    have_delimiter = false;
convert_control_chars( delimiter.text() );
if ( delimiter.isempty() )
    delimiter = TEXT( "," );

cp = EosComSetup.text();
while ( *cp != NullChar )
    {
    if ( firstfield )
        firstfield = false;
    else if ( have_delimiter )
        s += delimiter;

    if ( is_numeric(*cp) )
        {
        n = asctoint32( cp );
        rounded_double_to_tchar( buf, (double) n );
        if ( fixed_width > 0 )
            rjust( buf, (short) fixed_width, SpaceChar );
        s += buf;
        }
    else if ( *cp == CChar )
        {
        /*
        --------------------
        This is a cmi robot.
                  10        20
        0123456789_123456789_
        mm/dd/yy,hh,mm,ss\015
        -------------------- */
        bp = buf;
        lstrcpy( bp, alphadate(st) );
        bp[6] = bp[8];
        bp[7] = bp[9];
        bp[8] = CommaChar;
        bp += 9;
        lstrcpy( bp, alphatime(st) );
        bp[2] = CommaChar;
        bp[5] = CommaChar;
        bp[8] = CrChar;
        bp[9] = NullChar;
        s = buf;
        }
    else if ( *cp == LChar )
        {
        if ( s.len() > 0 )
            {
            dp = s.text() + s.len() - 1;
            if ( *dp == CommaChar )
                *dp = LfChar;
            else
                s += LfChar;
            firstfield = true;
            }
        }
    else if ( *cp == RChar )
        {
        if ( s.len() > 0 )
            {
            dp = s.text() + s.len() - 1;
            if ( *dp == CommaChar )
                *dp = CrChar;
            else
                s += CrChar;
            firstfield = true;
            }
        }
    else if ( *cp == GChar )
        {
        if ( goodness == 0 )
            s += EosHaveAlarmShot;
        else if ( goodness == 1 )
            s += EosHaveWarningShot;
        else
            s += EosHaveGoodShot;
        goodness ++;
        if ( goodness > 2 )
            goodness = 0;
        }
    else if ( *cp == MChar )
        {
        s += TEXT( "M01" );
        }
    else if ( *cp == PChar )
        {
        s += TEXT( "VISITRAK" );
        }
    else if ( *cp == SChar )
        {
        shot_number++;
        n = 0;
        if ( is_numeric(*(cp+1)) )
            {
            n = asctoint32(cp+1);
            if ( n > 0 )
                {
                insalph( buf, shot_number, n, ZeroChar, DECIMAL_RADIX );
                buf[n] = NullChar;
                }
            }
        if ( n > 0 )
            s += buf;
        else
            s += shot_number;
        }
    else if ( *cp == DChar )
        {
        s += alphadate( st );
        bp = s.text() + s.len() - ALPHADATE_LEN;
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
    else if ( *cp == QChar )
        {
        cp++;
        dp = cp;
        while ( *dp != CommaChar && *dp != NullChar )
            dp++;
        c = *dp;
        *dp = NullChar;
        copystring( buf, cp );
        convert_control_chars( buf );
        s += buf;
        *dp = c;
        }
    else if ( *cp == WChar )
        {
        cp++;
        dp = cp;
        while ( *dp != CommaChar && *dp != NullChar )
            dp++;
        c = *dp;
        *dp = NullChar;
        s += LTChar;
        s += cp;
        s += GTChar;
        *dp = c;
        }
    else if ( *cp == YChar )
        {
        copystring( buf,  alphadate(st) );
        bp = buf + ALPHADATE_LEN -1;
        if ( *(cp+1) == TwoChar )
            bp--;
        s += bp;
        }
    else if ( *cp == JChar )
        {
        j = dofyr( st );
        j++;
        n = 0;
        if ( is_numeric(*(cp+1)) )
            {
            n = asctoint32(cp+1);
            if ( n > 0 )
                {
                insalph( buf, j, n, ZeroChar, DECIMAL_RADIX );
                buf[n] = NullChar;
                }
            }
        if ( n > 0 )
            s += buf;
        else
            s += j;
        }
    else if ( *cp == TChar )
        {
        s += alphatime( st );
        bp = s.text() + s.len() - ALPHATIME_LEN;
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
        else if ( *cp == OneChar )
            {
            *(bp+2) = *(bp+3);
            *(bp+3) = *(bp+4);
            *(bp+4) = NullChar;
            }
        else
            {
            cp--;
            }
        }

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

s.set_text( MainDialog.control(MESSAGE_TBOX) );
}

/***********************************************************************
*                            SAVE_SETTINGS                             *
***********************************************************************/
static void save_settings( STRING_CLASS & s )
{
INI_CLASS ini;

ini.set_file( s.text() );
ini.set_section( ConfigSection );

ini.put_string( EosComBitsKey,        EosComBits.text()        );
ini.put_string( EosComDelimitCharKey, EosComDelimitChar.text() );
ini.put_string( EosComEolKey,         EosComEol.text()         );
ini.put_string( EosComParityKey,      EosComParity.text()      );
ini.put_string( EosComPortKey,        EosComPort.text()        );
ini.put_string( EosComSetupKey,       EosComSetup.text()       );
ini.put_string( EosComSpeedKey,       EosComSpeed.text()       );
ini.put_string( EosComStopKey,        EosComStop.text()        );
ini.put_string( EosFixedWidthKey,     EosFixedWidth.text()     );

if ( EosStartFile.isempty() )
    ini.remove_key( EosStartFileKey );
else
    ini.put_string( EosStartFileKey, EosStartFile.text()  );

if ( EosEndFile.isempty() )
    ini.remove_key( EosEndFileKey );
else
    ini.put_string( EosEndFileKey, EosEndFile.text()  );

ini.put_boolean( EosNeedsStxEtxKey, EosNeedsStxEtx );
ini.put_boolean( EosComCtsKey,      EosComCts );
ini.put_boolean( EosComDsrKey,      EosComDsr );
ini.put_boolean( EosComXonXoffKey,  EosComXonXoff );

s = exe_directory();
s.cat_path( SerialtestIniFile );
ini.set_file( s.text() );
ini.put_ul( TimerMsKey, TimerMs );
}

/***********************************************************************
*                         SAVE_SETTINGS_TO_MONALL                      *
***********************************************************************/
static void save_settings_to_monall()
{
STRING_CLASS s;
s = TEXT( "C:\\V5\\Exes" );
s.cat_path( MonallIniFile );

if ( file_exists(s.text()) )
    save_settings( s );
}

/***********************************************************************
*                    SAVE_SETTINGS_TO_SERIALTEST                       *
***********************************************************************/
static void save_settings_to_serialtest()
{
STRING_CLASS s;

s = exe_directory();
s.cat_path( SerialtestIniFile );
if ( file_exists(s.text()) )
    save_settings( s );
}

/***********************************************************************
*                       GET_SETTINGS_FROM_SCREEN                       *
***********************************************************************/
static void get_settings_from_screen()
{
int i;
int id;
STRING_CLASS s;
TCHAR buf[2];

if ( is_checked(MainDialog.control(BIT_7_RADIO)) )
    i = 7;
else
    i = 8;

EosComBits = int32toasc( i );

EosComStop = StopBitChoice[0];
id = STOP_1_RADIO;
for ( i=0; i<NofStopBitChoices; i++ )
    {
    if ( is_checked(MainDialog.control(id)) )
        {
        EosComStop = StopBitChoice[i];
        break;
        }
    id++;
    }

buf[0] = ParityChoice[0];
buf[1] = NullChar;
id = NO_PARITY_RADIO;
for ( i=0; i<NofParityChoices; i++ )
    {
    if ( is_checked(MainDialog.control(id)) )
        {
        buf[0] = ParityChoice[i];
        break;
        }
    id++;
    }
EosComParity = buf;

set_com_port_indexes();

EosComEol.get_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.get_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.get_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.get_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.get_text(       MainDialog.control(SETUP_EBOX)       );
EosComSetup.uppercase();
EosComSpeed.get_text(       MainDialog.control(SPEED_EBOX)       );
EosStartFile.get_text(      MainDialog.control(START_FILE_TBOX)  );
EosEndFile.get_text(        MainDialog.control(END_FILE_TBOX)    );

EosComCts      = is_checked( MainDialog.control(CTS_XBOX)      );
EosComDsr      = is_checked( MainDialog.control(DSR_XBOX)      );
EosComXonXoff  = is_checked( MainDialog.control(XON_XOFF_XBOX) );
EosNeedsStxEtx = is_checked( MainDialog.control(STX_ETX_XBOX)  );

s.get_text( MainDialog.control(SECONDS_EBOX) );
TimerMs = s.uint_value() * 1000;
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
bool init_serial_port( void )
{
static unsigned StopBitSetting[NofStopBitChoices] = { ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS };

HANDLE  fh;
DCB     dcb;
BOOL    status;

if ( EosComPort.isempty() )
    return false;

dcb.DCBlength = sizeof( DCB );
status = FALSE;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh == INVALID_HANDLE_VALUE )
    {
    MessageBox( 0, EosComPort.text(), "Unable to open port", MB_OK );
    return false;
    }

status = GetCommState( fh, &dcb );
CloseHandle( fh );

if ( !status )
    return false;

dcb.BaudRate = (DWORD) EosComSpeed.uint_value();
dcb.Parity   = EosComParityIndex;
if ( dcb.Parity == 0 )
    dcb.fParity = 0;
else
    dcb.fParity = 1;
dcb.ByteSize = unsigned( 7 + EosComBitsIndex );
dcb.StopBits = StopBitSetting[EosComStopIndex];

/*
---------------------------------------------------
Tell the other computer I am always ready for chars
--------------------------------------------------- */
dcb.fRtsControl = RTS_CONTROL_ENABLE;
dcb.fDtrControl = DTR_CONTROL_ENABLE;

if ( EosComCts )
    dcb.fOutxCtsFlow = 1;
else
    dcb.fOutxCtsFlow = 0;

if ( EosComDsr )
    dcb.fOutxDsrFlow = 1;
else
    dcb.fOutxDsrFlow = 0;

dcb.fDsrSensitivity   = 0;
dcb.fTXContinueOnXoff = 1;
dcb.fInX              = 0;

if ( EosComXonXoff )
    dcb.fOutX = 1;
else
    dcb.fOutX = 0;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    SetCommState( fh, &dcb );
    CloseHandle( fh );
    return true;
    }

MessageBox( 0, EosComPort.text(), "Unable to initialize port", MB_OK );
return false;
}

/***********************************************************************
*                           INIT_MAIN_DIALOG                           *
***********************************************************************/
void init_main_dialog()
{
INI_CLASS ini;
STRING_CLASS s;
int          i;

s = exe_directory();
s.cat_path( MonallIniFile );

if ( !file_exists(s.text()) )
    {
    s = exe_directory();
    s.cat_path( SerialtestIniFile );
    }

ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(EosComBitsKey) )
    EosComBits = ini.get_string();

if ( ini.find(EosComEolKey) )
    EosComEol = ini.get_string();

if ( ini.find(EosComParityKey) )
    EosComParity = ini.get_string();

if ( ini.find(EosComPortKey) )
    EosComPort = ini.get_string();

if ( ini.find(EosComSetupKey) )
    EosComSetup = ini.get_string();

if ( ini.find(EosComSpeedKey) )
    EosComSpeed = ini.get_string();

if ( ini.find(EosComStopKey) )
    EosComStop = ini.get_string();

if ( ini.find(EosFixedWidthKey) )
    EosFixedWidth = ini.get_string();

if ( ini.find(EosComDelimitCharKey) )
    EosComDelimitChar = ini.get_string( EosComDelimitCharKey );

if ( ini.find(EosStartFileKey) )
    EosStartFile = ini.get_string();
else
    EosStartFile.empty();

if ( ini.find(EosEndFileKey) )
    EosEndFile = ini.get_string();
else
    EosEndFile.empty();

if ( ini.find(EosHaveAlarmShotKey) )
    EosHaveAlarmShot = ini.get_string();
else
    EosHaveAlarmShot = AChar;

if ( ini.find(EosHaveGoodShotKey) )
    EosHaveGoodShot = ini.get_string();
else
    EosHaveGoodShot = GChar;

if ( ini.find(EosHaveWarningShotKey) )
    EosHaveWarningShot = ini.get_string();
else
    EosHaveWarningShot = WChar;

    EosHaveGoodShot = maketext( TEXT("G") );

EosNeedsStxEtx   = ini.get_boolean( EosNeedsStxEtxKey );
EosComCts        = ini.get_boolean( EosComCtsKey );
EosComDsr        = ini.get_boolean( EosComDsrKey );
EosComXonXoff    = ini.get_boolean( EosComXonXoffKey );

EosComEol.set_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.set_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.set_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.set_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.set_text(       MainDialog.control(SETUP_EBOX)       );
EosComSpeed.set_text(       MainDialog.control(SPEED_EBOX)       );
EosStartFile.set_text(      MainDialog.control(START_FILE_TBOX)  );
EosEndFile.set_text(        MainDialog.control(END_FILE_TBOX)    );

set_checkbox( MainDialog.control(CTS_XBOX),      EosComCts );
set_checkbox( MainDialog.control(DSR_XBOX),      EosComDsr );
set_checkbox( MainDialog.control(XON_XOFF_XBOX), EosComXonXoff );
set_checkbox( MainDialog.control(STX_ETX_XBOX),  EosNeedsStxEtx );

set_com_port_indexes();

i = EosComBitsIndex + BIT_7_RADIO;
CheckRadioButton( MainDialog.handle(), BIT_7_RADIO, BIT_8_RADIO, i );

i = EosComStopIndex + STOP_1_RADIO;
CheckRadioButton( MainDialog.handle(), STOP_1_RADIO, STOP_2_RADIO, i );

i = EosComStopIndex + NO_PARITY_RADIO;
CheckRadioButton( MainDialog.handle(), NO_PARITY_RADIO, MARK_PARITY_RADIO, i );

s = exe_directory();
s.cat_path( SerialtestIniFile );
ini.set_file( s.text() );

if ( ini.find(TimerMsKey) )
    TimerMs = ini.get_ul();
else
    TimerMs = 0;
s.empty();
s += unsigned( TimerMs/1000 );
s.set_text( MainDialog.control(SECONDS_EBOX) );
}

/***********************************************************************
*                             MAIN_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HWND countwindow;
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        init_main_dialog();
        countwindow = MainDialog.control( MESSAGE_COUNT_TBOX );
        set_text( countwindow, int32toasc( (int32) MessageCount) );
        return TRUE;

    case WM_INITDIALOG:
        MainDialog = hWnd;
        MainWindow.shrinkwrap( hWnd );
        MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_TIMER:
        if ( wParam == TimerId )
            {
            MessageCount++;
            set_text( countwindow, int32toasc( (int32) MessageCount) );
            make_message();
            send_message_to_serial_port();
            }
        else if ( wParam == DelayTimerId )
            {
            resume_sending_message_to_serial_port();
            }
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case INIT_COM_PORT_PB:
                get_settings_from_screen();
                init_serial_port();
                return TRUE;

            case SAVE_SETUP_PB:
            case SAVE_TO_MONALL_INI_PB:
                get_settings_from_screen();
                if ( id == SAVE_SETUP_PB )
                    save_settings_to_serialtest();
                else
                    save_settings_to_monall();
                return TRUE;

            case TIMED_MESSAGES_PB:
                get_settings_from_screen();
                init_serial_port();
                toggle_timer();
                return TRUE;

            case SEND_ONE_MESSAGE_PB:
                get_settings_from_screen();
                make_message();
                init_serial_port();
                MessageCount++;
                set_text( countwindow, int32toasc( (int32) MessageCount) );
                send_message_to_serial_port();
                return TRUE;

            case MAKE_MESSAGE_PB:
                get_settings_from_screen();
                make_message();
                return TRUE;

            case IDCANCEL:
                MainWindow.close();
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_DESTROY:
        if ( TimerIsActive )
            {
            TimerIsActive = false;
            KillTimer( MainDialog.handle(), TimerId );
            }
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "MAIN_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialog.handle() )
        status = IsDialogMessage( MainDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
