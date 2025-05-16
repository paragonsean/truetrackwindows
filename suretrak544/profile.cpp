/***********************************************************************
*      Copyright (c) 1994 by Visi-Trak Corp.             profile.cpp   *
************************************************************************/
#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\ftclass.h"
#include "..\include\plotclas.h"
#include "..\include\structs.h"
#include "..\include\stparam.h"
#include "..\include\stsetup.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "extern.h"

static const TCHAR EmptyString[] = TEXT( "" );

extern BOARD_CONTROL_CLASS SureTrakBoard;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                           SET_LIMIT_SWITCH                           *
***********************************************************************/
static int set_limit_switch( int i, float new_value )
{

new_value *= SureTrakParameters.counts_per_unit();
SureTrakBoard.send_data( new_value );

return SureTrakBoard.send_command_busy(CSET_LS1 + i);
}

/********************************j**************************************
*                        SEND_PROFILE_TO_BOARD                         *
***********************************************************************/
int send_profile_to_board( void )
{
int status;
int x;

float counts, accel, vel, goose, pos, licmd, tempf;

status  = FT_OK;

/* If a program has not yet been downloaded, ERROR, exit */
if ( !SureTrakHasProgram )
    {
    status = 10;
    goto ex;
    }

set_text( MainScreenWindow.handle(), WAITING_FOR_TEXTBOX, resource_string(DOWNLOADING_PROFILE_STRING) );

counts = SureTrakParameters.counts_per_unit();

x = 0;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    /* First clear the goose and accel settings */
    SureTrakBoard.send_data(0);
    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_ACCEL1); SureTrakBoard.send_command_busy(SET_GOOSE1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_ACCEL2); SureTrakBoard.send_command_busy(SET_GOOSE2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_ACCEL3); SureTrakBoard.send_command_busy(SET_GOOSE3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_ACCEL4); SureTrakBoard.send_command_busy(SET_GOOSE4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_ACCEL5); SureTrakBoard.send_command_busy(SET_GOOSE5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_ACCEL6); SureTrakBoard.send_command_busy(SET_GOOSE6); break;
        }

    vel = SureTrakSetup.vel_value();

    /* Decode ACCEL */
    if ( !SureTrakSetup.accel_is_percent() ) /* No goosing required */
        {
        /*
        -----------------------
        Establish accel command
        ----------------------- */
        accel = SureTrakSetup.accel_value();
        if ( accel == 0.0 && SureTrakSetup.vel_is_percent() )
            ;
        else  /* Accel command in inches */
            {
            accel *= counts/4.0;

            /*
            --------------------------------------------------------
            If this is a closed loop step and not step 1 (x==0) then
            force .5" of accel if anything less than .5" is entered
            -------------------------------------------------------- */
            if ( accel<10.0 && x!=0 )
                accel = 10.0;

            SureTrakBoard.send_data( (BOARD_DATA) accel );

            switch (x)
                {
                case 0:
                     SureTrakBoard.send_command_busy(SET_ACCEL1); break;
                case 1:
                     SureTrakBoard.send_command_busy(SET_ACCEL2); break;
                case 2:
                     SureTrakBoard.send_command_busy(SET_ACCEL3); break;
                case 3:
                     SureTrakBoard.send_command_busy(SET_ACCEL4); break;
                case 4:
                     SureTrakBoard.send_command_busy(SET_ACCEL5); break;
                case 5:
                     SureTrakBoard.send_command_busy(SET_ACCEL6); break;
                }
            }
        }
    else  /* Goose command */
        {
        goose = SureTrakSetup.accel_value();
        if (goose > 100.0) goose = 100.0;
        if (goose < 0.0)   goose = 0.0;
        goose = ((goose / 100.0) * 8191.0) + 8192.0;

        SureTrakBoard.send_data((BOARD_DATA)goose);
        switch (x)
            {
            case 0:
                SureTrakBoard.send_command_busy(SET_GOOSE1); break;
            case 1:
                SureTrakBoard.send_command_busy(SET_GOOSE2); break;
            case 2:
                SureTrakBoard.send_command_busy(SET_GOOSE3); break;
            case 3:
                SureTrakBoard.send_command_busy(SET_GOOSE4); break;
            case 4:
                SureTrakBoard.send_command_busy(SET_GOOSE5); break;
            case 5:
                SureTrakBoard.send_command_busy(SET_GOOSE6); break;
            }
        }

    /* Now set velocity command */
    if ( !SureTrakSetup.vel_is_percent() )  /* Actual closed loop velocity command */
        {
        vel = vel * 100.0;
        SureTrakBoard.send_data((BOARD_DATA)vel);
        }
    else   /* Open loop valve setting */
        {
        /* Now set goose command for open loop, -2 */
        SureTrakBoard.send_data(0xfffe);
        switch (x)
            {
            case 0:
                SureTrakBoard.send_command_busy(SET_GOOSE1); break;
            case 1:
                SureTrakBoard.send_command_busy(SET_GOOSE2); break;
            case 2:
                SureTrakBoard.send_command_busy(SET_GOOSE3); break;
            case 3:
                SureTrakBoard.send_command_busy(SET_GOOSE4); break;
            case 4:
                SureTrakBoard.send_command_busy(SET_GOOSE5); break;
            case 5:
                SureTrakBoard.send_command_busy(SET_GOOSE6); break;
            }

        if (vel > 100.0) vel = 100.0;
        if (vel < 0.0) vel = 0.0;
        vel = ((vel / 100.0) * 8191.0) + 8192.0;
        SureTrakBoard.send_data((BOARD_DATA)vel);
        }

    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_VEL1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_VEL2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_VEL3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_VEL4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_VEL5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_VEL6); break;
        }


    /* Now determine position command */
    pos = SureTrakSetup.end_pos_value();
    if ( pos == 0.0 )
        SureTrakBoard.send_data(0);
    else
        {
        pos *= counts;
        SureTrakBoard.send_data((BOARD_DATA)pos);
        }

    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_POS1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_POS2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_POS3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_POS4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_POS5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_POS6); break;
        }

    /* Now look for vacuum checking */

    if ( SureTrakSetup.have_vacuum_wait() )
        SureTrakBoard.send_data(0x0200);
    else
        SureTrakBoard.send_data(0);

    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_VAC1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_VAC2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_VAC3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_VAC4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_VAC5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_VAC6); break;
        }

    /* Now look for a low impact command */
    licmd = SureTrakSetup.low_impact_value();
    if ( licmd == 0.0 )
        SureTrakBoard.send_data(0);
    else
        {
        if (licmd > 100.0) licmd = 100.0;
        if (licmd < 0.0) licmd = 0.0;
        licmd = ((licmd / 100) * 8191) + 8192;
        SureTrakBoard.send_data((BOARD_DATA)licmd);
        }

    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_LICMD1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_LICMD2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_LICMD3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_LICMD4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_LICMD5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_LICMD6); break;
        }

    /* Now look for low impact input checking */
    if ( SureTrakSetup.have_low_impact_from_input() )
        SureTrakBoard.send_data(0x0800);
    else
        SureTrakBoard.send_data(0);

    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_LI_IO1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_LI_IO2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_LI_IO3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_LI_IO4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_LI_IO5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_LI_IO6); break;
        }

    x++;
    }

