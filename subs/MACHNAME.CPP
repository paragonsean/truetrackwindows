#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

/***********************************************************************
*                          MACHINE_NAME_LIST_CLASS                     *
***********************************************************************/
MACHINE_NAME_LIST_CLASS::MACHINE_NAME_LIST_CLASS( void )
{
current_entry = 0;
}

/***********************************************************************
*                MACHINE_NAME_LIST_CLASS::ADD_COMPUTER                 *
*   TRUE just means I was able to open the file, it doesn't            *
*   mean that there were any machines on the computer.                 *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::add_computer( TCHAR * computer_name )
{
BOOLEAN  status;
STRING_CLASS s;
DB_TABLE t;
MACHINE_NAME_LIST_ENTRY * mne;

s = machset_dbname( computer_name );
if ( !file_exists(s.text()) )
    return FALSE;

if ( t.open(s.text(), MACHSET_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        mne = new MACHINE_NAME_LIST_ENTRY;
        if ( mne )
            {
            status = FALSE;
            lstrcpy( mne->computer, computer_name );
            if ( t.get_alpha( mne->machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN) )
                {
                if ( mlist.append( mne ) )
                    status = TRUE;
                }
            if ( !status )
                delete mne;
            }
        }
    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::EMPTY                    *
***********************************************************************/
void MACHINE_NAME_LIST_CLASS::empty( void )
{
MACHINE_NAME_LIST_ENTRY * mne;

current_entry = 0;
mlist.rewind();
while ( TRUE )
    {
    mne = (MACHINE_NAME_LIST_ENTRY *) mlist.next();
    if ( !mne )
        break;
    delete mne;
    }

mlist.remove_all();
}

/***********************************************************************
*                   MACHINE_NAME_LIST_CLASS::REMOVE                    *
***********************************************************************/
void MACHINE_NAME_LIST_CLASS::remove( void )
{
if ( current_entry )
    {
    delete current_entry;
    current_entry = 0;
    mlist.remove();
    }

}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::NEXT                     *
***********************************************************************/
BOOLEAN  MACHINE_NAME_LIST_CLASS::next( void )
{
current_entry = (MACHINE_NAME_LIST_ENTRY *) mlist.next();

if ( current_entry )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                     MACHINE_NAME_LIST_CLASS::ADD                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::add( TCHAR * computer_name, TCHAR * machine_name, BITSETYPE sort_type )
{
MACHINE_NAME_LIST_ENTRY * mne;
short computer_compare_status;
short machine_compare_status;

mne = new MACHINE_NAME_LIST_ENTRY;
if ( !mne )
    return FALSE;

lstrcpy( mne->computer, computer_name );
lstrcpy( mne->machine,  machine_name );
rewind();
while ( next() )
    {
    /*
    ----------------------------------------------------------
    Make the computers equal unless I am sorting by them first
    ---------------------------------------------------------- */
    computer_compare_status = 0;
    if ( sort_type == SORT_BY_COMPUTER )
        computer_compare_status = compare( mne->computer, current_entry->computer, COMPUTER_NAME_LEN );

    machine_compare_status = compare( mne->machine,  current_entry->machine,  MACHINE_NAME_LEN );

    if ( computer_compare_status < 0 || (computer_compare_status == 0 && machine_compare_status < 0) )
        {
        if ( mlist.insert(mne) )
            {
            return TRUE;
            }
        else
            {
            delete mne;
            return FALSE;
            }
        }
    }

/*
------------------------------------------------------------
If I didn't have to insert it, append to the end of the list
------------------------------------------------------------ */
if ( mlist.append( mne ) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::FIND                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::find( TCHAR * name_to_find )
{
if ( !name_to_find )
    return FALSE;

mlist.rewind();

while ( TRUE )
    {
    current_entry = (MACHINE_NAME_LIST_ENTRY *) mlist.next();
    if ( !current_entry )
        break;
    if ( compare(current_entry->machine, name_to_find, MACHINE_NAME_LEN) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        MACHINE_NAME_LIST_CLASS                       *
*                           GET_COMPUTER_NAME                          *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::get_computer_name( TCHAR * destco, TCHAR * sorcma )
{
if ( find(sorcma) )
    {
    copystring( destco, current_entry->computer );
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                     MACHINE_NAME_LIST_CLASS::NAME                    *
***********************************************************************/
TCHAR * MACHINE_NAME_LIST_CLASS::name( void )
{
static TCHAR no_machine[] = NO_MACHINE;

if ( current_entry )
    return current_entry->machine;

return no_machine;
}

/***********************************************************************
*                MACHINE_NAME_LIST_CLASS::COMPUTER_NAME                *
***********************************************************************/
TCHAR * MACHINE_NAME_LIST_CLASS::computer_name( void )
{
static TCHAR no_computer[] = NO_COMPUTER;

if ( current_entry )
    return current_entry->computer;

return no_computer;
}

/***********************************************************************
*                         ~MACHINE_NAME_LIST_CLASS                     *
***********************************************************************/
MACHINE_NAME_LIST_CLASS::~MACHINE_NAME_LIST_CLASS( void )
{
empty();
}
