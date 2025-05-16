#include <windows.h>
#include <htmlhelp.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\catclass.h"
#include "..\include\computer.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\timeclas.h"
#include "..\include\shiftcl.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
LISTBOX_CLASS Lb;

static       TCHAR EmptyString[]     = TEXT( "" );
static const TCHAR MyClassName[]     = TEXT( "Test" );
static const TCHAR DeltaMinutesTag[] = TEXT( "{dt}" );
static const TCHAR ShiftTag[]        = TEXT( "{s}" );
static const TCHAR MinuteTag[]       = TEXT( "{mi}" );

static const TCHAR IniFileName[]        = TEXT( "vtextrac.ini" );

static const TCHAR AlarmsumSection[]    = TEXT( "Alarmsum" );
static const TCHAR ConfigSection[]      = TEXT( "Config" );
static const TCHAR DownTimeSection[]    = TEXT( "Downtime" );

static const TCHAR DeleteOldCsvFileKey[]= TEXT( "DeleteOldCsvFile" );
static const TCHAR RecordFormatKey[]    = TEXT( "RecordFormat" );
static const TCHAR FileNameKey[]        = TEXT( "FileName" );
static const TCHAR ParameterFormatKey[] = TEXT( "ParameterFormat" );
static const TCHAR StartMinimizedKey[]  = TEXT( "StartMinimized" );
static const TCHAR TimeOutMsKey[]       = TEXT( "TimeOutMs" );
static const TCHAR IgnoreComputerKey[]  = TEXT( "ComputerToIgnore" );

static const TCHAR NullChar          = TEXT( '\0' );
static const TCHAR SpaceChar         = TEXT( ' ' );
static const TCHAR YChar             = TEXT( 'Y' );
static const TCHAR LeftCurlyBracket  = TEXT( '{' );
static const TCHAR RightCurlyBracket = TEXT( '}' );

static bool  DeleteOldCsvFile = false;
static bool  StartMinimized = false;
static UINT  TimeOutMs      = 0;       // Ms to wait before auto close
static TCHAR ComputerToIgnore[]   = NO_COMPUTER;

static bool    UseShiftDate     = false;
static bool    HaveDeltaMinutes = false;

static TCHAR * CommandLine      = 0;
static TCHAR * Pattern          = 0;  // Record Format for Downtimes and Alarmsum
static TCHAR * ParameterFormat  = 0;  // Parameter Format for Alarmsum

TIME_CLASS BofShift;
TIME_CLASS EofShift;

TIME_CLASS ShiftDate;
int        ShiftNumber;

MACHINE_NAME_LIST_CLASS Machine;
FILE_CLASS              DestFile;
WORKER_LIST_CLASS       Worker;
DOWNCAT_NAME_CLASS      Cat;
DOWNSCAT_NAME_CLASS     SubCat;

class DOWNTIME_ENTRY
    {
    public:

    TIME_CLASS tm;
    int        seconds;
    TCHAR      category[DOWNCAT_NUMBER_LEN+1];
    TCHAR      subcategory[DOWNCAT_NUMBER_LEN+1];
    TCHAR      part[PART_NAME_LEN+1];
    TCHAR      worker[OPERATOR_NUMBER_LEN+1];

    DOWNTIME_ENTRY() { *category = NullChar; *subcategory = NullChar; *part=NullChar; *worker=NullChar; seconds=0; }
    ~DOWNTIME_ENTRY(){}
    bool read( DB_TABLE & t );
    bool read_names( DB_TABLE & t );
    };

