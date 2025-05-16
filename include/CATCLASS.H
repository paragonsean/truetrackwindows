#ifndef _DOWNCAT_NAME_CLASS_
#define _DOWNCAT_NAME_CLASS_

struct DOWNCAT_NAME_ENTRY
    {
    TCHAR number[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];

    DOWNCAT_NAME_ENTRY();
    };

class DOWNCAT_NAME_CLASS
    {
    private:

    DOWNCAT_NAME_ENTRY * p;
    int32   n;

    public:

    DOWNCAT_NAME_CLASS() { p = 0; n = 0; }
    ~DOWNCAT_NAME_CLASS() { cleanup(); }
    void    cleanup( void );
    int32   count() { return n; }
    BOOLEAN read( void );
    TCHAR * name( TCHAR * cat );
    DOWNCAT_NAME_ENTRY & operator[]( int32 i );
    };

struct DOWNSCAT_NAME_ENTRY
    {
    TCHAR catnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR subnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];
    COLORREF color;

    DOWNSCAT_NAME_ENTRY();
    };

class DOWNSCAT_NAME_CLASS
    {
    private:

    DOWNSCAT_NAME_ENTRY * p;
    int32   n;

    public:

    DOWNSCAT_NAME_CLASS() { p = 0; n = 0; }
    ~DOWNSCAT_NAME_CLASS() { cleanup(); }
    void                  cleanup( void );
    int32                 count() { return n; }
    int32                 index( TCHAR * cat, TCHAR * sub );
    TCHAR               * name( TCHAR * cat, TCHAR * sub );
    DOWNSCAT_NAME_ENTRY & operator[]( int32 i );
    COLORREF              color( TCHAR * cat, TCHAR * sub );
    BOOLEAN               read( void );
    };

#endif
