#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\machine.h"
#include "..\include\mrunlist.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\warmupcl.h"

static const TCHAR NullChar      = TEXT( '\0' );
static const TCHAR NChar         = TEXT( 'N' );
static const TCHAR LowerNChar    = TEXT( 'n' );
static       TCHAR ZeroString[]  = TEXT( "0" );
static       TCHAR EmptyString[] = TEXT( "" );

static       TCHAR MultipartSetupSection[] = TEXT( "Multipart" );
static       TCHAR SureTrakIniFile[]       = TEXT( "suretrak.ini" );
static       TCHAR EnabledKey[]            = TEXT( "Enabled" );
static       TCHAR MultipartBasePartKey[]  = TEXT( "Multipart" );
static       TCHAR MultipartBaseWireKey[]  = TEXT( "Wire" );

/***********************************************************************
*                            WARMUP_CLASS                              *
***********************************************************************/
WARMUP_CLASS::WARMUP_CLASS()
{

wire[0]     = NullChar;
partname[0] = NullChar;
is_enabled  = FALSE;
run_if_on   = FALSE;

}

/***********************************************************************
*                            WARMUP_CLASS                              *
***********************************************************************/
WARMUP_CLASS::WARMUP_CLASS( const WARMUP_CLASS & sorc )
{
copystring( wire,     sorc.wire     );
copystring( partname, sorc.partname );
is_enabled  = sorc.is_enabled;
run_if_on   = sorc.run_if_on;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                                GET                                   *
***********************************************************************/
BOOLEAN WARMUP_CLASS::get( TCHAR * cn, TCHAR * mn, TCHAR * pn )

{
BOOLEAN    have_new_data;
DB_TABLE   t;
TCHAR    * cp;

wire[0]     = NullChar;
partname[0] = NullChar;
is_enabled  = FALSE;
run_if_on   = FALSE;

have_new_data = FALSE;

cp = warmup_dbname( cn, mn, pn );
if ( file_exists(cp) )
    {
    if ( t.open( cp, WARMUP_RECLEN, PFL) )
        {
        if ( t.get_next_record(FALSE) )
            {
            is_enabled = t.get_boolean( WARMUP_ENABLED_OFFSET );
            run_if_on  = t.get_boolean( WARMUP_RUN_IF_ON_OFFSET );
            t.get_alpha( wire,     WARMUP_WIRE_OFFSET,      WARMUP_WIRE_LEN );
            t.get_alpha( partname, WARMUP_PART_NAME_OFFSET, PART_NAME_LEN   );
            have_new_data = TRUE;
            }
        t.close();
        }
    }

return have_new_data;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                                PUT                                   *
***********************************************************************/
BOOLEAN WARMUP_CLASS::put( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
DB_TABLE   t;
TCHAR    * cp;

cp = warmup_dbname( cn, mn, pn );
if ( !file_exists(cp) )
    t.create( cp );

if ( t.open( cp, WARMUP_RECLEN, PWL) )
    {
    t.empty();
    t.put_boolean( WARMUP_ENABLED_OFFSET,   is_enabled );
    t.put_boolean( WARMUP_RUN_IF_ON_OFFSET, run_if_on  );
    t.put_alpha(   WARMUP_WIRE_OFFSET,      wire,     WARMUP_WIRE_LEN );
    t.put_alpha(   WARMUP_PART_NAME_OFFSET, partname, PART_NAME_LEN   );
    t.rec_append();
    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                           create_runlist                             *
*                                                                      *
* Create a runlist table in the machine directory. The first entry     *
* will be for the current part. The second will be for the warmup      *
* part.                                                                *
*                                                                      *
***********************************************************************/
BOOLEAN WARMUP_CLASS::create_runlist( TCHAR * co, TCHAR * ma, TCHAR * pa )
{
DB_TABLE      t;
TCHAR       * cp;
STRING_CLASS  f;
BOOLEAN       need_table;
unsigned long x;
MACHINE_CLASS m;

f = runlist_dbname( co, ma );

/*
----------------------------------------------------------------------------------------
Delete the table unless the warmup part is enabled and the parent and warmup parts exist
---------------------------------------------------------------------------------------- */
need_table = is_enabled;

m.find( co, ma );

/*
------------------------------------
Make sure there is a valid opto wire
------------------------------------ */
if ( need_table )
    {
    x = asctoul( wire );
    if( !m.is_ftii )
        {
        if ( x < LOW_ST_INPUT_WIRE || x > HIGH_ST_INPUT_WIRE )
            need_table = FALSE;
        }
    }

if ( need_table )
    need_table = part_exists( co, ma, pa );

if ( need_table )
    need_table = part_exists( co, ma, partname );

if ( !need_table )
    {
    DeleteFile( f.text() );
    return TRUE;
    }

/*
---------------------------------------------------------------
Convert the wire number into the bit in the suretrak input word
--------------------------------------------------------------- */
if( m.is_ftii )
    x--;
else
    x -= LOW_ST_INPUT_WIRE;
x = 1<<x;

if ( !file_exists(f.text()) )
    t.create( f.text() );

if ( t.open( f.text(), RUNLIST_RECLEN, PWL) )
    {
    t.empty();

    /*
    ---------------------------------
    Write the entry for the main part
    --------------------------------- */
    if ( run_if_on )
        cp = ZeroString;
    else
        cp = ultohex(x);

    t.put_alpha( RUNLIST_BITS_OFFSET,      cp, MAX_HEX_LEN   );
    t.put_alpha( RUNLIST_PART_NAME_OFFSET, pa, PART_NAME_LEN );
    t.rec_append();

    /*
    -----------------------------------
    Write the entry for the warmup part
    ----------------------------------- */
    if ( run_if_on )
        cp = ultohex(x);
    else
        cp = ZeroString;

    t.put_alpha( RUNLIST_BITS_OFFSET,      cp,       MAX_HEX_LEN   );
    t.put_alpha( RUNLIST_PART_NAME_OFFSET, partname, PART_NAME_LEN );
    t.rec_append();

    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                       MULTIPART_RUNLIST_CLASS                        *
*                               get                                    *
*                  Returns true if it found a setup                    *
***********************************************************************/
BOOLEAN MULTIPART_RUNLIST_CLASS::get( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
int i;
INI_CLASS ini;
STRING_CLASS s;
BOOLEAN      b;

ini.set_file( config_ini_name(cn, mn, pn) );
ini.set_section( MultipartSetupSection );

b = ini.find( EnabledKey );

is_enabled = ini.get_boolean( EnabledKey );

for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
    {
    s = MultipartBasePartKey;
    s += int32toasc( i );
    if ( ini.find(s.text()) )
        partname[i] = ini.get_string();
    else
        partname[i] = EmptyString;
    }

return b;
}

/***********************************************************************
*                       MULTIPART_RUNLIST_CLASS                        *
*                               put                                    *
***********************************************************************/
BOOLEAN MULTIPART_RUNLIST_CLASS::put( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
int i;
INI_CLASS ini;
STRING_CLASS s;

ini.set_file( config_ini_name(cn, mn, pn) );
ini.set_section( MultipartSetupSection );

ini.put_boolean( EnabledKey, is_enabled );

for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
    {
    s = MultipartBasePartKey;
    s += int32toasc( i );
    ini.put_string( s.text(), partname[i].text() );
    }
return TRUE;
}

/***********************************************************************
*                       MULTIPART_RUNLIST_CLASS                        *
*                            create_runlist                            *
***********************************************************************/
BOOLEAN MULTIPART_RUNLIST_CLASS::create_runlist( TCHAR * cn, TCHAR * mn, TCHAR * pn )
{
struct WIRE_BITS_ENTRY {
    unsigned long sorc;
    unsigned long dest;
    };

int             i;
int             wire;
INI_CLASS       ini;
STRING_CLASS    tablename;
STRING_CLASS    s;
unsigned long   x;
BOOLEAN         need_table;
DB_TABLE        t;
WIRE_BITS_ENTRY wirebits[MAX_MULTIPART_RUNLIST_WIRES];

tablename = runlist_dbname( cn, mn );

need_table = is_enabled;

if ( need_table )
    need_table = part_exists( cn, mn, pn );

if ( need_table )
    {
    /*
    -------------------------
    Get the multipart runlist
    ------------------------- */
    get( cn, mn, pn );

    /*
    ------------------------------------
    Make sure there is at least one part
    ------------------------------------ */
    need_table = FALSE;
    for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
        {
        if ( !partname[i].isempty() )
            {
            need_table = TRUE;
            break;
            }
        }
    }

if ( !need_table )
    {
    DeleteFile( tablename.text() );
    return TRUE;
    }


x = 1;
for ( wire=0; wire<MAX_MULTIPART_RUNLIST_WIRES; wire++ )
    {
    wirebits[wire].sorc = x;
    wirebits[wire].dest = 0;
    x <<= 1;
    }

/*
-----------------------------------------------
Get the wire numbers from the suretrak.ini file
----------------------------------------------- */
s = ini_file_name( SureTrakIniFile );
ini.set_file( s.text() );
ini.set_section( MultipartSetupSection );
for ( i=0; i<MAX_MULTIPART_RUNLIST_WIRES; i++ )
    {
    s = MultipartBaseWireKey;
    s += int32toasc( i );

    if ( ini.find(s.text()) )
        {
        x = ini.get_int() - LOW_ST_INPUT_WIRE;
        wirebits[i].dest = 1<<x;
        }
    }

if ( !file_exists(tablename.text()) )
    t.create( tablename.text() );

if ( t.open( tablename.text(), RUNLIST_RECLEN, PWL) )
    {
    t.empty();

    /*
    ----------------------------------------
    The first entry must be the current part
    I can ignore any other entries for this
    part because it is the default.
    ---------------------------------------- */
    for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
        {
        if ( partname[i].isempty() || partname[i] == pn )
            {
            /*
            -------------------------------
            Convert the index to wire bits.
            ------------------------------- */
            x = 0;
            for ( wire=0; wire<MAX_MULTIPART_RUNLIST_WIRES; wire++ )
                {
                if ( wirebits[wire].sorc & (unsigned long) i )
                    x |= wirebits[wire].dest;
                }

            s = ultohex(x);

            t.put_alpha( RUNLIST_BITS_OFFSET,      s.text(), MAX_HEX_LEN   );
            t.put_alpha( RUNLIST_PART_NAME_OFFSET, pn,       PART_NAME_LEN );
            t.rec_append();
            break;
            }
        }

    for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
        {
        if ( !partname[i].isempty() && partname[i] != pn )
            {
            /*
            -------------------------------
            Convert the index to wire bits.
            ------------------------------- */
            x = 0;
            for ( wire=0; wire<MAX_MULTIPART_RUNLIST_WIRES; wire++ )
                {
                if ( wirebits[wire].sorc & (unsigned long) i )
                    x |= wirebits[wire].dest;
                }

            s = ultohex(x);

            t.put_alpha( RUNLIST_BITS_OFFSET,      s.text(),           MAX_HEX_LEN   );
            t.put_alpha( RUNLIST_PART_NAME_OFFSET, partname[i].text(), PART_NAME_LEN );
            t.rec_append();
            }
        }
    t.close();
    }
else
    {
    return FALSE;
    }

return TRUE;
}

