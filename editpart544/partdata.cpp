#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\GenList.h"
#include "..\include\Subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "PartData.h"

GENERIC_LIST_CLASS MachineList;

/***********************************************************************
*                           CLEANUP_PARTLIST                           *
***********************************************************************/
static void cleanup_partlist( GENERIC_LIST_CLASS & p )
{
PART_ENTRY * pe;

p.rewind();
while ( TRUE )
    {
    pe = (PART_ENTRY *) p.next();
    if ( !pe )
        break;
    delete pe;
    }

p.remove_all();
}

/***********************************************************************
*                            LOAD_PARTLIST                             *
***********************************************************************/
BOOLEAN load_partlist( GENERIC_LIST_CLASS & p, TCHAR * computer, TCHAR * machine )
{
DB_TABLE t;
PART_ENTRY * pe;

if ( t.open( parts_dbname(computer, machine), PARTS_RECLEN, PFL ) )
    {
    while ( t.get_next_record(FALSE) )
        {
        pe = new PART_ENTRY;
        if ( !pe )
            return FALSE;
        t.get_alpha( pe->name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
        p.append( pe );
        }
    t.close();
    }
return TRUE;
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
static void cleanup( void )
{
MACHINE_ENTRY * m;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;
    cleanup_partlist( m->partlist );
    delete m;
    }

MachineList.remove_all();
}

/***********************************************************************
*                       READ_MACHINES_AND_PARTS                        *
***********************************************************************/
BOOLEAN read_machines_and_parts( void )
{
COMPUTER_CLASS  c;
DB_TABLE        t;
MACHINE_ENTRY * m;

cleanup();

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        {
        if (t.open( machset_dbname(c.name()), MACHSET_RECLEN, PFL ))
            {
            while ( t.get_next_record(FALSE) )
                {
                m = new MACHINE_ENTRY;
                if ( !m )
                    {
                    cleanup();
                    return FALSE;
                    }
                lstrcpy( m->computer, c.name() );
                t.get_alpha( m->name, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
                if ( !load_partlist(m->partlist, m->computer, m->name) )
                    {
                    cleanup();
                    return FALSE;
                    }
                MachineList.append( m );
                }
            t.close();
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                      CLEANUP_MACHINES_AND_PARTS                      *
***********************************************************************/
void cleanup_machines_and_parts( void )
{
cleanup();
}

/***********************************************************************
*                           FIND_MACHINE_ENTRY                         *
***********************************************************************/
MACHINE_ENTRY * find_machine_entry( TCHAR * machine )
{
MACHINE_ENTRY * m;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( strings_are_equal(m->name, machine, MACHINE_NAME_LEN) )
        break;
    }

return m;
}

/***********************************************************************
*                           MACHINE_IS_LOCAL                           *
***********************************************************************/
BOOLEAN machine_is_local( TCHAR * ma )
{
COMPUTER_CLASS  c;
MACHINE_ENTRY * m;

m = find_machine_entry( ma );
if ( m )
    {
    if ( strings_are_equal(c.whoami(), m->computer) )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            MACHINE_COUNT                             *
*             Returns the number of machines at a computer.            *
***********************************************************************/
int machine_count( TCHAR * computer_to_search )
{
int count;
MACHINE_ENTRY * m;

count = 0;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( strings_are_equal(m->computer, computer_to_search) )
        count++;
    }

return count;
}

/***********************************************************************
*                           RELOAD_PARTLIST                            *
***********************************************************************/
BOOLEAN reload_partlist( TCHAR * machine )
{

MACHINE_ENTRY * m;

m = find_machine_entry( machine );
if ( !m )
    return FALSE;
cleanup_partlist( m->partlist );
return load_partlist( m->partlist, m->computer, m->name );
}

/***********************************************************************
*                           FIND_PART_ENTRY                            *
***********************************************************************/
PART_ENTRY * find_part_entry( MACHINE_ENTRY * m, TCHAR * part )
{

PART_ENTRY    * p;

m->partlist.rewind();
while ( TRUE )
    {
    p = (PART_ENTRY *) m->partlist.next();
    if ( !p )
        break;
    if ( strings_are_equal(p->name, part, PART_NAME_LEN) )
        break;
    }

return p;
}
