#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\marks.h"
#include "..\include\verrors.h"

/***********************************************************************
*                             MARKS_CLASS                               *
***********************************************************************/
MARKS_CLASS::MARKS_CLASS( void )
{
n = 0;
}

/***********************************************************************
*                          MARKS_CLASS::INDEX                          *
***********************************************************************/
short MARKS_CLASS::index( TCHAR type_to_find, short number_to_find )
{
short i;

for ( i=0; i<n; i++ )
    {
    if ( array[i].parameter_type == type_to_find && array[i].parameter_number == number_to_find )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                        MARKS_CLASS::NEW_INDEX                        *
*                                                                      *
*  Return the index where a new mark should go. Return NO_INDEX if     *
*  this already exists;                                                *
*                                                                      *
***********************************************************************/
short MARKS_CLASS::new_index( TCHAR type_to_add, short number_to_add )
{
short i;

/*
-----------------------------------------
If there is already and entry, do nothing
----------------------------------------- */
if ( index(type_to_add, number_to_add) != NO_INDEX )
    return NO_INDEX;

switch ( type_to_add )
    {
    case PARAMETER_MARK:
        for ( i=0; i<n; i++ )
            {
            if ( array[i].parameter_type != PARAMETER_MARK )
                return i;

            if ( array[i].parameter_number > number_to_add )
                return i;
            }
        break;

    case START_POS_MARK:
        for ( i=0; i<n; i++ )
            {
            if ( array[i].parameter_type == END_POS_MARK && array[i].parameter_number >= number_to_add )
                return i;

            if ( array[i].parameter_type == START_POS_MARK && array[i].parameter_number > number_to_add )
                return i;

            if ( array[i].parameter_type == VEL_POS_MARK )
                return i;
            }
        break;

    case END_POS_MARK:
        for ( i=0; i<n; i++ )
            {
            if ( array[i].parameter_type == START_POS_MARK && array[i].parameter_number > number_to_add )
                return i;

            if ( array[i].parameter_type == END_POS_MARK && array[i].parameter_number > number_to_add )
                return i;

            if ( array[i].parameter_type == VEL_POS_MARK )
                return i;
            }
        break;

    case VEL_POS_MARK:
        for ( i=0; i<n; i++ )
            {
            if ( array[i].parameter_type == VEL_POS_MARK && array[i].parameter_number > number_to_add )
                return i;
            }
        break;
    }

return n;
}

/***********************************************************************
*                         MARKS_CLASS::REMOVE                          *
***********************************************************************/
void MARKS_CLASS::remove( short i )
{

if ( i >= n || i < 0 )
    return;

if ( !n )
    return;

n--;

while ( i < n )
    {
    array[i] = array[i+1];
    i++;
    }

return;
}

/***********************************************************************
*                      MARKS_CLASS::SET_DEFAULTS                       *
***********************************************************************/
void MARKS_CLASS::set_defaults( short i )
{
static TCHAR BLUE[]   = TEXT( "BLUE" );
static TCHAR CYAN[]   = TEXT( "CYAN" );
static TCHAR GREEN[]  = TEXT( "GREEN" );
static TCHAR RED[]    = TEXT( "RED" );
static TCHAR WHITE[]  = TEXT( "WHITE" );
static TCHAR YELLOW[] = TEXT( "YELLOW" );
static TCHAR EmptyString[] = TEXT( "" );

COLORS_CLASS c;

switch ( array[i].parameter_type )
    {
    case PARAMETER_MARK:
        array[i].color  = c.index( WHITE );
        array[i].symbol = VLINE_MARK;
        break;

    case START_POS_MARK:
    case END_POS_MARK:
        array[i].symbol = BLINE_MARK;
        switch ( array[i].parameter_number )
            {
            case 1:
                array[i].color  = c.index( YELLOW );
                break;
            case 2:
                array[i].color  = c.index( CYAN );
                break;
            case 3:
                array[i].color  = c.index( RED );
                break;
            case 4:
                array[i].color  = c.index( BLUE );
                break;
            }
        break;

    case VEL_POS_MARK:
        array[i].symbol = BLINE_MARK;
        array[i].color  = c.index( GREEN );
        break;
    }

lstrcpy( array[i].label, EmptyString );
}

/***********************************************************************
*                           MARKS_CLASS::ADD                           *
***********************************************************************/
void MARKS_CLASS::add( TCHAR type_to_add, short number_to_add )
{
short dest;
short i;

dest = new_index( type_to_add, number_to_add );
if ( dest != NO_INDEX )
    {
    /*
    -----------------------
    Copy all entries up one
    ----------------------- */
    i = n;
    while ( i > dest )
        {
        array[i] = array[i-1];
        i--;
        }
    array[dest].parameter_type = type_to_add;
    array[dest].parameter_number = number_to_add;
    n++;
    set_defaults( dest );
    }

}

/***********************************************************************
*                           MARKS_CLASS::LOAD                           *
***********************************************************************/
BOOLEAN MARKS_CLASS::load( TCHAR * computer_to_find, TCHAR * machine_to_find, TCHAR * part_to_find )
{
short    i;
DB_TABLE t;
TCHAR colorname[COLOR_NAME_LEN+1];
COLORS_CLASS c;
MARKLIST_ENTRY * m;

lstrcpy( computer, computer_to_find );
lstrcpy( machine,  machine_to_find );
lstrcpy( part,     part_to_find );

i = 0;
m = array;

t.open( marklist_dbname(computer_to_find, machine_to_find, part_to_find), MARKLIST_RECLEN, PFL );
while( t.get_next_record(NO_LOCK) && i < MAX_MARKS )
    {
    m->parameter_type   = t.get_char( MARKLIST_PARM_TYPE_OFFSET );
    m->parameter_number = t.get_short( MARKLIST_PARM_NUMBER_OFFSET );
    t.get_alpha( colorname, MARKLIST_COLOR_OFFSET, COLOR_NAME_LEN );
    m->color = c.index( colorname );
    m->symbol           = (BITSETYPE) t.get_long( MARKLIST_SYMBOL_OFFSET );
    t.get_alpha( m->label, MARKLIST_LABEL_OFFSET, MARK_LABEL_LEN );
    i++;
    m++;
    }

t.close();

if ( i > 0 )
    {
    n = i;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         MARKS_CLASS::SAVE                            *
***********************************************************************/
BOOLEAN MARKS_CLASS::save( void )
{
short    i;
DB_TABLE t;
COLORS_CLASS c;
MARKLIST_ENTRY * m;
BOOLEAN status;

m = array;
status = FALSE;

t.open( marklist_dbname(computer, machine, part), MARKLIST_RECLEN, FL );
t.empty();
for ( i=0; i<n; i++ )
    {
    t.put_char(  MARKLIST_PARM_TYPE_OFFSET,   m->parameter_type );
    t.put_short( MARKLIST_PARM_NUMBER_OFFSET, m->parameter_number, PARAMETER_NUMBER_LEN );
    t.put_alpha( MARKLIST_COLOR_OFFSET,       c.name(m->color), COLOR_NAME_LEN );
    t.put_long(  MARKLIST_SYMBOL_OFFSET,      m->symbol, BITSETYPE_LEN );
    t.put_alpha( MARKLIST_LABEL_OFFSET,       m->label, MARK_LABEL_LEN );
    t.rec_append();
    m++;
    status = TRUE;
    }

t.close();

return status;
}