/***********************************************************************
*                             DOWNTIME_ENTRY                           *
*                              read_names                              *
***********************************************************************/
bool DOWNTIME_ENTRY::read_names( DB_TABLE & t )
{
t.get_alpha( category,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
t.get_alpha( subcategory, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
t.get_alpha( part,        DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
t.get_alpha( worker,      DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );

strip( worker);

return true;
}

/***********************************************************************
*                             DOWNTIME_ENTRY                           *
*                                 read                                 *
***********************************************************************/
bool DOWNTIME_ENTRY::read( DB_TABLE & t )
{
get_time_class_from_db( tm, t, DOWNTIME_DATE_OFFSET );
return read_names( t );
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         CLEANUP_GLOBAL_PATTERNS                      *
***********************************************************************/
static void cleanup_global_patterns()
{
if ( Pattern )
    {
    delete[] Pattern;
    Pattern = 0;
    }

if ( ParameterFormat )
    {
    delete[] ParameterFormat;
    ParameterFormat = 0;
    }
}

/***********************************************************************
*                           WRITE_DOWNTIME_RECORD                      *
*                                                                      *
* {c},{m},{dd}-{mm}-{yyyy} {hh}:{mi},{cat},{subcat},{ds},{op},{p}      *
*                                                                      *
***********************************************************************/
static void write_downtime_record( TCHAR * computer, TCHAR * machine, DOWNTIME_ENTRY & d )
{
int     dt;
int     i;
TCHAR * pcopy;
TCHAR   buf[2048];
TCHAR * dp;
TCHAR * p;
TIME_CLASS t;

if ( UseShiftDate )
    t = ShiftDate;
else
    t = d.tm;

Worker.find( d.worker );

pcopy = maketext( Pattern );
p = pcopy;
dp = buf;
while ( replace_char_with_null(p, LeftCurlyBracket) )
    {
    dp = copystring( dp, p );
    p = nextstring( p );
    if ( replace_char_with_null(p, RightCurlyBracket) )
        {
        if (strings_are_equal(p, TEXT("dd"), 2) )
            {
            dp = copystring( dp, t.dd() );
            }
        else if (strings_are_equal(p, TEXT("mmm"), 3) )
            {
            dp = copystring( dp, t.mmm() );
            }
        else if (strings_are_equal(p, TEXT("mm"), 2) )
            {
            dp = copystring( dp, t.mm() );
            }
        else if (strings_are_equal(p, TEXT("yyyy"), 4) )
            {
            dp = copystring( dp, t.yyyy() );
            }
        else if (strings_are_equal(p, TEXT("yy"), 2) )
            {
            dp = copystring( dp, t.yy() );
            }
        else if (strings_are_equal(p, TEXT("hh"), 2) || strings_are_equal(p, TEXT("mi"), 2) || strings_are_equal(p, TEXT("ss"), 2) )
            {
            TCHAR * hhmmss( void );
            if (strings_are_equal(p, TEXT("hh"), 2) )
                i = 0;
            else if ( strings_are_equal(p, TEXT("mi"), 2) )
                i = 3;
            else if ( strings_are_equal(p, TEXT("ss"), 2) )
                i = 6;
            copychars( dp, t.hhmmss()+i, 2 );
            dp += 2;
            *dp = NullChar;
            }
        else if (strings_are_equal(p, TEXT("catn"), 4) )
            {
            dp = copystring( dp, Cat.name(d.category) );
            }
        else if (strings_are_equal(p, TEXT("cat"), 3) )
            {
            dp = copystring( dp, d.category  );
            }
        else if (strings_are_equal(p, TEXT("c"), 1) )
            {
            dp = copystring( dp, computer );
            }
        else if (strings_are_equal(p, TEXT("m"), 1) )
            {
            dp = copystring( dp, machine );
            }
        else if (strings_are_equal(p, TEXT("p"), 1) )
            {
            dp = copystring( dp, d.part );
            }
        else if (strings_are_equal(p, TEXT("subcatn"), 7) )
            {
            dp = copystring( dp, SubCat.name(d.category, d.subcategory) );
            }
        else if (strings_are_equal(p, TEXT("subcat"), 6) )
            {
            dp = copystring( dp, d.subcategory  );
            }
        else if (strings_are_equal(p, TEXT("op"), 2) )
            {
            dp = copystring( dp, d.worker  );
            }
        else if (strings_are_equal(p, TEXT("ofn"), 2) )
            {
            dp = copystring( dp, Worker.first_name() );
            }
        else if (strings_are_equal(p, TEXT("oln"), 2) )
            {
            dp = copystring( dp, Worker.last_name() );
            }
        else if (strings_are_equal(p, TEXT("dt"), 2) )
            {
            dt = d.seconds/60;
            dp = copystring( dp, int32toasc(dt) );
            }
        else if (strings_are_equal(p, TEXT("ds"), 2) )
            {
            dt = d.seconds;
            if ( HaveDeltaMinutes )
                dt %= 60;
            dp = copystring( dp, int32toasc(dt) );
            }
        else if (strings_are_equal(p, TEXT("s"), 1) )
            {
            dp = copystring( dp, int32toasc(ShiftNumber) );
            }
        p = nextstring( p );
        }
    }

copystring( dp, p );

/*
------------------------------
I am finished with p and pcopy
------------------------------ */
delete[] pcopy;
p = 0;
pcopy = 0;

DestFile.writeline( buf );
}

/***********************************************************************
*                            READ_DOWNTIMES                            *
***********************************************************************/
void read_downtimes( TCHAR * computer, TCHAR * machine )
{
int            n;
DB_TABLE       t;
SYSTEMTIME     st;
DOWNTIME_ENTRY d;
TIME_CLASS     nextime;
STRING_CLASS   s;
bool           need_rec;
bool           finished;
bool           monitoring;
bool           found_a_downtime;

found_a_downtime = false;

/*
-----------------------------------------------------------------------------
See if I am monitoring this machine. If so, and there are no records for this
shift, I still want to write a record based upon the last record.
----------------------------------------------------------------------------- */
monitoring = false;
t.open( machset_dbname(computer), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    if ( BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET)) & MA_MONITORING_ON )
        monitoring = true;
    }
t.close();

st = BofShift.system_time();

t.open( downtime_dbname(computer, machine), DOWNTIME_RECLEN, PFL );
t.put_date( DOWNTIME_DATE_OFFSET, st );
t.put_time( DOWNTIME_TIME_OFFSET, st );
if ( t.goto_first_equal_or_greater_record(2) )
    {
    n = t.current_record_number();
    t.get_current_record( NO_LOCK );
    d.read( t );
    need_rec = true;

    /*
    --------------------------------------
    This is the first record of the shift.
    -------------------------------------- */
    if ( d.tm > BofShift )
        {
        if ( n == 0 )
            {
            /*
            -----------------------------------------------------------
            This is the 0th record, meaning that there is no way for me
            to know what happened since the start of the shift;
            ----------------------------------------------------------- */
            copystring( d.category,    DOWNCAT_SYSTEM_CAT );
            copystring( d.subcategory, DOWNCAT_UNSP_DOWN_SUBCAT );
			nextime = d.tm;
            need_rec = false;
            }
        else
            {
            /*
            ------------------------------------------------------------------
            Get the previous record and set his time to the start of the shift
            ------------------------------------------------------------------ */
            n--;
            t.goto_record_number( n );
            t.get_current_record( NO_LOCK );
            d.read( t );
            need_rec = true;
            }
        d.tm = BofShift;
        }

    finished = false;
    while ( true )
        {
        /*
        ------------------------------------------------------------------------
        Get the time of the next record so I can calculate the number of seconds
        for this one. The only time I don't need to read is at the start.
        ------------------------------------------------------------------------ */
        if ( need_rec )
            {
            if ( t.get_next_record(NO_LOCK) )
                {
                get_time_class_from_db( nextime, t, DOWNTIME_DATE_OFFSET );
                }
            else
                {
                nextime = EofShift;
                }
            }

        need_rec = true;

        if ( nextime >= EofShift )
            {
            finished = true;
            nextime  = EofShift;
            }
        /*
        ----------------------------------------------------------------------
        The elapsed time for this downtime event is from the event time to the
        time of the next event or to the end of the shift.
        ---------------------------------------------------------------------- */
        d.seconds = nextime - d.tm;
        if ( d.seconds > 0 )
            {
            write_downtime_record( computer, machine, d );
            found_a_downtime = true;
            }

        if ( finished )
            break;

        d.read_names( t );
        d.tm = nextime;
        }
    }
else if ( monitoring )
    {
    n = t.nof_recs();
    if ( n == 0 )
        {
        copystring( d.part,        unknown_string() );
        copystring( d.worker,      unknown_string() );
        copystring( d.category,    DOWNCAT_SYSTEM_CAT );
        copystring( d.subcategory, DOWNCAT_UNSP_DOWN_SUBCAT );
        }
    else
        {
        n--;
        t.goto_record_number( n );
        t.get_current_record( NO_LOCK );
        d.read_names( t );
        }
    d.tm = BofShift;
    d.seconds = EofShift - BofShift;
    write_downtime_record( computer, machine, d );
    found_a_downtime = true;
    }

t.close();

if ( found_a_downtime )
    {
    s = TEXT( "     Computer " );
    s += computer;
    s += TEXT( " Machine " );
    s += machine;
    Lb.add( s.text() );
    }

}

/***********************************************************************
*                     MAKE_DOWNTIME_RECORD_FORMAT                      *
***********************************************************************/
static bool make_downtime_record_format( TCHAR * sorc )
{
if ( Pattern )
    delete[] Pattern;

Pattern = maketext( sorc );
if ( Pattern )
    {
    if ( findstring(ShiftTag, Pattern) && !findstring(MinuteTag, Pattern) )
        UseShiftDate = true;
    else
        UseShiftDate = false;

    if ( findstring(DeltaMinutesTag, Pattern) )
        HaveDeltaMinutes = true;
    else
        HaveDeltaMinutes = false;

    return true;
    }
return false;
}

/***********************************************************************
*                             OPEN_DESTFILE                            *
***********************************************************************/
static bool open_destfile( TCHAR * file_name )
{
TCHAR * s;
STRING_CLASS msg;

/*
-----------------------
Create dir if necessary
----------------------- */
s = maketext( file_name );
if ( dir_from_filename(s) )
    {
    if ( !directory_exists(s) )
        {
        msg = TEXT( "Creating directory [" );
        msg.cat_w_char( s, TEXT( ']' ) );
        Lb.add( msg.text() );
        create_directory(s);
        }
    }
delete[] s;

/*
--------------------------------------
This will create the file if necessary
-------------------------------------- */
if ( DeleteOldCsvFile )
    {
    if ( DestFile.open_for_write(file_name) )
        return true;
    }
else
    {
    if ( DestFile.open_for_append(file_name) )
        return true;
    }

return false;
}

/***********************************************************************
*                              READ_DOWNTIMES                          *
***********************************************************************/
static void read_downtimes()
{
const int BUFLEN = 512;
TCHAR           buf[BUFLEN+1];
PART_NAME_ENTRY pn;
STRING_CLASS    s;

/*
-------------------------------------------------------------------------------------
Check that there is at least one machine and set the names for the dynamic file name
though you would not normally use the machine name as part of the downtime file name.
------------------------------------------------------------------------------------- */
Machine.rewind();
if ( !Machine.next() )
    return;

pn.set( Machine.computer_name(), Machine.name(), unknown_string() );

/*
------------------------------------------------------------------
The name of the network downtime file is in the vtextrac.ini file.
If there is no entry then don't do anything.
------------------------------------------------------------------ */
s = exe_directory();
s += IniFileName;

GetPrivateProfileString( DownTimeSection, RecordFormatKey, unknown_string(), buf, BUFLEN, s.text() );
if ( unknown(buf) )
    return;

make_downtime_record_format( buf );

GetPrivateProfileString( DownTimeSection, FileNameKey, unknown_string(), buf, BUFLEN, s.text() );
if ( unknown(buf) )
    return;

if ( findchar( LeftCurlyBracket, buf)  )
    copystring( buf, dynamic_file_name(BofShift, pn, buf) );

s = TEXT( "Downtime file = " );
s += buf;
Lb.add( s.text() );

Lb.add( TEXT("Adding Downtime records...") );

if ( open_destfile(buf) )
    {
    Machine.rewind();
    while ( Machine.next() )
        read_downtimes( Machine.computer_name(), Machine.name() );
    DestFile.close();
    }
}

/***********************************************************************
*                             GET_PART_LIST                            *
***********************************************************************/
static bool get_part_list( TEXT_LIST_CLASS & dest, TCHAR * computer, TCHAR * machine )
{
DB_TABLE t;
TCHAR    name[PART_NAME_LEN+1];

if ( !t.open( parts_dbname(computer, machine), PARTS_RECLEN, PFL ) )
    return false;

while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
    dest.append( name );
    }
t.close();

return true;
}

/***********************************************************************
*                        ALARMSUM_PARAMETER_STRING                     *
***********************************************************************/
TCHAR * alarmsum_parameter_string( ALARM_COUNTS_ENTRY & ace )
{
static bool    firstime = true;
static int     buflen = 0;
static TCHAR * buf = 0;
TCHAR * pcopy;
TCHAR * dp;
TCHAR * p;

if ( !ParameterFormat )
    return EmptyString;

if ( firstime )
    {
    buflen = lstrlen(ParameterFormat) + 4 * MAX_INTEGER_LEN;
    if ( buf )
        delete[] buf;

    buf = maketext( buflen );
    if ( !buf )
        {
        buflen = 0;
        return EmptyString;
        }
    firstime = false;
    }

*buf = NullChar;

pcopy = maketext( ParameterFormat );
p     = pcopy;
dp    = buf;

while ( replace_char_with_null(p, LeftCurlyBracket) )
    {
    dp = copystring( dp, p );
    p = nextstring( p );
    if ( replace_char_with_null(p, RightCurlyBracket) )
        {
        if (strings_are_equal(p, TEXT("pn"), 2) )
            dp = copystring( dp, int32toasc(ace.parameter_number) );
        else if (strings_are_equal(p, TEXT("lw"), 2) )
            dp = copystring( dp, int32toasc(ace.counts[WARNING_MIN]) );
        else if (strings_are_equal(p, TEXT("hw"), 2) )
            dp = copystring( dp, int32toasc(ace.counts[WARNING_MAX]) );
        else if (strings_are_equal(p, TEXT("la"), 2) )
            dp = copystring( dp, int32toasc(ace.counts[ALARM_MIN]) );
        else if (strings_are_equal(p, TEXT("ha"), 2) )
            dp = copystring( dp, int32toasc(ace.counts[ALARM_MAX]) );

        p = nextstring( p );
        }
    }

copystring( dp, p );
delete[] pcopy;

return buf;
}

/***********************************************************************
*                          WRITE_ALARMSUM_RECORD                       *
***********************************************************************/
void write_alarmsum_record( PART_NAME_ENTRY & pn, ALARM_SUMMARY_CLASS & a )
{
int     i;
TCHAR * pcopy;
TCHAR   buf[2048];
TCHAR * dp;
TCHAR * p;
ALARM_COUNTS_ENTRY   zero_ace;
ALARM_COUNTS_ENTRY * ap;

for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
    zero_ace.counts[i] = 0;

pcopy = maketext( Pattern );
p = pcopy;
dp = buf;
while ( replace_char_with_null(p, LeftCurlyBracket) )
    {
    dp = copystring( dp, p );
    p = nextstring( p );
    if ( replace_char_with_null(p, RightCurlyBracket) )
        {
        if (strings_are_equal(p, TEXT("dd"), 2) )
            {
            dp = copystring( dp, ShiftDate.dd() );
            }
        else if (strings_are_equal(p, TEXT("mmm"), 3) )
            {
            dp = copystring( dp, ShiftDate.mmm() );
            }
        else if (strings_are_equal(p, TEXT("mm"), 2) )
            {
            dp = copystring( dp, ShiftDate.mm() );
            }
        else if (strings_are_equal(p, TEXT("yyyy"), 4) )
            {
            dp = copystring( dp, ShiftDate.yyyy() );
            }
        else if (strings_are_equal(p, TEXT("yy"), 2) )
            {
            dp = copystring( dp, ShiftDate.yy() );
            }
        else if (strings_are_equal(p, TEXT("ts"), 2) )
            {
            dp = copystring( dp, int32toasc(a.total_shot_count()) );
            }
        else if (strings_are_equal(p, TEXT("ta"), 2) )
            {
            dp = copystring( dp, int32toasc(a.total_alarm_count()) );
            }
        else if (strings_are_equal(p, TEXT("tw"), 2) )
            {
            dp = copystring( dp, int32toasc(a.total_warning_count()) );
            }
        else if (strings_are_equal(p, TEXT("c"), 1) )
            {
            dp = copystring( dp, pn.computer );
            }
        else if (strings_are_equal(p, TEXT("m"), 1) )
            {
            dp = copystring( dp, pn.machine );
            }
        else if (strings_are_equal(p, TEXT("p"), 1) )
            {
            dp = copystring( dp, pn.part );
            }
        else if (strings_are_equal(p, TEXT("s"), 1) )
            {
            dp = copystring( dp, int32toasc(ShiftNumber) );
            }
        else if ( is_numeric(*p) )
            {
            i = asctoint32( p );
            ap = a.find(i);
            if ( !ap )
                {
                zero_ace.parameter_number = i;
                ap = &zero_ace;
                }
            dp = copystring( dp, alarmsum_parameter_string(*ap) );
            }

        p = nextstring( p );
        }
    }

copystring( dp, p );

/*
------------------------------
I am finished with p and pcopy
------------------------------ */
delete[] pcopy;
p = 0;
pcopy = 0;

DestFile.writeline( buf );
}

/***********************************************************************
*                              READ_ALARMSUMS                          *
***********************************************************************/
static void read_alarmsums()
{
const int BUFLEN = 512;

TCHAR               buf[BUFLEN+1];
PART_NAME_ENTRY     pn;
STRING_CLASS        s;
ALARM_SUMMARY_CLASS a;
TEXT_LIST_CLASS     parts;
SYSTEMTIME          st;

s = exe_directory();
s += IniFileName;

/*
----------------
Parameter format
---------------- */
GetPrivateProfileString( AlarmsumSection, ParameterFormatKey, unknown_string(), buf, BUFLEN, s.text() );
if ( unknown(buf) )
    return;

if ( ParameterFormat )
    delete[] ParameterFormat;
ParameterFormat = maketext( buf );

/*
-------------
Record format
------------- */
GetPrivateProfileString( AlarmsumSection, RecordFormatKey, unknown_string(), buf, BUFLEN, s.text() );
if ( unknown(buf) )
    return;

if ( Pattern )
    delete[] Pattern;
Pattern = maketext( buf );

/*
---------
File name
--------- */
GetPrivateProfileString( AlarmsumSection, FileNameKey, unknown_string(), buf, BUFLEN, s.text() );
if ( unknown(buf) )
    return;

/*
-------------------------------------------------------------------------------------
The name of this file should not contain the computer, machine, or part, as
it is intended that all these go into a single file. I need to have something for the
dynamic file routine though.
------------------------------------------------------------------------------------- */
Machine.rewind();
Machine.next();
pn.set( Machine.computer_name(), Machine.name(), unknown_string() );

if ( findchar( LeftCurlyBracket, buf)  )
    copystring( buf, dynamic_file_name(BofShift, pn, buf) );

s = TEXT( "Alarmsum File = " );
s += buf;
Lb.add( s.text() );
Lb.add( TEXT("Adding Alarmsum records...") );

if ( open_destfile(buf) )
    {
    Machine.rewind();
    while ( Machine.next() )
        {
        pn.set( Machine.computer_name(), Machine.name(), unknown_string() );
        parts.empty();
        get_part_list( parts, Machine.computer_name(), Machine.name() );
        parts.rewind();
        while ( parts.next() )
            {
            pn.set( parts.text() );
            a.empty();
            a.set_part( pn );
            st = ShiftDate.system_time();
            a.get_counts( st, ShiftNumber );
            if ( a.total_shot_count() > 0 )
                {
                s = TEXT( "     Computer " );
                s += pn.computer;
                s += TEXT( " Machine " );
                s += pn.machine;
                s += TEXT( " Part " );
                s += pn.part;
                Lb.add( s.text() );
                write_alarmsum_record( pn, a );
                }
            }
        }

    DestFile.close();
    }

}

/***********************************************************************
*                              GET_ALARMS                              *
***********************************************************************/
static void get_alarms()
{
COMPUTER_CLASS          c;
STRING_CLASS            s;
TEXT_LIST_CLASS         t;
TEXT_LIST_CLASS         parts;

/*
----------------------------
Make a list of the computers
---------------------------- */
c.rewind();
while ( c.next() )
    {
    if ( !strings_are_equal( c.name(), ComputerToIgnore) )
        {
        if ( c.is_present() )
            t.append( c.name() );
        }
    }

/*
---------------------------
Make a list of the machines
--------------------------- */
t.rewind();
while ( t.next() )
    {
    Machine.add_computer( t.text() );
    }

if ( Machine.count() > 0 )
    {
    read_downtimes();
    read_alarmsums();
    }
}

/***********************************************************************
*                              GET_SHIFTS                              *
***********************************************************************/
static void get_shifts()
{
DB_TABLE  t;
TCHAR day[DOW_NAME_LEN+1];
int   today;
int   shift_dow;
int   dow;
int   shift_number;
int   record_number;
SYSTEMTIME st;
TIME_CLASS now;
STRING_CLASS s;

bool    was_today = false;

if ( lstrlen(CommandLine) > 10 )
    {
    s = TEXT( "Command line = [" );
    s += CommandLine;
    s += TEXT( "]" );
    now = CommandLine;
    st = now.system_time();
    }
else
    {
    s = TEXT( "Start Time = " );
    GetLocalTime( &st );
    st.wMilliseconds = 0;
    now.set( st );
    s += now.text();
    }

Lb.add( s.text() );

today = st.wDayOfWeek;

record_number = 0;
if ( t.open(shift_dbname(), SHIFT_RECLEN, PFL) )
    {
    while ( true )
        {
        if ( !t.get_next_record(NO_LOCK) )
            {
            record_number--;
            break;
            }

        t.get_alpha( day, SHIFT_DAY_OFFSET, DOW_NAME_LEN );
        shift_number = t.get_long( SHIFT_NUMBER_OFFSET );

        t.get_alpha( day, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
        dow = (int) dow_from_string( day );

        t.get_time( st, SHIFT_START_TIME_OFFSET );
        BofShift.set( st );

        if ( dow == today )
            {
            if ( now < BofShift )
                {
                /*
                ----------------------
                I am at the next shift
                ---------------------- */
                record_number--;
                break;
                }
            was_today = true;
            }
        else if ( was_today )
            {
            record_number--;
            break;
            }

        record_number++;
        }

    if ( record_number < 0 )
        record_number = t.nof_recs();

    t.goto_record_number( record_number );
    t.get_rec();
    t.get_alpha( day, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
    dow = (int) dow_from_string( day );
    t.get_time( st, SHIFT_START_TIME_OFFSET );

    /*
    ------------------------------------------------------------------------------
    This is the start of the current shift which is the end of the previous shift.
    The previous shift is the one I want.
    ----------------------------------------------------------------------------- */
    EofShift.set( st );
    if ( dow != today )
        {
        EofShift -= SECONDS_PER_DAY;
        st = EofShift.system_time();
        }
    record_number--;
    if ( record_number < 0 )
        record_number = t.nof_recs();
    t.goto_record_number( record_number );
    t.get_rec();

    ShiftNumber = t.get_long( SHIFT_NUMBER_OFFSET );
    t.get_alpha( day, SHIFT_DAY_OFFSET, DOW_NAME_LEN );
    shift_dow = (int) dow_from_string( day );

    t.get_alpha( day, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
    t.get_time( st, SHIFT_START_TIME_OFFSET );
    BofShift.set( st );
    /*
    -------------------------------------------------------------------------------------------
    See if I have to back up still another day (only happens if there is only one shift per day
    ------------------------------------------------------------------------------------------- */
    if ( dow != (int) dow_from_string(day) )
        BofShift -= SECONDS_PER_DAY;

    /*
    -------------------------
    Figure out the shift date
    ------------------------- */
    ShiftDate = BofShift;
    dow = (int) dow_from_string( day );
    if ( dow != shift_dow )
        {
        if ( shift_dow < dow || (dow==0 && shift_dow>1) )
            ShiftDate -= SECONDS_PER_DAY;
        else

            ShiftDate += SECONDS_PER_DAY;
        }

    s = TEXT( "Previous shift = " );
    s += ShiftDate.mmddyyyy();
    s += TEXT( " Shift " );
    s += int32toasc( ShiftNumber );
    Lb.add( s.text() );

    s = TEXT( "Range = " );
    s += BofShift.text();
    s += TEXT( " to " );
    s += EofShift.text();
    Lb.add( s.text() );

    t.close();
    }

}

/***********************************************************************
*                            UPDATE_THREAD                             *
***********************************************************************/
DWORD update_thread( int * notused )
{
get_shifts();
get_alarms();

MainWindow.post( WM_DBCLOSE );
return 0;
}

/***********************************************************************
*                         START_UPDATE_THREAD                          *
***********************************************************************/
static void start_update_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) update_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, THREAD_STRING );
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
append_filename_to_path(  s, TEXT("VtExtrac.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static UINT timer_id;
static bool immortal = false;
static int  id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        RECT r;
        GetClientRect( hWnd, &r );
        Lb.init( CreateWindow( TEXT("LISTBOX"), 0, WS_VSCROLL | WS_CHILD | WS_VISIBLE | LBS_NOSEL, 0,  0, r.right-r.left, r.bottom-r.top, hWnd, (HMENU) MAIN_LISTBOX, MainInstance, NULL ) );
        MainWindow.post( WM_DBINIT );
        break;

    case WM_HELP:
        gethelp( hWnd );
        return 0;

    case WM_DBINIT:
        start_update_thread();
        return 0;

    case WM_DBCLOSE:
        if ( TimeOutMs > 0 )
            timer_id = SetTimer (hWnd, 0, TimeOutMs, NULL) ;
        return 0;

    case WM_TIMER:
        KillTimer(hWnd, timer_id );
        if ( !immortal )
            MainWindow.close();
        return 0;

    case WM_NCLBUTTONDOWN:
    case WM_MOUSEACTIVATE:
        immortal = true;
        break;

    case WM_ACTIVATE:
        /*
        ---------------------------------------------------------------------
        The high word of wParam is zero if I am not minimized so if I started
        minimized and am not now somebody must have activated me.
        --------------------------------------------------------------------- */
        if ( StartMinimized && !HIWORD(wParam) )
            immortal = true;
        break;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
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
STRING_CLASS   title;
WNDCLASS       wc;
COMPUTER_CLASS c;
TCHAR        * cp;

names_startup();
c.startup();
shifts_startup();
Worker.load();
Cat.read();
SubCat.read();

cp = get_ini_string( IniFileName, ConfigSection, TimeOutMsKey );
if ( !unknown(cp) )
    TimeOutMs = asctoint32( cp );

cp = get_ini_string( IniFileName, ConfigSection, StartMinimizedKey );
if ( !unknown(cp) )
    {
    upper_case( *cp );
    if ( *cp == YChar )
        StartMinimized = true;
    }

cp = get_ini_string( IniFileName, ConfigSection, IgnoreComputerKey );
if ( !unknown(cp) && lstrlen(cp) <= COMPUTER_NAME_LEN )
    copystring( ComputerToIgnore, cp );

cp = get_ini_string( IniFileName, ConfigSection, DeleteOldCsvFileKey );
if ( !unknown(cp) )
    {
    upper_case( *cp );
    if ( *cp == YChar )
        DeleteOldCsvFile = true;
    }

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;                          /* TEXT( "MAINMENU" ); */
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
    507, 501,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( StartMinimized )
    cmd_show = SW_SHOWMINIMIZED;

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
if ( CommandLine )
    {
    delete[] CommandLine;
    CommandLine = 0;
    }

cleanup_global_patterns();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

CommandLine = maketext( (TCHAR *)cmd_line );

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

shutdown();
return( msg.wParam );
}
