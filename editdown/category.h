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

int x;
int n;
CATEGORY_ENTRY * cp;
void cleanup( void );

public:

CATEGORY_CLASS( void );
~CATEGORY_CLASS( void );
BOOLEAN find( TCHAR * cat_to_find );
BOOLEAN findname( const TCHAR * name_to_find );
BOOLEAN next( void );
BOOLEAN read( void );
void    rewind( void ) { x = NO_INDEX; }
TCHAR * name( void );
TCHAR * cat( void );
};

struct SUBCAT_ENTRY
    {
    TCHAR catnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR subnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];
    };

class SUBCAT_CLASS
{
private:

int x;
int n;
SUBCAT_ENTRY * sp;
void cleanup( void );

public:

SUBCAT_CLASS( void ) { n = 0; x = NO_INDEX;  sp = 0; }
~SUBCAT_CLASS( void );
BOOLEAN find( TCHAR * cat_to_find, TCHAR * subcat_to_find );
BOOLEAN findname( TCHAR * cat_to_find, TCHAR * name_to_find );
BOOLEAN read( void );
void    rewind( void ) { x = NO_INDEX; }
BOOLEAN next( void );
TCHAR * name( void );
TCHAR * cat( void );
TCHAR * subcat(void );
};

#endif
