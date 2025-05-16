#ifndef _CATEGORY_CLASS_
#define _CATEGORY_CLASS_

struct CATEGORY_ENTRY
    {
    TCHAR number[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];
    };

class CATEGORY_CLASS
{
private:

int n;
CATEGORY_ENTRY * cat;
void cleanup( void );

public:

CATEGORY_CLASS( void ) { n = 0; cat = 0; }
~CATEGORY_CLASS( void );
BOOLEAN read( void );
BOOLEAN save( LISTBOX_CLASS & lb );
TCHAR * name( TCHAR * number );
};

#endif
