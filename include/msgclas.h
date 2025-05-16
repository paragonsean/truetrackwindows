#ifndef _BLOCK_MESSAGE_CLASS_
#define _BLOCK_MESSAGE_CLASS_

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

struct BLOCK_MESSAGE_ENTRY
    {
    int32   low_block_number;
    int32   high_block_number;
    TCHAR * msg;
    };

class BLOCK_MESSAGE_CLASS
    {
    private:

    GENERIC_LIST_CLASS msglist;

    public:

    BLOCK_MESSAGE_CLASS( void );
    ~BLOCK_MESSAGE_CLASS( void );
    void    cleanup( void );
    BOOLEAN load( TCHAR * filename );
    TCHAR * message( int32 block_number );
    };

#endif