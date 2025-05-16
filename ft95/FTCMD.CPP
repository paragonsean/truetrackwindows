#include <windows.h>
#include "..\include\ft.h"

#define MAX_WAIT_LOOPS  5000

int FtClearCmdWait = 400;

volatile int WaitCount;

/***********************************************************************
*                        FT_SET_CLEAR_CMD_WAIT                         *
***********************************************************************/
void ft_set_clear_cmd_wait( int new_wait_count )
{
if ( new_wait_count > 0 )
    FtClearCmdWait = new_wait_count;
}

/***********************************************************************
*                              SHORT_WAIT                              *
***********************************************************************/
void short_wait( void )
{
WaitCount = 0;
while ( WaitCount < FtClearCmdWait )
    WaitCount++;
}

/***********************************************************************
*                      FT_WAIT_FOR_BUSY_BIT_CLEAR                      *
***********************************************************************/
BOOLEAN ft_wait_for_busy_bit_clear( void )
{

int count;

count = 0;
while ( ft_read_status() & (unsigned short) VIS_BUSY )
    {
    count++;
    if ( count > MAX_WAIT_LOOPS )
        return FALSE;

    short_wait();
    }

short_wait();
return TRUE;
}

/***********************************************************************
*                     FT_WAIT_FOR_BUSY_BIT_SET                         *
***********************************************************************/
BOOLEAN ft_wait_for_busy_bit_set( void )
{

int count;

count = 0;
while ( !(ft_read_status() & (unsigned short) VIS_BUSY) )
    {
    count++;
    if ( count > MAX_WAIT_LOOPS )
        return FALSE;
    short_wait();
    }

short_wait();
return TRUE;
}

/***********************************************************************
*                           FT_SEND_COMMAND                            *
***********************************************************************/
int ft_send_command( BOARD_DATA command )
{

if ( !ft_wait_for_busy_bit_clear() )
    return FT_BUSY;

if ( ft_send_cmd_wo_check(command) )
    return FT_OK;

return FT_ERROR;
}

/***********************************************************************
*                          FT_SET_SCAN_MODE                            *
***********************************************************************/
int ft_set_scan_mode( void )
{
return ft_send_command(SET_SCAN_MODE);
}

/***********************************************************************
*                        FT_SET_DATA_COLL_MODE                         *
***********************************************************************/
int ft_set_data_coll_mode( void )
{

static int MAX_TRIES = 5;
int i;
int status;

if ( ft_read_status() & VIS_DATA_COLL_MODE )
    return FT_OK;

for ( i=0; i<MAX_TRIES; i++ )
    {
    status = ft_send_command( DATA_COLL_CMD );
    if ( status == FT_OK && !(ft_read_status() & VIS_POWER_UP) )
        return FT_OK;
    }

return FT_LOCKED_UP;
}

/***********************************************************************
*                         FT_SET_COMMAND_MODE                          *
***********************************************************************/
int ft_set_command_mode( void )
{

BOARD_DATA mask;
BOARD_DATA status;

mask = VIS_DATA_COLL_MODE | SCAN_MODE;

status = ft_read_status();
if ( !(status & mask) )
    return FT_OK;

if ( !ft_send_cmd_wo_check(NMI) )
    return FT_ERROR;

if ( !ft_wait_for_busy_bit_clear() )
    return FT_BUSY;

return FT_OK;
}

/***********************************************************************
*                         FT_SEND_CMD_BUSY                             *
*                                                                      *
*   Function send_cmd_busy() first sets the busy bit and then waits    *
*   until it is cleared before proceding.  This is not suitable for    *
*   some commands such as set_data_collection_mode() as busy will be   *
*   set for a long time.  If the busy bit is set when this function    *
*   is first entered, the status is set to 2 and the function exits.   *
*   This function will also sense a board hang up.  If after a         *
*   certain period of time, the board has not cleared the busy bit,    *
*   status is set to 3 and the function exits. If this function was    *
*   unable to set the busy bit, the status is set to 4.                *
*                                                                      *
***********************************************************************/
int ft_send_cmd_busy( BOARD_DATA command )
{

int count;

if ( !ft_wait_for_busy_bit_clear() )
    return FT_BUSY;

/*
-----------------------
Try to set the busy bit
----------------------- */
for ( count=0; count<3; count++ )
    {
    ft_send_cmd_wo_check( SET_BUSY );
    if ( ft_wait_for_busy_bit_set() )
        {
        ft_send_cmd_wo_check( command );
        if ( ft_wait_for_busy_bit_clear() )
            return FT_OK;

        return FT_LOCKED_UP;
        }
    }

return FT_ERROR;
}

/***********************************************************************
*                                                                      *
*                         FT_READ_PARAMETER                            *
*                                                                      *
*  Function read parameter is used when a value must be returned       *
*  from the data collection card, such as total stroke length, etc.    *
*                                                                      *
***********************************************************************/
int ft_read_parameter( BOARD_DATA parameter, BOARD_DATA * ptr_result )
{

int status;

status = ft_send_cmd_busy( parameter );

if ( status != FT_OK )
   *ptr_result = 0;
else
   *ptr_result = ft_read_data();

return status;
}

/***********************************************************************
*                         FT_SET_PARAMETER                             *
***********************************************************************/
int ft_set_parameter( BOARD_DATA data, BOARD_DATA command )
{
ft_send_data( data );
return ft_send_cmd_busy( command );
}