/* Blocks are disabled by setting the goose setting to 0xffff */
x = SureTrakSetup.nof_steps();
while ( x < MAX_ST_STEPS )
    {
    SureTrakBoard.send_data( 0xffff );
    switch (x)
        {
        case 0:
            SureTrakBoard.send_command_busy(SET_GOOSE1); break;
        case 1:
            SureTrakBoard.send_command_busy(SET_GOOSE2); break;
        case 2:
            SureTrakBoard.send_command_busy(SET_GOOSE3); break;
        case 3:
            SureTrakBoard.send_command_busy(SET_GOOSE4); break;
        case 4:
            SureTrakBoard.send_command_busy(SET_GOOSE5); break;
        case 5:
            SureTrakBoard.send_command_busy(SET_GOOSE6); break;
        }
    x++;
    }

/*-----------------------------------------------------
Now get the retract valve setting
tempf = SureTrakParameters.retract_valve_percent();
if ( tempf == 0.0 )
    SureTrakBoard.send_data(0);
else
    {
    if (tempf > 100) tempf = 100;
    if (tempf < -100) tempf = -100;
    tempf = ((tempf / 100) * 8191) + 8192;
    SureTrakBoard.send_data((BOARD_DATA)tempf);
    }
----------------------------------------------------- */

SureTrakBoard.send_data(0);
SureTrakBoard.send_command_busy(SET_RETR);

/*-----------------------------------------------------
Now get the follow throuh valve setting
tempf = SureTrakParameters.ft_valve_percent();
if ( tempf == 0.0 )
    SureTrakBoard.send_data(0);
else
    {
    if (tempf > 100) tempf = 100;
    if (tempf < 0) tempf = 0;
    tempf = ((tempf / 100) * 8191) + 8192;
    SureTrakBoard.send_data((BOARD_DATA)tempf);
    }
----------------------------------------------------- */

SureTrakBoard.send_data(0);
SureTrakBoard.send_command_busy(SET_FT);

/*-----------------------------------------------------
Now get the jog shot valve setting
tempf = SureTrakParameters.jog_valve_percent();
if ( tempf == 0.0 )
    SureTrakBoard.send_data(0);
else
    {
    if (tempf > 100) tempf = 100;
    if (tempf < 0) tempf = 0;
    tempf = ((tempf / 100) * 8191) + 8192;
    SureTrakBoard.send_data((BOARD_DATA)tempf);
    }
----------------------------------------------------- */

SureTrakBoard.send_data(0);
SureTrakBoard.send_command_busy(SET_JS);

for ( x=0; x<4; x++ )
    {
    if ( is_empty(SureTrakSetup.limit_switch_pos_string(x)) )
        {
        tempf = 7920.0 / counts; /* Default to 99 inches if entered blank. */
        }
    else
        tempf = SureTrakSetup.limit_switch_pos_value(x);

    set_limit_switch( x, tempf );
    }

ex:
set_text( MainScreenWindow.handle(), WAITING_FOR_TEXTBOX, EmptyString );
return status;
}

