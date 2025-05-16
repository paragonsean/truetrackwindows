#include <windows.h>
#include <process.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\fileclas.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "extern.h"
#include "shotsave.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR CrChar        = TEXT( '\r' );
static TCHAR EtxChar       = TEXT( '\003' );
static TCHAR LowerNChar    = TEXT( 'n' );
static TCHAR LowerRChar    = TEXT( 'r' );
static TCHAR CChar         = TEXT( 'C' );
static TCHAR DChar         = TEXT( 'D' );
static TCHAR GChar         = TEXT( 'G' );
static TCHAR JChar         = TEXT( 'J' );
static TCHAR LChar         = TEXT( 'L' );
static TCHAR LfChar        = TEXT( '\n' );
static TCHAR MChar         = TEXT( 'M' );
static TCHAR NChar         = TEXT( 'N' );
static TCHAR OneChar       = TEXT( '1' );
static TCHAR PChar         = TEXT( 'P' );
static TCHAR RChar         = TEXT( 'R' );
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

static TCHAR NullCharString[] = TEXT( "\\0" );

static BOOLEAN SaveAllAlarmShots = FALSE;

static BOOLEAN ShowMachine       = TRUE;
static BOOLEAN ShowPart          = TRUE;
static BOOLEAN ShowDate          = TRUE;
static BOOLEAN ShowTime          = TRUE;
static BOOLEAN ShowShot          = TRUE;
static BOOLEAN ShowParam[MAX_PARMS];

static TCHAR * EosComPort     = 0;
static TCHAR * EosComSetup    = 0;
static TCHAR * EosHaveAlarmShot   = 0;
static TCHAR * EosHaveWarningShot = 0;
static TCHAR * EosHaveGoodShot    = 0;
static TCHAR   EosComDelimitChar = TEXT( ',' );
static TCHAR   EosComEol[3]      = TEXT( "\r\n" );
static int     EosFixedWidth     = 0;
static BOOLEAN EosNeedsStxEtx    = FALSE;
static BOOLEAN EosOnlyIfNoAlarms = FALSE;
static TCHAR   EosPyramidLines   = ZeroChar;
static BOOLEAN HaveDelimitChar   = TRUE;
static char EtxBuf[]   = "\003";
static char StxBuf[]   = "\002";

static NAME_CLASS EosEndFile;
static NAME_CLASS EosStartFile;

static TCHAR EosEndFileKey[]     = TEXT( "EosEndFile" );
static TCHAR EosHaveAlarmShotKey[]   = TEXT( "EosHaveAlarmShot" );
static TCHAR EosHaveGoodShotKey[]    = TEXT( "EosHaveGoodShot" );
static TCHAR EosHaveWarningShotKey[] = TEXT( "EosHaveWarningShot" );
static TCHAR EosStartFileKey[]   = TEXT( "EosStartFile" );

static short  WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */

static const float MaxFloatShort = float( 0xFFFF );

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
if ( !unknown(cp) )
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
if ( !unknown(cp) && *cp != TabChar )
    DelimitChar = *cp;

cp = get_ini_string( MonallIniFile, ConfigSection, EosStartFileKey );
if ( !unknown(cp) )
    EosStartFile.get_local_ini_file_name( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosEndFileKey );
if ( !unknown(cp) )
    EosEndFile.get_local_ini_file_name( cp );

