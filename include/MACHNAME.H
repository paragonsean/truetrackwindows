#ifndef _MACHINE_NAME_LIST_CLASS_
#define _MACHINE_NAME_LIST_CLASS_

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#define NO_SORT          0
#define SORT_BY_COMPUTER 1
#define SORT_BY_MACHINE  2

struct MACHINE_NAME_LIST_ENTRY
    {
    TCHAR computer[COMPUTER_NAME_LEN+1];
    TCHAR machine[MACHINE_NAME_LEN+1];
    };

class MACHINE_NAME_LIST_CLASS
    {
    private:
    MACHINE_NAME_LIST_ENTRY * current_entry;
    GENERIC_LIST_CLASS        mlist;

    public:
    MACHINE_NAME_LIST_CLASS( void );
    ~MACHINE_NAME_LIST_CLASS( void );
    BOOLEAN    add_computer( TCHAR * computer );
    BOOLEAN    add( TCHAR * computer, TCHAR * machine, BITSETYPE sort_type );
    int        count() { return (int) mlist.count(); }
    void       empty( void );
    BOOLEAN    get_computer_name( TCHAR * destco, TCHAR * sorcma );
    void       remove( void );
    void       rewind( void ) { mlist.rewind(); current_entry = 0; }
    BOOLEAN    next( void );
    BOOLEAN    find( TCHAR * machine_name );
    TCHAR    * name( void );
    TCHAR    * computer_name( void );
    };

#endif
