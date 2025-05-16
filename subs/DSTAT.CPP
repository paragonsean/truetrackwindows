#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\names.h"
#include "..\include\subs.h"

static TCHAR BackslashChar = TEXT( '\\' );

/***********************************************************************
*                           MAKE_DSTAT_DBNAME                          *
***********************************************************************/
static BOOLEAN make_dstat_dbname( TCHAR * dest, TCHAR * computer_name, TCHAR * machine_name )
{
COMPUTER_CLASS c;
TCHAR * cp;

if ( c.find(computer_name) )
    {
    cp = copystring( dest, c.directory() );
    cp = copystring( cp, machine_name );
    *cp++ = BackslashChar;
    copystring( cp, DSTAT_DB );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               DOWNSTATE                              *
***********************************************************************/
static int downstate( TCHAR * mycat, TCHAR * mysubcat )
{
if ( compare(mycat, NO_DOWNCAT_NUMBER, DOWNCAT_NUMBER_LEN) == 0 )
    return NO_DOWN_STATE;

if ( compare(mycat, DOWNCAT_SYSTEM_CAT, DOWNCAT_NUMBER_LEN) != 0 )
    return HUMAN_DOWN_STATE;

if ( compare(mysubcat, DOWNCAT_UP_SUBCAT, DOWNCAT_NUMBER_LEN) == 0 )
    return MACH_UP_STATE;

if ( compare(mysubcat, DOWNCAT_PROG_EXIT_SUBCAT, DOWNCAT_NUMBER_LEN) == 0 )
    return PROG_DOWN_STATE;

if ( compare(mysubcat, DOWNCAT_UNSP_DOWN_SUBCAT, DOWNCAT_NUMBER_LEN) == 0 )
    return AUTO_DOWN_STATE;

if ( compare(mysubcat, DOWNCAT_PROG_START_SUBCAT, DOWNCAT_NUMBER_LEN) == 0 )
    return PROG_UP_STATE;

if ( compare(mysubcat, DOWNCAT_NO_CONNECT_SUBCAT, DOWNCAT_NUMBER_LEN) == 0 )
    return NO_CONNECT_STATE;

return NO_DOWN_STATE;
}

/***********************************************************************
*                       DSTAT_CLASS::DOWN_STATE                        *
***********************************************************************/
int DSTAT_CLASS::down_state( void )
{

return downstate( cat, subcat );

}

/***********************************************************************
*                               SAME_CODE                              *
***********************************************************************/
BOOLEAN DSTAT_CLASS::same_code( TCHAR * newcat, TCHAR * newsubcat )
{

if ( compare( cat, newcat, DOWNCAT_NUMBER_LEN) != 0 )
    return FALSE;

if ( compare(subcat, newsubcat, DOWNCAT_NUMBER_LEN) != 0 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                         DSTAT_CLASS::IS_DOWN                         *
***********************************************************************/
BOOLEAN DSTAT_CLASS::is_down( void )
{
if ( down_state() == MACH_UP_STATE )
    return FALSE;

if ( down_state() == PROG_UP_STATE )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              UPDATE_SEC                              *
***********************************************************************/
long DSTAT_CLASS::update_sec( void )
{
return sec_since1990( st );
}

/***********************************************************************
*                                 GET                                  *
***********************************************************************/
BOOLEAN DSTAT_CLASS::get( TCHAR * computer, TCHAR * machine )
{
DB_TABLE t;
BOOLEAN  status;
TCHAR    code[DSTAT_CODE_LEN+1];
TCHAR    fname[MAX_PATH+1];

status = FALSE;

if ( make_dstat_dbname(fname, computer, machine) )
    {
    if ( file_exists(fname) )
        {
        if ( t.open(fname, DSTAT_RECLEN, PFL) )
            {
            if ( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( code,   DSTAT_CODE_OFFSET,  DSTAT_CODE_LEN );
                t.get_date(  st,     DSTAT_DATE_OFFSET );
                t.get_time(  st,     DSTAT_TIME_OFFSET );

                lstrcpyn( cat,    code,                      DOWNCAT_NUMBER_LEN+1 );
                lstrcpyn( subcat, (code+DOWNCAT_NUMBER_LEN), DOWNCAT_NUMBER_LEN+1 );

                status = TRUE;
                }
            t.close();
            }
        }
    }

return status;
}

/***********************************************************************
*                                 PUT                                  *
***********************************************************************/
BOOLEAN DSTAT_CLASS::put( TCHAR * computer, TCHAR * machine )
{
DB_TABLE t;
BOOLEAN  have_rec;
BOOLEAN  status;
TCHAR    fname[MAX_PATH+1];
TCHAR    code[DSTAT_CODE_LEN+1];
int      old_state;
int      new_state;
SYSTEMTIME oldst;

status = FALSE;

if ( !make_dstat_dbname(fname, computer, machine) )
    return status;

if ( !file_exists(fname) )
    t.create(fname);

if ( t.open(fname, DSTAT_RECLEN, PWL) )
    {
    have_rec = t.get_next_record(WITH_LOCK);
    status   = TRUE;
    if ( have_rec )
        {
        t.get_alpha( code, DSTAT_CODE_OFFSET,  DSTAT_CODE_LEN );
        t.get_date(  oldst,     DSTAT_DATE_OFFSET );
        t.get_time(  oldst,     DSTAT_TIME_OFFSET );

        if ( same_code(code) )
            {
            status = FALSE;
            }
        else
            {
            old_state = downstate( code, (code+DOWNCAT_NUMBER_LEN) );
            new_state = down_state();

            if ( old_state == HUMAN_DOWN_STATE && new_state != MACH_UP_STATE && new_state != HUMAN_DOWN_STATE )
                {
                status = FALSE;
                }
            }
        }

    if ( status )
        {
        lstrcpy( code, cat );
        lstrcat( code, subcat );
        t.put_alpha( DSTAT_CODE_OFFSET, code,  DSTAT_CODE_LEN );
        t.put_date(  DSTAT_DATE_OFFSET, st );
        t.put_time(  DSTAT_TIME_OFFSET, st );
        if ( have_rec )
            t.rec_update();
        else
            t.rec_append();
        }

    t.close();
    }

return status;
}