/*
----------------------------------------------
Get the name of the comm port "COM1" or "COM2"
---------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComPort") );
if ( lstrcmp(cp, UNKNOWN) != 0 && (*cp != NChar) && (*cp != LowerNChar) )
    EosComPort = maketext( cp );

/*
-----------------------------------------------------------
Get the names of the alarm, warning, and good shot strings.
Default to A,W,G.
----------------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveAlarmShotKey );
if ( unknown(cp) )
    EosHaveAlarmShot = maketext( TEXT("A") );
else
    EosHaveAlarmShot = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveWarningShotKey );
if ( unknown(cp) )
    EosHaveWarningShot = maketext( TEXT("W") );
else
    EosHaveWarningShot = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveGoodShotKey );
if ( unknown(cp) )
    EosHaveGoodShot = maketext( TEXT("G") );
else
    EosHaveGoodShot = maketext( cp );

/*
--------------------
See if I need an STX
-------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosNeedsStxEtx") );
if ( *cp == YChar )
    EosNeedsStxEtx = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosOnlyIfNoAlarms") );
if ( *cp == YChar )
    EosOnlyIfNoAlarms = TRUE;

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
if ( lstrcmp(cp, NullCharString) == 0 )
    HaveDelimitChar = FALSE;
else if ( lstrcmp(cp, UNKNOWN) != 0 )
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

if ( EosComPort )
    {
    cp = get_long_ini_string( MonallIniFile, ConfigSection, TEXT("EosComSetup") );
    if ( !unknown(cp) )
        {
        EosComSetup = maketext( cp );
        }
    else
        {
        delete[] EosComPort;
        EosComPort = 0;
        }
    if ( EosComPort )
        init_serial_port();
    }

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

killtext( EosComPort );
killtext( EosComSetup );
killtext( EosHaveAlarmShot );
killtext( EosHaveWarningShot );
killtext( EosHaveGoodShot );
}

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
BOOLEAN nextshot( SHOT_BUFFER_ENTRY * b )
{

DB_TABLE   t;
SYSTEMTIME shot_time;
int32      shot_number;
BOOLEAN    have_rec;
BOOLEAN    need_to_insert = FALSE;
STRING_CLASS s;

FileTimeToSystemTime( &b->f.shot_time, &shot_time );

/*
--------------------------------------------
If there is no plookup record, start at one.
-------------------------------------------- */
shot_number = 0;

if ( !get_machine_directory_file_name(s, b->pn.computer, b->pn.machine, PLOOKUP_DB) )
    return FALSE;

t.open( s.text(), PLOOKUP_RECLEN, PWL );
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, b->pn.part, PART_NAME_LEN);
t.reset_search_mode();
have_rec = t.get_next_key_match(1, TRUE);
if ( have_rec )
    shot_number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
else
    need_to_insert = t.goto_first_greater_than_record( 1 );

shot_number++;
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, b->pn.part, PART_NAME_LEN);
t.put_long( PLOOKUP_LAST_SHOT_OFFSET, shot_number, SHOT_LEN );
t.put_date( PLOOKUP_DATE_OFFSET, shot_time );
t.put_time( PLOOKUP_TIME_OFFSET, shot_time );

if ( have_rec )
    {
    t.rec_update();
    t.unlock_record();
    }
else
    {
    if ( need_to_insert )
        t.rec_insert();
    else
        t.rec_append();
    }

t.close();

if ( t.get_global_error() == VS_SUCCESS )
    {
    copystring( b->pn.shot, int32toasc(shot_number) );
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                        WRITE_FTII_PARAMETERS                         *
***********************************************************************/
BOOLEAN write_ftii_parameters( SHOT_BUFFER_ENTRY * b, SYSTEMTIME & st, BITSETYPE save_flags )
{
int         i;
int         shot_number;
FIELDOFFSET fo;
DWORD       n;
DB_TABLE    t;
STRING_CLASS s;

shot_number = asctoint32( b->pn.shot );

if ( !get_part_results_file_name(s, b->pn.computer, b->pn.machine, b->pn.part, SHOTPARM_DB) )
    return FALSE;

if ( !file_exists(s.text()) )
    t.create( s.text() );

if ( t.open( s.text(), SHOTPARM_RECLEN, WL, WRITE_PARAM_TIMEOUT) )
    {
    t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
    t.put_date( SHOTPARM_DATE_OFFSET, st );
    t.put_time( SHOTPARM_TIME_OFFSET, st );
    t.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) save_flags, SHOTPARM_SAVE_FLAGS_LEN );
    fo = SHOTPARM_PARAM_1_OFFSET;
    for ( i=0; i<MAX_PARMS; i++ )
        {
        t.put_double( fo, b->param[i], SHOTPARM_FLOAT_LEN );
        fo += SHOTPARM_FLOAT_LEN+1;
        }
    t.rec_append();

    /*
    ----------------------------------------------------------------------
    Check back 51 shots and see if I should delete the record.
    Record the flags for write_profile so he doesn't have to look this up.
    ---------------------------------------------------------------------- */
    n = t.nof_recs();
    if ( n > (DWORD) HistoryShotCount )
        {
        n -= HistoryShotCount;
        /*
        --------------------------------------------------
        Subtract one more because the last record is n - 1
        -------------------------------------------------- */
        n--;
        t.goto_record_number(n);
        t.get_rec();

        save_flags = (BITSETYPE) t.get_short( SHOTPARM_SAVE_FLAGS_OFFSET );

        if ( !(save_flags & SAVE_PARM_DATA) )
            t.rec_delete();
        }

    n = max_parameters_saved( b->pn.machine );
    if ( n > 0 && n < t.nof_recs() )
        {
        t.goto_record_number( 0 );
        t.rec_delete();
        }

    t.close();

    if ( t.get_global_error() == VS_SUCCESS )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         ADD_TO_SHOTSAVE                              *
*                                                                      *
* This is called by getdata to add the shot to the buffer. Once I      *
* set the BufPtr, I should no longer make any changes to it.           *
* This is the only routine that is known globally.                     *
***********************************************************************/
bool add_to_shotsave( bool new_has_alarm, bool new_has_warning, PART_NAME_ENTRY & partname, FTII_FILE_DATA & fsorc, DOUBLE_ARRAY_CLASS & psorc )
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
            b->save_failed = false;
            b->has_alarm   = new_has_alarm;
            b->has_warning = new_has_warning;
            b->pn          = partname;
            b->f.move( fsorc );
            b->param.move( psorc );
            BufPtr[i] = b;
            return true;
            }
        }
    }

