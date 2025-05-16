#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

const static TCHAR NullChar     = TEXT( '\0' );
const static TCHAR SpaceChar    = TEXT( ' ' );
const static TCHAR NoComputer[] = NO_COMPUTER;
const static TCHAR NoMachine[]  = NO_MACHINE;

static BITSETYPE FtAnalogCurveType[MAX_FT_CHANNELS] =
    {
    CHAN_1_ANALOG_CURVE, CHAN_2_ANALOG_CURVE, CHAN_3_ANALOG_CURVE, CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE, CHAN_6_ANALOG_CURVE, CHAN_7_ANALOG_CURVE, CHAN_8_ANALOG_CURVE
    };

/***********************************************************************
*                            CURVE_DATA                                *
*                         FT_CHANNEL_NUMBER                            *
*                              [1,8]                                   *
***********************************************************************/
short CURVE_DATA::ft_channel_number()
{

int i;

for ( i=0; i<MAX_FT_CHANNELS; i++ )
    {
    if ( type & FtAnalogCurveType[i] )
        return i+1;
    }

return NO_FT_CHANNEL;
}

/***********************************************************************
*                            CURVE_DATA                                *
*                          FT_CHANNEL_TYPE                             *
***********************************************************************/
BITSETYPE CURVE_DATA::ft_channel_type()
{
int i;

for ( i=0; i<MAX_FT_CHANNELS; i++ )
    {
    if ( type & FtAnalogCurveType[i] )
        return FtAnalogCurveType[i];
    }

return 0;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                 SET                                  *
***********************************************************************/
void PART_NAME_ENTRY::set( const TCHAR * pn )
{
*part = NullChar;

if ( pn )
    if ( lstrlen(pn) > 0 )
        lstrcpyn( part, pn, PART_NAME_LEN+1 );
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                 SET                                  *
***********************************************************************/
void PART_NAME_ENTRY::set( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn )
{
*computer = NullChar;
if ( cn )
    if ( lstrlen(cn) > 0 )
        lstrcpyn( computer, cn, COMPUTER_NAME_LEN+1 );

*machine = NullChar;
if ( mn )
    if ( lstrlen(mn) > 0 )
        lstrcpyn( machine,  mn, MACHINE_NAME_LEN+1 );

set( pn );
};

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                            PART_NAME_ENTRY                           *
***********************************************************************/
PART_NAME_ENTRY::PART_NAME_ENTRY()
{
set( NoComputer, NoMachine, NO_PART );
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                  ==                                  *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::operator==(const PART_NAME_ENTRY & sorc )
{
if ( !strings_are_equal(computer, sorc.computer) )
    return FALSE;

if ( !strings_are_equal(machine, sorc.machine) )
    return FALSE;

if ( !strings_are_equal(part, sorc.part) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                   =                                  *
***********************************************************************/
void PART_NAME_ENTRY::operator=(const PART_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine, sorc.part );
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                               IS_EQUAL                               *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::is_equal( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn )
{
if ( !strings_are_equal(computer, cn) )
    return FALSE;

if ( !strings_are_equal(machine, mn) )
    return FALSE;

if ( !strings_are_equal(part, pn) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                 EXISTS                               *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::exists( void )
{
return part_exists( computer, machine, part );
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                IS_PART                               *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::is_part( const TCHAR * pn )
{
return strings_are_equal( part, pn  );
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                              IS_MACHINE                              *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::is_machine( const TCHAR * mn )
{
return strings_are_equal( machine, mn  );
}


/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                                 SET                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::set( const TCHAR * mn )
{
*machine = NullChar;

if ( mn )
    if ( lstrlen(mn) > 0 )
        lstrcpyn( machine, mn, MACHINE_NAME_LEN+1 );
}

/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                                 SET                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::set( const TCHAR * cn, const TCHAR * mn )
{
*computer = NullChar;
if ( cn )
    if ( lstrlen(cn) > 0 )
        lstrcpyn( computer, cn, COMPUTER_NAME_LEN+1 );

set( mn );
};

/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                                   =                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::operator=(const MACHINE_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine );
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                               IS_EQUAL                               *
***********************************************************************/
BOOLEAN MACHINE_NAME_ENTRY::is_equal( const TCHAR * sorc_computer, const TCHAR * sorc_machine )
{
if ( !strings_are_equal(computer, sorc_computer) )
    return FALSE;

if ( !strings_are_equal(machine, sorc_machine) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                          MACHINE_NAME_ENTRY                          *
***********************************************************************/
MACHINE_NAME_ENTRY::MACHINE_NAME_ENTRY()
{
set( NoComputer, NoMachine );
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                               EMPTY                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::empty( void )
{
set( NoComputer, NoMachine );
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                              IS_EMPTY                                *
***********************************************************************/
BOOLEAN MACHINE_NAME_ENTRY::is_empty( void )
{
return is_equal( NoComputer, NoMachine );
}

/***********************************************************************
*                         PROFILE_NAME_ENTRY                           *
*                                SET                                   *
***********************************************************************/
void PROFILE_NAME_ENTRY::set( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn, const TCHAR * sn )
{
*computer = NullChar;
if ( cn )
    if ( lstrlen(cn) > 0 )
        lstrcpyn( computer, cn, COMPUTER_NAME_LEN+1 );

*machine = NullChar;
if ( mn )
    if ( lstrlen(mn) > 0 )
        lstrcpyn( machine,  mn, MACHINE_NAME_LEN+1 );

*part = NullChar;
if ( pn )
    if ( lstrlen(pn) > 0 )
        lstrcpyn( part,  pn, PART_NAME_LEN+1 );

*shot = NullChar;
if ( sn )
    if ( lstrlen(sn) > 0 )
        lstrcpyn( shot,  sn, SHOT_LEN+1 );
};

/***********************************************************************
*                           PROFILE_NAME_ENTRY                         *
*                                   =                                  *
***********************************************************************/
void PROFILE_NAME_ENTRY::operator=(const PROFILE_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine, sorc.part, sorc.shot );
}

/***********************************************************************
*                           PROFILE_NAME_ENTRY                         *
*                                   =                                  *
***********************************************************************/
void PROFILE_NAME_ENTRY::operator=(const PART_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine, sorc.part, 0 );
}

