#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\color.h"
#include "..\include\verrors.h"

struct COLOR_ENTRY
    {
    TCHAR    name[COLOR_NAME_LEN+1];
    COLORREF value;
    };

static short NofColors = 0;
static COLOR_ENTRY * ColorArray = 0;

/***********************************************************************
*                                CLEAR                                 *
***********************************************************************/
void clear( void )
{

if ( ColorArray )
    {
    delete[] ColorArray;
    ColorArray = 0;
    NofColors  = 0;
    }

}

/***********************************************************************
*                         COLORS_CLASS::RELOAD                         *
***********************************************************************/
BOOLEAN  COLORS_CLASS::reload( void )
{
DB_TABLE t;
short    n;
BOOLEAN  status;
BYTE red;
BYTE green;
BYTE blue;

clear();

status = TRUE;
if ( t.open( color_dbname(), COLOR_RECLEN, PFL ) )
    {
    n = (short) t.nof_recs();
    if ( n > 0 )
        {
        NofColors = 0;
        ColorArray = new COLOR_ENTRY[n];
        if ( ColorArray )
            {
            while ( t.get_next_record(NO_LOCK) && NofColors < n )
                {
                t.get_alpha( ColorArray[NofColors].name, COLOR_NAME_OFFSET, UNITS_LEN );
                red   = (BYTE) t.get_short( COLOR_RED_OFFSET );
                green = (BYTE) t.get_short( COLOR_GREEN_OFFSET );
                blue  = (BYTE) t.get_short( COLOR_BLUE_OFFSET );

                ColorArray[NofColors].value = RGB( red, green, blue );
                NofColors++;
                }
            }
        else
            {
            error_message( COLOR_RELOAD_SUB, NO_MEM_ALLOC_ERROR );
            status = FALSE;
            }

        }
    t.close();
    }

return status;
}

/***********************************************************************
*                      COLORS_CLASS::COLORS_CLASS                      *
***********************************************************************/
COLORS_CLASS::COLORS_CLASS( void )
{
if ( !ColorArray )
    reload();

}

/***********************************************************************
*                      COLORS_CLASS::~COLORS_CLASS                     *
***********************************************************************/
COLORS_CLASS::~COLORS_CLASS( void )
{
clear();
}

/***********************************************************************
*                         COLORS_CLASS::INDEX                          *
***********************************************************************/
short COLORS_CLASS::index( TCHAR * name_to_find )
{
short i;

for ( i=0; i<NofColors; i++ )
    {
    if ( compare(ColorArray[i].name, name_to_find, COLOR_NAME_LEN) == 0 )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                       COLORS_CLASS::OPERATOR[]                       *
***********************************************************************/
COLORREF COLORS_CLASS::operator[]( short i )
{
if ( i>=0 && i<NofColors )
    return ColorArray[i].value;

return 0;
}


/***********************************************************************
*                       COLORS_CLASS::OPERATOR[]                       *
***********************************************************************/
COLORREF COLORS_CLASS::operator[]( TCHAR * name_to_find )
{
short i;

i = index( name_to_find );

if ( i != NO_INDEX )
    return ColorArray[i].value;

return 0;
}

/***********************************************************************
*                         COLORS_CLASS::COUNT                          *
***********************************************************************/
short    COLORS_CLASS::count( void )
{
return NofColors;
}

/***********************************************************************
*                        *  COLORS_CLASS::NAME                         *
***********************************************************************/
TCHAR * COLORS_CLASS::name( short i )
{

static TCHAR unknown_name[] = UNKNOWN;

if ( i>=0 && i<NofColors )
    return ColorArray[i].name;

return unknown_name;
}
