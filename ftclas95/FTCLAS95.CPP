#include <windows.h>
#include <conio.h>
#include "..\include\ftclass.h"

const int MAX_WAIT_LOOPS = 5000;

int BOARD_CONTROL_CLASS::FtClearCmdWait = 400;

volatile int WaitCount;

/***********************************************************************
*                              STARTUP                                 *
***********************************************************************/
BOOLEAN BOARD_CONTROL_CLASS::startup( void )
{
return TRUE;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
void BOARD_CONTROL_CLASS::shutdown( void )
{
}

/***********************************************************************
*                              SHORT_WAIT                              *
***********************************************************************/
void BOARD_CONTROL_CLASS::short_wait( void )
{

WaitCount = 0;
while ( WaitCount < FtClearCmdWait )
    WaitCount++;

}

/***********************************************************************
*                         WAIT_FOR_BUSY_BIT_CLEAR                      *
***********************************************************************/
BOOLEAN BOARD_CONTROL_CLASS::wait_for_busy_bit_clear( void )
{
int count;

count = 0;
while ( read_status() & VIS_BUSY )
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
BOOLEAN BOARD_CONTROL_CLASS::wait_for_busy_bit_set( void )
{

int count;

count = 0;
while ( !(read_status() & VIS_BUSY) )
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
*                              SETPORTS                                *
***********************************************************************/
void BOARD_CONTROL_CLASS::setports( unsigned short base_port_id )
{
data_port           = base_port_id;
command_status_port = base_port_id + 2;
interrupt_port      = base_port_id + 4;
clear_command_port  = base_port_id + 6;
}

/***********************************************************************
*                          BOARD_CONTROL_CLASS                         *
***********************************************************************/
BOARD_CONTROL_CLASS::BOARD_CONTROL_CLASS()
{
setports( LOW_FASTRAK_PORT_ADDRESS );
}

/***********************************************************************
*                         ~BOARD_CONTROL_CLASS                         *
***********************************************************************/
BOARD_CONTROL_CLASS::~BOARD_CONTROL_CLASS()
{
}

/***********************************************************************
*                             SEND_DATA                                *
***********************************************************************/
BOOLEAN BOARD_CONTROL_CLASS::send_data( BOARD_DATA data )
{
_outpw( data_port, data );
return TRUE;
}

/***********************************************************************
*                              CLEAR_COMMAND                           *
***********************************************************************/
void BOARD_CONTROL_CLASS::clear_command( void )
{
_outpw( clear_command_port, 0x0000 );
}

/***********************************************************************
*                         SEND_COMMAND_WO_CHECK                        *
***********************************************************************/
void BOARD_CONTROL_CLASS::send_command_wo_check( BOARD_DATA command )
{
_outpw( command_status_port, command );
_outpw( clear_command_port, 0x0000 );
short_wait();
}

/***********************************************************************
*                             READ_DATA                                *
***********************************************************************/
BOARD_DATA BOARD_CONTROL_CLASS::read_data( void )
{
return (BOARD_DATA) _inpw( data_port );
}

/***********************************************************************
*                             READ_STATUS                              *
***********************************************************************/
BOARD_DATA BOARD_CONTROL_CLASS::read_status( void )
{
return (BOARD_DATA) _inpw( command_status_port );
}

/***********************************************************************
*                            CLEAR_INTERRUPT                           *
***********************************************************************/
void BOARD_CONTROL_CLASS::clear_interrupt( void )
{
_inpw( interrupt_port );
}

/***********************************************************************
*                              SEND_COMMAND                            *
***********************************************************************/
int BOARD_CONTROL_CLASS::send_command( BOARD_DATA command )
{

if ( !wait_for_busy_bit_clear() )
    return FT_BUSY;

send_command_wo_check( command );

return FT_OK;
}

/***********************************************************************
*                         SEND_COMMAND_BUSY                            *
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
int BOARD_CONTROL_CLASS::send_command_busy( BOARD_DATA command )
{

int count;

if ( !wait_for_busy_bit_clear() )
    return FT_BUSY;

/*
-----------------------
Try to set the busy bit
----------------------- */
for ( count=0; count<3; count++ )
    {
    send_command_wo_check( SET_BUSY );
    if ( wait_for_busy_bit_set() )
        {
        send_command_wo_check( command );
        if ( wait_for_busy_bit_clear() )
            return FT_OK;

        return FT_LOCKED_UP;
        }
    }

return FT_ERROR;
}

/***********************************************************************
*                            SET_COMMAND_MODE                          *
***********************************************************************/
int BOARD_CONTROL_CLASS::set_command_mode( void )
{
send_command_wo_check( NMI );

if ( !wait_for_busy_bit_clear() )
    return FT_BUSY;

return FT_OK;
}

/***********************************************************************
*                             READ_PARAMETER                           *
***********************************************************************/
int BOARD_CONTROL_CLASS::read_parameter( BOARD_DATA parameter, BOARD_DATA * dest )
{

if ( send_command_busy(parameter) == FT_OK )
    {
    *dest = read_data();
    return TRUE;
    }
else
    {
    *dest = 0;
    return FALSE;
    }
}

/***********************************************************************
*                              SET_PARAMETER                           *
***********************************************************************/
int BOARD_CONTROL_CLASS::set_parameter( BOARD_DATA parameter, BOARD_DATA sorc )
{
send_data( sorc );
return send_command_busy( parameter );
}

