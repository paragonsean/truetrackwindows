#include <windows.h>
#include <conio.h>
#include "..\include\ft.h"

/***********************************************************************
*                Copyright (c) 1993 by Visi-Trak Corp.                 *
***********************************************************************/

unsigned short DataPort      = 0x220;
unsigned short CmdStatusPort = 0x222;
unsigned short IntPort       = 0x224;
unsigned short ClrCmdPort    = 0x226;

extern void short_wait( void );

/***********************************************************************
*                          FT_BOARD_ADDRESS                            *
***********************************************************************/
unsigned short ft_board_address( void )
{
return DataPort;
}

/***********************************************************************
*                             FT_SETPORTS                              *
***********************************************************************/
void ft_setports( unsigned short base_port_id )
{

DataPort      = base_port_id;
CmdStatusPort = base_port_id + 2;
IntPort       = base_port_id + 4;
ClrCmdPort    = base_port_id + 6;

}

/***********************************************************************
*                             FT_SEND_DATA                             *
***********************************************************************/
BOOLEAN ft_send_data( BOARD_DATA data )
{
_outpw( DataPort, data );
return TRUE;
}

/***********************************************************************
*                          FT_CLEAR_CMD_PORT                           *
***********************************************************************/
void ft_clear_cmd_port( void )
{
_outpw( ClrCmdPort, 0x0000 );
}

/***********************************************************************
*                         FT_SEND_CMD_WO_CHECK                         *
***********************************************************************/
BOOLEAN ft_send_cmd_wo_check( BOARD_DATA command )
{
_outpw( CmdStatusPort, command );
_outpw( ClrCmdPort, 0x0000 );

short_wait();
return TRUE;
}

/***********************************************************************
*                             FT_READ_DATA                             *
***********************************************************************/
BOARD_DATA ft_read_data( void )
{
return (BOARD_DATA) _inpw( DataPort );
}

/***********************************************************************
*                            FT_READ_STATUS                            *
***********************************************************************/
BOARD_DATA ft_read_status( void )
{
return (BOARD_DATA) _inpw( CmdStatusPort );
}

/***********************************************************************
*                          FT_CLEAR_INTERRUPT                          *
***********************************************************************/
void ft_clear_interrupt( void )
{
_inpw( IntPort );
}

