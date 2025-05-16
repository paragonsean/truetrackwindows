#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\mrunlist.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\verrors.h"

#include "..\include\ftii.h"
#include "..\include\ftii_file.h"

/***********************************************************************
*                         SAVE_PLOOKUP_RECORD                          *
***********************************************************************/
static BOOLEAN save_plookup_record( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
DB_TABLE   t;
TCHAR      this_part[PART_NAME_LEN+1];
TCHAR      s[MAX_PATH+1];
BOOLEAN    need_to_insert;
BOOLEAN    status;
SYSTEMTIME mytime;

need_to_insert = FALSE;

copystring( s, plookup_dbname(computer, machine) );
t.open( s, PLOOKUP_RECLEN, PWL );
while( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( this_part, PLOOKUP_PART_NAME_OFFSET, PART_NAME_LEN );
    if ( compare(part, this_part, PART_NAME_LEN) < 0 )
        {
        need_to_insert = TRUE;
        break;
        }

    }

t.put_alpha( PLOOKUP_PART_NAME_OFFSET, part, PART_NAME_LEN );
t.put_long(  PLOOKUP_LAST_SHOT_OFFSET, 0, SHOT_LEN );

GetSystemTime( &mytime );
t.put_date(  PLOOKUP_DATE_OFFSET, mytime );
t.put_time(  PLOOKUP_TIME_OFFSET, mytime );

if ( need_to_insert )
    status = t.rec_insert();
else
    status = t.rec_append();

t.close();

return status;
}

