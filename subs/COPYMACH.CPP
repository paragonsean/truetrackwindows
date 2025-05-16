#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\machine.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\verrors.h"

static TCHAR DefaultPartName[] = DEFAULT_PART_NAME;

/***********************************************************************
*                           GET_DEFAULT_PART                           *
***********************************************************************/
static BOOLEAN get_default_part( TCHAR * partname, MACHINE_CLASS & sorc )
{
TCHAR    s[MAX_PATH+1];
DB_TABLE t;
BOOLEAN have_part;

have_part = FALSE;

copystring( s, parts_dbname( sorc.computer, sorc.name) );
t.open( s, PARTS_RECLEN, PFL );
t.put_alpha( PARTS_PART_NAME_OFFSET, DefaultPartName, PART_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    lstrcpy( partname, DEFAULT_PART_NAME );
    have_part = TRUE;
    }
else
    {
    t.reset_search_mode();
    if ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( partname, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
        have_part = TRUE;
        }
    }

t.close();

return have_part;
}

/***********************************************************************
*                             COPYMACHINE                              *
***********************************************************************/
BOOLEAN copymachine( TCHAR * computer, TCHAR * machine, MACHINE_CLASS & sorc )
{
TCHAR destfile[MAX_PATH+1];
TCHAR sorcfile[MAX_PATH+1];

COMPUTER_CLASS c;
MACHINE_CLASS  m;
DB_TABLE       t;
TCHAR          partname[PART_NAME_LEN+1];

if ( !c.find(computer) )
    {
    /* error_message */
    return FALSE;
    }

if ( !get_default_part(partname, sorc) )
    return FALSE;

/*
----------------------
Copy the machine class
---------------------- */
m = sorc;

/*
------------------
Rename and save it
------------------ */
lstrcpy( m.computer,     computer );
lstrcpy( m.name,         machine  );
lstrcpy( m.current_part, partname );

/*
-------------------
Turn off monitoring
------------------- */
m.monitor_flags &= ~MA_MONITORING_ON;

m.save();

/*
---------------------------------------------------------------------
If there is a control program in the sorc machine directory, copy it.
--------------------------------------------------------------------- */
copystring( sorcfile, madir_name(sorc.computer, sorc.name, FTII_CONTROL_PROGRAM_FILE) );
copystring( destfile, madir_name(m.computer,    m.name,    FTII_CONTROL_PROGRAM_FILE) );
if ( file_exists(sorcfile) )
    CopyFile( sorcfile, destfile, FALSE );

copystring( sorcfile, ftii_editor_settings_name(sorc.computer, sorc.name) );
copystring( destfile, ftii_editor_settings_name(m.computer,    m.name) );
if ( file_exists(sorcfile) )
    CopyFile( sorcfile, destfile, FALSE );

copystring( sorcfile, ftii_global_settings_name(sorc.computer, sorc.name) );
copystring( destfile, ftii_global_settings_name(m.computer,    m.name) );
if ( file_exists(sorcfile) )
    CopyFile( sorcfile, destfile, FALSE );

copystring( sorcfile, ftii_ctrl_prog_steps_name(sorc.computer, sorc.name) );
copystring( destfile, ftii_ctrl_prog_steps_name(m.computer,    m.name) );
if ( file_exists(sorcfile) )
    CopyFile( sorcfile, destfile, FALSE );

copystring( sorcfile, ftii_machine_settings_name(sorc.computer, sorc.name) );
copystring( sorcfile, ftii_machine_settings_name(m.computer,    m.name) );
if ( file_exists(sorcfile) )
    CopyFile( sorcfile, destfile, FALSE );

/*
-------------
Copy one part
------------- */
copypart( computer, machine, partname, sorc.computer, sorc.name, partname, COPY_PART_ALL );
return TRUE;
}
