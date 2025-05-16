#include <windows.h>
#include <winioctl.h>

#include "..\include\ft.h"

#define VPD_TYPE 40000

#define IOCTL_GPD_READ_PORT_USHORT  CTL_CODE( VPD_TYPE, 0x901, METHOD_BUFFERED,   FILE_READ_ACCESS  )
#define IOCTL_GPD_WRITE_PORT_USHORT CTL_CODE( VPD_TYPE, 0x911, METHOD_BUFFERED,   FILE_WRITE_ACCESS )
#define IOCTL_VISI_READ_ALL_DATA    CTL_CODE( VPD_TYPE, 0x912, METHOD_OUT_DIRECT, FILE_READ_ACCESS  )

struct GENPORT_WRITE_ENTRY
    {
    ULONG   port;     // Port # to write to
    USHORT  data;
    };

static TCHAR MyDeviceFile[] = TEXT( "\\\\.\\VpdDev" );

static HANDLE Fp = INVALID_HANDLE_VALUE;
extern void short_wait( void );

/***********************************************************************
*                Copyright (c) 1993 by Visi-Trak Corp.                 *
***********************************************************************/

unsigned short DataPort      = 0x220;
unsigned short CmdStatusPort = 0x222;
unsigned short IntPort       = 0x224;
unsigned short ClrCmdPort    = 0x226;

/***********************************************************************
*                          FT_OPEN_DEVICE                              *
***********************************************************************/
BOOLEAN ft_open_device( void )
{

Fp = CreateFile(
        MyDeviceFile,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        );

if ( Fp == INVALID_HANDLE_VALUE )
    {
    return FALSE;
    }
    
return TRUE;
}

/***********************************************************************
*                           FT_CLOSE_DEVICE                            *
***********************************************************************/
void    ft_close_device( void )
{
if ( Fp != INVALID_HANDLE_VALUE )
    {
    CloseHandle( Fp );
    Fp = INVALID_HANDLE_VALUE;
    }
}

/***********************************************************************
*                       FT_READ_ALL_DATA                               *
***********************************************************************/
DWORD ft_read_all_data( unsigned short portid, BOARD_DATA * dest, DWORD destbytes )
{
ULONG  port_number;
DWORD  returned_length;

if ( Fp != INVALID_HANDLE_VALUE )
    {
    port_number = portid;

    DeviceIoControl(
                Fp,                          // Handle to device
                IOCTL_VISI_READ_ALL_DATA,    // IO Control code
                &port_number,                // Buffer to driver.
                sizeof(port_number),         // Length of buffer in bytes.
                dest,                        // Buffer from driver.
                destbytes,                   // Length of buffer in bytes.
                &returned_length,            // Bytes placed in x by driver
                NULL                         // NULL means wait till op. completes.
                );

    }

return returned_length;
}

/***********************************************************************
*                                INPORT                                *
***********************************************************************/
unsigned short inport( unsigned short portid )
{
ULONG  port_number;
USHORT x;
DWORD  returned_length;

x = 0;

if ( Fp != INVALID_HANDLE_VALUE )
    {
    port_number = portid;
    /* port_number -= (ULONG) DataPort; */

    DeviceIoControl(
                Fp,                          // Handle to device
                IOCTL_GPD_READ_PORT_USHORT,  // IO Control code for Read
                &port_number,                // Buffer to driver.
                sizeof(port_number),         // Length of buffer in bytes.
                &x,                          // Buffer from driver.
                sizeof(x),                   // Length of buffer in bytes.
                &returned_length,            // Bytes placed in x by driver
                NULL                         // NULL means wait till op. completes.
                );

    }

return x;
}

/***********************************************************************
*                               OUTPORT                                *
***********************************************************************/
BOOL outport( unsigned short portid, unsigned short data_to_write )
{
BOOL           status;
DWORD          returned_length;
GENPORT_WRITE_ENTRY gwe;

status = FALSE;

if ( Fp != INVALID_HANDLE_VALUE )
    {
    gwe.port = portid;
    /* gwe.port -= (ULONG) DataPort; */
    gwe.data = data_to_write;

    status = DeviceIoControl(
                Fp,                          // Handle to device
                IOCTL_GPD_WRITE_PORT_USHORT, // IO Control code for write
                &gwe,                        // Buffer to driver.
                sizeof(gwe),                 // Length of buffer in bytes.
                NULL,                        // Buffer from driver.
                0,                           // Length of buffer in bytes.
                &returned_length,            // Bytes placed in x by driver
                NULL                         // NULL means wait till op. completes.
                );

    }

return status;
}

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
outport( DataPort, data );
return TRUE;
}

/***********************************************************************
*                          FT_CLEAR_CMD_PORT                           *
***********************************************************************/
void ft_clear_cmd_port( void )
{
outport( ClrCmdPort, 0x0000 );
}

/***********************************************************************
*                         FT_SEND_CMD_WO_CHECK                         *
***********************************************************************/
BOOLEAN ft_send_cmd_wo_check( BOARD_DATA command )
{

outport( CmdStatusPort, command );
outport( ClrCmdPort, 0x0000 );

short_wait();
return TRUE;
}

/***********************************************************************
*                             FT_READ_DATA                             *
***********************************************************************/
BOARD_DATA ft_read_data( void )
{
return (BOARD_DATA) inport( DataPort );
}

/***********************************************************************
*                            FT_READ_STATUS                            *
***********************************************************************/
BOARD_DATA ft_read_status( void )
{
return (BOARD_DATA) inport( CmdStatusPort );
}

/***********************************************************************
*                          FT_CLEAR_INTERRUPT                          *
***********************************************************************/
void ft_clear_interrupt( void )
{
inport( IntPort );
}