return false;
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

FileTimeToSystemTime( &b->f.shot_time, &st );

cp = buf;

if ( ShowMachine )
    cp = copy_w_char( cp, b->pn.machine, DelimitChar );

if ( ShowPart )
    cp = copy_w_char( cp, b->pn.part,    DelimitChar );

if ( ShowDate )
    cp = copy_w_char( cp, alphadate(st), DelimitChar );

if ( ShowTime )
    cp = copy_w_char( cp, alphatime(st), DelimitChar );

/*
-----------
Shot Number
----------- */
if ( ShowShot )
    cp = copystring( cp, b->pn.shot );

/*
----------
Parameters
---------- */
for ( i=0; i<b->param.count(); i++ )
    {
    if ( ShowParam[i] )
        {
        *cp = DelimitChar;
        cp++;
        cp = copystring( cp, ascii_double(b->param[i]) );
        }
    }

*cp = NullChar;

return buf;
}

/***********************************************************************
*                      WRITE_BYTES_TO_SERIAL_PORT                      *
***********************************************************************/
static BOOLEAN write_bytes_to_serial_port( void * sorc, DWORD bytes_to_write )
{
HANDLE  fh;
DWORD   bytes_written;

bytes_written  = 0;

fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );
    CloseHandle( fh );
    }

return TRUE;
}

