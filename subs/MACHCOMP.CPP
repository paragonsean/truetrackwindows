#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

/***********************************************************************
*                         MACHINE_COMPUTER_NAME                        *
*                                                                      *
*  This routine searches the computers for a machine. If none is       *
*  found, I return a NULL.                                             *
*                                                                      *
***********************************************************************/
TCHAR * machine_computer_name( TCHAR * machine_name )
{
static TCHAR computer[COMPUTER_NAME_LEN+1];
COMPUTER_CLASS c;

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        {
        if ( machine_exists(c.name(), machine_name) )
            {
            lstrcpy( computer, c.name() );
            return computer;
            }
        }
    }

return 0;
}