/***********************************************************************
*                         COPY_PARAMETER_NAMES                         *
***********************************************************************/
static BOOLEAN copy_parameter_names( PART_CLASS & dest, PART_CLASS & sorc )
{
PARAMETER_CLASS d;
PARAMETER_CLASS s;
int32 i;
int32 n;

if ( d.find(dest.computer, dest.machine, dest.name) )
    {
    if ( s.find(sorc.computer, sorc.machine, sorc.name) )
        {
        n = s.count();
        if ( d.count() < n )
            n = d.count();
        for ( i=0; i<n; i++ )
            {
            if ( s.parameter[i].vartype == d.parameter[i].vartype )
                lstrcpy( d.parameter[i].name, s.parameter[i].name );
            }
        d.save();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                               COPYPART                               *
***********************************************************************/
BOOLEAN copypart( TCHAR * dest_computer, TCHAR * dest_machine, TCHAR * dest_part, TCHAR * sorc_computer, TCHAR * sorc_machine, TCHAR * sorc_part, BITSETYPE flags )
{
static int TSL_INDEX           =  0;
static int MSL_INDEX           =  1;
static int ESV_INDEX           =  2;

static int LS1_INDEX           =  3;           /* Limit Switch Positions */
static int LS2_INDEX           =  4;
static int LS3_INDEX           =  5;
static int LS4_INDEX           =  6;
static int LS5_INDEX           =  7;
static int LS6_INDEX           =  8;

static int LS1_MASK_INDEX      =  9;            /* Limit Switch Wires */
static int LS2_MASK_INDEX      = 10;
static int LS3_MASK_INDEX      = 11;
static int LS4_MASK_INDEX      = 12;
static int LS5_MASK_INDEX      = 13;
static int LS6_MASK_INDEX      = 14;

static int VEL_LOOP_GAIN_INDEX = 15;
static int BREAK_VEL_1_INDEX   = 16;
static int BREAK_VEL_2_INDEX   = 17;
static int BREAK_VEL_3_INDEX   = 18;
static int BREAK_GAIN_1_INDEX  = 19;
static int BREAK_GAIN_2_INDEX  = 20;
static int BREAK_GAIN_3_INDEX  = 21;
static int TIME_INTERVAL_MS_INDEX = 22;
static int BISCUIT_DELAY_MS_INDEX = 23;
static int NOF_TIME_SAMPLES_INDEX = 24;

static int vars[] = { TSL_VN, MSL_VN, ESV_VN,
 LS1_VN, LS2_VN, LS3_VN, LS4_VN, LS5_VN, LS6_VN,
 LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN,
 VEL_LOOP_GAIN_VN,
 BREAK_VEL_1_VN,  BREAK_VEL_2_VN,  BREAK_VEL_3_VN,
 BREAK_GAIN_1_VN, BREAK_GAIN_2_VN, BREAK_GAIN_3_VN,
 TIME_INTERVAL_MS_VN, NOF_TIME_SAMPLES_VN,
 BISCUIT_DELAY_MS_VN
 };

const int nof_vars = sizeof(vars)/sizeof(int);

TCHAR                   destfile[MAX_PATH+1];
TCHAR                   sorcfile[MAX_PATH+1];
PART_CLASS              p;
PART_CLASS              d;
DB_TABLE                t;
short                   i;
BOOLEAN                 part_exists_already;
BOOLEAN                 dest_ftii_exists;
BOOLEAN                 sorc_ftii_exists;
BOOLEAN                 need_ftii_updates;
MULTIPART_RUNLIST_CLASS mrl;
FTII_VAR_ARRAY          dva;
FTII_VAR_ARRAY          sva;
NAME_CLASS              s;

need_ftii_updates = FALSE;

if ( !p.find(sorc_computer, sorc_machine, sorc_part) )
    return FALSE;

sorc_ftii_exists = FALSE;
lstrcpy(  sorcfile, ftii_part_settings_name(sorc_computer, sorc_machine, sorc_part) );
if ( file_exists(sorcfile) )
    {
    sorc_ftii_exists = TRUE;
    sva.get( sorcfile );
    }

part_exists_already = part_exists(dest_computer, dest_machine, dest_part);

if ( part_exists_already )
    {
    if ( !d.find(dest_computer, dest_machine, dest_part) )
        return FALSE;

    lstrcpy(  sorcfile, ftii_part_settings_name(dest_computer, dest_machine, dest_part) );
    if ( file_exists(sorcfile) )
        {
        dest_ftii_exists = TRUE;
        dva.get( sorcfile );
        if ( sorc_ftii_exists )
            need_ftii_updates = TRUE;
        }
    }
else
    {
    /*
    ------------------
    Copy the part data
    ------------------ */
    d = p;
    lstrcpy( d.computer, dest_computer );
    lstrcpy( d.machine,  dest_machine  );
    lstrcpy( d.name,     dest_part     );

    /*
    -----------------------
    Copy the ftii part data
    ----------------------- */
    if ( sorc_ftii_exists )
        dva = sva;

    /*
    -------------------------
    Create the part directory
    ------------------------- */
    CreateDirectory( madir_name(dest_computer, dest_machine, dest_part), NULL );

    /*
    --------------------------
    Create a results directory
    -------------------------- */
    CreateDirectory( padir_name(dest_computer, dest_machine, dest_part, RESULTS_DIRECTORY_NAME), NULL );
    t.create( graphlst_dbname(dest_computer, dest_machine, dest_part) );
    t.create( shotparm_dbname(dest_computer, dest_machine, dest_part) );
    t.create( alarmsum_dbname(dest_computer, dest_machine, dest_part) );
    }

if ( flags & COPY_PART_PARAMETERS )
    {
    lstrcpy(  sorcfile, ftanalog_csvname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        {
        CopyFile( sorcfile, ftanalog_csvname(dest_computer, dest_machine, dest_part), FALSE );
        }
    else
        {
        lstrcpy(  sorcfile, ftanalog_dbname(sorc_computer, sorc_machine, sorc_part) );
        CopyFile( sorcfile, ftanalog_dbname(dest_computer, dest_machine, dest_part), FALSE );
        }

    lstrcpy(  sorcfile, parmlist_dbname(sorc_computer, sorc_machine, sorc_part) );
    CopyFile( sorcfile, parmlist_dbname(dest_computer, dest_machine, dest_part), FALSE );
    }

if ( flags & COPY_PART_CHANNELS )
    {
    lstrcpy(  sorcfile, ftchan_dbname(sorc_computer, sorc_machine, sorc_part) );
    CopyFile( sorcfile, ftchan_dbname(dest_computer, dest_machine, dest_part), FALSE );
    if ( part_exists_already )
        {
        d.head_pressure_channel = p.head_pressure_channel;
        d.rod_pressure_channel  = p.rod_pressure_channel;
        for ( i=0; i<MAX_FT_CHANNELS; i++ )
            d.analog_sensor[i] = p.analog_sensor[i];
        }
    }

if ( flags & COPY_PART_MARKS )
    {
    lstrcpy(  sorcfile, marklist_dbname(sorc_computer, sorc_machine, sorc_part) );
    CopyFile( sorcfile, marklist_dbname(dest_computer, dest_machine, dest_part), FALSE );
    }

if ( flags & COPY_SUBPART_NAMES )
    {
    lstrcpy(  sorcfile, subparts_dbname(sorc_computer, sorc_machine, sorc_part) );
    CopyFile( sorcfile, subparts_dbname(dest_computer, dest_machine, dest_part), FALSE );
    }

if ( flags & COPY_PART_REFERENCE_TRACE )
    {
    lstrcpy(  sorcfile, master_name(sorc_computer, sorc_machine, sorc_part, CURRENT_MASTER_TRACE_NAME) );
    if ( file_exists(sorcfile) )
        {
        CopyFile( sorcfile, master_name(dest_computer, dest_machine, dest_part, CURRENT_MASTER_TRACE_NAME), FALSE );
        }
    else
        {
        lstrcpy(  sorcfile, master_ii_name(sorc_computer, sorc_machine, sorc_part, CURRENT_MASTER_TRACE_NAME) );
        if ( file_exists(sorcfile) )
            {
            /*
            ------------------------------------------------------------------------------
            I am going to copy a ft2 master trace. Make sure there is no ft1 master trace.
            ------------------------------------------------------------------------------ */
            lstrcpy( destfile, master_name(dest_computer, dest_machine, dest_part, CURRENT_MASTER_TRACE_NAME) );
            if ( file_exists(destfile) )
                DeleteFile( destfile );

            lstrcpy( destfile, master_ii_name(dest_computer, dest_machine, dest_part, CURRENT_MASTER_TRACE_NAME) );
            CopyFile( sorcfile, destfile, FALSE );
            }
        }
    }

if ( flags & COPY_PART_SURETRAK_SETUP )
    {
    lstrcpy(  sorcfile, ststeps_dbname(sorc_computer, sorc_machine, sorc_part) );
    CopyFile( sorcfile, ststeps_dbname(dest_computer, dest_machine, dest_part), FALSE );
    /*
    -----------------------------------------------------
    If the new file (stlimits.dat) exists, copy only that
    ----------------------------------------------------- */
    lstrcpy(  sorcfile, stlimits_datname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        {
        CopyFile( sorcfile, stlimits_datname(dest_computer, dest_machine, dest_part), FALSE );
        }
    else
        {
        lstrcpy(  sorcfile, stlimits_dbname(sorc_computer, sorc_machine, sorc_part) );
        CopyFile( sorcfile, stlimits_dbname(dest_computer, dest_machine, dest_part), FALSE );
        }

    lstrcpy( sorcfile, ftii_limit_switch_wires_name(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, ftii_limit_switch_wires_name(dest_computer, dest_machine, dest_part), FALSE );

    /*
    ----------------------------------------------------------
    Copy the list of general purpose variables if there is one
    ---------------------------------------------------------- */
    lstrcpy(  sorcfile, gpvar_dbname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, gpvar_dbname(dest_computer, dest_machine, dest_part), FALSE );

    /*
    ---------------------------------------
    Copy the part based suretrak parameters
    --------------------------------------- */
    lstrcpy( sorcfile, stpart_datname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, stpart_datname(dest_computer, dest_machine, dest_part), FALSE );

    lstrcpy( sorcfile, presure_control_datname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, presure_control_datname(dest_computer, dest_machine, dest_part), FALSE );

    if ( need_ftii_updates )
        {
        dva = sva[LS1_INDEX];
        dva = sva[LS2_INDEX];
        dva = sva[LS3_INDEX];
        dva = sva[LS4_INDEX];
        dva = sva[LS5_INDEX];
        dva = sva[LS6_INDEX];
        dva = sva[VEL_LOOP_GAIN_INDEX];
        dva = sva[BREAK_VEL_1_INDEX];
        dva = sva[BREAK_VEL_2_INDEX];
        dva = sva[BREAK_VEL_3_INDEX];
        dva = sva[BREAK_GAIN_1_INDEX];
        dva = sva[BREAK_GAIN_2_INDEX];
        dva = sva[BREAK_GAIN_3_INDEX];

        if ( sva.find(ST_ACCEL_1_VN) )
            {
            i = sva.current_index();
            while ( i<sva.count() )
                {
                dva = sva[i];
                i++;
                }
            }
        }
    }

if ( flags & COPY_PART_PRES_CONTROL )
    {
    lstrcpy( sorcfile, presure_control_datname(sorc_computer, sorc_machine, sorc_part) );
    lstrcpy( destfile, presure_control_datname(dest_computer, dest_machine, dest_part) );
    CopyFile( sorcfile, destfile, FALSE );
    }

if ( flags & COPY_PART_WARMUP_PART )
    {
    /*
    ------------------------------------
    Copy the warmup part if there is one
    ------------------------------------ */
    lstrcpy( sorcfile, warmup_dbname(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, warmup_dbname(dest_computer, dest_machine, dest_part), FALSE );

    /*
    ------------------------------------------
    Copy the multipart runlist if there is one
    ------------------------------------------ */
    if ( mrl.get(sorc_computer, sorc_machine, sorc_part) )
        mrl.put(dest_computer, dest_machine, dest_part);
    }

if ( flags & COPY_PART_DISPLAY_SETUP )
    {
    lstrcpy( sorcfile, part_display_ini_name( sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, part_display_ini_name(dest_computer, dest_machine, dest_part), FALSE );

    lstrcpy( sorcfile, tc_parameter_list_filename( sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, tc_parameter_list_filename(dest_computer, dest_machine, dest_part), FALSE );
    }

if ( flags & COPY_PART_CHAXIS_LABEL )
    {
    lstrcpy( sorcfile, chaxis_label_datname( sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, chaxis_label_datname(dest_computer, dest_machine, dest_part), FALSE );
    }
if ( flags & COPY_PART_PARAM_SORT )
    {
    s.get_parameter_sort_order_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( s.file_exists() )
        {
        lstrcpy( sorcfile, s.text() );
        s.get_parameter_sort_order_file_name( dest_computer, dest_machine, dest_part );
        CopyFile( sorcfile, s.text(), FALSE );
        }
    }
/*
----------------------------------------
Copy the calculator setups for this part
---------------------------------------- */
if ( !part_exists_already || (flags & COPY_PART_BASIC) )
    {
    lstrcpy(  sorcfile, calc_file_name(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, calc_file_name(dest_computer, dest_machine, dest_part), FALSE );
    }

/*
-------------------------
Copy the setup sheet file
------------------------- */
if ( !part_exists_already )
    {
    lstrcpy(  sorcfile, setup_sheet_name(sorc_computer, sorc_machine, sorc_part) );
    if ( file_exists(sorcfile) )
        CopyFile( sorcfile, setup_sheet_name(dest_computer, dest_machine, dest_part), FALSE );
    }

if ( part_exists_already )
    {
    if ( flags & COPY_PART_BASIC )
        {
        d.velocity_units      = p.velocity_units;
        d.distance_units      = p.distance_units;
        d.total_stroke_length = p.total_stroke_length;
        d.min_stroke_length   = p.min_stroke_length;
        d.eos_velocity        = p.eos_velocity;

        if ( need_ftii_updates )
            {
            dva = sva[TSL_INDEX];
            dva = sva[MSL_INDEX];
            dva = sva[ESV_INDEX];
            }
        }

    if ( flags & COPY_PART_ADVANCED )
        {
        d.biscuit_time_delay     = p.biscuit_time_delay;
        d.sleeve_fill_distance   = p.sleeve_fill_distance;
        d.runner_fill_distance   = p.runner_fill_distance;
        d.csfs_min_velocity      = p.csfs_min_velocity;
        d.csfs_rise_velocity     = p.csfs_rise_velocity;
        d.csfs_min_position      = p.csfs_min_position;
        d.plunger_diameter       = p.plunger_diameter;
        d.time_for_intens_pres   = p.time_for_intens_pres;
        d.pres_for_response_time = p.pres_for_response_time;
        }

    if ( flags & COPY_PART_USER_POSITIONS )
        {
        for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
            d.limit_switch_position[i] = p.limit_switch_position[i];

        for ( i=0; i<USER_VEL_COUNT; i++ )
            d.user_velocity_position[i] = p.user_velocity_position[i];

        for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
            {
            d.user_avg_velocity_start[i] = p.user_avg_velocity_start[i];
            d.user_avg_velocity_end[i]   = p.user_avg_velocity_end[i];
            }
        }

    }

if ( !d.save() )
    {
    if ( !part_exists_already )
        {
        kill_directory( madir_name(dest_computer, dest_machine, dest_part) );
        kill_directory( padir_name(dest_computer, dest_machine, dest_part, RESULTS_DIRECTORY_NAME) );
        }
    return FALSE;
    }

/*
-------------------------------
Save the fastrak2 part settings
------------------------------- */
lstrcpy( sorcfile, ftii_part_settings_name(dest_computer, dest_machine, dest_part) );
if ( need_ftii_updates )
    dva.put_into( sorcfile );
else if ( sorc_ftii_exists )
    sva.put( sorcfile );

if ( flags & COPY_PART_PARAM_NAMES )
    copy_parameter_names( d, p );

/*
---------------------
Save the plookup data
--------------------- */
if ( !part_exists_already )
    save_plookup_record( dest_computer, dest_machine, dest_part );

return TRUE;
}