/***********************************************************************
*                    WRITE_HEX_FILE_TO_SERIAL_PORT                     *
* This is just the test program. The file should be in the same dir    *
* as I am.                                                             *
***********************************************************************/
static BOOLEAN write_hex_file_to_serial_port( NAME_CLASS & fn )
{
FILE_CLASS   f;
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
*                         WRITE_SERIAL_PORT_INFO                       *
***********************************************************************/
void write_serial_port_info( SHOT_BUFFER_ENTRY * b )
{
const int32 SLEN = 1 + MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + MAX_INTEGER_LEN + ALPHADATE_LEN + 1 + ALPHATIME_LEN + (MAX_PARMS*MAX_FLOAT_LEN) + MAX_PARMS + 100;

int32   j;
int32   n;
TCHAR * bp;
TCHAR * buf;
TCHAR * cp;
BOOLEAN firstfield;
DWORD   bytes_to_write;
SYSTEMTIME st;
TCHAR   shot_number[SHOT_LEN+1];

if ( EosOnlyIfNoAlarms )
    {
    if ( b->has_alarm )
        return;
    }

buf = maketext( SLEN );
if ( !buf )
    return;

copystring( shot_number, b->pn.shot );
rjust( shot_number, 6, ZeroChar );

FileTimeToSystemTime( &b->f.shot_time, &st );

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
    bp  = copystring( bp, shot_number );
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
    bp  = copystring( bp, shot_number );
    *bp = CrChar;
    bp++;
    *bp = NullChar;
    }
else
    {
    if ( EosNeedsStxEtx )
        write_bytes_to_serial_port( StxBuf, 1 );

    write_hex_file_to_serial_port( EosStartFile );

    while ( TRUE )
        {
        if ( *cp == NullChar )
            break;

        if ( firstfield )
            {
            firstfield = FALSE;
            }
        else if ( HaveDelimitChar )
            {
            *bp = EosComDelimitChar;
            bp++;
            }

        if ( is_numeric(*cp) )
            {
            n = asctoint32( cp );
            if ( n >= 0 && n <= MAX_PARMS )
                {
                rounded_double_to_tchar( bp, b->param[n] );
                if ( EosFixedWidth > 0 )
                    rjust( bp, (short) EosFixedWidth, SpaceChar );
                }
            else
                {
                lstrcpy( bp, unknown_string() );
                }
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
            }
        else if ( *cp == LChar )
            {
            if ( bp > buf && HaveDelimitChar )
                {
                bp--;
                if ( *bp != EosComDelimitChar )
                    bp++;
                }
            *bp = LfChar;
            bp++;
            *bp = NullChar;
            firstfield = true;
            }
        else if ( *cp == RChar )
            {
            if ( bp > buf && HaveDelimitChar )
                {
                bp--;
                if ( *bp != EosComDelimitChar )
                    bp++;
                }
            *bp = CrChar;
            bp++;
            *bp = NullChar;
            firstfield = true;
            }
        else if ( *cp == GChar )
            {
            if ( b->has_alarm )
                copystring( bp, EosHaveAlarmShot );
            else if ( b->has_warning )
                copystring( bp, EosHaveWarningShot );
            else
                copystring( bp, EosHaveGoodShot );
            }
        else if ( *cp == MChar )
            {
            lstrcpy( bp, b->pn.machine );
            }
        else if ( *cp == PChar )
            {
            lstrcpy( bp, b->pn.part );
            }
        else if ( *cp == SChar )
            {
            n = 0;
            if ( is_numeric(*(cp+1)) )
                n = asctoint32(cp+1);
            copystring( bp, b->pn.shot );
            if ( n > 0 )
                {
                rjust( bp, n, ZeroChar );
                bp += n;
                }
            else if ( *bp != NullChar )
                {
                bp += lstrlen(bp);;
                }
            *bp = NullChar;
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
        else if ( *cp == YChar )
            {
            copystring( bp,  alphadate(st) );
            *bp     = *(bp + ALPHADATE_LEN -1);
            *(bp+1) = NullChar;
            }
        else if ( *cp == JChar )
            {
            j = dofyr( st );
            j++;
            n = 0;
            if ( is_numeric(*(cp+1)) )
                n = asctoint32(cp+1);
            copystring( bp, int32toasc(j)  );
            if ( n > 0 )
                {
                rjust( bp, n, ZeroChar );
                bp += n;
                }
            else if ( *bp != NullChar )
                {
                bp += lstrlen(bp);;
                }
            *bp = NullChar;
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

    *bp = NullChar;
    }

bytes_to_write = lstrlen(buf) * sizeof( TCHAR );
write_bytes_to_serial_port( buf, bytes_to_write );

write_hex_file_to_serial_port( EosEndFile );

if ( EosNeedsStxEtx )
    write_bytes_to_serial_port( EtxBuf, 1 );

delete[] buf;
}

/***********************************************************************
*                             * BIT_STATE                              *
***********************************************************************/
static TCHAR * bit_state( unsigned x )
{
static TCHAR On[] = TEXT( " ON" );
static TCHAR Off[] = TEXT( " OFF" );
if ( x )
    return On;
return Off;
}

/***********************************************************************
*                               MAKE_LINE                              *
***********************************************************************/
static void make_line( STRING_CLASS & s, unsigned x, double timer, int index )
{
TCHAR buf[100];

s += bit_state( x );
if ( index > 0 )
    {
    s += TEXT( " at time: " );
    if ( double_to_tchar( buf, timer, 3) )
        s += buf;
    }
}

/***********************************************************************
*                            WRITE_ALS_LOG                             *
***********************************************************************/
static void write_als_log( TCHAR * shot_number, FTII_FILE_DATA & fd )
{
/*
-------
Outputs
------- */
static unsigned slow_shot_output_mask   = 0x10000;
static unsigned accumulator_output_mask = 0x20000;

/*
------
Inputs
------ */
static unsigned retract_mask        = 0x80000;
static unsigned follow_through_mask = 0x40000;
static unsigned start_shot_mask     = 0x20000;
static unsigned accumulator_mask    = 0x40000000;
static unsigned slow_shot_mask      = 0x20000000;

FILE_CLASS f;
int      i;
unsigned last_input;
unsigned last_output;
unsigned x;
double   seconds;
double   real_timer_frequency;

STRING_CLASS s;
FTII_PROFILE fp;

if ( !AlsLogIsActive )
    return;

fp = fd;
real_timer_frequency = (double) fp.timer_frequency;
if ( is_zero(real_timer_frequency) )
    real_timer_frequency = 1.0;

if ( !f.open_for_append(AlsLogFile) )
    return;

f.writeline( TEXT(" ") );

s = TEXT( "Begin Shot Data: Shot Number " );
s += shot_number;
f.writeline( s.text() );

last_input = ~fp.isw1[0];
last_output = ~fp.osw1[0];

f.writeline( TEXT("--------------- At the Start of shot-------------------") );

for ( i=0; i<fp.n; i++ )
    {
    seconds = (double) fp.timer[i];
    seconds /= real_timer_frequency;
    x = fp.isw1[i];
    if ( (x & start_shot_mask) != (last_input & start_shot_mask) )
        {
        s = TEXT( "Start Shot     INPUT" );
        make_line( s, (x & start_shot_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & retract_mask) != ( last_input & retract_mask) )
        {
        s = TEXT( "Retract        INPUT" );
        make_line( s, (x & retract_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & follow_through_mask) != ( last_input & follow_through_mask) )
        {
        s = TEXT( "Follow Through INPUT" );
        make_line( s, (x & follow_through_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & slow_shot_mask) != ( last_input & slow_shot_mask) )
        {
        s = TEXT( "Slow Shot      INPUT" );
        make_line( s, (x & slow_shot_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & accumulator_mask) != ( last_input & accumulator_mask) )
        {
        s = TEXT( "Accumulator    INPUT" );
        make_line( s, (x & accumulator_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( i == fp.np )
        {
        s = TEXT( "Impact at time: " );
        s += ascii_double(seconds);
        f.writeline( s.text() );
        }

    x = fp.osw1[i];
    if ( (x & slow_shot_output_mask) != (last_output & slow_shot_output_mask) )
        {
        s = TEXT( "Slow Shot      OUTPUT" );
        make_line( s, (x & slow_shot_output_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & accumulator_output_mask) != (last_output & accumulator_output_mask) )
        {
        s = TEXT( "Accumulator    OUTPUT" );
        make_line( s, (x & accumulator_output_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( i == 0 )
        f.writeline( TEXT("-------------------------------------------------------") );

    last_input  = fp.isw1[i];
    last_output = fp.osw1[i];
    }

f.writeline( TEXT("End of Shot Data") );
f.writeline( TEXT(" ") );
f.close();
}

/***********************************************************************
*                               SAVE_SHOT                              *
***********************************************************************/
void save_shot( SHOT_BUFFER_ENTRY * b )
{
static BOOLEAN have_enough_disk_space = TRUE;
int       n;
DB_TABLE  t;
BITSETYPE save_flags;
TCHAR     root[MAX_PATH];
TCHAR   * buf;
TCHAR   * cp;
STRING_CLASS   s;
SYSTEMTIME     st;

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

nextshot( b );
save_flags = skip_new_shot_notify( b->pn.machine, b->f.shot_time);

if ( SaveAllAlarmShots && ( b->has_alarm || b->has_warning) )
    {
    save_flags |= SAVE_PARM_DATA;
    save_flags |= SAVE_PROFILE_DATA;
    }

write_als_log( b->pn.shot, b->f );

get_profile_ii_name( s, b->pn.computer, b->pn.machine, b->pn.part, b->pn.shot );

/*
-----------------
Write the profile
----------------- */
b->f.put( s );

FileTimeToSystemTime( &b->f.shot_time, &st );

/*
---------------------
Update the graph list
--------------------- */

if ( get_part_results_file_name(s, b->pn.computer, b->pn.machine, b->pn.part, GRAPHLST_DB) )
    {
    t.open( s.text(), GRAPHLST_RECLEN, PWL );
    t.put_long(  GRAPHLST_SHOT_NUMBER_OFFSET, asctoint32(b->pn.shot), SHOT_LEN );
    t.put_date(  GRAPHLST_DATE_OFFSET, st );
    t.put_time(  GRAPHLST_TIME_OFFSET, st );
    t.put_short( GRAPHLST_SAVE_FLAGS_OFFSET, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
    t.rec_append();

    /*
    ----------------------------------------------------------------------
    Check back 51 shots and see if I should delete the record and profile.
    ---------------------------------------------------------------------- */
    n = t.nof_recs();
    if ( n > HistoryShotCount )
        {
        n -= HistoryShotCount;
        /*
        --------------------------------------------------
        Subtract one more because the last record is n - 1
        -------------------------------------------------- */
        n--;

        t.goto_record_number(n);
        t.get_rec();

        /*
        ------------------------------------
        See if I need to delete this profile
        ------------------------------------ */
        save_flags = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET );
        if ( !(save_flags & SAVE_PROFILE_DATA) )
            {
            n = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
            t.rec_delete();
            if ( get_profile_ii_name( s, b->pn.computer, b->pn.machine, b->pn.part, n) )
                DeleteFile( s.text() );
            }

        n = max_profiles_saved( b->pn.machine );
        if ( n > 0 && n < (int32) t.nof_recs() )
            {
            t.goto_record_number(0);
            t.get_rec();
            n = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
            t.rec_delete();
            if ( get_profile_ii_name( s, b->pn.computer, b->pn.machine, b->pn.part, n) )
                DeleteFile( s.text() );
            }
        }

    t.close();
    }

write_ftii_parameters( b, st, save_flags );

buf = maketext( MACHINE_NAME_LEN+1+PART_NAME_LEN);
if ( buf )
    {
    cp = copy_w_char( buf, b->pn.machine, CommaChar );
    lstrcpy( cp, b->pn.part );

    cp = maketext( b->pn.shot );
    if ( cp )
        MainWindow.post( WM_NEWSHOT, (WPARAM) cp, (LPARAM) buf );
    else
        delete[] buf;
    }

buf = make_parameter_event_string( b );
if ( buf )
    MainWindow.post( WM_POKEMON, (WPARAM) PARAM_DATA_INDEX, (LPARAM) buf );

if ( EosComPort )
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
        if ( lstrcmp(b->pn.machine, machine_to_save) == 0 )
            {
            new_time = b->f.shot_time;
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
    if ( get_shotsave_semaphore(b->pn.computer, b->pn.machine) )
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
                if ( lstrcmp(b->pn.machine, machine_to_save) == 0 )
                    b->save_failed = true;
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
            save_this_machine( b->pn.machine );
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
void shotsave_thread( void * notused )
{
DWORD status;
DWORD ms_to_wait;
int   i;

startup();

ShotSaveIsRunning = true;

while ( TRUE )
    {
    ms_to_wait = INFINITE;
    for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
        {
        if ( BufPtr[i] )
            {
            ms_to_wait = SHOTSAVE_WAIT_TIMEOUT;
            BufPtr[i]->save_failed = false;
            }
        }

    status = WaitForSingleObject( ShotSaveEvent, ms_to_wait );

    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance, UNABLE_TO_CONTINUE_STRING, SHOTSAVE_WAIT_FAIL_STRING, MB_OK | MB_SYSTEMMODAL );

    save_all_shots();

    if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
        {
        ShotSaveIsRunning = false;
        break;
        }

    }

shutdown();
}

/***********************************************************************
*                        START_SHOTSAVE_THREAD                         *
***********************************************************************/
void start_shotsave_thread()
{
unsigned long h;

h = _beginthread( shotsave_thread, 0, NULL );
if ( h == -1 )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, SHOTSAVE_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}
